# Custom Dice Weapon Template

This document provides a template for adding custom dice weapons to Goblin Dice Rollaz.

## Overview

To add a new dice weapon, you need to modify several files:
- `src/doom/d_items.c` - Weapon definition and damage tables
- `src/doom/d_items.h` - Weapon enum and struct definitions  
- `src/doom/info.c` - Actor/state definitions
- `src/doom/p_pspr.c` - Weapon firing logic
- `src/doom/d_englsh.h` - Weapon name strings

---

## Step 1: Add Weapon Enum

In `src/doom/d_items.h`, add your weapon to the `weapontype_t` enum:

```c
// Add after existing weapons:
wp_custom,           // Custom dice weapon
NUMWEAPONS
```

---

## Step 2: Define Weapon Info

In `src/doom/d_items.c`, add to `weaponinfo[]`:

```c
{
    // Custom Dice Weapon
    am_lightdice,           // ammo type
    S_CUSTOMUP,             // upstate
    S_CUSTOMDOWN,           // downstate
    S_CUSTOM,               // readystate
    S_CUSTOM1,              // atkstate
    S_CUSTOMFLASH           // flashstate
},
```

---

## Step 3: Define Dice Weapon Stats

In `src/doam/d_items.c`, add to `dice_weapon_info[]`:

```c
{   // wp_custom - Custom Dice Weapon Template
    10,     // die_type: d10
    10,     // crit_chance: 10% (roll of 10)
    2,      // crit_multiplier: 2x on crit
    2,      // min_damage
    10,     // crit_roll: 10
    {2, 2, 4, 4, 6, 8, 20},  // damage_table: 1-2=2, 3-4=4, 5=6, 6-9=8, 10=crit(20)
    0,      // gamble_shot: 0 or 1
    0,      // ricochet_bounces: 0 = no ricochet
    0,      // misfire_roll: 0 = no misfire
    0,      // misfire_penalty
    SPF_NONE,  // spawn_flags (SPF_ALWAYS_SPAWN, SPF_RARE, SPF_EARLY_GAME, SPF_LATE_GAME)
    35,     // spawn_weight: relative spawn probability
    CRIT_SCALING_LINEAR,   // crit_scaling_type
    2       // crit_scaling_param
},
```

### Damage Table Format

The 7-element damage table maps roll results:
```
{ bucket0, bucket1, bucket2, bucket3, bucket4, bucket5, crit_base }
```

- Rolls 1-6 map to buckets 0-5
- `crit_base` is the base damage when a critical hit occurs (multiplied by `crit_multiplier`)

### Spawn Flags

| Flag | Description |
|------|-------------|
| `SPF_NONE` | Default behavior |
| `SPF_RARE` | Reduced spawn frequency |
| `SPF_EARLY_GAME` | Only maps 1-8 |
| `SPF_LATE_GAME` | Only maps 24+ |
| `SPF_BOSS_AREA` | Boss arena only |
| `SPF_ALWAYS_SPAWN` | Always spawns |
| `SPF_HIDDEN` | Secret/trigger only |
| `SPF_DIFFICULTY_EASY` | Easy mode only |
| `SPF_DIFFICULTY_HARD` | Hard mode bonus |

### Crit Scaling Types

| Type | Description |
|------|-------------|
| `CRIT_SCALING_LINEAR` | Simple multiplier |
| `CRIT_SCALING_EXPONENTIAL` | Multiplier increases with roll |
| `CRIT_SCALING_BONUS_FLAT` | Flat bonus added |
| `CRIT_SCALING_BONUS_PERCENT` | Percentage bonus |
| `CRIT_SCALING_CRIT_CHANCE` | Multiplier based on crit chance |

---

## Step 4: Add State Definitions

In `src/doom/info.c`, add state definitions for weapon animation:

```c
// Weapon states - add to state table
S_CUSTOMUP,    S_NULL,     0,      0,      0,      S_NULL, 0,  0,  0, 0, 0, 0,
S_CUSTOMDOWN,  S_NULL,     0,      0,      0,      S_NULL, 0,  0,  0, 0, 0, 0,
S_CUSTOM,      S_CUSTOM,   0,      0,      0,      S_NULL, 0,  0,  0, 0, 0, 0,
S_CUSTOM1,    S_CUSTOM2,  6,      0,      0,      S_NULL, 0,  0,  0, 0, 0, 0,
S_CUSTOM2,    S_CUSTOM3,  6,      0,      0,      S_NULL, 0,  0,  0, 0, 0, 0,
S_CUSTOM3,    S_CUSTOM,   6,      A_FireCustom, 0,  S_NULL, 0,  0,  0, 0, 0, 0,
S_CUSTOMFLASH, S_NULL,    5,      0,      0,      S_NULL, 0,  0,  0, 0, 0, 0,
```

Note: States require sprite definitions in your WAD file.

---

## Step 5: Implement Firing Logic

In `src/doom/p_pspr.c`, implement the fire function:

```c
void A_FireCustom(player_t *player, pspdef_t *psp)
{
    // Roll the dice
    int diceRoll = P_RollDice(10);  // d10
    
    // Calculate damage using the dice backend
    int critRoll, misfire, actualRoll;
    int damage = P_CalculateDiceDamage(wp_custom, 0, &critRoll, &misfire, &actualRoll, player);
    
    // Create projectile or apply direct damage
    // ... existing projectile spawning code ...
    
    // Update player stats
    player->ammo[am_lightdice]--;
    player->refire = 0;
    P_SetMobjState(player->mo, S_PLAY_ATK1);
}
```

---

## Step 6: Add Weapon Name String

In `src/doom/d_englsh.h`, add weapon name:

```c
// Add to string tables
#define PW_CUSTOM "Custom Dice Weapon"
```

---

## Example: Creating a "d100 Chaos Die"

Here's a complete example of a high-risk, high-reward weapon:

### d_items.c - Weapon Stats

```c
{   // wp_chaosdie - d100 Chaos Die
    100,    // die_type: d100
    1,      // crit_chance: 1% (roll of 100)
    4,      // crit_multiplier: 4x on crit
    1,      // min_damage: 1
    100,    // crit_roll: 100
    {1, 5, 10, 25, 50, 75, 200},  // Chaos table: 1=1, 2-10=5, 11-30=10, 31-60=25, 61-85=50, 86-99=75, 100=crit(200)
    1,      // gamble_shot: wide variance
    0,      // ricochet_bounces: none
    10,     // misfire_roll: roll 1-10 triggers misfire (10% chance)
    10,     // misfire_penalty: 10% damage on misfire
    SPF_RARE | SPF_LATE_GAME,  // Rare, late game
    10,     // spawn_weight: very rare
    CRIT_SCALING_EXPONENTIAL,  // Exponential crit scaling
    3       // scaling parameter
},
```

### Damage Profile

| Roll Range | Damage | Notes |
|------------|--------|-------|
| 1 | 1 | Minimum |
| 2-10 | 5 | Low |
| 11-30 | 10 | Below average |
| 31-60 | 25 | Average |
| 61-85 | 50 | Above average |
| 86-99 | 75 | High |
| 100 | 200 | CRITICAL! (75×4 + bonus) |
| 1-10 (misfire) | ×0.1 | Misfire penalty |

---

## Weapon Balancing Guidelines

1. **Expected Damage**: Calculate average damage per shot
   - `(sum of bucket damages / 6) × (1 - crit_chance) + (crit_base × crit_multiplier) × crit_chance`

2. **Fire Rate Trade-off**: Faster weapons should have lower per-shot damage

3. **Crit Frequency**: Higher crit chance = more consistent damage
   - d4: 25% crit = frequent small crits
   - d20: 5% crit = rare large crits

4. **Spawn Weight**: Use lower weights (10-30) for powerful weapons

---

## Troubleshooting

### Weapon Not Firing
- Check ammo type is defined and player has ammo
- Verify state transitions are correct

### No Damage on Hit
- Ensure damage calculation returns > 0
- Check projectile has correct damage field

### Crash on Pickup
- Verify NUMWEAPONS enum includes new weapon
- Ensure arrays are sized correctly

### Crit Not Working
- Confirm crit_chance and crit_roll values are valid
- Check crit_scaling_type is defined

---

## References

- See `src/doom/d_items.c` for full weapon implementations
- See `src/doom/d_items.h` for struct definitions  
- See `DESIGN.md` for damage mathematics
