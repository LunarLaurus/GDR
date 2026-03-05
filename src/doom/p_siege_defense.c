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

#include "doomdef.h"
#include "p_siege_defense.h"
#include "p_mobj.h"
#include "p_ai_faction.h"
#include "g_game.h"
#include "doomstat.h"

static defense_position_t defense_positions[MAX_DEFENSE_POSITIONS];
static int num_defense_positions = 0;
static boolean defense_active = false;

#define DEFENSE_BONUS_DAMAGE 20
#define DEFENSE_BONUS_DEFENSE 15

void P_SpawnDefensePosition(mapthing_t* mthing)
{
    defense_position_t* defense;
    int subtype;

    if (num_defense_positions >= MAX_DEFENSE_POSITIONS)
    {
        I_Error("P_SpawnDefensePosition: Too many defense positions (max %d)", 
                MAX_DEFENSE_POSITIONS);
        return;
    }

    defense = &defense_positions[num_defense_positions];

    subtype = mthing->type - DEFENSE_POSITION_BASE;

    defense->position = *mthing;
    defense->is_active = true;

    if (subtype >= 10 && subtype < 20)
    {
        defense->defense_radius = DEFENSE_RADIUS_LARGE;
        defense->defense_strength = 3;
    }
    else if (subtype >= 20 && subtype < 30)
    {
        defense->defense_radius = DEFENSE_RADIUS_DEFAULT;
        defense->defense_strength = 2;
    }
    else
    {
        defense->defense_radius = DEFENSE_RADIUS_DEFAULT;
        defense->defense_strength = 1;
    }

    num_defense_positions++;
    defense_active = true;
}

void P_ShutdownDefensePositions(void)
{
    int i;

    for (i = 0; i < MAX_DEFENSE_POSITIONS; i++)
    {
        defense_positions[i].is_active = false;
    }

    num_defense_positions = 0;
    defense_active = false;
}

defense_position_t* P_GetNearestDefensePosition(mobj_t* actor)
{
    defense_position_t* nearest = NULL;
    fixed_t nearest_dist = 0x7FFFFFFF;
    fixed_t x, y;
    int i;

    if (!actor)
        return NULL;

    x = actor->x;
    y = actor->y;

    for (i = 0; i < num_defense_positions; i++)
    {
        defense_position_t* defense = &defense_positions[i];
        fixed_t dx, dy, dist;
        fixed_t def_x, def_y;

        if (!defense->is_active)
            continue;

        def_x = defense->position.x << FRACBITS;
        def_y = defense->position.y << FRACBITS;

        dx = def_x - x;
        dy = def_y - y;
        dist = P_AproxDistance(dx, dy);

        if (dist < defense->defense_radius && dist < nearest_dist)
        {
            nearest_dist = dist;
            nearest = defense;
        }
    }

    return nearest;
}

boolean P_IsWithinDefensePosition(mobj_t* actor, defense_position_t* defense)
{
    fixed_t dx, dy, dist;

    if (!actor || !defense || !defense->is_active)
        return false;

    dx = (defense->position.x << FRACBITS) - actor->x;
    dy = (defense->position.y << FRACBITS) - actor->y;
    dist = P_AproxDistance(dx, dy);

    return dist <= defense->defense_radius;
}

int P_GetDefenseBonus(mobj_t* actor)
{
    defense_position_t* defense;
    fixed_t dist_to_center;
    fixed_t dx, dy;
    int bonus_pct;

    if (!actor)
        return 0;

    if (P_GetFaction(actor->type) != FACTION_DWARF)
        return 0;

    defense = P_GetNearestDefensePosition(actor);
    if (!defense)
        return 0;

    if (!P_IsWithinDefensePosition(actor, defense))
        return 0;

    dx = (defense->position.x << FRACBITS) - actor->x;
    dy = (defense->position.y << FRACBITS) - actor->y;
    dist_to_center = P_AproxDistance(dx, dy);

    if (dist_to_center <= DEFENSE_BONUS_MAX_DISTANCE)
    {
        bonus_pct = defense->defense_strength * DEFENSE_BONUS_DAMAGE;
    }
    else
    {
        fixed_t radius = defense->defense_radius;
        fixed_t normalized = (radius - dist_to_center) / radius;
        bonus_pct = (normalized * defense->defense_strength * DEFENSE_BONUS_DAMAGE) / FRACUNIT;
    }

    return bonus_pct;
}

boolean P_SiegeDefenseActive(void)
{
    return defense_active;
}
