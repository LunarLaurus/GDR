# Goblin Dice Rollaz - Design Document

## Overview

Goblin Dice Rollaz is a total conversion of Chocolate Doom that replaces all firearms with dice-based weaponry. Every weapon attack rolls a die (d4, d6, d12, d20, or d100) to determine damage, with critical hit mechanics for thematic flavor.

This document details all implemented dice weapons, their damage mathematics, and balance rationale.

---

## Core Dice Mechanics

### Damage Calculation

When a dice weapon fires, the game performs the following steps:

1. **Roll the die** (1 to N, where N is the die size)
2. **Map roll to damage bucket** using the weapon's damage table
3. **Check for critical hit** (roll equals crit_roll threshold)
4. **Apply multipliers** if crit occurs
5. **Check for misfire** (percentile weapons only)
6. **Return final damage**

### Damage Table Structure

Each weapon defines a 7-element damage table:
```
{ bucket0, bucket1, bucket2, bucket3, bucket4, bucket5, crit_base }
```

Rolls 1-6 map to buckets 0-5. The crit_base is used when a critical hit occurs.

### Critical Hit System

- **Crit Chance**: Percentage chance to roll a critical (die equals crit_roll)
- **Crit Multiplier**: Applied to the crit_base damage
- **Guaranteed Crit**: Some powerups can force next hit to crit

### Misfire Mechanic (Percentile Weapons)

Certain high-risk weapons have a misfire chance:
- If roll ≤ misfire_roll, apply misfire_penalty (damage × percentage)

---

## Implemented Weapons

### 1. d4 Throwing Knives

| Property | Value |
|----------|-------|
| **Weapon ID** | `wp_d4` |
| **Die Type** | 4-sided |
| **Crit Chance** | 25% (roll of 4) |
| **Crit Multiplier** | 2x |
| **Min Damage** | 1 |
| **Max Damage** | 8 (on crit) |
| **Fire Rate** | 4 ticks (fast) |
| **Spawn Flags** | SPF_EARLY_GAME |
| **Spawn Weight** | 60 |

**Damage Table:**
```
Roll 1 → 1 damage
Roll 2 → 2 damage
Roll 3 → 3 damage
Roll 4 → 8 damage (crit: 4 × 2)
```

**Analysis:**
- Highest crit chance in the game (25%)
- Very fast fire rate
- Low per-shot damage compensated by crit frequency
- Expected average: ~3.5 damage per roll, ~8.75 effective with crits
- Best for: Close-quarters, rapid engagement, players who enjoy gambling

---

### 2. d6 Blaster

| Property | Value |
|----------|-------|
| **Weapon ID** | `wp_d6blaster` |
| **Die Type** | 6-sided |
| **Crit Chance** | 16.67% (roll of 6) |
| **Crit Multiplier** | 2x |
| **Min Damage** | 1 |
| **Max Damage** | 10 (on crit) |
| **Fire Rate** | 4 ticks (fast) |
| **Spawn Flags** | SPF_ALWAYS_SPAWN |
| **Spawn Weight** | 100 |

**Damage Table:**
```
Roll 1-2  → 1 damage
Roll 3-4  → 2 damage
Roll 5    → 3 damage
Roll 6    → 10 damage (crit: 5 × 2)
```

**Analysis:**
- Starting weapon - reliable and balanced
- Consistent mid-range damage
- Expected average: ~3.56 damage per roll
- Best for: Learning the game, consistent DPS

---

### 3. d12 Heavy Impact

| Property | Value |
|----------|-------|
| **Weapon ID** | `wp_d12` |
| **Die Type** | 12-sided |
| **Crit Chance** | 8.33% (roll of 12) |
| **Crit Multiplier** | 2x |
| **Min Damage** | 3 |
| **Max Damage** | 24 (on crit) |
| **Fire Rate** | 6 ticks (medium) |
| **Spawn Flags** | SPF_EARLY_GAME \| SPF_RARE |
| **Spawn Weight** | 25 |

**Damage Table:**
```
Roll 1-3   → 3 damage
Roll 4-6   → 6 damage
Roll 7-9   → 9 damage
Roll 10-12 → 24 damage (crit: 12 × 2)
```

**Analysis:**
- Rare early-game heavy weapon
- Higher damage but slower than d4/d6
- Has charged smash alternate fire (bonus crit chance when charged)
- Expected average: ~7.9 damage per roll
- Best for: Players wanting heavy hits without committing to late-game

---

### 4. d20 Cannon

| Property | Value |
|----------|-------|
| **Weapon ID** | `wp_d20cannon` |
| **Die Type** | 20-sided |
| **Crit Chance** | 5% (roll of 20) |
| **Crit Multiplier** | 2x |
| **Min Damage** | 5 |
| **Max Damage** | 50 (on crit) |
| **Fire Rate** | 8 ticks (slow) |
| **Spawn Flags** | SPF_LATE_GAME |
| **Spawn Weight** | 30 |

**Damage Table:**
```
Roll 1-5   → 5 damage
Roll 6-10   → 10 damage
Roll 11-15 → 15 damage
Roll 16-19 → 25 damage
Roll 20    → 50 damage (crit: 25 × 2)
```

**Analysis:**
- Premium late-game weapon
- Highest consistent damage in the game
- Slow fire rate penalizes spam
- Expected average: ~13.59 damage per roll
- Best for: Boss encounters, methodical players

---

### 5. Percentile Dice (Gamble Shot)

| Property | Value |
|----------|-------|
| **Weapon ID** | `wp_percentile` |
| **Die Type** | 100-sided |
| **Crit Chance** | 1% (roll of 100) |
| **Crit Multiplier** | 3x |
| **Min Damage** | 1 |
| **Max Damage** | 300 (on crit) |
| **Fire Rate** | 7 ticks (slow) |
| **Misfire Chance** | 5% (roll 1-5) |
| **Misfire Penalty** | 25% damage |
| **Spawn Flags** | SPF_RARE \| SPF_LATE_GAME |
| **Spawn Weight** | 20 |

**Damage Table:**
```
Roll 1-15   → 1 damage
Roll 16-35  → 3 damage
Roll 36-55  → 5 damage
Roll 56-75  → 15 damage
Roll 76-90  → 35 damage
Roll 91-99  → 75 damage
Roll 100    → 300 damage (crit: 100 × 3)
```

**Analysis:**
- Highest risk, highest reward weapon
- Extremely wide damage variance (1-300)
- Misfire mechanic adds additional risk
- 3x crit multiplier (highest in game)
- Expected average: ~25.1 damage per roll, but extremely volatile
- Best for: High-risk players, endgame diversity

---

## Non-Dice Weapons (Legacy)

These weapons exist for compatibility but are not thematic to Goblin Dice Rollaz:

| Weapon | Notes |
|--------|-------|
| wp_fist | Unarmed |
| wp_shotgun | Legacy weapon |
| wp_chaingun | Legacy weapon |
| wp_missile | Legacy weapon |
| wp_plasma | Legacy weapon |
| wp_bfg | Legacy weapon |
| wp_chainsaw | Legacy weapon |
| wp_supershotgun | Legacy weapon |

---

## Planned Weapons (Not Yet Implemented)

See IMPLEMENTATION_PLAN.md for development status:

- **d8 Mid-Tier**: Balanced fire rate and crit rate
- **d10 Ricochet**: Projectiles bounce once
- **Twin d6 Scatter**: Close-range burst (2 dice)
- **Arcane d20 Beam**: Continuous roll tick damage
- **Cursed Die**: High damage with self-risk mechanic

---

## Damage Math Reference

### Expected Value Formula

For a weapon with damage table `{b0, b1, b2, b3, b4, b5}` and crit_base `C`:
```
EV = (b0 + b1 + b2 + b3 + b4 + b5) / 6
     + (crit_chance × crit_multiplier × C) / die_size
```

### DPS Calculation

```
DPS = Expected Damage / Fire Rate (ticks)
```

### Critical Hit Effective Multiplier

```
Effective Multiplier = 1 + (crit_chance × (crit_multiplier - 1))
```

---

## Balance Philosophy

1. **Higher die = higher floor, lower ceiling ratio**: d20 has minimum 5 damage but only 5% crit
2. **Fire rate penalty**: Heavy weapons fire slower to prevent spam
3. **Ammo economy**: Heavy dice drops are rarer
4. **No true zero damage**: Every roll deals at least 1 damage (except gamble shot)
5. **Power fantasy with tradeoffs**: Each weapon has a clear niche

---

## Technical Notes

### Deterministic RNG

All dice rolls use Doom's built-in RNG (`P_Random()`) to maintain:
- Demo compatibility
- Multiplayer synchronization
- Reproducible bug reports

### Weapon Spawn System

Weapons have spawn flags that control when/where they appear:
- `SPF_ALWAYS_SPAWN`: Available on any map/difficulty
- `SPF_EARLY_GAME`: Maps 1-8 only
- `SPF_LATE_GAME`: Maps 24+ only
- `SPF_RARE`: Lower spawn weight

---

*Last updated: Implementation Phase 13.5*
