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
//      Weapon balance tuning constants for Goblin Dice Rollaz.
//      Modify these values to adjust game balance without code changes.
//

#ifndef __G_BALANCE__
#define __G_BALANCE__

// ============================================================================
// CRITICAL HIT SYSTEM
// ============================================================================

// Default crit settings (applied when weapon doesn't specify)
#define DEFAULT_CRIT_CHANCE        10      // Base crit chance in percent (d20 roll of 2+)
#define DEFAULT_CRIT_MULTIPLIER    2       // Damage multiplier on crit hit
#define DEFAULT_MIN_DAMAGE_CAP    1       // Minimum damage floor (prevents 0-damage rolls)

// Crit scaling configuration
#define DEFAULT_CRIT_SCALING_TYPE      CRIT_SCALING_LINEAR  // 0=linear, 1=exp, 2=flat, 3=pct, 4=chance
#define DEFAULT_CRIT_SCALING_PARAM      2       // Parameter for scaling curve
#define CRIT_CHANCE_SCALE              10      // Denominator for crit chance multiplier formula

// Crit combo system
#define DEFAULT_CRIT_COMBO_TIMEOUT     (TICRATE * 3)    // Tics before combo resets
#define DEFAULT_CRIT_COMBO_BONUS        15      // Bonus crit chance per combo hit
#define DEFAULT_CRIT_COMBO_MAX          4       // Maximum combo multiplier

// ============================================================================
// PARRY DEFENSE SYSTEM
// ============================================================================

// Parry window duration after attacking (in tics)
// Player has this many tics after an attack to parry incoming damage
#define PARRY_WINDOW_DURATION      (TICRATE / 5)   // ~0.2 seconds (7 tics at 35fps)

// Optimal parry window (the "sweet spot" in the parry window)
// If hit during this window, parry is most effective
#define PARRY_OPTIMAL_WINDOW       (TICRATE / 10)  // ~0.1 seconds (3-4 tics)

// Parry cooldown between successful parries (prevents spam)
#define PARRY_COOLDOWN             (TICRATE / 2)   // 0.5 seconds

// Damage reduction when parrying (percent of original damage)
#define PARRY_DAMAGE_REDUCTION     75      // Take only 25% damage when parrying

// Bonus damage reduction at optimal timing (percent)
#define PARRY_OPTIMAL_REDUCTION    90      // Take only 10% damage at perfect timing

// Parry combo bonus - each consecutive parry increases reduction slightly
#define PARRY_COMBO_BONUS          10      // Extra % reduction per consecutive parry
#define PARRY_COMBO_MAX            5       // Max combo bonus

// Chance to auto-parry without button press (for passive timing window)
// This gives players a chance even if they don't press anything
#define PARRY_AUTO_CHANCE          15      // 15% chance to auto-parry in optimal window

// ============================================================================
// POWERUP BALANCE
// ============================================================================

// Critical Hit Powerup
#define CRIT_POWERUP_DURATION      (TICRATE * 30)   // 30 seconds at 35 tics/sec
#define CRIT_POWERUP_BONUS          15              // Additional crit chance %

// Double Damage Powerup
#define DOUBLE_DAMAGE_DURATION      (TICRATE * 20)  // 20 seconds
#define DOUBLE_DAMAGE_MULTIPLIER   2               // 2x damage

// Dice Fortune Powerup (guaranteed next hit crit)
#define FORTUNE_DURATION            (TICRATE * 45)  // 45 seconds to use guaranteed crit

// Powerup spawn rates (percent chance per map)
#define POWERUP_SPAWN_RATE_EASY     8
#define POWERUP_SPAWN_RATE_NORMAL   5
#define POWERUP_SPAWN_RATE_HARD     3

// ============================================================================
// WEAPON DAMAGE TABLES
// ============================================================================

// Damage bucket thresholds for d6 blaster (roll 1-6)
// Buckets: 1, 2, 3, 4, 5, 6, crit
#define D6_BLAST_DAMAGE_1       1
#define D6_BLAST_DAMAGE_2       2
#define D6_BLAST_DAMAGE_3       3
#define D6_BLAST_DAMAGE_4       4
#define D6_BLAST_DAMAGE_5       5
#define D6_BLAST_DAMAGE_6       6
#define D6_BLAST_CRIT_DAMAGE    12      // 6 * 2x multiplier

// d20 cannon damage
#define D20_CANNON_DAMAGE_1     8
#define D20_CANNON_DAMAGE_2     10
#define D20_CANNON_DAMAGE_3     12
#define D20_CANNON_DAMAGE_4     14
#define D20_CANNON_DAMAGE_5     16
#define D20_CANNON_DAMAGE_6     18
#define D20_CANNON_DAMAGE_7     20
#define D20_CANNON_DAMAGE_8     22
#define D20_CANNON_DAMAGE_9     24
#define D20_CANNON_DAMAGE_10    26
#define D20_CANNON_DAMAGE_11    28
#define D20_CANNON_DAMAGE_12    30
#define D20_CANNON_DAMAGE_13    32
#define D20_CANNON_DAMAGE_14    34
#define D20_CANNON_DAMAGE_15    36
#define D20_CANNON_DAMAGE_16    38
#define D20_CANNON_DAMAGE_17    40
#define D20_CANNON_DAMAGE_18    42
#define D20_CANNON_DAMAGE_19    44
#define D20_CANNON_DAMAGE_20    50      // 20 * 2x + bonus

// d4 throwing knives damage
#define D4_DAMAGE_1             1
#define D4_DAMAGE_2             2
#define D4_DAMAGE_3             3
#define D4_DAMAGE_4             4

// d8 balanced weapon damage
#define D8_DAMAGE_1             2
#define D8_DAMAGE_2             3
#define D8_DAMAGE_3             4
#define D8_DAMAGE_4             5
#define D8_DAMAGE_5             6
#define D8_DAMAGE_6             7
#define D8_DAMAGE_7             8
#define D8_DAMAGE_8             10

// d10 ricochet damage
#define D10_DAMAGE_1            4
#define D10_DAMAGE_2            5
#define D10_DAMAGE_3            6
#define D10_DAMAGE_4            7
#define D10_DAMAGE_5            8
#define D10_DAMAGE_6            9
#define D10_DAMAGE_7            10
#define D10_DAMAGE_8            11
#define D10_DAMAGE_9            12
#define D10_DAMAGE_10           15

// d12 heavy impact damage
#define D12_DAMAGE_1            6
#define D12_DAMAGE_2            7
#define D12_DAMAGE_3            8
#define D12_DAMAGE_4            9
#define D12_DAMAGE_5            10
#define D12_DAMAGE_6            11
#define D12_DAMAGE_7            12
#define D12_DAMAGE_8            13
#define D12_DAMAGE_9            14
#define D12_DAMAGE_10           15
#define D12_DAMAGE_11           16
#define D12_DAMAGE_12           20

// Percentile (d100) gamble shot damage
#define PERCENTILE_DAMAGE_1     1
#define PERCENTILE_DAMAGE_2     2
#define PERCENTILE_DAMAGE_3     3
#define PERCENTILE_DAMAGE_25    5
#define PERCENTILE_DAMAGE_50    10
#define PERCENTILE_DAMAGE_75    15
#define PERCENTILE_DAMAGE_100   50      // Explosive crit

// Twin d6 scatter damage (per die)
#define TWIND6_DAMAGE_1         1
#define TWIND6_DAMAGE_2         2
#define TWIND6_DAMAGE_3         3
#define TWIND6_DAMAGE_4         4
#define TWIND6_DAMAGE_5         5
#define TWIND6_DAMAGE_6         6

// Arcane d20 beam damage (per tick)
#define ARCANE_D20_DAMAGE_MIN   1
#define ARCANE_D20_DAMAGE_MAX   3

// Cursed die damage
#define CURSED_DAMAGE_MIN       8
#define CURSED_DAMAGE_MAX       24
#define CURSED_SELF_DAMAGE      5       // Damage to self on misfire

// ============================================================================
// WEAPON FIRING RATES (in tics)
// ============================================================================

#define FIRE_RATE_FIST           8
#define FIRE_RATE_D6_BLAST       10
#define FIRE_RATE_D20_CANNON    30
#define FIRE_RATE_D4            6       // Rapid fire
#define FIRE_RATE_D8            12
#define FIRE_RATE_D10           15
#define FIRE_RATE_D12           25      // Slow, heavy
#define FIRE_RATE_PERCENTILE    35
#define FIRE_RATE_TWIND6        8
#define FIRE_RATE_ARCANE_BEAM   3       // Fast tick rate
#define FIRE_RATE_CURSED        20
#define FIRE_RATE_SHOTGUN       50
#define FIRE_RATE_CHAINGUN      10
#define FIRE_RATE_MISSILE       50
#define FIRE_RATE_PLASMA        8
#define FIRE_RATE_BFG           100
#define FIRE_RATE_CHAINSAW      10

// ============================================================================
// AMMO CONFIGURATION
// ============================================================================

// Ammo per clip
#define AMMO_CLIP_SIZE          50
#define AMMO_SHELL_SIZE         8
#define AMMO_CELL_SIZE          40
#define AMMO_MISSILE_SIZE       2
#define AMMO_LIGHT_DICE_SIZE    30
#define AMMO_HEAVY_DICE_SIZE    15
#define AMMO_ARCANE_DICE_SIZE   20

// Ammo pickup amounts
#define AMMO_PICKUP_CLIP        10
#define AMMO_PICKUP_SHELL       4
#define AMMO_PICKUP_CELL        20
#define AMMO_PICKUP_MISSILE     1
#define AMMO_PICKUP_LIGHT_DICE  10
#define AMMO_PICKUP_HEAVY_DICE  5
#define AMMO_PICKUP_ARCANE_DICE 8

// ============================================================================
// ENEMY BALANCE
// ============================================================================

// Enemy HP by difficulty (percentage of base)
#define ENEMY_HP_EASY           75
#define ENEMY_HP_NORMAL        100
#define ENEMY_HP_HARD          150

// Enemy damage by difficulty
#define ENEMY_DMG_EASY          75
#define ENEMY_DMG_NORMAL        100
#define ENEMY_DMG_HARD          125

// Critical hit resistance (reduces crit chance by this percent)
#define CRIT_RESISTANCE_NONE    0
#define CRIT_RESISTANCE_LOW     5
#define CRIT_RESISTANCE_MEDIUM 10
#define CRIT_RESISTANCE_HIGH    20
#define CRIT_RESISTANCE_IMMUNE 100

// Enemy spawn weights for map generation
#define SPAWN_WEIGHT_IMP        100
#define SPAWN_WEIGHT_DEMON      50
#define SPAWN_WEIGHT_CACODEMON  30
#define SPAWN_WEIGHT_BARREL     20
#define SPAWN_WEIGHT_GOBLIN     80
#define SPAWN_WEIGHT_DWARF      70
#define SPAWN_WEIGHT_DWARF_BERSERKER  30
#define SPAWN_WEIGHT_DWARF_ENGINEER   25
#define SPAWN_WEIGHT_GOBLIN_SHAMAN     15

// ============================================================================
// GAME MODES
// ============================================================================

// Challenge mode multipliers
#define CHALLENGE_CRIT_ONLY_MULTIPLIER    1   // All hits crit
#define CHALLENGE_NO_POWERUPS_MULTIPLIER  1   // No powerups
#define CHALLENGE_DAMAGE_REDUCTION       50 // Percent

// Survival mode
#define SURVIVAL_WAVE_SPAWN_DELAY        (TICRATE * 5)   // Between spawns
#define SURVIVAL_WAVE_SIZE_INCREASE      2   // Extra enemies per wave

// ============================================================================
// UI/TUNING
// ============================================================================

// Damage number display
#define DAMAGE_NUMBER_LIFETIME    (TICRATE)        // 1 second
#define DAMAGE_NUMBER_FADE_START  (DAMAGE_NUMBER_LIFETIME - 10)
#define DAMAGE_NUMBER_MAX_DISPLAY 10  // Max damage numbers on screen

// Screen shake
#define SCREEN_SHAKE_MIN_ROLL     4       // Roll amount for min shake
#define SCREEN_SHAKE_CRIT_BONUS   2       // Additional roll per crit tier
#define DEFAULT_SHAKE_SCALE       1.0f    // Default screen shake intensity
#define SHAKE_LOW                 0.5f    // Minimal shake intensity
#define SHAKE_MEDIUM              1.0f    // Normal shake intensity
#define SHAKE_HIGH                2.0f    // Maximum shake intensity
#define DEFAULT_DAMAGE_SCALE      1.0f    // Default damage number scale

// ============================================================================
// DEBUG / DEVELOPMENT
// ============================================================================

#ifdef DEVELOPMENT
#define DEBUG_WEAPON_STATS       1       // Show weapon overlay
#define DEBUG_DAMAGE_LOG         1       // Log all damage
#define DEBUG_SPAWN_INFO         1       // Log enemy spawns
#define DEBUG_POWERUP_TIMERS     1       // Show powerup timers
#else
#define DEBUG_WEAPON_STATS       0
#define DEBUG_DAMAGE_LOG         0
#define DEBUG_SPAWN_INFO         0
#define DEBUG_POWERUP_TIMERS     0
#endif

// ============================================================================
// CROSSHAIR SYSTEM
// ============================================================================

// Crosshair types
#define CROSSHAIR_NONE           0
#define CROSSHAIR_DOT            1
#define CROSSHAIR_CROSS          2
#define CROSSHAIR_CIRCLE         3
#define CROSSHAIR_DIAMOND        4
#define CROSSHAIR_TRIANGLE       5
#define CROSSHAIR_DICE_DOTS      6       // Dice-themed: dots for d6 face
#define CROSSHAIR_MAX            6

// Crosshair colors
#define CROSSHAIR_COLOR_DEFAULT  0       // White
#define CROSSHAIR_COLOR_RED      1       // Red (classic)
#define CROSSHAIR_COLOR_GREEN    2       // Green
#define CROSSHAIR_COLOR_YELLOW   3       // Yellow
#define CROSSHAIR_COLOR_CYAN     4       // Cyan

#endif
