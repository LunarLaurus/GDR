//
// Copyright(C) 2024 Goblin Dice Rollaz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// DESCRIPTION:
//   GDR stub implementations for symbols referenced in doom.lib but not yet
//   implemented in the main executable. Each stub is marked with the reason
//   it is not yet implemented.
//

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "doomtype.h"
#include "m_fixed.h"

/* --------------------------------------------------------------------------
   Global variables expected by doom.lib code
   -------------------------------------------------------------------------- */

/* doomstat.h externs */
boolean showmem = false;
int     frame_profiling_enabled = 0;
int     frame_total_time_ms = 0;
int     frame_tic_time_ms = 0;
int     frame_render_time_ms = 0;
boolean lowres_turn = false;

/* --------------------------------------------------------------------------
   SHORT: byteswap function fallback.
   doom.lib files that use SHORT without including i_swap.h compiled it as an
   implicit function call. Provide a real function so the linker is satisfied.
   SDL_SwapLE16 is the proper implementation; on a little-endian machine this
   is a no-op.
   -------------------------------------------------------------------------- */
#include "SDL.h"
short SHORT(short x)
{
    return (short) SDL_SwapLE16((Uint16)x);
}

/* --------------------------------------------------------------------------
   MAX: integer maximum fallback (same situation as SHORT).
   -------------------------------------------------------------------------- */
int MAX(int a, int b)
{
    /* GDR STUB: MAX called as function instead of macro */
    return a > b ? a : b;
}

/* --------------------------------------------------------------------------
   Math helpers
   -------------------------------------------------------------------------- */
fixed_t D_abs(fixed_t x)
{
    /* GDR STUB: absolute value for fixed_t */
    return x < 0 ? -x : x;
}

fixed_t FixedHypot(fixed_t x, fixed_t y)
{
    /* GDR STUB: hypotenuse of two fixed-point values */
    double fx = (double)x / 65536.0;
    double fy = (double)y / 65536.0;
    return (fixed_t)(sqrt(fx*fx + fy*fy) * 65536.0);
}

/* --------------------------------------------------------------------------
   Video stubs
   -------------------------------------------------------------------------- */
#include "doom/r_defs.h"
#include "v_video.h"

void V_DrawPatchTranslated(int x, int y, byte *translation, patch_t *patch)
{
    /* GDR STUB: translated patch rendering not yet implemented; fall back */
    (void)translation;
    V_DrawPatch(x, y, patch);
}

void V_DrawPixel(int x, int y, byte col, int alpha)
{
    /* GDR STUB: alpha-blended pixel drawing not yet implemented */
    (void)alpha;
    if (x >= 0 && x < SCREENWIDTH && y >= 0 && y < SCREENHEIGHT)
        I_VideoBuffer[y * SCREENWIDTH + x] = col;
}

/* I_GetColor: returns a pointer to palette entries for a given color.
   Signature inferred from call sites: I_GetColor(x, y, r, g, b) -> byte* */
byte* I_GetColor(int x, int y, int r, int g, int b)
{
    /* GDR STUB: true color lookup not yet implemented */
    (void)x; (void)y; (void)r; (void)g; (void)b;
    static byte dummy[3] = { 0, 0, 0 };
    return dummy;
}

/* --------------------------------------------------------------------------
   Menu stubs
   -------------------------------------------------------------------------- */
void M_AnnounceMenuItem(void)
{
    /* GDR STUB: audio feedback for menu navigation not yet implemented */
}

/* --------------------------------------------------------------------------
   Map/geometry stubs
   -------------------------------------------------------------------------- */
#include "doom/p_local.h"

int P_AproximalDistance(fixed_t dx, fixed_t dy)
{
    /* GDR STUB: approximate distance (Manhattan metric) */
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return (int)(dx > dy ? dx + dy/2 : dy + dx/2);
}

boolean P_IsAllyInDanger(void *actor)
{
    /* GDR STUB: faction-based friendly fire detection not yet implemented */
    (void)actor;
    return false;
}

/* --------------------------------------------------------------------------
   Weapon action stubs (GDR dice-based weapon system)
   -------------------------------------------------------------------------- */
void A_FireD20Cannon(void *player, void *psp)
{
    /* GDR STUB: D20 cannon fire action */
    (void)player; (void)psp;
}

void A_FireD12(void *player, void *psp)
{
    /* GDR STUB: D12 weapon fire action */
    (void)player; (void)psp;
}

void A_FirePercentile(void *player, void *psp)
{
    /* GDR STUB: percentile die (D100) weapon fire action */
    (void)player; (void)psp;
}

void A_FireD4(void *player, void *psp)
{
    /* GDR STUB: D4 weapon fire action */
    (void)player; (void)psp;
}

void A_FireD8(void *player, void *psp)
{
    /* GDR STUB: D8 weapon fire action */
    (void)player; (void)psp;
}

void A_FireD10(void *player, void *psp)
{
    /* GDR STUB: D10 weapon fire action */
    (void)player; (void)psp;
}

/* Classic Doom weapon actions missing from doom.lib */
void A_FireShotgun(void *player, void *psp)
{
    /* GDR STUB: shotgun fire action */
    (void)player; (void)psp;
}

void A_FireShotgun2(void *player, void *psp)
{
    /* GDR STUB: super shotgun fire action */
    (void)player; (void)psp;
}

void A_FireCGun(void *player, void *psp)
{
    /* GDR STUB: chaingun fire action */
    (void)player; (void)psp;
}

void A_CPosAttack(void *actor)
{
    /* GDR STUB: chaingunner attack */
    (void)actor;
}

void A_CPosRefire(void *actor)
{
    /* GDR STUB: chaingunner refire check */
    (void)actor;
}

void A_SpidRefire(void *actor)
{
    /* GDR STUB: spider mastermind refire check */
    (void)actor;
}

void A_BspiAttack(void *actor)
{
    /* GDR STUB: baby spider attack */
    (void)actor;
}

/* --------------------------------------------------------------------------
   Enemy / turret stubs
   -------------------------------------------------------------------------- */
void A_TurretThink(void *actor)
{
    /* GDR STUB: engineer turret AI not yet implemented */
    (void)actor;
}

void A_EngineerDeployTurret(void *actor)
{
    /* GDR STUB: engineer turret deployment not yet implemented */
    (void)actor;
}
