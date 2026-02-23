//
// Copyright(C) 2026 Goblin Dice Rollaz
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
// DESCRIPTION:  Client-side prediction system for dice roll results
//

#ifndef __DICE_PREDICT_H__
#define __DICE_PREDICT_H__

#include "doomdef.h"

#define MAX_PREDICTED_DAMAGE 64

typedef struct predicted_damage_s {
    int id;
    int target_x;
    int target_y;
    int predicted_damage;
    int actual_damage;
    boolean predicted_critical;
    boolean actual_critical;
    int predicted_crit_roll;
    int actual_crit_roll;
    int weapon;
    int tic;
    boolean pending;
    boolean validated;
    boolean desynced;
} predicted_damage_t;

void PREDICT_Init(void);

int PREDICT_RecordDamage(int x, int y, int damage, boolean critical, 
                         int crit_roll, int weapon);

void PREDICT_ValidateDamage(int predict_id, int actual_damage, 
                            boolean actual_critical, int actual_crit_roll);

void PREDICT_Clear(void);

int PREDICT_GetPendingCount(void);

int PREDICT_GetDesyncCount(void);

void PREDICT_Ticker(void);

// Server-authoritative damage validation
int PREDICT_CalculateServerDamage(int weapon, player_t *player, mobj_t *target);
void PREDICT_ValidateServerDamage(int predict_id, int weapon, player_t *player, mobj_t *target);

#endif
