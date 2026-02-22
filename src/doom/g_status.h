//
// Copyright(C) 2024 Goblin Dice Rollaz
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
//     Status effect framework for Goblin Dice Rollaz.
//     Provides duration-based status effects that can be applied
//     to players and monsters (burning, frozen, stunned, etc).
//

#ifndef __G_STATUS__
#define __G_STATUS__

#include "doomdef.h"

extern status_info_t status_effects[NUMSTATUSEFFECTS];

extern int burn_damage;
extern int burn_tics;
extern int frozen_speed_mul;
extern int frozen_tics;
extern int stunned_tics;
extern int dice_curse_tics;
extern int dice_curse_variance_min;
extern int dice_curse_variance_max;
extern int enraged_tics;
extern int enraged_damage_mult;

boolean G_StatusEffectIsActive(mobj_t* target, int effect_id);
int     G_StatusEffectTimeRemaining(mobj_t* target, int effect_id);
void    G_StatusEffectApply(mobj_t* target, int effect_id, int duration);
void    G_StatusEffectRemove(mobj_t* target, int effect_id);
void    G_StatusEffectRemoveAll(mobj_t* target);
int     G_StatusEffectGetMoveSpeedMultiplier(mobj_t* target);
boolean G_StatusEffectCanAttack(mobj_t* target);
void    G_StatusEffectTick(mobj_t* target);
boolean G_StatusEffectDamageTarget(mobj_t* target, int damage);
int     G_GetDiceCurseDamageMultiplier(mobj_t* target);
int     G_GetStatusEffectDamageMultiplier(mobj_t* target);
void    G_UpdateStatusEffectInfo(void);

#define STATUS(c) status_effects[c]

#endif
