//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Weapon debug overlay for Goblin Dice Rollaz.
//

#ifndef __ST_WEAPON_DEBUG_H__
#define __ST_WEAPON_DEBUG_H__

#include "doomtype.h"

extern int goblin_weapon_stats;
extern int goblin_think_profiler;

void ST_DrawWeaponStats(int x, int y);
void ST_PrintWeaponStats(void);
void ST_DrawThinkProfiler(int x, int y);

#endif
