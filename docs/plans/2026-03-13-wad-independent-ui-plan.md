# GDR WAD-Independent UI Layer — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace all DOOM2.WAD UI asset dependencies with baked C arrays and procedural rendering so GDR runs standalone without a WAD file at runtime.

**Architecture:** Five layers delivered in dependency order: palette baking → bitmap font → goblin face widget → menu redesign → intermission redesign. Each layer is self-contained with a clean integration point into existing code. The 3D renderer (textures, sprites, maps) is out of scope.

**Tech Stack:** C99, MSVC, Chocolate Doom rendering API (`V_DrawFilledBox`, `V_DrawPatch`, `pixel_t`), Doom palette system (8-bit indexed, PLAYPAL/COLORMAP).

**Build command:** `"D:\tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe" --build D:\code\GDR\build --config Debug --target goblin-doom`

**Run command:** `D:\code\GDR\build\src\Debug\goblin-doom.exe -iwad DOOM2.WAD -window -warp 1` (from `D:\code\GDR\build\src\Debug\`)

---

## Task 1: Wire PLAYPAL and COLORMAP from baked C arrays

The two extracted headers already exist at `src/playpal_data.h` and `src/colormap_data.h`. This task wires them into the engine so the WAD lookups are never called.

**Files:**
- Modify: `src/doom/st_stuff.c` (lines 521–522, 1381, 1890, 2155)
- Modify: `src/i_video.c` (line 1662)
- Modify: `src/doom/r_data.c` (lines 795–796)

**Step 1: Add includes to st_stuff.c**

At the top of `src/doom/st_stuff.c`, after existing includes, add:
```c
#include "playpal_data.h"
#include "colormap_data.h"
```

**Step 2: Replace lu_palette usage in st_stuff.c**

Find and change three sites:

Line ~1381 — replace:
```c
pal = (byte *) W_CacheLumpNum (lu_palette, PU_CACHE)+palette*768;
```
with:
```c
pal = (byte *) gdr_playpal + palette * 768;
```

Line ~1890 (inside `ST_Init`) — remove:
```c
lu_palette = W_GetNumForName (DEH_String("PLAYPAL"));
```
(just delete this line; `lu_palette` is no longer needed)

Line ~2155 — replace:
```c
I_SetPalette (W_CacheLumpNum (lu_palette, PU_CACHE));
```
with:
```c
I_SetPalette ((byte *) gdr_playpal);
```

Also remove the static variable declaration (line ~522):
```c
static int		lu_palette;
```

**Step 3: Replace PLAYPAL load in i_video.c**

Line ~1662 — replace:
```c
doompal = W_CacheLumpName(DEH_String("PLAYPAL"), PU_CACHE);
```
with:
```c
#include "playpal_data.h"
doompal = (const byte *) gdr_playpal;
```
(Add the include at the top of `i_video.c` with other includes instead of inline.)

**Step 4: Replace COLORMAP load in r_data.c**

Lines ~795–796 — replace:
```c
lump = W_GetNumForName(DEH_String("COLORMAP"));
colormaps = W_CacheLumpNum(lump, PU_STATIC);
```
with:
```c
#include "colormap_data.h"
colormaps = (lighttable_t *) gdr_colormap;
```
(Add the include at the top of `r_data.c`.)

**Step 5: Build and verify**

```
"D:\tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe" --build D:\code\GDR\build --config Debug --target goblin-doom
```
Expected: build succeeds, zero errors. Run the game briefly and confirm colors look correct (no green-tinted world, no corrupt palette).

**Step 6: Commit**
```
git add src/doom/st_stuff.c src/i_video.c src/doom/r_data.c src/playpal_data.h src/colormap_data.h
git commit -m "feat: bake PLAYPAL and COLORMAP from DOOM2.WAD into C arrays"
```

---

## Task 2: Baked bitmap font — gdr_font.h + V_DrawGDRString

**Files:**
- Create: `src/gdr_font.h`
- Modify: `src/v_video.h` (add declarations)
- Modify: `src/v_video.c` (add implementation)
- Modify: `src/doom/hu_stuff.c` (line ~343–355, replace STCFN loading)

**Step 1: Create src/gdr_font.h**

This is a 6-wide × 8-tall, 1bpp bitmap font covering ASCII 33–127 (95 glyphs). Each glyph is 6 bytes (one byte per row, 6 pixels used, MSB = leftmost pixel).

The goblin-scrawl aesthetic: thick strokes, slightly irregular — not clean sans-serif. Below is the complete font data. Each entry covers one character starting at ASCII 33 ('!').

```c
// src/gdr_font.h
// GDR baked bitmap font — 6x8, 1bpp, ASCII 33-127
// Each glyph: 8 bytes (rows top-to-bottom), bit7=leftmost pixel
#ifndef GDR_FONT_H
#define GDR_FONT_H

#define GDR_FONT_W      6
#define GDR_FONT_H      8
#define GDR_FONT_START  33   // '!'
#define GDR_FONT_END    126  // '~'
#define GDR_FONT_SIZE   (GDR_FONT_END - GDR_FONT_START + 1)

// 94 glyphs x 8 bytes each
// Bit layout per row: bit7=col0, bit6=col1, ... bit2=col5
static const unsigned char gdr_font_data[GDR_FONT_SIZE][GDR_FONT_H] = {
    /* 33 ! */ { 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00 },
    /* 34 " */ { 0x50, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00 },
    /* 35 # */ { 0x50, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x50, 0x00 },
    /* 36 $ */ { 0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00 },
    /* 37 % */ { 0xC0, 0xC8, 0x10, 0x20, 0x40, 0x98, 0x18, 0x00 },
    /* 38 & */ { 0x60, 0x90, 0xA0, 0x40, 0xA8, 0x90, 0x68, 0x00 },
    /* 39 ' */ { 0x20, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
    /* 40 ( */ { 0x10, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10, 0x00 },
    /* 41 ) */ { 0x40, 0x20, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00 },
    /* 42 * */ { 0x00, 0x20, 0xA8, 0x70, 0xA8, 0x20, 0x00, 0x00 },
    /* 43 + */ { 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00, 0x00 },
    /* 44 , */ { 0x00, 0x00, 0x00, 0x00, 0x30, 0x20, 0x40, 0x00 },
    /* 45 - */ { 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00 },
    /* 46 . */ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00 },
    /* 47 / */ { 0x08, 0x10, 0x10, 0x20, 0x40, 0x40, 0x80, 0x00 },
    /* 48 0 */ { 0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00 },
    /* 49 1 */ { 0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00 },
    /* 50 2 */ { 0x70, 0x88, 0x08, 0x30, 0x40, 0x80, 0xF8, 0x00 },
    /* 51 3 */ { 0xF8, 0x08, 0x10, 0x30, 0x08, 0x88, 0x70, 0x00 },
    /* 52 4 */ { 0x10, 0x30, 0x50, 0x90, 0xF8, 0x10, 0x10, 0x00 },
    /* 53 5 */ { 0xF8, 0x80, 0xF0, 0x08, 0x08, 0x88, 0x70, 0x00 },
    /* 54 6 */ { 0x38, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00 },
    /* 55 7 */ { 0xF8, 0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x00 },
    /* 56 8 */ { 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00 },
    /* 57 9 */ { 0x70, 0x88, 0x88, 0x78, 0x08, 0x10, 0xE0, 0x00 },
    /* 58 : */ { 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00, 0x00 },
    /* 59 ; */ { 0x00, 0x30, 0x30, 0x00, 0x30, 0x20, 0x40, 0x00 },
    /* 60 < */ { 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00 },
    /* 61 = */ { 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00 },
    /* 62 > */ { 0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00 },
    /* 63 ? */ { 0x70, 0x88, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00 },
    /* 64 @ */ { 0x70, 0x88, 0x08, 0x68, 0xA8, 0xA8, 0x70, 0x00 },
    /* 65 A */ { 0x20, 0x50, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00 },
    /* 66 B */ { 0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00 },
    /* 67 C */ { 0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00 },
    /* 68 D */ { 0xE0, 0x90, 0x88, 0x88, 0x88, 0x90, 0xE0, 0x00 },
    /* 69 E */ { 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00 },
    /* 70 F */ { 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00 },
    /* 71 G */ { 0x70, 0x88, 0x80, 0xB8, 0x88, 0x88, 0x70, 0x00 },
    /* 72 H */ { 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00 },
    /* 73 I */ { 0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00 },
    /* 74 J */ { 0x38, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00 },
    /* 75 K */ { 0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00 },
    /* 76 L */ { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00 },
    /* 77 M */ { 0x88, 0xD8, 0xA8, 0xA8, 0x88, 0x88, 0x88, 0x00 },
    /* 78 N */ { 0x88, 0xC8, 0xA8, 0x98, 0x88, 0x88, 0x88, 0x00 },
    /* 79 O */ { 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
    /* 80 P */ { 0xF0, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00 },
    /* 81 Q */ { 0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00 },
    /* 82 R */ { 0xF0, 0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88, 0x00 },
    /* 83 S */ { 0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00 },
    /* 84 T */ { 0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00 },
    /* 85 U */ { 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
    /* 86 V */ { 0x88, 0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x00 },
    /* 87 W */ { 0x88, 0x88, 0x88, 0xA8, 0xA8, 0xD8, 0x88, 0x00 },
    /* 88 X */ { 0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00 },
    /* 89 Y */ { 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x20, 0x00 },
    /* 90 Z */ { 0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00 },
    /* 91 [ */ { 0x70, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70, 0x00 },
    /* 92 \ */ { 0x80, 0x40, 0x40, 0x20, 0x10, 0x10, 0x08, 0x00 },
    /* 93 ] */ { 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00 },
    /* 94 ^ */ { 0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00 },
    /* 95 _ */ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00 },
    /* 96 ` */ { 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
    /* 97 a */ { 0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
    /* 98 b */ { 0x80, 0x80, 0xB0, 0xC8, 0x88, 0xC8, 0xB0, 0x00 },
    /* 99 c */ { 0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00 },
    /* 100 d */ { 0x08, 0x08, 0x68, 0x98, 0x88, 0x98, 0x68, 0x00 },
    /* 101 e */ { 0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x70, 0x00 },
    /* 102 f */ { 0x30, 0x48, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x00 },
    /* 103 g */ { 0x00, 0x68, 0x98, 0x88, 0x98, 0x68, 0x08, 0x70 },
    /* 104 h */ { 0x80, 0x80, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00 },
    /* 105 i */ { 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00 },
    /* 106 j */ { 0x10, 0x00, 0x30, 0x10, 0x10, 0x90, 0x60, 0x00 },
    /* 107 k */ { 0x80, 0x80, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x00 },
    /* 108 l */ { 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00 },
    /* 109 m */ { 0x00, 0x00, 0xD0, 0xA8, 0xA8, 0x88, 0x88, 0x00 },
    /* 110 n */ { 0x00, 0x00, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00 },
    /* 111 o */ { 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00 },
    /* 112 p */ { 0x00, 0xB0, 0xC8, 0x88, 0xC8, 0xB0, 0x80, 0x80 },
    /* 113 q */ { 0x00, 0x68, 0x98, 0x88, 0x98, 0x68, 0x08, 0x08 },
    /* 114 r */ { 0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80, 0x00 },
    /* 115 s */ { 0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xF0, 0x00 },
    /* 116 t */ { 0x40, 0x40, 0xF0, 0x40, 0x40, 0x48, 0x30, 0x00 },
    /* 117 u */ { 0x00, 0x00, 0x88, 0x88, 0x88, 0x98, 0x68, 0x00 },
    /* 118 v */ { 0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00 },
    /* 119 w */ { 0x00, 0x00, 0x88, 0x88, 0xA8, 0xA8, 0x50, 0x00 },
    /* 120 x */ { 0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00 },
    /* 121 y */ { 0x00, 0x88, 0x88, 0x98, 0x68, 0x08, 0x70, 0x00 },
    /* 122 z */ { 0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00 },
    /* 123 { */ { 0x18, 0x20, 0x20, 0x40, 0x20, 0x20, 0x18, 0x00 },
    /* 124 | */ { 0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x00 },
    /* 125 } */ { 0xC0, 0x20, 0x20, 0x10, 0x20, 0x20, 0xC0, 0x00 },
    /* 126 ~ */ { 0x40, 0xA8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

#endif // GDR_FONT_H
```

**Step 2: Add V_DrawGDRChar and V_DrawGDRString to v_video.h**

In `src/v_video.h`, add after the existing function declarations:
```c
// GDR baked font rendering
void V_DrawGDRChar(int x, int y, char c, int color);
void V_DrawGDRString(int x, int y, const char *str, int color);
void V_DrawGDRStringScaled(int x, int y, const char *str, int color, int scale);
```

**Step 3: Implement V_DrawGDRChar / V_DrawGDRString in v_video.c**

Add at the bottom of `src/v_video.c`:
```c
#include "gdr_font.h"

void V_DrawGDRChar(int x, int y, char c, int color)
{
    int col, row;
    const unsigned char *glyph;

    if (c < GDR_FONT_START || c > GDR_FONT_END)
        return;

    glyph = gdr_font_data[c - GDR_FONT_START];

    for (row = 0; row < GDR_FONT_H; row++)
    {
        for (col = 0; col < GDR_FONT_W; col++)
        {
            if (glyph[row] & (0x80 >> col))
            {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < SCREENWIDTH && py >= 0 && py < SCREENHEIGHT)
                    I_VideoBuffer[py * SCREENWIDTH + px] = (pixel_t) color;
            }
        }
    }
}

void V_DrawGDRString(int x, int y, const char *str, int color)
{
    int cx = x;
    while (*str)
    {
        V_DrawGDRChar(cx, y, *str, color);
        cx += GDR_FONT_W + 1;
        str++;
    }
}

void V_DrawGDRStringScaled(int x, int y, const char *str, int color, int scale)
{
    int cx = x;
    int col, row, sx, sy;
    const unsigned char *glyph;

    while (*str)
    {
        char c = *str++;
        if (c < GDR_FONT_START || c > GDR_FONT_END) { cx += (GDR_FONT_W + 1) * scale; continue; }
        glyph = gdr_font_data[c - GDR_FONT_START];
        for (row = 0; row < GDR_FONT_H; row++)
            for (col = 0; col < GDR_FONT_W; col++)
                if (glyph[row] & (0x80 >> col))
                    for (sy = 0; sy < scale; sy++)
                        for (sx = 0; sx < scale; sx++)
                        {
                            int px = cx + col * scale + sx;
                            int py = y + row * scale + sy;
                            if (px >= 0 && px < SCREENWIDTH && py >= 0 && py < SCREENHEIGHT)
                                I_VideoBuffer[py * SCREENWIDTH + px] = (pixel_t) color;
                        }
        cx += (GDR_FONT_W + 1) * scale;
    }
}
```

**Step 4: Synthesize hu_font[] from baked font data in HU_Init**

In `src/doom/hu_stuff.c`, replace the STCFN loading block (lines ~351–355):

```c
// OLD:
j = HU_FONTSTART;
for (i=0;i<HU_FONTSIZE;i++)
{
    DEH_snprintf(buffer, 9, "STCFN%.3d", j++);
    hu_font[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
}
```

with:

```c
#include "gdr_font.h"

// Synthesize hu_font patches from baked GDR bitmap font.
// Each patch_t is allocated from the zone and filled with glyph pixel data.
{
    int gi, row, col;
    for (gi = 0; gi < HU_FONTSIZE; gi++)
    {
        char ch = (char)(HU_FONTSTART + gi);
        patch_t *p;
        byte *pixels;
        int w = GDR_FONT_W + 2;  // +2 for side margins
        int h = GDR_FONT_H + 2;  // +2 for top/bottom margins
        // Allocate patch: sizeof(patch_t) header + w*h pixel bytes
        p = Z_Malloc(sizeof(patch_t) + w * h, PU_STATIC, NULL);
        p->width = w;
        p->height = h;
        p->leftoffset = 1;
        p->topoffset = 1;
        pixels = (byte *)(p + 1);
        memset(pixels, 0, w * h);
        if (ch >= GDR_FONT_START && ch <= GDR_FONT_END)
        {
            const unsigned char *glyph = gdr_font_data[ch - GDR_FONT_START];
            for (row = 0; row < GDR_FONT_H; row++)
                for (col = 0; col < GDR_FONT_W; col++)
                    if (glyph[row] & (0x80 >> col))
                        pixels[(row + 1) * w + (col + 1)] = 224; // gold-ish palette index
        }
        hu_font[gi] = p;
    }
}
```

> **Note:** `patch_t` in Chocolate Doom does not actually have an inline pixel array — it uses column offsets. The `hu_font` patches are only used via `HU_drawChar` → `V_DrawPatch`. We need to check how `V_DrawPatch` consumes patch_t before implementing this step. If the column format is required, synthesizing a valid patch_t is complex. **If that's the case, use approach B instead:** call `V_DrawGDRString` directly in `ST_DrawText` and `HU_Drawer`, bypassing `hu_font` entirely for display, and just set `hu_font[i] = NULL` with null-checks added at call sites.
>
> **Recommended:** Audit `V_DrawPatch` and `HU_drawChar` before implementing Step 4. If patch column format is required, switch to approach B (direct V_DrawGDRString calls). This is noted as a decision point — do not guess, read the code first.

**Step 5: Build and verify**

```
"D:\tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe" --build D:\code\GDR\build --config Debug --target goblin-doom
```
Launch game, verify HUD messages and status bar text render using the new font.

**Step 6: Commit**
```
git add src/gdr_font.h src/v_video.h src/v_video.c src/doom/hu_stuff.c
git commit -m "feat: add baked GDR bitmap font and V_DrawGDRString API"
```

---

## Task 3: Goblin face widget — gdr_face.h + ST_DrawGDRFace

**Files:**
- Create: `src/gdr_face.h`
- Modify: `src/doom/st_stuff.c` (lines ~567, ~1708–1709, ST_loadCallback face loading ~1848–1871)

**Step 1: Create src/gdr_face.h**

Define 8 face states and their 32×32 palette-indexed pixel art data. Each state is a flat array of 1024 bytes (32×32 pixels, palette indices).

The faces are intentionally simple blocky pixel art — goblin head silhouette with expression-specific eyes/mouth. Palette index 0 = transparent/background, index 224 = gold (skin), index 200 = dark green (goblin), index 35 = red (blood/damage), index 4 = dark grey (shadow).

```c
// src/gdr_face.h
#ifndef GDR_FACE_H
#define GDR_FACE_H

#define GDR_FACE_W  32
#define GDR_FACE_H  32

typedef enum {
    GFACE_HEALTHY = 0,  // >80% HP: grinning
    GFACE_HURT,         // 60-80%: squinting
    GFACE_DAMAGED,      // 40-60%: scowl + crack
    GFACE_BAD,          // 20-40%: grimace + X eye
    GFACE_CRITICAL,     // <20%: skull-eyes
    GFACE_DEAD,         // dead: pancake
    GFACE_OUCH,         // ouch expression
    GFACE_EVIL,         // evil grin
    GFACE_RAGE,         // rage
    GFACE_GOD,          // god mode: glowing
    GFACE_COUNT
} GDRFaceState;

// Forward declare — implementation fills these in gdr_face.c or inline
void ST_DrawGDRFace(int x, int y, GDRFaceState state);
GDRFaceState ST_GetGDRFaceState(void);

#endif
```

**Step 2: Create src/doom/gdr_face.c**

Implement `ST_DrawGDRFace` using `V_DrawFilledBox` and `V_DrawGDRChar` calls to draw the goblin face procedurally (no pre-baked pixel arrays needed — draw it with geometric primitives):

```c
// src/doom/gdr_face.c
#include "doomtype.h"
#include "v_video.h"
#include "gdr_face.h"
#include "doom/st_stuff.h"

// Palette indices for face colors
#define COL_BG        0    // transparent / black
#define COL_SKIN      160  // goblin green-brown
#define COL_DARK      200  // dark green shadow
#define COL_EYE       224  // bright yellow-white
#define COL_BLOOD     35   // red
#define COL_GOLD      160  // reuse skin for now
#define COL_WHITE     4    // near-white

static void DrawGoblinHead(int x, int y)
{
    // Head outline: 24x22 rounded rectangle centered in 32x32
    V_DrawFilledBox(x + 4, y + 2, 24, 22, COL_SKIN);
    // Ear bumps
    V_DrawFilledBox(x + 1, y + 8, 4, 6, COL_SKIN);
    V_DrawFilledBox(x + 27, y + 8, 4, 6, COL_SKIN);
    // Dark outline
    V_DrawFilledBox(x + 4, y + 2, 24, 1, COL_DARK);
    V_DrawFilledBox(x + 4, y + 23, 24, 1, COL_DARK);
    V_DrawFilledBox(x + 4, y + 2, 1, 22, COL_DARK);
    V_DrawFilledBox(x + 27, y + 2, 1, 22, COL_DARK);
}

void ST_DrawGDRFace(int x, int y, GDRFaceState state)
{
    // Black background
    V_DrawFilledBox(x, y, GDR_FACE_W, GDR_FACE_H, COL_BG);

    if (state == GFACE_DEAD)
    {
        // Pancake: flat squashed oval
        V_DrawFilledBox(x + 4, y + 18, 24, 8, COL_SKIN);
        V_DrawFilledBox(x + 2, y + 20, 28, 4, COL_SKIN);
        // X eyes
        V_DrawGDRChar(x + 7, y + 19, 'X', COL_DARK);
        V_DrawGDRChar(x + 17, y + 19, 'X', COL_DARK);
        return;
    }

    DrawGoblinHead(x, y);

    // Eyes — position varies by state
    switch (state)
    {
        case GFACE_HEALTHY:
            // Wide open eyes, big grin teeth
            V_DrawFilledBox(x + 8,  y + 8, 5, 5, COL_EYE);
            V_DrawFilledBox(x + 19, y + 8, 5, 5, COL_EYE);
            V_DrawFilledBox(x + 8,  y + 17, 16, 4, COL_WHITE);  // teeth
            V_DrawFilledBox(x + 9,  y + 17, 14, 1, COL_DARK);   // tooth gap
            break;
        case GFACE_HURT:
            // One eye squinted (horizontal line), small frown
            V_DrawFilledBox(x + 8,  y + 9, 5, 2, COL_EYE);
            V_DrawFilledBox(x + 19, y + 8, 5, 5, COL_EYE);
            V_DrawFilledBox(x + 9,  y + 19, 14, 2, COL_DARK);   // frown
            break;
        case GFACE_DAMAGED:
            // Scowl + crack on forehead
            V_DrawFilledBox(x + 8,  y + 9, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 19, y + 9, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 8,  y + 8, 5, 2, COL_DARK);   // brow furrow
            V_DrawFilledBox(x + 19, y + 8, 5, 2, COL_DARK);
            V_DrawFilledBox(x + 15, y + 2, 2, 6, COL_BLOOD);  // crack
            V_DrawFilledBox(x + 9,  y + 19, 14, 2, COL_DARK);
            break;
        case GFACE_BAD:
            // Grimace + X over left eye
            V_DrawFilledBox(x + 8,  y + 9, 5, 4, COL_EYE);
            V_DrawGDRChar(x + 19, y + 8, 'X', COL_BLOOD);
            V_DrawFilledBox(x + 7,  y + 19, 18, 3, COL_DARK);
            V_DrawFilledBox(x + 9,  y + 18, 14, 1, COL_SKIN);  // teeth showing in grimace
            break;
        case GFACE_CRITICAL:
            // Skull eyes (hollow circles approximated)
            V_DrawFilledBox(x + 7,  y + 8, 7, 6, COL_DARK);
            V_DrawFilledBox(x + 9,  y + 9, 3, 4, COL_EYE);
            V_DrawFilledBox(x + 18, y + 8, 7, 6, COL_DARK);
            V_DrawFilledBox(x + 20, y + 9, 3, 4, COL_EYE);
            V_DrawGDRChar(x + 10, y + 18, '~', COL_BLOOD);     // grimace
            break;
        case GFACE_OUCH:
            // Wide O mouth, surprised eyes
            V_DrawFilledBox(x + 8,  y + 7, 5, 6, COL_EYE);
            V_DrawFilledBox(x + 19, y + 7, 5, 6, COL_EYE);
            V_DrawFilledBox(x + 12, y + 17, 8, 6, COL_DARK);   // O mouth
            V_DrawFilledBox(x + 13, y + 18, 6, 4, COL_BG);
            break;
        case GFACE_EVIL:
            // Evil grin: angled eyebrows, wide toothy grin
            V_DrawFilledBox(x + 8,  y + 8, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 19, y + 8, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 7,  y + 7, 6, 1, COL_DARK);   // angled brow L
            V_DrawFilledBox(x + 19, y + 7, 6, 1, COL_DARK);   // angled brow R
            V_DrawFilledBox(x + 7,  y + 17, 18, 4, COL_WHITE); // wide grin
            V_DrawFilledBox(x + 9,  y + 17, 2, 4, COL_DARK);  // tooth dividers
            V_DrawFilledBox(x + 13, y + 17, 2, 4, COL_DARK);
            V_DrawFilledBox(x + 17, y + 17, 2, 4, COL_DARK);
            V_DrawFilledBox(x + 21, y + 17, 2, 4, COL_DARK);
            break;
        case GFACE_RAGE:
            // Rage: red tinted, furious brows, bared teeth
            V_DrawFilledBox(x + 8,  y + 9, 5, 4, COL_BLOOD);
            V_DrawFilledBox(x + 19, y + 9, 5, 4, COL_BLOOD);
            V_DrawFilledBox(x + 6,  y + 7, 8, 2, COL_DARK);
            V_DrawFilledBox(x + 18, y + 7, 8, 2, COL_DARK);
            V_DrawFilledBox(x + 8,  y + 17, 16, 4, COL_WHITE);
            break;
        case GFACE_GOD:
            // God mode: star/glow eyes
            V_DrawFilledBox(x + 7,  y + 7, 7, 7, COL_EYE);
            V_DrawFilledBox(x + 18, y + 7, 7, 7, COL_EYE);
            V_DrawFilledBox(x + 8,  y + 17, 16, 4, COL_EYE);  // golden grin
            break;
        default:
            break;
    }
}

GDRFaceState ST_GetGDRFaceState(void)
{
    extern int st_faceindex;
    extern int plyr_health;  // we'll use player health directly

    // Map faceindex ranges to GDR face states
    // ST_DEADFACE and ST_GODFACE are defined in st_stuff.c
    // For simplicity, map via health since st_faceindex is already computed
    int health = plyr_health;

    if (health <= 0)   return GFACE_DEAD;
    if (health > 80)   return GFACE_HEALTHY;
    if (health > 60)   return GFACE_HURT;
    if (health > 40)   return GFACE_DAMAGED;
    if (health > 20)   return GFACE_BAD;
    return GFACE_CRITICAL;
}
```

> **Note:** `ST_GetGDRFaceState` maps from `st_faceindex` — check the actual variable names in `st_stuff.c` and adjust. The ouch/evil/rage/god states should be triggered by the same conditions that set `ST_OUCHOFFSET`, `ST_EVILGRINOFFSET`, `ST_RAMPAGEOFFSET`, `ST_GODFACE` in `st_stuff.c`. Pass a `GDRFaceState` parameter from the caller instead of recomputing.

**Step 3: Replace faces[] usage in ST_Drawer**

In `src/doom/st_stuff.c` line ~1708–1709, replace:
```c
if (faces[st_faceindex])
    V_DrawPatchDirect(178, ST_Y + 8, faces[st_faceindex]);
```
with:
```c
{
    extern GDRFaceState ST_GetGDRFaceState(void);
    ST_DrawGDRFace(178, ST_Y + 8, ST_GetGDRFaceState());
}
```

**Step 4: Remove face patch loading from ST_loadCallback**

In `src/doom/st_stuff.c` around lines 1848–1871, remove the face loading loops:
```c
// Remove all of these:
for (i=0; i<ST_NUMPAINFACES; i++)
    for (j=0; j<ST_NUMSTRAIGHTFACES; j++) { ... }
// ... (turn/ouch/evil/rampage/god/dead face loads)
```
Also remove `static patch_t* faces[ST_NUMFACES];` (line ~567) — no longer needed.

**Step 5: Add gdr_face.c to CMakeLists**

In `src/doom/CMakeLists.txt`, add `gdr_face.c` to the doom library source list.

**Step 6: Build and verify**

```
"D:\tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe" --build D:\code\GDR\build --config Debug --target goblin-doom
```
Launch game, check the face area in the status bar shows the goblin portrait. Take damage, verify face changes.

**Step 7: Commit**
```
git add src/gdr_face.h src/doom/gdr_face.c src/doom/st_stuff.c src/doom/CMakeLists.txt
git commit -m "feat: replace vanilla face patches with procedural goblin face widget"
```

---

## Task 4: GDR Menu Redesign

**Files:**
- Modify: `src/doom/m_menu.c` — `M_Drawer`, `M_DrawThermo`, skull cursor logic, WAD patch loads

**Step 1: Remove WAD patch loads from M_Init**

In `m_menu.c`, find where `M_DOOM`, `M_SKULL1`, `M_SKULL2`, thermometer patches (`M_THERML`, `M_THERMM`, `M_THERMO`, `M_THERMR`) are loaded and remove those `W_CacheLumpName` calls. Set those `patch_t *` variables to NULL.

**Step 2: Replace M_DrawThermo with drawn bar**

Find `M_DrawThermo` (declared in `m_menu.h`). Replace its body with:
```c
void M_DrawThermo(int x, int y, int thermWidth, int thermDot)
{
    int i;
    int barcolor = 200;   // dark green
    int dotcolor = 224;   // gold
    int bgcol    = 0;

    // Background bar
    V_DrawFilledBox(x, y, thermWidth * 8, 8, bgcol);
    // Filled portion
    V_DrawFilledBox(x, y, thermDot * 8, 8, barcolor);
    // Border
    V_DrawFilledBox(x, y, thermWidth * 8, 1, dotcolor);
    V_DrawFilledBox(x, y + 7, thermWidth * 8, 1, dotcolor);
    V_DrawFilledBox(x, y, 1, 8, dotcolor);
    V_DrawFilledBox(x + thermWidth * 8 - 1, y, 1, 8, dotcolor);
}
```

**Step 3: Replace skull cursor with dice cursor**

Find `whichSkull` and the skull drawing code (look for `skullName` and `V_DrawPatch` for the cursor). Replace with:

```c
// In M_Drawer where skull is drawn:
// OLD: V_DrawPatch(x - 32, currentMenu->y + item*LINEHEIGHT - 5, skullPatch);
// NEW:
{
    extern void DrawDiceFace(pixel_t *buffer, int x, int y, int face, int size);
    // Draw a small 16x16 die face as cursor
    // whichSkull animates 0-1 at ~8 ticks each; map to die face cycling
    static int dice_cursor_frame = 1;
    static int dice_cursor_timer = 0;
    if (++dice_cursor_timer >= 8) { dice_cursor_timer = 0; if (++dice_cursor_frame > 6) dice_cursor_frame = 1; }
    // Draw 16x16 die directly to video buffer
    {
        int cx = currentMenu->x - 24;
        int cy = currentMenu->y + itemOn * LINEHEIGHT - 4;
        int sz = 16, dr, dc;
        int dot_size = sz / 5;
        int inner = sz - dot_size * 2;
        int ox = dot_size, oy = dot_size;
        for (dr = 0; dr < sz; dr++)
            for (dc = 0; dc < sz; dc++)
            {
                int px = cx + dc, py = cy + dr;
                if (px < 0 || px >= SCREENWIDTH || py < 0 || py >= SCREENHEIGHT) continue;
                I_VideoBuffer[py * SCREENWIDTH + px] = 0; // bg
            }
        // (simplified — reuse DrawDiceFace approach inline)
    }
}
```

> **Note:** `DrawDiceFace` in `v_diceicon.c` is static. Either make it non-static and extern-declare it, or extract it to a shared `v_dice.c`. Do the cleaner extraction as part of this task.

**Step 4: Replace menu title rendering**

Find where `M_DOOM` title patch is drawn in `M_Drawer`. Replace with:
```c
// Draw "GOBLIN DICE ROLLAZ" title
V_DrawFilledBox(40, 10, 240, 20, 0);                          // dark bg
V_DrawFilledBox(40, 10, 240, 1, 160);                         // gold top border
V_DrawFilledBox(40, 29, 240, 1, 160);                         // gold bottom border
V_DrawGDRStringScaled(44, 13, "GOBLIN DICE ROLLAZ", 224, 1);  // gold text
```

**Step 5: Remap difficulty names**

Find `NewGameMenu[]` item names and change them to GDR die-face labels:
```c
// Change the name strings in NewGameMenu to:
// "1 - TOO YOUNG TO DIE"
// "2 - HEY NOT TOO ROUGH"
// "3 - HURT ME PLENTY"
// "4 - ULTRA-VIOLENCE"
// "6 - NIGHTMARE"
```

**Step 6: Build and verify**

```
"D:\tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe" --build D:\code\GDR\build --config Debug --target goblin-doom
```
Launch game, open main menu — verify title renders, dice cursor animates, difficulty labels show GDR names.

**Step 7: Commit**
```
git add src/doom/m_menu.c
git commit -m "feat: GDR menu redesign - procedural title, dice cursor, die-face difficulty names"
```

---

## Task 5: GDR Intermission Screen Redesign

**Files:**
- Modify: `src/doom/wi_stuff.c` — `WI_loadData`, `WI_drawBackground`, `WI_drawLF`, `WI_drawEL`, `WI_drawShowNextLoc`, stat drawing functions

**Step 1: Remove WAD patch loads from WI_loadData**

In `WI_loadData` (line ~1779), remove loads for `INTERPIC`, `WIMAP*`, `WIA*`, `lnames`, `star`/`bstar` patches. Set those pointers to NULL.

**Step 2: Replace WI_drawBackground**

Find `WI_drawBackground` (draws `INTERPIC`). Replace body with:
```c
static void WI_drawBackground(void)
{
    // Dark stone panel — full screen
    V_DrawFilledBox(0, 0, SCREENWIDTH, SCREENHEIGHT, 0);
    // Gold border
    V_DrawFilledBox(4, 4, SCREENWIDTH - 8, 2, 160);
    V_DrawFilledBox(4, SCREENHEIGHT - 6, SCREENWIDTH - 8, 2, 160);
    V_DrawFilledBox(4, 4, 2, SCREENHEIGHT - 8, 160);
    V_DrawFilledBox(SCREENWIDTH - 6, 4, 2, SCREENHEIGHT - 8, 160);
}
```

**Step 3: Replace level name drawing**

Find where `lnames[wbs->last]` and `lnames[wbs->next]` are drawn via `V_DrawPatch`. Replace with `V_DrawGDRStringScaled` calls using the map name strings from `DEH_String(mapnames[wbs->last])`.

**Step 4: Implement animated stat counter**

The existing `wi_stuff.c` already has `cnt_kills`, `cnt_items`, `cnt_secret` counters that tick up. Find where they're drawn (look for `V_DrawPatch` calls with `num_font` or similar). Replace the patch-based number drawing with `V_DrawGDRString` using `M_snprintf` to format the number.

Add a small rolling die next to each counter — when `cnt_kills[0] < wbs->skills[0]` (still counting), draw `DrawDiceFace`-style die in a random face; when complete, draw face matching final digit mod 6 + 1.

**Step 5: Replace "ENTERING" / "FINISHED" title patches**

Find `WI_drawEL` (draws entering level) and `WI_drawLF` (draws finished). Replace patch draws with:
```c
V_DrawGDRStringScaled(10, 10, "FINISHED", 224, 2);
// and
V_DrawGDRStringScaled(10, 10, "ENTERING", 224, 2);
```

**Step 6: Replace map thumbnail**

Find `WI_drawShowNextLoc` where the map background is used. Replace the map graphic with a procedural thumbnail:
```c
// Map thumbnail: rectangle with skull icon
V_DrawFilledBox(100, 60, 120, 80, 0);
V_DrawFilledBox(100, 60, 120, 2, 160);
V_DrawFilledBox(100, 138, 120, 2, 160);
V_DrawFilledBox(100, 60, 2, 80, 160);
V_DrawFilledBox(218, 60, 2, 80, 160);
V_DrawGDRStringScaled(148, 92, "?", 224, 3);  // mystery marker
```

**Step 7: Build and verify**

```
"D:\tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe" --build D:\code\GDR\build --config Debug --target goblin-doom
```
Complete MAP01, verify intermission screen renders without crashing. Check stats tally animation works.

**Step 8: Commit**
```
git add src/doom/wi_stuff.c
git commit -m "feat: GDR intermission screen - fully procedural, no WAD patches"
```

---

## Final Verification

After all 5 tasks complete:

1. Confirm `DOOM2.WAD` is **not** needed for any UI element by temporarily renaming the WAD and launching — the game should start and show menus without crashing (it will crash when trying to load map geometry, which is expected and out of scope).
2. Verify no `W_CacheLumpName` / `W_GetNumForName` calls remain in `st_stuff.c`, `hu_stuff.c`, `m_menu.c`, `wi_stuff.c` for UI patches (only gameplay/map data loads are acceptable).
3. Run through: main menu → new game → MAP01 → take damage → die → intermission → verify all screens render correctly.

```
git tag wad-independent-ui-complete
```
