# Dice Weapon DEHACKED Mappings

This document details the DEHACKED patch support for Goblin Dice Rollaz dice weapons.

## Overview

DEHACKED patches allow modders to modify game data without recompiling the source code. Goblin Dice Rollaz supports DEHACKED weapon modifications through the standard Doom weapon mapping system, plus additional dice-specific fields.

## Weapon IDs

Goblin Dice Rollaz uses the following weapon IDs:

| ID | Weapon Name | Die Type |
|----|-------------|----------|
| 0 | Fist | N/A |
| 1 | d6 Blaster | d6 |
| 2 | d20 Cannon | d20 |
| 3 | d12 | d12 |
| 4 | Shotgun | N/A |
| 5 | Chaingun | N/A |
| 6 | Missile Launcher | N/A |
| 7 | Plasma Rifle | N/A |
| 8 | BFG 9000 | N/A |
| 9 | Chainsaw | N/A |
| 10 | Super Shotgun | N/A |
| 11 | Percentile (d100) | d100 |
| 12 | d4 Throwing Knives | d4 |
| 13 | d8 | d8 |
| 14 | d10 Ricochet | d10 |
| 15 | Twin d6 | d6×2 |
| 16 | Arcane d20 | d20 |
| 17 | Cursed Die | d6 |

## Standard DEHACKED Mappings

The following weapon properties are exposed through standard DEHACKED and can be modified using standard Doom DEHACKED syntax:

```
[Weapon #]
Ammo type = value
Deselect frame = value
Select frame = value
Bobbing frame = value
Shooting frame = value
Firing frame = value
```

### Field Descriptions

- **Ammo type**: Which ammo type the weapon uses (0 = no ammo, 1 = bullets, 2 = shells, 3 = cells, 4 = missiles)
- **Deselect frame**: State number for weapon lowering animation
- **Select frame**: State number for weapon raising animation
- **Bobbing frame**: State number for idle weapon animation
- **Shooting frame**: State number for attack animation
- **Firing frame**: State number for muzzle flash animation

### Example DEHACKED Patch

```dehacked
[Weapon 1]
Ammo type = 1
Deselect frame = 30
Select frame = 31
Bobbing frame = 32
Shooting frame = 33
Firing frame = 34
```

This modifies the d6 Blaster (weapon #1) to use bullet ammo and change its animation frames.

## Dice-Specific DEHACKED Mappings (Extended)

Goblin Dice Rollaz extends DEHACKED with custom fields for dice weapon properties. These are available when using the extended DEHACKED parser:

```
[Weapon #]
# Dice properties
Die type = value           # Number of sides (4, 6, 8, 10, 12, 20, 100)
Crit chance = value        # Percentage (0-100)
Crit multiplier = value    # Damage multiplier on crit
Min damage = value        # Minimum damage value
Crit roll = value          # Die face that triggers crit
Damage bucket 0 = value   # Damage for roll 1
Damage bucket 1 = value   # Damage for roll 2
Damage bucket 2 = value   # Damage for roll 3
Damage bucket 3 = value   # Damage for roll 4
Damage bucket 4 = value   # Damage for roll 5
Damage bucket 5 = value   # Damage for roll 6
Crit base damage = value  # Base damage before multiplier on crit
Gamble shot = value      # 1 = wide variance mode
Ricochet bounces = value # Number of ricochets (0 = off)
Misfire roll = value     # Die roll that triggers misfire
Misfire penalty = value  # Damage multiplier on misfire
Spawn flags = value      # Bitfield for spawn behavior
Spawn weight = value     # Relative spawn probability
Crit scaling type = value # 0=linear, 1=exp, 2=flat bonus, 3=% bonus
Crit scaling param = value # Parameter for scaling formula
```

### Field Descriptions

| Field | Description | Valid Range |
|-------|-------------|-------------|
| Die type | Number of sides on the die | 4, 6, 8, 10, 12, 20, 100 |
| Crit chance | Base critical hit chance | 0-100 (percentage) |
| Crit multiplier | Damage multiplier when crit occurs | 1-10 |
| Min damage | Minimum possible damage | 0-100 |
| Crit roll | Die face that triggers critical hit | 1 to Die type |
| Damage bucket 0-5 | Damage for roll ranges 1-6 | 0-1000 |
| Crit base damage | Base damage for crit calculation | 0-1000 |
| Gamble shot | Enable wide variance/exploding rolls | 0 or 1 |
| Ricochet bounces | Number of projectile ricochets | 0-5 |
| Misfire roll | Die roll that causes misfire | 0 to Die type |
| Misfire penalty | Damage multiplier on misfire (-1 = self-damage) | -1 to 100 |
| Spawn flags | Map balancing flags | Bitfield |
| Spawn weight | Relative spawn probability | 0-1000 |
| Crit scaling type | Formula for crit damage calculation | 0-4 |
| Crit scaling param | Parameter for scaling formula | 0-100 |

### Spawn Flags Bitfield

| Flag | Value | Description |
|------|-------|-------------|
| SPF_NONE | 0 | No special spawn rules |
| SPF_EARLY_GAME | 1 | Can spawn in early maps |
| SPF_LATE_GAME | 2 | Only in later maps |
| SPF_RARE | 4 | Rare spawn |
| SPF_ALWAYS_SPAWN | 8 | Always available |

### Crit Scaling Types

| Type | Value | Formula |
|------|-------|---------|
| CRIT_SCALING_LINEAR | 0 | `damage * multiplier` |
| CRIT_SCALING_EXPONENTIAL | 1 | `damage * (multiplier + (roll/10))` |
| CRIT_SCALING_BONUS_FLAT | 2 | `damage + (scaling_param)` |
| CRIT_SCALING_BONUS_PERCENT | 3 | `damage * (1 + scaling_param/100)` |

### Example: Modify d6 Blaster Crit Chance

```dehacked
[Weapon 1]
Die type = 6
Crit chance = 25
Crit multiplier = 2
Crit roll = 6
Damage bucket 0 = 1
Damage bucket 1 = 1
Damage bucket 2 = 2
Damage bucket 3 = 2
Damage bucket 4 = 3
Damage bucket 5 = 5
Crit base damage = 10
```

This increases the d6 Blaster's crit chance from 16% to 25% (rolling 6).

### Example: Create High-Risk Weapon

```dehacked
[Weapon 3]
Die type = 12
Crit chance = 8
Crit multiplier = 4
Misfire roll = 2
Misfire penalty = 25
```

This modifies d12 to have a 4x crit multiplier but adds a 10% misfire chance (rolling 1-2) that reduces damage to 25%.

## Weapon State Numbers

When modifying animation frames, reference the state numbers from your WAD's STATS lump or use existing weapon state numbers as templates. The game expects valid state numbers for each weapon animation phase.

## Limitations

- Sprite replacements for dice weapons must follow Doom sprite naming conventions
- Animation states must exist in the WAD or be defined in DEHACKED
- Damage values are capped at 1000 per hit
- Crit chance is capped at 100%
- Negative misfire penalty values trigger self-damage instead of reduced damage

## See Also

- `DESIGN.md` - Full weapon damage mathematics
- `specs/weapon-balance.md` - Balancing guidelines
- `src/doom/d_items.c` - Source code for weapon definitions
