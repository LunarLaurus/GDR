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
    {
    	// d8 balanced mid-tier
    	am_lightdice,
    	S_D8UP,
    	S_D8DOWN,
    	S_D8,
    	S_D8_1,
    	S_D8FLASH
    },
    {
    	// d10 ricochet weapon
    	am_heavydice,
    	S_D10UP,
    	S_D10DOWN,
    	S_D10,
    	S_D10_1,
    	S_D10FLASH
    },
    {
    	// twin d6 scatter weapon
    	am_lightdice,
    	S_TWIND6UP,
    	S_TWIND6DOWN,
    	S_TWIND6,
    	S_TWIND6_1,
    	S_TWIND6FLASH
    },
    {
    	// arcane d20 beam weapon
    	am_arcanedice,
    	S_ARCANED20UP,
    	S_ARCANED20DOWN,
    	S_ARCANED20,
    	S_ARCANED20_1,
    	S_ARCANED20FLASH
    },
    {
     	// cursed die weapon (high damage, self-risk)
     	am_heavydice,
     	S_CURSEDDOWN,
     	S_CURSEDUP,
     	S_CURSED,
     	S_CURSED_1,
     	S_CURSEDFLASH
     },
     {
 	// d2 Flip of Fate weapon (binary damage: 50% 1dmg/50% 2dmg)
 	am_lightdice,
 	S_D2DOWN,
 	S_D2UP,
 	S_D2,
 	S_D2_1,
 	S_D2FLASH
     },
};

// Goblin Dice Rollaz: Dice weapon configuration table
// Each entry defines die type, crit modifiers, damage mapping, and spawn flags
// damage_table: indices 0-5 are damage for buckets, index 6 is base damage for crit (multiplied)
dice_weapon_info_t dice_weapon_info[NUMWEAPONS] =
{
    {   // wp_fist - no dice
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_NONE, 0, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL
    },
    {   // wp_d6blaster - d6
        6,      // die_type: d6
        16,     // crit_chance: 16% (roll of 6)
        2,      // crit_multiplier: 2x on crit
        1,      // min_damage
        6,      // crit_roll: 6
        {1, 1, 2, 2, 3, 5, 10}, 0, 0, 0, 0,   // Balanced: 1-2=1, 3-4=2, 5=3, 6=crit(10)
        SPF_ALWAYS_SPAWN, 100, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL  // Common starting weapon
    },
    {   // wp_d20cannon - d20
        20,     // die_type: d20
        5,      // crit_chance: 5% (roll of 20)
        2,      // crit_multiplier: 2x on crit
        5,      // min_damage
        20,     // crit_roll: 20
        {5, 5, 10, 10, 15, 25, 50}, 0, 0, 0, 0,  // Balanced: 1-5=5, 6-10=10, 11-15=15, 16-19=25, 20=crit(50)
        SPF_LATE_GAME, 30, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_EXPLOSIVE  // Heavy weapon, later maps
    },
    {   // wp_d12 - d12
        12,     // die_type: d12
        8,      // crit_chance: 8% (roll of 12)
        2,      // crit_multiplier: 2x on crit
        3,      // min_damage
        12,     // crit_roll: 12
        {3, 3, 6, 6, 9, 12, 24}, 0, 0, 0, 0,  // 1-3=3, 4-6=6, 7-9=9, 10-12=12, 12=crit(24)
        SPF_EARLY_GAME | SPF_RARE, 25, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_PIERCING  // Rare early game weapon
    },
    {   // wp_shotgun - shotgun (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_ALWAYS_SPAWN, 80, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL
    },
    {   // wp_chaingun - chaingun (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_ALWAYS_SPAWN, 50, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL
    },
    {   // wp_missile - missile launcher (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_NONE, 25, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_EXPLOSIVE
    },
    {   // wp_plasma - plasma rifle (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_NONE, 30, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_FIRE
    },
    {   // wp_bfg - bfg 9000 (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_NONE, 15, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_FIRE
    },
    {   // wp_chainsaw - chainsaw (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_NONE, 20, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL
    },
    {   // wp_supershotgun - super shotgun (not dice)
        0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, SPF_NONE, 10, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_PIERCING
    },
    {   // wp_percentile - d100 percentile (Gamble Shot - wide variance)
        100,    // die_type: d100
        1,      // crit_chance: 1% (roll of 100)
        3,      // crit_multiplier: 3x on crit (higher for gamble)
        1,      // min_damage: 1 (can roll very low)
        100,    // crit_roll: 100
        {1, 3, 5, 15, 35, 75, 100},  // Gamble Shot: 1-15=1, 16-35=3, 36-55=5, 56-75=15, 76-90=35, 91-99=75, 100=crit(100*3=300)
        1,      // gamble_shot: wide variance with exploding roll mechanic
        0,      // ricochet_bounces: none
        5,      // misfire_roll: roll 1-5 triggers misfire (5% chance)
        25,     // misfire_penalty: 25% damage on misfire
        SPF_RARE | SPF_LATE_GAME, 20, CRIT_SCALING_EXPONENTIAL, 3, DAMAGETYPE_CURSE  // Rare, high-risk weapon - exponential scaling
    },
    {   // wp_d4 - d4 throwing knives
        4,      // die_type: d4
        25,     // crit_chance: 25% (roll of 4)
        2,      // crit_multiplier: 2x on crit
        1,      // min_damage
        4,      // crit_roll: 4
        {1, 2, 3, 4, 0, 0, 8}, 0, 0, 0, 0,  // Fixed: 1=1, 2=2, 3=3, 4=crit(8), no zeros in main table
        SPF_EARLY_GAME, 60, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_PIERCING  // Fast-firing early game weapon
    },
    {   // wp_d8 - d8 balanced mid-tier
        8,      // die_type: d8
        12,     // crit_chance: 12.5% (roll of 8)
        2,      // crit_multiplier: 2x on crit
        2,      // min_damage
        8,      // crit_roll: 8
        {1, 2, 3, 4, 5, 6, 16}, 0, 0, 0, 0,  // 1=1, 2-3=2, 4=3, 5=4, 6=5, 7=6, 8=crit(16)
        SPF_NONE, 45, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL  // Mid-tier weapon, moderate spawn weight
    },
    {   // wp_d10 - d10 ricochet weapon
        10,     // die_type: d10
        10,     // crit_chance: 10% (roll of 10)
        2,      // crit_multiplier: 2x on crit
        2,      // min_damage
        10,     // crit_roll: 10
        {2, 2, 4, 4, 6, 8, 20},  // 1-2=2, 3-4=4, 5=6, 6-9=8, 10=crit(20)
        0,      // gamble_shot: no
        1,      // ricochet_bounces: 1 bounce
        0, 0,   // No gamble/misfire
        SPF_NONE, 35, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_EXPLOSIVE  // Mid-tier heavy weapon
    },
    {   // wp_twind6 - twin d6 scatter weapon (close-range burst)
        6,      // die_type: d6 (two dice fired)
        16,     // crit_chance: 16% per die (roll of 6 on each die)
        2,      // crit_multiplier: 2x on crit per die
        1,      // min_damage
        6,      // crit_roll: 6
        {1, 1, 2, 2, 3, 5, 10},  // 1-2=1, 3-4=2, 5=3, 6=crit(10)
        0,      // gamble_shot: no
        0,      // ricochet_bounces: no
        0, 0,   // No misfire
        SPF_EARLY_GAME | SPF_RARE, 20, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_PIERCING  // Close-range, early game, rare
    },
    {   // wp_arcaned20 - arcane d20 beam (continuous roll tick damage)
        20,     // die_type: d20
        5,      // crit_chance: 5% (roll of 20)
        2,      // crit_multiplier: 2x on crit
        2,      // min_damage: tick damage
        20,     // crit_roll: 20
        {2, 3, 4, 5, 7, 10, 20},  // Continuous beam: 1-3=2, 4-6=3, 7-10=4, 11-14=5, 15-17=7, 18-19=10, 20=crit(20)
        0,      // gamble_shot: no
        0,      // ricochet_bounces: no
        0, 0,   // No misfire
        SPF_NONE, 25, CRIT_SCALING_BONUS_PERCENT, 2, DAMAGETYPE_ICE  // Arcane mid-tier weapon - percent bonus scaling
    },
    {   // wp_cursed - cursed die (high damage, self-risk mechanic)
        6,      // die_type: d6
        16,     // crit_chance: 16% (roll of 6)
        3,      // crit_multiplier: 3x on crit (higher for cursed)
        2,      // min_damage
        6,      // crit_roll: 6
        {2, 4, 6, 8, 12, 18, 30},  // Cursed: 1=2, 2=4, 3=6, 4=8, 5=12, 6=crit(18*3=54)
        0,      // gamble_shot: no
        0,      // ricochet_bounces: no
        2,      // misfire_roll: roll 1-2 triggers self-damage (33% chance)
        -1,     // misfire_penalty: -1 means self-damage instead of damage penalty
        SPF_RARE | SPF_LATE_GAME, 15, CRIT_SCALING_EXPONENTIAL, 2, DAMAGETYPE_CURSE  // Rare, high-risk weapon - exponential scaling
    },
    {   // wp_d2 - Flip of Fate (binary damage: 50% 1dmg/50% 2dmg)
        2,      // die_type: d2
        50,     // crit_chance: 50% (roll of 2 = crit)
        2,      // crit_multiplier: 2x on crit
        1,      // min_damage
        2,      // crit_roll: 2
        {1, 0, 0, 0, 0, 0, 2},  // Binary: 1=1, 2=crit(2*2=4)
        0,      // gamble_shot: no
        0,      // ricochet_bounces: no
        0, 0,   // No misfire
        SPF_EARLY_GAME, 55, CRIT_SCALING_LINEAR, 2, DAMAGETYPE_NORMAL  // Early game weapon, common spawn
    },
};

// Get weapon spawn flags for map balancing
weapon_spawn_flags_t P_GetWeaponSpawnFlags(int weapon)
{
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return SPF_NONE;
    return dice_weapon_info[weapon].spawn_flags;
}

// Get weapon spawn weight for random spawn selection
int P_GetWeaponSpawnWeight(int weapon)
{
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return 0;
    return dice_weapon_info[weapon].spawn_weight;
}

// Check if a weapon can spawn in the given map and difficulty context
// mapNumber: 1-based map number (e.g., 1 for MAP01)
// difficulty: 0=ITYTD, 1=NOTRR, 2=HMP, 3=UV, 4=NM
int P_CanWeaponSpawn(int weapon, int mapNumber, int difficulty)
{
    weapon_spawn_flags_t flags;

    if (weapon < 0 || weapon >= NUMWEAPONS)
        return 0;

    flags = dice_weapon_info[weapon].spawn_flags;

    // SPF_ALWAYS_SPAWN overrides all other checks
    if (flags & SPF_ALWAYS_SPAWN)
        return 1;

    // SPF_HIDDEN weapons never auto-spawn
    if (flags & SPF_HIDDEN)
        return 0;

    // Check early game flag (maps 1-8)
    if (flags & SPF_EARLY_GAME)
    {
        if (mapNumber > 8)
            return 0;
    }

    // Check late game flag (maps 24+)
    if (flags & SPF_LATE_GAME)
    {
        if (mapNumber < 24)
            return 0;
    }

    // Check difficulty flags
    if (flags & SPF_DIFFICULTY_EASY)
    {
        if (difficulty > 1)  // Only easy/hard
            return 0;
    }

    if (flags & SPF_DIFFICULTY_HARD)
    {
        if (difficulty < 3)  // Only UV and NM
            return 0;
    }

    // Check rare flag - reduces spawn chance (handled in spawn selection logic)
    // This flag is informational; actual probability handled by spawn_weight

    return 1;
}








