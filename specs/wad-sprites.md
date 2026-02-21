# WAD File Format & Sprite System

## Overview

WAD (Where's All the Data) files contain game assets for Doom engine games. There are two types:
- **IWAD** (Internal WAD): Main game data (commercial game files)
- **PWAD** (Patch WAD): Additional/modified content (community created)

## WAD File Structure

```
Header:
- identification: "IWAD" or "PWAD" (4 bytes)
- numlumps: number of lumps in directory (4 bytes)
- infotableofs: offset to directory (4 bytes)

Directory (numlumps entries):
- lump name: 8 bytes, padded with nulls
- lump size: 4 bytes
- lump offset: 4 bytes
```

## Sprite System

### Sprite Naming Convention

Sprites use 4-character names padded to 8 bytes:
- Format: `XXXXNN00` where:
  - `XXXX` = 4-char sprite prefix (e.g., TROO, PUNG, SHTG)
  - `NN` = frame number (01-16)
  - `00` = padding

### Sprite Frames

Each sprite has multiple frames for animations:
- Frame A: `TROOABCD` (4 frames)
- Frame E: `TROOE...`

### Key Sprite Categories

| Category | Prefix | Example |
|----------|--------|---------|
| Player | PLAY | PLAYA1-8 |
| Weapons | PUNG, SHTG, MGUN, etc. | PUNGA1 |
| Monsters | TROO, SARG, CYBR, etc. | TROOA1 |
| Items | BON1, BON2, MEDI, STIM | BON1A0 |
| Powerups | SOUL, MEGA, PINV, PSTR | SOULA0 |

### Sprite Sections

WAD files organize sprites between markers:
```
S_START  (or SS_START for soundless sprites)
  [sprite lumps]
S_END
```

## Creating Sprite Replacements for Goblin Dice Rollaz

### Required Sprite Names

To replace existing Doom content, use these sprite prefixes:

#### Player Weapons
| Weapon | Sprite Prefix |
|--------|---------------|
| Punch | PUNG |
| Pistol | PISG/PISF |
| Shotgun | SHTG/SHTF |
| Chaingun | CHGG/CHGF |
| Rocket Launcher | MISG/MISF |
| Plasma Rifle | PLAS/PLSF |
| BFG | BFUG/BFGF |
| Chainsaw | CSAW |
| Super Shotgun | SHT2/SHTF |

#### Player States
- PLAYA1-8: Idle/walk frames

#### Pickups
| Item | Sprite |
|------|--------|
| Health | STIM, MEDI |
| Armor | ARM1, ARM2 |
| Ammo | CLIP, SHEL, ROCK, CELL, AMMO |
| Keys | BKEY, YKEY, RKEY, BSKU, YSKU, RSKU |
| Powerups | MEGA, SOUL, INV1, INV2, etc. |

## Creating Custom Sprites

### Tools Needed
1. **SLADE3** or **XWE** - WAD editor
2. **DMGRSPR** or similar - Sprite graphic creator
3. Image files: 320x200 max, indexed color (256 colors)

### Sprite Dimensions
- Standard: 256x256 pixels max
- Typical: 64x64 to 128x128
- Must use Doom palette (256 colors)

### WAD Creation Steps

1. Create PNG/TGA sprites ( Doom palette)
2. Open SLADE3
3. Create new PWAD
4. Add S_START marker
5. Import sprites with correct 8-char names
6. Add S_END marker
7. Save as `goblin.wad`

### Loading PWADs

```bash
./goblin-doom -file goblin.wad
```

Engine loads PWAD sprites **overriding** IWAD sprites with matching names.

## Goblin Dice Rollaz Sprite Specs

### Goblin Player Character
- Replace PLAY sprites with goblin character
- Suggested prefix: `GOBL` (GOBLA1-GOBLHH)
- Animation: 8 frame walk cycle

### Dice-Themed Weapons

#### D6 Blaster (Starting Weapon)
- Replace: PISTOL (PISG/PISF)
- Sprite prefix: `D6BL`
- Animation: 4 frames fire

#### D20 Cannon
- Replace: Rocket Launcher
- Sprite prefix: `D20C`
- Animation: 5 frames fire, 3 frames ready

#### Percentile Gun
- Replace: Plasma Rifle
- Sprite prefix: `PERC`
- Animation: Continuous fire loop

#### Critical Hit Dagger
- Replace: Chainsaw
- Sprite prefix: `DAGG`
- Animation: 6 frames slash

### Dwarf Enemies

#### Standard Dwarf
- Replace: Imp (TROO)
- Prefix: `DWAR`
- Health: 60
- Attack: Melee (pickaxe)

#### Dwarf Berserker
- Replace: Demon (SARG)
- Prefix: `DBER`
- Health: 150
- Attack: Frenzied melee

#### Dwarf Engineer
- Replace: Cacodemon
- Prefix: `DENG`
- Health: 200
- Attack: Dynamite throwing

### Dice Ammo Pickups

#### Light Dice Ammo (d4, d6, d8)
- Replace: CLIP ammo
- Sprite prefix: `DLIG` (Dice LIGHT)
- Frames: DLIGA0-DLIGD0 (idle animation, 4 frames)
- Pickup gives: 5 light dice ammo
- Uses existing MT_AMMO_LIGHTDICE mobj type

#### Heavy Dice Ammo (d10, d12, d20, percentile)
- Replace: SHELL ammo
- Sprite prefix: `DHEAVY` (truncated to DHEA as 4-char)
- Frames: DHEAA0-DHEAD0 (idle animation, 4 frames)
- Pickup gives: 3 heavy dice ammo
- Uses existing MT_AMMO_HEAVYDICE mobj type

#### Arcane Dice Ammo (magic dice)
- Replace: CELL ammo  
- Sprite prefix: `DARC` (Dice ARCANE)
- Frames: DARCA0-DARCD0 (idle animation, 4 frames)
- Pickup gives: 10 arcane dice ammo
- Uses existing MT_AMMO_ARCANEDICE mobj type

> **Note**: The ammo mobj types are defined in the engine (info.h/info.c) and use placeholder sprites (S_CLIP) until custom sprites are created. The actual sprite assets need to be created and added to a WAD file.

## Implementation Notes

1. Sprite compatibility: Use standard Doom palette (doom.pal)
2. Transparency: Use color 247 as transparent
3. Naming: Must be exactly 8 characters, null-padded
4. Frame ordering: Sprite frames sorted alphabetically by engine
5. PWAD loading: -file loads after IWAD, overrides matching lumps

## References
- Doom Wiki: https://doomwiki.org/wiki/WAD
- Chocolate Doom source: src/w_wad.c, src/w_merge.c
