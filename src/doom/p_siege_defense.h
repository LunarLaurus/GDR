//
// Copyright(C) 2024 Jack Hobhouse
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
//  Defensive position system for siege battles.
//  Allows enemies to defend strategic positions rather than just chasing player.
//

#ifndef __P_SIEGE_DEFENSE__
#define __P_SIEGE_DEFENSE__

#include "p_mobj.h"
#include "doomdata.h"

#define MAX_DEFENSE_POSITIONS 32
#define DEFENSE_RADIUS_DEFAULT (256*FRACUNIT)
#define DEFENSE_RADIUS_LARGE (512*FRACUNIT)
#define DEFENSE_BONUS_MAX_DISTANCE (128*FRACUNIT)
#define DEFENSE_POSITION_BASE 9200

typedef struct {
    mapthing_t position;
    fixed_t defense_radius;
    int defense_strength;
    boolean is_active;
} defense_position_t;

void P_SpawnDefensePosition(mapthing_t* mthing);
void P_ShutdownDefensePositions(void);
defense_position_t* P_GetNearestDefensePosition(mobj_t* actor);
boolean P_IsWithinDefensePosition(mobj_t* actor, defense_position_t* defense);
int P_GetDefenseBonus(mobj_t* actor);
boolean P_SiegeDefenseActive(void);

#endif
