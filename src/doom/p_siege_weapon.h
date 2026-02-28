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
//  Siege weapon interaction system - allows players to mount and use
//  stationary siege weapons like cannons, ballistas, and turrets.
//

#ifndef __P_SIEGE_WEAPON__
#define __P_SIEGE_WEAPON__

#include "doomdef.h"

#define MAX_SIEGE_WEAPONS 32

typedef struct
{
    mobj_t* weapon_mobj;
    int fire_cooldown;
    int ammo_type;
    int ammo_per_shot;
    int damage;
    int damage_modifier;
    int missile_type;
    fixed_t missile_speed;
    angle_t min_angle;
    angle_t max_angle;
    angle_t current_angle;
    boolean player_mounted;
} siege_weapon_t;

void P_InitSiegeWeapons(void);
void P_ShutdownSiegeWeapons(void);
void P_SpawnSiegeWeapon(fixed_t x, fixed_t y, angle_t angle, int siege_type);
boolean P_TryMountSiegeWeapon(player_t* player);
void P_DismountSiegeWeapon(player_t* player);
void P_UpdateSiegeWeapons(void);
boolean P_CanPlayerUseSiegeWeapon(player_t* player);
void P_FireSiegeWeapon(player_t* player);
siege_weapon_t* P_GetSiegeWeaponAtPosition(fixed_t x, fixed_t y);

#endif
