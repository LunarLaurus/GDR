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
//  Faction AI system for coordinated enemy behavior.
//  Handles faction assignment, tactics, pack behavior, and morale.
//

#ifndef P_AI_FACTION_H
#define P_AI_FACTION_H

#include "doomdef.h"
#include "p_local.h"

typedef enum
{
    FACTION_NONE,
    FACTION_GOBLIN,
    FACTION_DWARF
} faction_t;

typedef enum
{
    TACTIC_DEFAULT,
    TACTIC_FLANK,
    TACTIC_HOLD,
    TACTIC_SHIELD_WALL,
    TACTIC_WEDGE,
    TACTIC_LINE
} tactic_t;

typedef enum
{
    FORMATION_NONE,
    FORMATION_SHIELD_WALL,
    FORMATION_WEDGE,
    FORMATION_LINE,
    FORMATION_CIRCLE
} formation_t;

#define MAX_FORMATION_MEMBERS 16

faction_t P_GetFaction(mobjtype_t type);
tactic_t P_GetTactic(mobjtype_t type);
boolean P_CanBeLeader(mobjtype_t type);
boolean P_CanFollow(mobjtype_t type);

int P_GetNearbyAllyCount(mobj_t *actor);
int P_GetPackAggression(mobj_t *actor);
fixed_t P_GetPackRetreatThreshold(mobj_t *actor);
int P_GetCritAuraBonus(mobj_t *actor);

void P_InitMorale(mobj_t *actor);
void P_UpdateMorale(mobj_t *actor);
boolean P_MoraleBroken(mobj_t *actor);
void P_InitLeader(mobj_t *actor);

formation_t P_GetFormation(mobj_t *actor);
void P_SetFormation(mobj_t *actor, formation_t form);
fixed_t P_GetFormationSlot(mobj_t *actor, int slot);
void P_UpdateFormation(mobj_t *actor);
boolean P_IsInShieldWall(mobj_t *actor);
mobj_t *P_GetFormationLeader(mobj_t *actor);
void P_AlertNearbyAllies(mobj_t *actor, mobj_t *source);

void P_InitPursuitStamina(mobj_t *actor);
void P_UpdatePursuitStamina(mobj_t *actor);
boolean P_IsPursuitExhausted(mobj_t *actor);
void P_ReturnToSpawn(mobj_t *actor);

#endif
