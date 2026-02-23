# Goblin Dice Rollaz - Demo WAD Bundle Specification

## Overview

This document describes the contents of the `goblin_demo.wad` bundle - a demonstration WAD that showcases Goblin Dice Rollaz content including:
- Custom map themes (cavern, forge, treasure)
- Dice weapon pickups
- Powerup placement
- Enemy spawn configurations

## WAD Structure

```
goblin_demo.wad
├── MAPINFO           # Map definitions
├── THINGS            # Actor spawn points
├── LINEDEFS          # Line definitions
├── SIDEDEFS          # Side definitions
├── VERTEXES          # Vertices
├── SEGS              # Segments
├── SECTORS           # Sectors
├── BLOCKMAP          # Collision data
├── NODES             # BSP tree
└── REJECT           # rejection table
```

## Map Definitions

### MAP01: Goblin Cavern (Mine Theme)

**Theme**: Underground goblin encampment
**Texture Palette**: Stone floors, timber supports, cave walls
**Lighting**: Low (lightlevel 48-64), warm torch tones
**Enemies**: Goblin Warriors, Goblin Scouts, Goblin Sneaks
**Items**: d4 ammo, d6 blaster, health pickups

**Sector Layout**:
- Central chamber (128x128, lightlevel 64)
- Side tunnels (64 wide, lightlevel 48)
- Cave-in hazard zones
- d6 weapon spawn in back alcove

### MAP02: Dwarven Forge (Lava Theme)

**Theme**: Active dwarven forge/armory
**Texture Palette**: Lava floors, metal grates, anvils
**Lighting**: Red/orange bias (lightlevel 80-160), bright sparks
**Enemies**: Dwarf Defenders, Dwarf Marksmen, Dwarf Miners
**Items**: d12 ammo, d20 cannon, Double Damage powerup

**Sector Layout**:
- Main forge (large arena, lightlevel 128)
- Lava pit (sector damage 4/tick)
- Anvil platforms (elevated combat)
- d20 cannon reward in northwest

### MAP03: Treasure Vault

**Theme**: Rich treasure chamber with traps
**Texture Palette**: Gold, gems, ornate stone
**Lighting**: Bright (lightlevel 192), echo ambience
**Enemies**: Elite Dwarf Captain, Armored Dwarves
**Items**: All dice weapons, Critical Hit powerup, Fortune powerup

**Sector Layout**:
- Central vault (64x64, lightlevel 192)
- Pressure plate traps (sector 666)
- Elevated platforms with powerups
- Boss-scale encounter

## Thing Definitions

### Player Starts

| Type | Map | Position | Angle |
|------|-----|----------|-------|
| 1 (Player) | MAP01 | (1024, 2048) | 0 |
| 1 (Player) | MAP02 | (1024, 2048) | 0 |
| 1 (Player) | MAP03 | (1024, 2048) | 0 |

### Weapons & Ammo

| Type | Map | Position | Notes |
|------|-----|----------|-------|
| 2005 (Clip) | MAP01 | Various | d4 ammo |
| 2006 (Clip) | MAP01 | Various | d6 ammo |
| 2007 (Shell) | MAP02 | Various | d12 ammo |
| 2008 (Rocket) | MAP02 | Various | d20 ammo |

### Powerups

| Type | Map | Position | Effect |
|------|-----|----------|--------|
| 2013 (Invulnerability) | MAP03 | (1536, 1536) | Crit Boost (30s) |
| 2014 (Invisibility) | MAP02 | (2048, 1024) | Double Damage (20s) |
| 2015 (RadSuits) | MAP03 | (1024, 2048) | Fortune (1 hit) |

### Enemies

| Type | Doom ID | GDR ID | Map | Count |
|------|---------|--------|-----|-------|
| Goblin Warrior | 9 | 8100 | MAP01 | 8 |
| Goblin Scout | 7 | 8102 | MAP01 | 4 |
| Goblin Sneak | 32 | 8103 | MAP01 | 2 |
| Dwarf Defender | 58 | 8200 | MAP02 | 6 |
| Dwarf Marksman | 59 | 8201 | MAP02 | 3 |
| Dwarf Miner | 60 | 8202 | MAP02 | 4 |
| Elite Captain | 64 | 8205 | MAP03 | 1 |
| Armored Dwarf | 56 | 8203 | MAP03 | 4 |

## Usage

```bash
# Load demo WAD with Goblin Dice Rollaz
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD

# Start at specific map
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD -warp 1

# Test specific theme
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD -warp 2  # Forge
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD -warp 3  # Treasure
```

## Build Requirements

To build `goblin_demo.wad` from source:

1. Use SLADE3, XWE, or Doom Builder 2
2. Import map data from `maps/demo_maps.txt`
3. Place thing definitions from `things/demo_things.txt`
4. Save as PWAD format

Alternatively, use the build script:

```bash
cd data
./build_demo_wad.sh
```

## Validation

After loading, verify:
- [ ] All three maps load without errors
- [ ] Enemies spawn and behave correctly
- [ ] Weapons deal dice-based damage
- [ ] Critical hits trigger on correct roll values
- [ ] Powerups activate with correct duration
- [ ] Theme lighting appears correctly

## Version

Demo WAD Bundle v0.1 - Matches Goblin Dice Rollaz engine v0.1
