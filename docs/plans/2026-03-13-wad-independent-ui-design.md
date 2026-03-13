# GDR WAD-Independent UI Layer — Design

## Goal

Replace all DOOM2.WAD UI asset dependencies with procedurally generated or baked C array equivalents, enabling GDR to run standalone without a WAD file present at runtime.

## Architecture

Five self-contained layers, each replacing one WAD-dependent UI subsystem. All layers share a consistent dark-panel / gold-border / goblin-scrawl aesthetic. The 3D renderer (textures, sprites, map geometry) is out of scope — WAD independence for gameplay assets is a separate future effort.

---

## Layer 1: Palette & Colormap (`src/gdr_palette.h`)

**What:** PLAYPAL (768 bytes × 14 palettes) and COLORMAP (256 × 34 bytes) extracted from a local `DOOM2.WAD` at design time and committed as `static const uint8_t` C arrays.

**How:** Extraction performed once as part of implementation work. The arrays are committed to the repo; no WAD needed at runtime or build time after that.

**Integration:** `I_InitGraphics` and `ST_Init` switch from `W_CacheLumpName("PLAYPAL")` / `W_CacheLumpName("COLORMAP")` to direct array pointers. All downstream palette consumers (screen effects, lighting, sprites) work unchanged.

---

## Layer 2: Baked Bitmap Font (`src/gdr_font.h`)

**What:** A 6×8 pixel-art bitmap font covering ASCII 33–127 (95 glyphs), stored as a `static const uint8_t` array (1 bit per pixel, 6 bytes per glyph). Aesthetic: chunky, slightly irregular — goblin-scrawl / carved-stone flavour.

**How:** Font data hand-authored or generated as part of implementation. Two new functions added to `v_video.c`:
- `V_DrawGDRChar(x, y, c, color)` — draws one glyph to the framebuffer
- `V_DrawGDRString(x, y, str, color)` — draws a null-terminated string

**Integration:** At `HU_Init`, the `hu_font[]` patch array is populated by synthesizing `patch_t`-shaped buffers from the baked font data. All existing call sites (`HU_Drawer`, `ST_Drawer`, menu text rendering) work without modification.

---

## Layer 3: GDR Menu Redesign (`src/doom/m_menu.c`)

**What:** Full procedural redraw of the menu — no `M_DOOM`, `M_NEWG`, `M_SKILL`, skull cursor, or thermometer patches loaded or used.

**Layout:**
- Dark stone-panel background with gold border (consistent with status bar aesthetic)
- Title "GOBLIN DICE ROLLAZ" in baked font at 2× scale, gold with dark drop-shadow
- Menu items rendered in baked font, standard scale

**Cursor:** Animated rolling die (faces 1→6, ~4fps) replaces the vanilla skull cursor. Reuses `DrawDiceFace`-style rendering.

**Difficulty select:** Five skill levels mapped to die faces:
- 1 = I'm Too Young To Die
- 2 = Hey, Not Too Rough
- 3 = Hurt Me Plenty
- 4 = Ultra-Violence
- 6 = Nightmare (no 5 — because 6 is the die maximum)

On difficulty confirm, a brief dice-spin animation plays before the game starts.

**Sliders/thermometer:** Replaced with horizontal bars drawn using baked font box characters.

---

## Layer 4: GDR Face Widget (`src/gdr_face.h`)

**What:** ~40 vanilla face patch frames replaced with a procedurally drawn 32×32 goblin portrait. States baked as palette-indexed pixel art C arrays in `gdr_face.h`.

**Health tiers** (matching vanilla thresholds):
- >80%: Grinning goblin, pointy teeth
- 60–80%: One eye squinted, small frown
- 40–60%: Scowl, crack on forehead
- 20–40%: Grimace, X over one eye
- <20%: Skull-eyed, tongue out
- Dead: Flat goblin pancake
- Ouch / evil grin / rage: Expression overlays on top of health-tier base

**Integration:** New `ST_DrawGDRFace(x, y, state)` in `st_stuff.c` replaces the `faces[]` patch array lookup. Health tier and expression state computed from existing `st_faceindex` logic.

---

## Layer 5: GDR Intermission Screen (`src/doom/wi_stuff.c`)

**What:** Full procedural redraw — no `INTERPIC`, `WIA*`, `WIMAP*`, or score patches loaded.

**Layout:** Dark panel with gold border, consistent with menu and status bar.

**Three phases:**

1. **Stats:** Level name at 2× baked font. Kill%, item%, secret% tally one at a time — each counter animates through random digits before landing on the real value (driven by existing `wi_stuff.c` ticker). A small rolling die animates alongside each counter as it resolves.

2. **Next level:** "ENTERING" header + map name in baked font. Procedural map thumbnail: a rectangle outline with a goblin skull icon. No `WIMAP` patches.

3. **Co-op / DM scores:** Table layout using baked font, one bordered cell per player.

---

## Constraints & Non-Goals

- **3D renderer is out of scope.** Wall textures, floor/ceiling flats, sprites, and map geometry still load from WAD. Full art independence is a separate future effort.
- **Sounds and music are out of scope.**
- **Save/load screen** reuses the menu panel design; no separate design needed.
- **STBAR, STARMS, STKEYS patches** are already bypassed by the existing custom `ST_Drawer` — no additional work needed there.

---

## Delivery Order

1. `gdr_palette.h` — unblocks everything downstream
2. `gdr_font.h` + `V_DrawGDRChar/String` + `hu_font` synthesis
3. `gdr_face.h` + `ST_DrawGDRFace`
4. Menu redesign
5. Intermission redesign
