# Difficulty-Based Spawn Tuning

## Overview

Goblin Dice Rollaz uses Doom's native skill-based spawning system to control enemy and item spawn rates across difficulty levels. This document specifies how map designers should configure spawn options for custom goblin/dwarf enemies and powerups.

## Technical Background

### Mapthing Options Field

Each mapthing in a WAD file has an `options` field (2 bytes) containing bitflags:

| Bit | Value | Meaning |
|-----|-------|---------|
| 0   | 1     | Spawn on Easy (skill 1-2) |
| 1   | 2     | Spawn on Medium (skill 3) |
| 2   | 4     | Spawn on Hard (skill 4) |
| 3   | 8     | Spawn on Nightmare (skill 5) |
| 4   | 16    | Never spawn in netgame |

### Skill Level Mapping

```
gameskill = sk_baby    -> bit = 1 (Easy)
gameskill = sk_easy    -> bit = 1 (Easy)
gameskill = sk_medium  -> bit = 2 (Medium)
gameskill = sk_hard    -> bit = 4 (Hard)
gameskill = sk_nightmare -> bit = 4 (Hard, but uses special respawn)
```

## Enemy Spawn Guidelines

### Goblin Enemies

| Enemy Type | Easy | Medium | Hard | Notes |
|------------|------|--------|------|-------|
| Goblin Scout (8020) | Yes | Yes | Yes | Fragile, spawn everywhere |
| Goblin Sneak (8021) | No | Yes | Yes | Ambush behavior |
| Goblin Alchemist (8022) | No | No | Yes | Dangerous projectile |
| Goblin Shaman (8023) | No | No | Yes | Support spellcaster |
| Goblin Totemist | No | No | Yes | Buff/debuff totems |

**Recommended options values:**
- Goblin Scout: `7` (1+2+4) - spawns on all difficulties
- Goblin Sneak: `6` (2+4) - medium/hard only
- Goblin Alchemist: `4` - hard only
- Goblin Shaman: `4` - hard only

### Dwarf Enemies

| Enemy Type | Easy | Medium | Hard | Notes |
|------------|------|--------|------|-------|
| Dwarf Worker | Yes | Yes | Yes | Basic enemy |
| Dwarf Defender | Yes | Yes | Yes | Tanky, frontal block |
| Dwarf Marksman | No | Yes | Yes | Ranged, slow |
| Dwarf Miner | No | Yes | Yes | Medium-range pickaxe |
| Dwarf Berserker | No | Yes | Yes | Fast melee |
| Dwarf Engineer | No | No | Yes | Explosives |
| Dwarf Captain | No | No | Yes | Buffs nearby |
| Dwarf Bombardier | No | No | Yes | Timed explosives |
| Armored Dwarf | No | No | Yes | Weak rear hitbox |

**Recommended options values:**
- Dwarf Worker: `7` - all difficulties
- Dwarf Defender: `7` - all difficulties
- Dwarf Marksman: `6` - medium/hard
- Dwarf Miner: `6` - medium/hard
- Dwarf Berserker: `6` - medium/hard
- Dwarf Engineer: `4` - hard only
- Dwarf Captain: `4` - hard only
- Dwarf Bombardier: `4` - hard only
- Armored Dwarf: `4` - hard only

## Powerup Spawn Guidelines

### Dice Powerups

| Powerup | Easy | Medium | Hard | Notes |
|---------|------|--------|------|-------|
| Critical Hit Boost (8050) | Yes | Yes | Yes | Duration scales |
| Double Damage (8051) | Yes | Yes | Yes | Rarer on easy |
| Dice Fortune (8052) | No | Yes | Yes | Guaranteed crit |
| Full Health | Yes | Yes | Yes | Standard health |
| Armor | Yes | Yes | Yes | Standard armor |

**Recommended options values:**
- Critical Hit Boost: `7` - all, consider `6` for harder balance
- Double Damage: `6` - medium/hard only
- Dice Fortune: `6` - medium/hard only (rare pickup)

### Ammo Pickups

Light dice ammo (d4, d6, d8):
- Easy: `7` - abundant
- Medium: `7` - standard
- Hard: `6` - slightly reduced

Heavy dice ammo (d10, d12, d20):
- Easy: `1` - minimal
- Medium: `6` - available
- Hard: `7` - more scarce (high risk/reward)

Arcane dice ammo (d100):
- Easy: `0` - do not spawn
- Medium: `4` - hard only (very rare)
- Hard: `4` - hard only

## Difficulty Scaling Factors

### Health Multipliers

The base game applies HP multipliers based on skill:
- Easy: 1.0x
- Medium: 1.0x
- Hard: 1.0x
- Nightmare: 1.0x (but fast respawn)

For Goblin Dice Rollaz, consider additional scaling in `p_inter.c` for new enemies:
- Goblin HP: Easy=0.8x, Medium=1.0x, Hard=1.2x
- Dwarf HP: Easy=1.0x, Medium=1.0x, Hard=1.3x (armored)

### Damage Scaling

Optional CVARs for damage scaling by difficulty:
```
gdr_skill_damage_easy = 0.8
gdr_skill_damage_medium = 1.0
gdr_skill_damage_hard = 1.25
```

### Critical Chance Scaling

Optional CVAR to increase base crit chance on harder difficulties:
```
gdr_skill_crit_easy = 5      // 5% base
gdr_skill_crit_medium = 8    // 8% base
gdr_skill_crit_hard = 12     // 12% base
```

## Mapthing ID Reference

### Goblins (8000-8099)

| ID | Enemy | Spawnstate | HP | Notes |
|----|-------|------------|-----|-------|
| 8020 | Goblin Scout | S_SPOS_STND | 20 | Light infantry |
| 8021 | Goblin Sneak | S_SPOS_STND | 25 | Ambush AI |
| 8022 | Goblin Alchemist | S_SPOS_STND | 45 | Potion projectiles |
| 8023 | Goblin Shaman | S_SPOS_STND | 40 | Spellcasting |

### Dwarves (8100-8199)

| ID | Enemy | Spawnstate | HP | Notes |
|----|-------|------------|-----|-------|
| 8100 | Dwarf Worker | S_POSS_STND | 35 | Basic enemy |
| 8101 | Dwarf Defender | S_POSS_STND | 60 | Shield block |
| 8102 | Dwarf Marksman | S_POSS_STND | 50 | Ranged rifle |
| 8103 | Dwarf Miner | S_POSS_STND | 45 | Pickaxe throw |
| 8104 | Dwarf Berserker | S_POSS_STND | 40 | Fast melee |
| 8105 | Dwarf Engineer | S_POSS_STND | 55 | Turret deploy |
| 8106 | Dwarf Captain | S_POSS_STND | 80 | Boss-like, buffs |
| 8107 | Dwarf Bombardier | S_POSS_STND | 35 | Explosives |
| 8108 | Armored Dwarf | S_POSS_STND | 100 | Heavy armor |

### Powerups (8050-8059)

| ID | Powerup | Effect | Duration |
|----|---------|--------|----------|
| 8050 | Critical Hit Boost | +20% crit chance | 30s |
| 8051 | Double Damage | 2x damage | 20s |
| 8052 | Dice Fortune | Next hit = crit | 1 hit |

### Per-Powerup Rarity System

Goblin Dice Rollaz implements per-powerup rarity via CVARs, allowing fine-tuned balance without modifying WAD files:

| CVAR | Default | Rarity Tier | Description |
|------|---------|-------------|-------------|
| `critboost_spawn_rate` | 100 | Common | Crit boost is the baseline powerup |
| `doubledamage_spawn_rate` | 75 | Uncommon | Strong but has duration limit |
| `dicefortune_spawn_rate` | 40 | Rare | Guaranteed crit is very powerful |

**Difficulty Scaling:** All powerup rates are scaled by skill level:
- Baby: 30% of base rate
- Easy: 50% of base rate  
- Medium: 75% of base rate
- Hard: 100% of base rate
- Nightmare: 150% of base rate

**Example Configurations:**

Balanced (default):
```
critboost_spawn_rate = 100
doubledamage_spawn_rate = 75
dicefortune_spawn_rate = 40
```

Harder (rarer powerups):
```
critboost_spawn_rate = 60
doubledamage_spawn_rate = 40
dicefortune_spawn_rate = 20
```

Easier (more powerups):
```
critboost_spawn_rate = 100
doubledamage_spawn_rate = 100
dicefortune_spawn_rate = 60
```

## Implementation Notes

### Adding New Enemies

1. Define enemy in `info.c` with unique `doomednum`
2. Set appropriate spawn flags based on desired difficulty
3. Add to this spec with recommended options value

### Tuning Existing Spawns

To adjust spawn rates without modifying WAD:
- Use DeHackEd patches to modify mapthing options
- Or implement CVAR-based spawn filtering in `P_SpawnMapThing`

### Debug Commands

```
spawnstats    // Show spawn counts by type and skill
forcespawn    // Force spawn a specific enemy type
```

## Balance Philosophy

- **Easy**: Focus on teaching dice mechanics, abundant crit powerups
- **Medium**: Balanced challenge, standard spawn rates
- **Hard**: Fewer but tougher enemies, powerups are rare
- **Nightmare**: Fast respawn, maximum challenge (optional mode)

## See Also

- `src/doom/p_mobj.c` - P_SpawnMapThing function
- `src/doom/doomdata.h` - mapthing_t structure
- `specs/wad-sprites.md` - Sprite naming conventions
