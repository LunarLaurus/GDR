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
//	Items: key cards, artifacts, weapon, ammunition.
//


#ifndef __D_ITEMS__
#define __D_ITEMS__

#include "doomdef.h"

struct player_s;
typedef struct player_s player_t;



// Weapon info: sprite frames, ammunition use.
typedef struct
{
    ammotype_t	ammo;
    int		upstate;
    int		downstate;
    int		readystate;
    int		atkstate;
    int		flashstate;

} weaponinfo_t;

// Goblin Dice Rollaz: Spawn flags for map balancing
// Used to control weapon appearance in different game contexts
typedef enum
{
    SPF_NONE = 0,
    SPF_RARE = 1,           // Rare spawn, reduced frequency
    SPF_EARLY_GAME = 2,    // Only spawn in early game (MAP01-MAP08)
    SPF_LATE_GAME = 4,     // Only spawn in late game (MAP24+)
    SPF_BOSS_AREA = 8,     // Boss arena only
    SPF_ALWAYS_SPAWN = 16, // Always spawn regardless of flags
    SPF_HIDDEN = 32,       // Hidden weapon, requires secret/trigger
    SPF_DIFFICULTY_EASY = 64,    // Easy mode only
    SPF_DIFFICULTY_HARD = 128,   // Hard mode bonus
} weapon_spawn_flags_t;

// Goblin Dice Rollaz: Damage type identifiers
typedef enum
{
    DAMAGETYPE_NORMAL = 0,
    DAMAGETYPE_FIRE,
    DAMAGETYPE_ICE,
    DAMAGETYPE_PIERCING,
    DAMAGETYPE_EXPLOSIVE,
    DAMAGETYPE_CURSE,
    DAMAGETYPE_MAX
} damage_type_t;

// Goblin Dice Rollaz: Crit scaling curve types
typedef enum
{
    CRIT_SCALING_LINEAR = 0,      // Simple multiplier (base behavior)
    CRIT_SCALING_EXPONENTIAL,      // Multiplier increases with roll value
    CRIT_SCALING_BONUS_FLAT,       // Flat bonus added to base damage
    CRIT_SCALING_BONUS_PERCENT,    // Percentage bonus based on roll
    CRIT_SCALING_CRIT_CHANCE,      // Crit chance affects multiplier
    CRIT_SCALING_MAX               // Number of scaling types
} crit_scaling_type_t;

// Goblin Dice Rollaz: Dice weapon configuration
typedef struct
{
    int die_type;           // Number of sides (4, 6, 10, 12, 20, 100)
    int crit_chance;        // Base crit chance (percent)
    int crit_multiplier;    // Damage multiplier on crit
    int min_damage;         // Minimum roll that deals damage
    int crit_roll;          // Roll value that triggers crit
    int damage_table[7];    // Damage mapping for roll ranges
    int gamble_shot;        // Wide variance mode (exploding rolls, misfire chance)
    int ricochet_bounces;   // Number of ricochets allowed (0 = no ricochet)
    int misfire_roll;       // Roll at or below this triggers misfire (0 = no misfire)
    int misfire_penalty;    // Damage multiplier on misfire (e.g., 25 = quarter damage)
    weapon_spawn_flags_t spawn_flags;  // Map balancing flags
    int spawn_weight;       // Relative spawn probability (higher = more common)
    crit_scaling_type_t crit_scaling_type;   // Type of scaling curve
    int crit_scaling_param; // Parameter for scaling curve (e.g., bonus damage, exponent)
    damage_type_t damage_type;  // Damage type for HUD indicator
} dice_weapon_info_t;

extern  weaponinfo_t    weaponinfo[NUMWEAPONS];
extern  dice_weapon_info_t dice_weapon_info[NUMWEAPONS];

// Dice roll backend functions
int P_RollDice(int sides);
int P_CalculateDiceDamage(int weapon, int guaranteedCrit, int *outCritRoll, int *outMisfire, int *outDiceRoll, player_t* player);
int P_WeaponCanCrit(int weapon);

// Weapon spawn flag functions for map balancing
weapon_spawn_flags_t P_GetWeaponSpawnFlags(int weapon);
int P_GetWeaponSpawnWeight(int weapon);
int P_CanWeaponSpawn(int weapon, int mapNumber, int difficulty);

// Crit scaling functions
int P_ApplyCritScaling(int baseDamage, int critRoll, int dieType, crit_scaling_type_t scalingType, int scalingParam);

#endif
