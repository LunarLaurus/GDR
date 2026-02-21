# Weapon Balancing Specification

## Overview
This document defines the damage tiers, expected DPS, and balance guidelines for all dice weapons in Goblin Dice Rollaz.

## Dice Tier System

### Light Dice (am_lightdice)
- **Weapons**: d4, d6, d8
- **Characteristics**: Fast fire rate, lower base damage, higher crit chance
- **Ammo efficiency**: Moderate (lower damage per ammo unit)
- **Role**: Close-quarters combat, rapid engagement

### Heavy Dice (am_heavydice)
- **Weapons**: d10, d12, d20, percentile
- **Characteristics**: Slow fire rate, high base damage, lower crit chance
- **Ammo efficiency**: High (higher damage per ammo unit)
- **Role**: Mid-to-long range, boss encounters

## Current Weapon Stats (After Balancing)

### d4 Throwing Knives (wp_d4)
- **Die**: 4-sided
- **Crit Chance**: 25% (roll of 4)
- **Crit Multiplier**: 2x
- **Fire Rate**: 4 ticks (fast)
- **Expected DPS**: ~1.0
- **Role**: High-risk rapid fire, very high crit chance
- **Damage Table**: {1, 2, 3, 4} → {1, 2, 3, 8} (fixed: no zero values)

### d6 Blaster (wp_d6blaster)
- **Die**: 6-sided
- **Crit Chance**: 16% (roll of 6)
- **Crit Multiplier**: 2x
- **Fire Rate**: 4 ticks (fast)
- **Expected DPS**: ~1.1
- **Role**: Reliable rapid fire
- **Damage Table**: {1, 1, 2, 2, 3, 5} → {1, 1, 2, 2, 3, 10}

### d12 Heavy Impact (wp_d12)
- **Die**: 12-sided
- **Crit Chance**: 8% (roll of 12)
- **Crit Multiplier**: 2x
- **Fire Rate**: 6 ticks (medium)
- **Expected DPS**: ~1.5
- **Role**: Heavy hitter, balanced
- **Damage Table**: {3, 3, 6, 6, 9, 12} → {3, 3, 6, 6, 9, 24}

### d20 Cannon (wp_d20cannon)
- **Die**: 20-sided
- **Crit Chance**: 5% (roll of 20)
- **Crit Multiplier**: 2x
- **Fire Rate**: 8 ticks (slow)
- **Expected DPS**: ~1.9
- **Role**: Premium heavy weapon, boss killer
- **Damage Table**: {5, 5, 10, 10, 15, 25} → {5, 5, 10, 10, 15, 50}

### Percentile Dice (wp_percentile) - Gamble Shot
- **Die**: 100-sided
- **Crit Chance**: 1% (roll of 100)
- **Crit Multiplier**: 3x
- **Fire Rate**: 7 ticks (slow)
- **Expected DPS**: ~4.0 (very high variance)
- **Role**: High-risk/high-reward specialty weapon
- **Damage Table**: {1, 3, 5, 15, 35, 75} → {1, 3, 5, 15, 35, 300}

## Ammo Balancing

### Ammo Pickup Quantities
- **Light dice pickup**: 10 units
- **Heavy dice pickup**: 5 units

### Ammo Drop Rates
- Light dice enemies: Higher ammo drop frequency
- Heavy dice enemies: Lower ammo drop frequency

## DPS Comparison (Theoretical)

| Weapon    | Avg Damage | Fire Rate | DPS    | Crit % |
|-----------|------------|-----------|--------|--------|
| d4        | 3.5        | 4         | 0.875  | 25%    |
| d6        | 3.56       | 4         | 0.89   | 16%    |
| d12       | 7.9        | 6         | 1.32   | 8%     |
| d20       | 13.59      | 8         | 1.70   | 5%     |
| Percentile| 25.1       | 7         | 3.59   | 1%     |

## Balancing Principles

1. **Tier Progression**: Each die type should have measurably different DPS
2. **Crit Trade-off**: Higher damage = lower crit chance
3. **Fire Rate Penalty**: Higher damage weapons have slower fire rates
4. **Ammo Economy**: Heavy dice should feel more precious
5. **No Zero Damage**: All rolls must deal at least 1 damage (except percentile gamble)

## Future Weapons (Planned)

### d8 Mid-Tier
- **Die**: 8-sided
- **Crit Chance**: 12.5% (roll of 8)
- **Crit Multiplier**: 2x
- **Fire Rate**: 5 ticks
- **Expected DPS**: ~1.2

### d10 Ricochet
- **Die**: 10-sided
- **Crit Chance**: 10% (roll of 10)
- **Crit Multiplier**: 2x
- **Fire Rate**: 6 ticks
- **Special**: Projectile ricochet

### Twin d6 Scatter
- **Die**: 2x d6 rolled
- **Crit Chance**: ~30% (at least one 6)
- **Crit Multiplier**: 2x
- **Fire Rate**: 5 ticks
- **Special**: Close-range burst

### Arcane d20 Beam
- **Die**: Continuous d20 tick
- **Crit Chance**: 5% per tick
- **Crit Multiplier**: 1.5x
- **Fire Rate**: Continuous
- **Special**: Beam weapon

### Cursed Die
- **Die**: Variable
- **Special**: High damage but self-inflicted damage chance
