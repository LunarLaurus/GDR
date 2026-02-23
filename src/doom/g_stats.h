//
// Copyright(C) 2024 Goblin Dice Rollaz
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
//     Dice roll statistics tracking system for Goblin Dice Rollaz.
//     Tracks roll counts, critical hits, damage dealt, and other
//     metrics for each dice weapon.
//

#ifndef __G_STATS__
#define __G_STATS__

#include "doomdef.h"

#define MAX_DICE_WEAPONS 16

typedef struct {
    int weapon;
    int die_type;
    int total_rolls;
    int crit_rolls;
    int misfire_rolls;
    int total_damage;
    int highest_roll;
    int highest_damage;
    int lowest_damage;
    unsigned long long total_roll_sum;
} dice_weapon_stats_t;

typedef struct {
    int total_dice_rolls;
    int total_crits;
    int total_misfires;
    int total_damage_dealt;
    int highest_single_hit;
    int highest_roll_achieved;
    int kills;
    int deaths;
} global_dice_stats_t;

extern global_dice_stats_t global_dice_stats;
extern dice_weapon_stats_t weapon_stats[MAX_DICE_WEAPONS];

void G_ResetDiceStats(void);
void G_TrackDiceRoll(int weapon, int die_type, int roll, int damage, boolean is_crit, boolean is_misfire);
void G_TrackKill(int killer_weapon);
void G_TrackDeath(void);
void G_PrintDiceStats(void);
void G_PrintWeaponStats(int weapon);
const char* G_GetWeaponName(int weapon);

#endif
