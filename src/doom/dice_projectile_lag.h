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
// DESCRIPTION:  Lag compensation system for projectile hits
//

#ifndef DICE_PROJECTILE_LAG_H
#define DICE_PROJECTILE_LAG_H

#include "doomdef.h"
#include "p_mobj.h"

#define MAX_PROJECTILE_RECORDS 256
#define LAG_COMPENSATION_TICS 8

typedef struct {
    int id;
    mobj_t *projectile;
    mobj_t *target;
    fixed_t spawn_x, spawn_y, spawn_z;
    fixed_t target_x, target_y;
    angle_t angle;
    fixed_t speed;
    int spawn_tic;
    int weapon;
    boolean hit_registered;
    boolean validated;
} projectile_record_t;

extern int sv_lag_compensation;
extern int lag_compensation_window;

void PLAG_Init(void);
int PLAG_RecordProjectile(mobj_t *proj, mobj_t *target, int weapon);
void PLAG_MarkHit(int record_id);
void PLAG_Clear(void);
void PLAG_Ticker(void);

boolean PLAG_ValidateHit(mobj_t *projectile, mobj_t *target);

void PLAG_NetArchivePlayers(void);

#endif
