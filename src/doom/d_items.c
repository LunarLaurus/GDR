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
//


// We are referring to sprite numbers.
#include "info.h"

#include "d_items.h"


//
// PSPRITE ACTIONS for waepons.
// This struct controls the weapon animations.
//
// Each entry is:
//   ammo/amunition type
//  upstate
//  downstate
// readystate
// atkstate, i.e. attack/fire/hit frame
// flashstate, muzzle flash
//
weaponinfo_t	weaponinfo[NUMWEAPONS] =
{
    {
	// fist
	am_noammo,
	S_PUNCHUP,
	S_PUNCHDOWN,
	S_PUNCH,
	S_PUNCH1,
	S_NULL
    },	
    {
	// d6 blaster
	am_lightdice,
	S_D6BLASTUP,
	S_D6BLASTDOWN,
	S_D6BLAST,
	S_D6BLAST1,
	S_D6BLASTFLASH
    },
    {
	// d20 cannon
	am_heavydice,
	S_D20CANNONUP,
	S_D20CANNONDOWN,
	S_D20CANNON,
	S_D20CANNON1,
	S_D20CANNONFLASH
    },
    {
	// d12 heavy impact
	am_heavydice,
	S_D12UP,
	S_D12DOWN,
	S_D12,
	S_D12_1,
	S_D12FLASH
    },
    {
	// shotgun
	am_shell,
	S_SGUNUP,
	S_SGUNDOWN,
	S_SGUN,
	S_SGUN1,
	S_SGUNFLASH1
    },
    {
	// chaingun
	am_clip,
	S_CHAINUP,
	S_CHAINDOWN,
	S_CHAIN,
	S_CHAIN1,
	S_CHAINFLASH1
    },
    {
	// missile launcher
	am_misl,
	S_MISSILEUP,
	S_MISSILEDOWN,
	S_MISSILE,
	S_MISSILE1,
	S_MISSILEFLASH1
    },
    {
	// plasma rifle
	am_cell,
	S_PLASMAUP,
	S_PLASMADOWN,
	S_PLASMA,
	S_PLASMA1,
	S_PLASMAFLASH1
    },
    {
	// bfg 9000
	am_cell,
	S_BFGUP,
	S_BFGDOWN,
	S_BFG,
	S_BFG1,
	S_BFGFLASH1
    },
    {
	// chainsaw
	am_noammo,
	S_SAWUP,
	S_SAWDOWN,
	S_SAW,
	S_SAW1,
	S_NULL
    },
    {
	// super shotgun
	am_shell,
	S_DSGUNUP,
	S_DSGUNDOWN,
	S_DSGUN,
	S_DSGUN1,
	S_DSGUNFLASH1
    },	
    {
	// percentile dice
	am_heavydice,
	S_PERCENTILEUP,
	S_PERCENTILEDOWN,
	S_PERCENTILE,
	S_PERCENTILE1,
	S_PERCENTILEFLASH
    },
    {
	// d4 throwing knives
	am_lightdice,
	S_D4UP,
	S_D4DOWN,
	S_D4,
	S_D4_1,
	S_D4FLASH
    },
};

// Goblin Dice Rollaz: Dice weapon configuration table
// Each entry defines die type, crit modifiers, and damage mapping
// damage_table: indices 0-5 are damage for buckets, index 6 is base damage for crit (multiplied)
dice_weapon_info_t dice_weapon_info[NUMWEAPONS] =
{
    {   // wp_fist - no dice
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_d6blaster - d6
        6,      // die_type: d6
        16,     // crit_chance: 16% (roll of 6)
        2,      // crit_multiplier: 2x on crit
        1,      // min_damage
        6,      // crit_roll: 6
        {1, 1, 2, 2, 3, 5, 5}, 0  // damage table: 1-2=1, 3-4=2, 5=3, 6=crit(5*2=10)
    },
    {   // wp_d20cannon - d20
        20,     // die_type: d20
        5,      // crit_chance: 5% (roll of 20)
        2,      // crit_multiplier: 2x on crit
        5,      // min_damage
        20,     // crit_roll: 20
        {5, 5, 10, 10, 15, 25, 25}, 0  // 1-5=5, 6-10=10, 11-15=15, 16-19=25, 20=crit(25*2=50)
    },
    {   // wp_d12 - d12
        12,     // die_type: d12
        8,      // crit_chance: 8% (roll of 12)
        2,      // crit_multiplier: 2x on crit
        3,      // min_damage
        12,     // crit_roll: 12
        {3, 3, 6, 6, 9, 12, 12}, 0  // 1-3=3, 4-6=6, 7-9=9, 10-12=12, 12=crit(12*2=24)
    },
    {   // wp_shotgun - shotgun (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_chaingun - chaingun (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_missile - missile launcher (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_plasma - plasma rifle (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_bfg - bfg 9000 (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_chainsaw - chainsaw (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_supershotgun - super shotgun (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0
    },
    {   // wp_percentile - d100 percentile (Gamble Shot - wide variance)
        100,    // die_type: d100
        1,      // crit_chance: 1% (roll of 100)
        3,      // crit_multiplier: 3x on crit (higher for gamble)
        1,      // min_damage: 1 (can roll very low)
        100,    // crit_roll: 100
        {1, 3, 5, 15, 35, 75, 100},  // Gamble Shot: 1-15=1, 16-35=3, 36-55=5, 56-75=15, 76-90=35, 91-99=75, 100=crit(100*3=300)
        1       // gamble_shot: wide variance with exploding roll mechanic
    },
    {   // wp_d4 - d4 throwing knives
        4,      // die_type: d4
        25,     // crit_chance: 25% (roll of 4)
        2,      // crit_multiplier: 2x on crit
        1,      // min_damage
        4,      // crit_roll: 4
        {1, 2, 3, 0, 0, 0, 4}, 0  // 1=1, 2=2, 3=3, 4=crit(4*2=8)
    },
};








