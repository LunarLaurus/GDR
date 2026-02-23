# Goblin Raiders - Example Total Conversion Pack

This document provides a complete example of creating a goblin-themed Total Conversion (TC) pack for Goblin Dice Rollaz.

## Overview

The **Goblin Raiders** TC replaces Doom's marine/space marine protagonist with a goblin warrior fighting against invading dwarves in their underground mines. This example demonstrates:

- Custom player character (goblin)
- Replacing existing enemies with thematic variants
- Custom dice weapons
- Map theme integration
- DEHACKED/BEX patching

---

## Project Structure

A TC pack WAD should contain:

```
GOBLINRC.WAD
├── ANIMATED    # Animation sequences
├── COLORMAP    # Color translation tables
├── DEMO1       # Demo lump (optional)
├── DENORMAL    # Endoom graphic (optional)
├── FLAT        # Floor/ceiling textures
├── GENMIDI     # Music (optional)
├── GRAPHICS    # UI graphics, sprites, textures
├── MAPINFO     # Map definitions (if new maps)
├── MUSICS      # Music tracks
├── PATCH       # Audio/SFX replacements
├── PLAYPAL     # Color palettes
├── SNDINFO     # Sound definitions
├── SPRITES     # Sprite definitions
├── TEXTURE1    # Texture definitions
├── TX_HI       # High-resolution textures (optional)
├── ZMUSIC      # Music (optional)
└── [MAPS]      # Level data (MAP01, MAP02, etc.)
```

---

## Step 1: Player Character (Goblin)

### Sprite Requirements

| Sprite Prefix | Frames | Description |
|--------------|--------|-------------|
| PLAY | 20 | Player idle/walk/attack |
| PSTA | 5 | Pistol (default weapon) |
| PAMB | 5 | Arm bob |
| PTNT | 8 | Projectile |
| BLOOD | 3 | Blood particles |

### Replacement Strategy

Since vanilla Doom cannot replace the player sprite via `-file`, use `-merge`:

```bash
./goblin-doom -merge goblinrc.wad -iwad DOOM.WAD
```

### Custom Goblin Sprites

Create 64x64 pixel sprites with green tint palette:

```
GOBLINA1 - GOBLINA8  (idle/stand)
GOBLINB1 - GOBLINB8  (walk cycle)
GOBLINP1 - GOBLINP4  (attack frames)
GOBLIND1 - GOBLIND8  (pain)
GOBLINE1 - GOBLINE6  (death)
```

### Code Changes (Optional - Requires Engine Modification)

To fully replace the player model, modify `src/doom/info.c`:

```c
// Replace MT_PLAYER sprite
mobjinfo[MT_PLAYER].spawnstate = S_GOBLIN_STAND;
mobjinfo[MT_PLAYER].height = 36*FRACUNIT;  // Goblins are shorter
mobjinfo[MT_PLAYER].radius = 12*FRACUNIT;
```

---

## Step 2: Enemy Replacements

### Goblin Warrior (Replaces Zombieman)

| Original | Replacement | Health | Speed | Notes |
|----------|--------------|--------|-------|-------|
| MT_TROOP | MT_GOBLIN_WARRIOR | 15 | 12 | Fast, melee |
| MT_SERGEANT | MT_GOBLIN_AXEMAN | 25 | 10 | Axe wielder |
| MT_SHADOWS | MT_GOBLIN_SNEAK | 10 | 16 | Stealthy |
| MT_PAIN | MT_GOBLIN_SHAMAN | 40 | 8 | Spellcaster |

### Sprite Naming Convention

Doom sprite names are 4 characters:

```
First 4: Base name (GOBL)
5th: Frame (A, B, C...)
6th: Rotation (1-8)
```

Example: `GOBLA1D1` = Goblin, Frame A, Rotation 1, Direction D (down)

### Goblin Warrior Definition

In `src/doom/info.c`:

```c
{           // MT_GOBLIN_WARRIOR
    8100,           // doomednum
    S_GOBL_STND,   // spawnstate
    15,             // spawnhealth (low HP)
    S_GOBL_RUN1,   // seestate
    sfx_posit2,    // seesound (goblin yell)
    8,              // reactiontime
    sfx_pistol,    // attacksound
    S_GOBL_PAIN,   // painstate
    200,           // painchance (high - easily staggered)
    sfx_popain,    // painsound
    S_GOBL_ATK1,   // meleestate
    0,             // missilestate (no ranged)
    S_GOBL_DIE1,   // deathstate
    S_GOBL_XDIE1,  // xdeathstate
    sfx_podth2,    // deathsound
    12,            // speed (fast!)
    14*FRACUNIT,   // radius (small)
    32*FRACUNIT,   // height (short)
    50,            // mass
    0,             // damage
    sfx_posact,    // activesound
    MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL|MF_FLOORCLIP,
    S_GOBL_RAISE1,
    30,            // crit_resistance
    90,            // aggression
    0,             // shield_reduction
    10,            // rear_weakness (backstab bonus)
    0
},
```

### Goblin AI Modifications

In `src/doom/p_enemy.c`:

```c
case MT_GOBLIN_WARRIOR:
    return FACTION_GOBLIN;  // New faction for goblins

case MT_GOBLIN_WARRIOR:
    return TACTIC_CHARGE;   // Goblins charge directly
```

---

## Step 3: Weapon Replacements

### Dice Weapons Theme

Replace Doom weapons with dice-themed versions:

| Original | Dice Replacement | Die Type | Damage |
|----------|-------------------|----------|--------|
| Pistol | Dice Blaster | d6 | 2-12 |
| Shotgun | Scatter Dice | 2×d6 | 4-24 |
| Chaingun | Rapid Roll | d8 | 8-48 |
| Rocket Launcher | Explosive Die | d12 | 12-144 |

### Weapon Definitions

In `src/doom/d_items.c`:

```c
{   // wp_goblinblade - Goblin Dagger
    4,      // die_type: d4
    25,     // crit_chance: 25% (roll of 1 on d4)
    2,      // crit_multiplier: 2x
    1,      // min_damage
    4,      // crit_roll: 4
    {1, 2, 3, 4, 6, 8, 10},  // damage_table
    0,      // gamble_shot
    0,      // ricochet
    0,      // misfire
    0,      // misfire_penalty
    SPF_ALWAYS_SPAWN,  // spawn flags
    100,    // spawn_weight (common)
    CRIT_SCALING_LINEAR,
    1
},
```

---

## Step 4: Map Theme Integration

### Mine/Cavern Theme

Create maps using the mine/cavern theme specification:

#### Texture Palettes

| Category | Textures | Source |
|----------|----------|--------|
| Floor | FLOOR4_8, FLOOR5_1, MFLR8_1 | Use WAD textures |
| Wall | STONE6, STONE3, BRICK5 | Replace via TEXTURE1 |
| Support | WOOD5, METAL | Timber/beam props |

#### Sector Setup

```c
// Cavern sector (low light, warm)
sector_t cavern = {
    floorheight: 0,
    ceilingheight: 128,
    floorpic: FLOOR4_8,
    ceilingpic: CEILING3,
    lightlevel: 48,  // Dark!
    flags: 0
};
```

#### Environmental Hazards

| Hazard | Implementation | Damage |
|--------|---------------|--------|
| Lava pit | Sector type 666 | 5/sec |
| Pit | Sector type 666 | 10/sec |
| Drowning | Sector type 667 | - |

---

## Step 5: Sound Replacements

### Audio Mapping

Replace original sounds with thematic alternatives:

| Original | Goblin Replacement |
|----------|-------------------|
| sfx_posit1 | Goblin chatter |
| sfx_popain | Goblin squeal |
| sfx_podth1 | Goblin death |
| sfx_pistol | Dice roll clatter |
| sfx_sawful | Metal clash |

### SNDINFO Lump Format

```lump
$Sound

sgobchrt = "goblin_chatter" // Alert sound
sgobpain = "goblin_squeal"  // Pain sound  
sgobdeth = "goblin_screams" // Death sound
sdicerol = "dice_shake"     // Weapon fire
scrithit = "crit_slam"      // Critical hit
```

---

## Step 6: Pickups and Items

### Goblin Dice Pickups

| Item | Sprite | Effect |
|------|--------|--------|
| d4 Pickup | DICE4 | +8 d4 ammo |
| d6 Pickup | DICE6 | +6 d6 ammo |
| d8 Pickup | DICE8 | +5 d8 ammo |
| d12 Pickup | DICE12 | +3 d12 ammo |
| d20 Pickup | DICE20 | +3 d20 ammo |
| d100 Pickup | DICE00 | +1 d100 ammo |

### Powerup Sprites

| Powerup | Sprite | Duration | Effect |
|---------|--------|----------|--------|
| Critical Boost | PSTR_CRIT | 30s | +25% crit |
| Double Damage | PSTR_DMG2 | 20s | 2× damage |
| Fortune | PSTR_FRNTY | 1 hit | Guaranteed crit |
| Speed | PSTR_SPED | 60s | +50% speed |
| Health | BON1 | Instant | +25% HP |

---

## Step 7: DEHACKED/BEX Patching

### BEX Pointer Format

Create a `DECORATE` or BEX file for simpler modifications:

```dehacked
// Weapon replacements via BEX
Thing 2001 (Pistol) -> Goblin Dice Blaster
  Frame 0 -> GOBLWEAP
  Damage 1d6 -> 1d6
  
// Enemy replacements
Thing 9 (Zombieman) -> MT_GOBLIN_WARRIOR
  Health 20 -> 15
  Speed 8 -> 12
```

### Loading the Patch

```bash
./goblin-doom -merge goblinrc.wad -deh custom.deh
```

---

## Step 8: Creating the WAD

### Using SLADE3

1. Open SLADE3
2. Create new WAD file
3. Add graphics to `GRAPHICS` namespace
4. Add sounds to `SNDINFO` namespace  
5. Add maps to `MAP` namespace
6. Save as `goblinrc.wad`

### Using Command Line

```bash
# Combine multiple WADs
wadmerge doom.wad custom.wad -o goblinrc.wad

# Or use python-wadtools
python -c "import wads; wads.merge(['doom.wad', 'sprites.wad'], 'goblinrc.wad')"
```

---

## Complete Example: Map E1M1 - Goblin Camp

### Map Header

```
LEVEL 01
NAME "Goblin Camp"
INTERPIC "HELP"
MUSIC "E1M1"
FLAGS "NOCLIP,TRIPLE"

# Player start (goblin)
# Thing type 1, position (1024, 2048), angle 0

# Goblin warriors
# Thing type 8100 (MT_GOBLIN_WARRIOR), 8 enemies
# Thing type 8101 (MT_GOBLIN_AXEMAN), 2 enemies
# Thing type 8102 (MT_GOBLIN_SHAMAN), 1 enemy

# Items
# d4 ammo: Thing type 2004
# Health: Thing type 2018
# Dice powerup: Thing type 2015
```

### Example Sector Layout

```
Sector 0: Floor 0, Ceiling 128, Flat FLOOR4_8, Light 96
Sector 1: Floor -16, Ceiling 128, Flat NUKAGE1, Light 48, Damage 5
Sector 2: Floor 0, Ceiling 64, Flat FLOOR5_1, Light 64 (tunnel)
```

---

## Testing Your TC Pack

### Basic Load Test

```bash
./goblin-doom -merge goblinrc.wad -iwad DOOM.WAD -warp 1
```

### Debug Commands

```bash
# Show all objects
nolid

# God mode
iddqd

# All weapons
idkfa

# Show FPS
showfps

# Dump entity info
thingstat
```

### Validation Checklist

- [ ] No crashes on startup
- [ ] Player sprite visible
- [ ] Enemies spawn correctly
- [ ] Weapons fire and deal damage
- [ ] Critical hits work
- [ ] Powerups function
- [ ] Map theme applied
- [ ] Sounds play correctly

---

## Performance Considerations

### Sprite Limits

- Maximum ~2000 sprites in memory
- Use 64×64 or 128×128 pixel sprites
- Optimize with DOOM flats (64×64)

### Map Optimization

- Keep sector count under 200
- Use linedef specials sparingly
- Avoid complex sector effects in early maps

---

## Distribution

### Release Checklist

- [ ] All sprites included
- [ ] All sounds included
- [ ] Maps tested on all difficulties
- [ ] No missing assets (check console)
- [ ] README with installation instructions
- [ ] Version number in WAD name

### Version Naming

```
goblinrc_v1.0.wad  - Initial release
goblinrc_v1.1.wad  - Bug fixes
goblinrc_v2.0.wad  - New content
```

---

## Troubleshooting

### Common Issues

| Error | Cause | Fix |
|-------|-------|-----|
| "Sprite not found" | Missing sprite lump | Add to GRAPHICS namespace |
| "Sound not found" | Missing SNDINFO | Add sound and entry |
| "Invalid map" | Bad format | Check map lumps |
| "Too many sprites" | Memory | Reduce sprite count |

### Debugging Steps

1. Run with `-debug` flag
2. Check console for errors
3. Use `path` command to verify WAD loading
4. Test with minimal content first

---

## References

- See `/specs/wad-sprites.md` for sprite creation guide
- See `/specs/map-themes.md` for theme documentation
- See `/specs/custom-weapon-template.md` for weapon creation
- See `/specs/enemy-template.md` for enemy creation
- See `/docs/MODDING.md` for general modding guide
- See `src/doom/info.c` for enemy definitions
- See `src/doom/d_items.c` for weapon definitions
