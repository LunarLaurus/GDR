//
// Copyright(C) 2024 Glenn Rumsey
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
//  Enemy definition templates for Goblin Dice Rollaz.
//  Provides base structures for common enemy types to simplify
//  enemy expansion and ensure consistency.
//

#ifndef ENEMY_TEMPLATES_H
#define ENEMY_TEMPLATES_H

#include "m_fixed.h"
#include "info.h"

#define DWARF_STATS_DEFAULT(doomednum, health, speed_val, painchance_val, damage_val) \
    doomednum, \
    S_SPOS_STND, \
    health, \
    S_SPOS_RUN1, \
    sfx_posit2, \
    8, \
    sfx_pistol, \
    S_SPOS_PAIN, \
    painchance_val, \
    sfx_popain, \
    0, \
    S_SPOS_ATK1, \
    S_SPOS_DIE1, \
    S_SPOS_XDIE1, \
    sfx_podth2, \
    speed_val, \
    20*FRACUNIT, \
    56*FRACUNIT, \
    100, \
    damage_val, \
    sfx_posact, \
    MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL, \
    S_SPOS_RAISE1

#define DWARF_RANGED_STATS(doomednum, health, speed_val, painchance_val) \
    DWARF_STATS_DEFAULT(doomednum, health, speed_val, painchance_val, 0)

#define DWARF_MELEE_STATS(doomednum, health, speed_val, painchance_val, damage_val) \
    DWARF_STATS_DEFAULT(doomednum, health, speed_val, painchance_val, damage_val)

#define GOBLIN_STATS_DEFAULT(doomednum, health, speed_val, painchance_val) \
    doomednum, \
    S_POSS_STND, \
    health, \
    S_POSS_RUN1, \
    sfx_posit1, \
    8, \
    0, \
    S_POSS_PAIN, \
    painchance_val, \
    sfx_popain, \
    0, \
    S_POSS_ATK1, \
    S_POSS_DIE1, \
    S_POSS_XDIE1, \
    sfx_podth1, \
    speed_val, \
    16*FRACUNIT, \
    44*FRACUNIT, \
    60, \
    0, \
    sfx_posact, \
    MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL, \
    S_POSS_RAISE1

#define GOBLIN_SCOUT_STATS(doomednum, health, speed_val) \
    GOBLIN_STATS_DEFAULT(doomednum, health, speed_val, 200)

#define GOBLIN_CASTER_STATS(doomednum, health, speed_val, painchance_val) \
    GOBLIN_STATS_DEFAULT(doomednum, health, speed_val, painchance_val)

typedef struct {
    int spawnhealth;
    int speed;
    int painchance;
    int radius_val;
    int height_val;
    int mass_val;
    int damage;
} enemy_base_stats_t;

typedef struct {
    enemy_base_stats_t base;
    int crit_resistance;
    int reaction_time;
} dwarf_base_stats_t;

typedef struct {
    enemy_base_stats_t base;
    int ambush_behavior;
    int flank_bonus;
} goblin_base_stats_t;

static const dwarf_base_stats_t dwarf_standard = {
    {60, 8, 100, 20*FRACUNIT, 56*FRACUNIT, 100, 0},
    10,
    8
};

static const dwarf_base_stats_t dwarf_heavy = {
    {120, 4, 50, 24*FRACUNIT, 64*FRACUNIT, 200, 0},
    25,
    12
};

static const dwarf_base_stats_t dwarf_elite = {
    {200, 6, 30, 22*FRACUNIT, 60*FRACUNIT, 150, 0},
    20,
    6
};

static const dwarf_base_stats_t dwarf_marksman = {
    {50, 2, 80, 18*FRACUNIT, 56*FRACUNIT, 80, 0},
    20,
    12
};

static const dwarf_base_stats_t dwarf_captain = {
    {150, 5, 40, 20*FRACUNIT, 60*FRACUNIT, 200, 0},
    30,
    8
};

static const goblin_base_stats_t goblin_standard = {
    {20, 8, 200, 16*FRACUNIT, 44*FRACUNIT, 60, 0},
    0,
    5
};

static const goblin_base_stats_t goblin_scout = {
    {15, 12, 250, 14*FRACUNIT, 40*FRACUNIT, 40, 0},
    1,
    15
};

static const goblin_base_stats_t goblin_caster = {
    {35, 6, 150, 16*FRACUNIT, 44*FRACUNIT, 80, 0},
    0,
    8
};

static const goblin_base_stats_t goblin_elite = {
    {80, 7, 100, 18*FRACUNIT, 52*FRACUNIT, 100, 0},
    1,
    10
};

#endif
