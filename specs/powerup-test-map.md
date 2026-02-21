# Powerup Test Map Specification

## Overview

This document describes how to create and use a dedicated test map for tuning Goblin Dice Rollaz powerups. The test map is designed to allow rapid iteration on powerup balance without needing to play through full levels.

## Map Design Guidelines

### Recommended Layout

```
+--------------------------------------------------+
|                                                  |
|   [P1]                              [PWR_SPAWN]  |
|                                                  |
|   [ENEMY_CLUSTER]                   [ENEMY_CLUSTER]|
|                                                  |
|                                                  |
|   [ENEMY_CLUSTER]                   [ENEMY_CLUSTER]|
|                                                  |
|   [AMMO]                    [HEALTH]  [ARMOR]    |
|                                                  |
+--------------------------------------------------+

Legend:
- P1: Player start position
- PWR_SPAWN: Powerup spawn location (multiple recommended)
- ENEMY_CLUSTER: Grouped enemy spawns for damage testing
- AMMO/HEALTH/ARMOR: Standard pickups for sustain
```

### Design Principles

1. **Small footprint**: 512x512 unit arena is sufficient
2. **Open center**: Clear line of sight for testing
3. **Multiple powerup positions**: Test different pickup behaviors
4. **Enemy clusters**: Groups of 3-5 enemies for damage testing
5. **Clear geometry**: Simple walls, no complex routing

### Sector Setup

```c
// Testing arena sector (example from map editor)
// Floor height: 0
// Ceiling height: 128
// Floor texture: FLAT1 (neutral)
// Light level: 160 (bright for clear testing)
```

## Powerup Spawn Locations

### Critical Hit Boost (ID: 8050)

Place at multiple positions around the arena:
- Position 1: Center-left (x: -200, y: 0)
- Position 2: Center-right (x: 200, y: 0)
- Position 3: Far corner (x: -300, y: -200)

Mapthing options: `7` (spawns on all difficulties)

### Double Damage (ID: 8051)

Place for testing duration and stacking:
- Position 1: Near enemy cluster A
- Position 2: Near enemy cluster B

Mapthing options: `6` (medium/hard only for testing balance)

### Dice Fortune (ID: 8052)

Single position, tests one-shot crit mechanic:
- Position 1: Player path toward first encounter

Mapthing options: `6` (medium/hard only)

## Enemy Test Placements

### For Critical Hit Testing

| Enemy Type | Mapthing ID | Count | Position |
|------------|-------------|-------|----------|
| Dwarf Worker | 8100 | 5 | Cluster A |
| Goblin Scout | 8020 | 5 | Cluster B |

### For Double Damage Testing

| Enemy Type | Mapthing ID | Count | Position |
|------------|-------------|-------|----------|
| Dwarf Defender | 8101 | 3 | Cluster A |
| Armored Dwarf | 8108 | 2 | Cluster B |

### For Damage Output Testing

| Enemy Type | Mapthing ID | Count | Notes |
|------------|-------------|-------|-------|
| Various | - | 10+ | Mix for sustained testing |

## Testing Procedures

### Critical Hit Boost Testing

1. Start game on test map
2. Collect Critical Hit Boost powerup
3. Attack enemies and record:
   - Hit count
   - Crit count
   - Total damage dealt
4. Compare against expected crit rate

Expected formula: `actual_crits / hits >= expected_crit_rate`

### Double Damage Testing

1. Collect powerup
2. Attack single enemy, record damage
3. Continue attacking until powerup expires
4. Compare damage with/without powerup

Expected: Damage should be exactly 2x base damage

### Dice Fortune Testing

1. Collect powerup (no timer - one-shot)
2. First attack should be critical hit
3. Record if crit occurred
4. Verify powerup deactivates after hit

### Stacking Prevention

1. Collect powerup A
2. Attempt to collect powerup B (with EXCLUSIVE flag)
3. Verify: Original powerup timer continues, new powerup does not activate

## CVARs for Testing

### Powerup Spawn Rates

```c
critboost_spawn_rate     // Default: 100
doubledamage_spawn_rate  // Default: 75
dicefortune_spawn_rate   // Default: 40
```

### Powerup Duration

```c
critboost_duration       // Default: 30 seconds (TICRATE * 30)
doubledamage_duration    // Default: 20 seconds
```

### Critical Hit System

```c
gdr_base_crit_chance     // Default: 10 (10% base)
gdr_crit_multiplier      // Default: 2.0 (2x damage on crit)
gdr_critboost_bonus      // Default: 20 (additional 20% with powerup)
```

### Debug Commands

```
idpow [0-8]    // Give specific powerup (debug cheat)
gdr_debug      // Toggle debug mode (shows damage numbers)
powerupstats   // Show active powerup timers
dmgstats       // Show damage dealt/taken
```

## Testing Checklist

- [ ] Powerup pickup triggers correct effect
- [ ] Timer displays accurately in HUD
- [ ] Powerup expires at correct time
- [ ] Stacking prevention works correctly
- [ ] Visual effects (screen tint, glow) display
- [ ] Audio cues play on pickup and expiration
- [ ] Crit damage calculation correct with powerups active
- [ ] Double damage calculation correct (not stacking)
- [ ] One-shot powerup clears after hit (Dice Fortune)
- [ ] Powerups persist across weapon switches
- [ ] Powerups clear on player death

## Mapthing Reference

### Powerup IDs (8000-8099 reserved for Goblin Dice Rollaz)

| ID | Item | Sprite | Notes |
|----|------|--------|-------|
| 8050 | Critical Hit Boost | PCRT | Timed powerup |
| 8051 | Double Damage | PDMD | Timed powerup |
| 8052 | Dice Fortune | PFTR | One-shot powerup |

### Standard Doom Powerups (for comparison)

| ID | Item | Sprite |
|----|------|--------|
| 2019 | Invulnerability | PINV |
| 2022 | Berserk | PSTR |
| 2023 | Invisibility | PINS |
| 2025 | Iron Feet | SUIT |
| 2026 | Allmap | PMAP |
| 2027 | Infrared | PVIS |

## Tools for Map Creation

### Recommended Editors

- **Doom Builder 2**: Modern, supports UDMF
- **Doom Builder 64**: N64-specific but compatible
- **Yadex**: Linux native

### Export Format

Export as ZIP containing:
- `MAP01` (map lump)
- `TEXTURE1`, `TEXTURE2` (if custom textures)
- `PNAMES` (if custom sprites)

Test with:
```bash
./goblin-doom -merge testmap.wad -iwad DOOM.WAD
```

## Balance Tuning Values

### Suggested Ranges

| Powerup | Too Weak | Balanced | Too Strong |
|---------|----------|----------|------------|
| Critical Hit Boost | 10s | 30s | 60s |
| Double Damage | 10s | 20s | 45s |
| Dice Fortune | N/A | 1-hit | N/A |

### Spawn Rate Guidelines

| Setting | Rare | Standard | Abundant |
|---------|------|----------|----------|
| critboost_spawn_rate | 50 | 100 | 150 |
| doubledamage_spawn_rate | 40 | 75 | 100 |
| dicefortune_spawn_rate | 20 | 40 | 60 |

## See Also

- `specs/spawn-tuning.md` - Difficulty-based spawn configuration
- `src/doom/g_powerup.c` - Powerup implementation
- `src/doom/st_stuff.c` - HUD display for powerups
- `src/doom/p_inter.c` - Damage calculation with powerups
