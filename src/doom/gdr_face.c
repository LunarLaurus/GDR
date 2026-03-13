// src/doom/gdr_face.c
// GDR procedural goblin face widget — drawn with V_DrawFilledBox primitives.
#include "doomtype.h"
#include "v_video.h"
#include "gdr_face.h"
#include "st_stuff.h"
// Face index constants (mirrors defines in st_stuff.c)
#define ST_NUMSTRAIGHTFACES 3
#define ST_NUMTURNFACES     2
#define ST_NUMSPECIALFACES  3
#define ST_NUMPAINFACES     5
#define ST_FACESTRIDE       (ST_NUMSTRAIGHTFACES + ST_NUMTURNFACES + ST_NUMSPECIALFACES)
#define ST_TURNOFFSET       (ST_NUMSTRAIGHTFACES)
#define ST_OUCHOFFSET       (ST_TURNOFFSET + ST_NUMTURNFACES)
#define ST_EVILGRINOFFSET   (ST_OUCHOFFSET + 1)
#define ST_RAMPAGEOFFSET    (ST_EVILGRINOFFSET + 1)
#define ST_GODFACE          (ST_NUMPAINFACES * ST_FACESTRIDE)
#define ST_DEADFACE         (ST_GODFACE + 1)

// Palette indices
#define COL_BG      0    // black / transparent
#define COL_SKIN    160  // goblin green-brown
#define COL_DARK    200  // dark green shadow
#define COL_EYE     224  // bright yellow-white
#define COL_BLOOD   35   // red
#define COL_WHITE   4    // near-white

static void DrawGoblinHead(int x, int y)
{
    // Head body: 24x22 centered in 32x32
    V_DrawFilledBox(x + 4, y + 2, 24, 22, COL_SKIN);
    // Ear bumps
    V_DrawFilledBox(x + 1, y + 8, 4, 6, COL_SKIN);
    V_DrawFilledBox(x + 27, y + 8, 4, 6, COL_SKIN);
    // Dark outline
    V_DrawFilledBox(x + 4, y + 2,  24, 1, COL_DARK);
    V_DrawFilledBox(x + 4, y + 23, 24, 1, COL_DARK);
    V_DrawFilledBox(x + 4, y + 2,  1, 22, COL_DARK);
    V_DrawFilledBox(x + 27, y + 2, 1, 22, COL_DARK);
}

void ST_DrawGDRFace(int x, int y, GDRFaceState state)
{
    // Black background
    V_DrawFilledBox(x, y, GDR_FACE_W, GDR_FACE_H, COL_BG);

    if (state == GFACE_DEAD)
    {
        // Pancake: flat squashed oval
        V_DrawFilledBox(x + 4,  y + 18, 24, 8, COL_SKIN);
        V_DrawFilledBox(x + 2,  y + 20, 28, 4, COL_SKIN);
        V_DrawGDRChar(x + 7,  y + 19, 'X', COL_DARK);
        V_DrawGDRChar(x + 17, y + 19, 'X', COL_DARK);
        return;
    }

    DrawGoblinHead(x, y);

    switch (state)
    {
        case GFACE_HEALTHY:
            V_DrawFilledBox(x + 8,  y + 8,  5, 5, COL_EYE);
            V_DrawFilledBox(x + 19, y + 8,  5, 5, COL_EYE);
            V_DrawFilledBox(x + 8,  y + 17, 16, 4, COL_WHITE);
            V_DrawFilledBox(x + 9,  y + 17, 14, 1, COL_DARK);
            break;
        case GFACE_HURT:
            V_DrawFilledBox(x + 8,  y + 9, 5, 2, COL_EYE);
            V_DrawFilledBox(x + 19, y + 8, 5, 5, COL_EYE);
            V_DrawFilledBox(x + 9,  y + 19, 14, 2, COL_DARK);
            break;
        case GFACE_DAMAGED:
            V_DrawFilledBox(x + 8,  y + 9, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 19, y + 9, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 8,  y + 8, 5, 2, COL_DARK);
            V_DrawFilledBox(x + 19, y + 8, 5, 2, COL_DARK);
            V_DrawFilledBox(x + 15, y + 2, 2, 6, COL_BLOOD);
            V_DrawFilledBox(x + 9,  y + 19, 14, 2, COL_DARK);
            break;
        case GFACE_BAD:
            V_DrawFilledBox(x + 8,  y + 9, 5, 4, COL_EYE);
            V_DrawGDRChar(x + 19, y + 8, 'X', COL_BLOOD);
            V_DrawFilledBox(x + 7,  y + 19, 18, 3, COL_DARK);
            V_DrawFilledBox(x + 9,  y + 18, 14, 1, COL_SKIN);
            break;
        case GFACE_CRITICAL:
            V_DrawFilledBox(x + 7,  y + 8, 7, 6, COL_DARK);
            V_DrawFilledBox(x + 9,  y + 9, 3, 4, COL_EYE);
            V_DrawFilledBox(x + 18, y + 8, 7, 6, COL_DARK);
            V_DrawFilledBox(x + 20, y + 9, 3, 4, COL_EYE);
            V_DrawGDRChar(x + 10, y + 18, '~', COL_BLOOD);
            break;
        case GFACE_OUCH:
            V_DrawFilledBox(x + 8,  y + 7, 5, 6, COL_EYE);
            V_DrawFilledBox(x + 19, y + 7, 5, 6, COL_EYE);
            V_DrawFilledBox(x + 12, y + 17, 8, 6, COL_DARK);
            V_DrawFilledBox(x + 13, y + 18, 6, 4, COL_BG);
            break;
        case GFACE_EVIL:
            V_DrawFilledBox(x + 8,  y + 8, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 19, y + 8, 5, 4, COL_EYE);
            V_DrawFilledBox(x + 7,  y + 7, 6, 1, COL_DARK);
            V_DrawFilledBox(x + 19, y + 7, 6, 1, COL_DARK);
            V_DrawFilledBox(x + 7,  y + 17, 18, 4, COL_WHITE);
            V_DrawFilledBox(x + 9,  y + 17, 2, 4, COL_DARK);
            V_DrawFilledBox(x + 13, y + 17, 2, 4, COL_DARK);
            V_DrawFilledBox(x + 17, y + 17, 2, 4, COL_DARK);
            V_DrawFilledBox(x + 21, y + 17, 2, 4, COL_DARK);
            break;
        case GFACE_RAGE:
            V_DrawFilledBox(x + 8,  y + 9, 5, 4, COL_BLOOD);
            V_DrawFilledBox(x + 19, y + 9, 5, 4, COL_BLOOD);
            V_DrawFilledBox(x + 6,  y + 7, 8, 2, COL_DARK);
            V_DrawFilledBox(x + 18, y + 7, 8, 2, COL_DARK);
            V_DrawFilledBox(x + 8,  y + 17, 16, 4, COL_WHITE);
            break;
        case GFACE_GOD:
            V_DrawFilledBox(x + 7,  y + 7, 7, 7, COL_EYE);
            V_DrawFilledBox(x + 18, y + 7, 7, 7, COL_EYE);
            V_DrawFilledBox(x + 8,  y + 17, 16, 4, COL_EYE);
            break;
        default:
            break;
    }
}

// Map st_faceindex to a GDRFaceState.
// Called from ST_Drawer — st_faceindex is already updated by ST_updateFaceWidget.
GDRFaceState ST_GetGDRFaceState(void)
{

    // Check special states first
    if (st_faceindex == ST_DEADFACE) return GFACE_DEAD;
    if (st_faceindex == ST_GODFACE)  return GFACE_GOD;

    // Within each pain band, check expression offsets
    {
        int pain_offset = st_faceindex - (st_faceindex % ST_FACESTRIDE);
        int expr = st_faceindex % ST_FACESTRIDE;
        int pain_band = pain_offset / ST_FACESTRIDE;  // 0=healthy .. 4=critical

        if (expr == ST_RAMPAGEOFFSET) return GFACE_RAGE;
        if (expr == ST_EVILGRINOFFSET) return GFACE_EVIL;
        if (expr == ST_OUCHOFFSET)    return GFACE_OUCH;

        switch (pain_band)
        {
            case 0: return GFACE_HEALTHY;
            case 1: return GFACE_HURT;
            case 2: return GFACE_DAMAGED;
            case 3: return GFACE_BAD;
            default: return GFACE_CRITICAL;
        }
    }
}
