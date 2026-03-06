# Goblin Dice Rollaz - Modding Guide

This guide covers creating mods (PWADs) for Goblin Dice Rollaz.

## Overview

Goblin Dice Rollaz is a total conversion of Chocolate Doom that replaces firearms with dice-based weaponry. All damage is determined by rolling dice (d4, d6, d8, d10, d12, d20, d100) with critical hit mechanics.

### Key Concepts

- **Dice Roll Damage**: Every weapon rolls 1-N to determine damage
- **Critical Hits**: Rolling the weapon's crit number triggers enhanced damage
- **Misfire**: High-risk weapons (percentile dice) can misfire
- **Deterministic RNG**: All rolls use Doom's P_Random() for demo compatibility

---

## Creating a WAD File

### Basic WAD Structure

A Goblin Dice Rollaz PWAD uses the standard Doom WAD format:

```
MYMOD.WAD
├── MAPINFO            # Map definitions
├── THINGS             # Actors, items, weapons
├── LINEDEFS           # Geometry
├── SIDEDEFS           # Textures
├── VERTEXES           # Map vertices
├── SEGS               # Line segments
├── SSECTORS           # Subsectors
├── SECTORS            # Sectors
├── NODES              # BSP nodes
├── BLOCKMAP           # Collision data
├── SCRIPTS            # ACS scripts (optional)
├── SNDINFO            # Sound definitions (optional)
├── DEHACKED           # Engine modifications (optional)
└── TEXTURES           # Custom textures (optional)
```

### Loading Your WAD

```bash
# Using -merge (recommended for sprite replacements)
./goblin-doom -merge mymod.wad

# Using -file (sprite replacements won't work in vanilla Doom)
./goblin-doom -file mymod.wad
```

---

## Dice Weapons

### Weapon Definition

Dice weapons are defined in `src/doom/d_items.c` using the `dice_weapon_info_t` structure:

```c
typedef struct
{
    int die_type;           // Number of sides (4, 6, 10, 12, 20, 100)
    int crit_chance;        // Base crit chance (percent)
    int crit_multiplier;    // Damage multiplier on crit
    int min_damage;         // Minimum roll that deals damage
    int crit_roll;          // Roll value that triggers crit
    int damage_table[7];    // Damage mapping for roll ranges
    int gamble_shot;        // Wide variance mode
    int ricochet_bounces;   // Number of ricochets (0 = none)
    int misfire_roll;       // Roll at/below triggers misfire
    int misfire_penalty;    // Damage multiplier on misfire
    weapon_spawn_flags_t spawn_flags;
    int spawn_weight;
    crit_scaling_type_t crit_scaling_type;
    int crit_scaling_param;
} dice_weapon_info_t;
```

### Example: Custom d8 Weapon

To add a custom d8 weapon, define it in the `dice_weapon_info` array:

```c
[wp_mycustom] =
{
    .die_type = 8,
    .crit_chance = 12,      // 12.5% (roll of 8)
    .crit_multiplier = 2,
    .min_damage = 2,
    .crit_roll = 8,
    .damage_table = {2, 4, 6, 8, 10, 12, 16},  // buckets 0-5, crit_base
    .gamble_shot = 0,
    .ricochet_bounces = 0,
    .misfire_roll = 0,
    .misfire_penalty = 0,
    .spawn_flags = SPF_ALWAYS_SPAWN,
    .spawn_weight = 50,
    .crit_scaling_type = CRIT_SCALING_LINEAR,
    .crit_scaling_param = 0
}
```

### Damage Table Reference

The damage table maps roll ranges to buckets:

| Roll Range | Bucket Index |
|------------|--------------|
| 1          | damage_table[0] |
| 2          | damage_table[1] |
| 3          | damage_table[2] |
| 4          | damage_table[3] |
| 5          | damage_table[4] |
| 6          | damage_table[5] |
| Crit       | damage_table[6] (crit_base) |

### Spawn Flags

Control weapon appearance with these flags:

| Flag | Description |
|------|-------------|
| `SPF_NONE` | Default spawn rules |
| `SPF_RARE` | Reduced spawn frequency |
| `SPF_EARLY_GAME` | Maps 1-8 only |
| `SPF_LATE_GAME` | Maps 24+ only |
| `SPF_BOSS_AREA` | Boss arenas only |
| `SPF_ALWAYS_SPAWN` | Always available |
| `SPF_HIDDEN` | Secret/trigger required |
| `SPF_DIFFICULTY_EASY` | Easy mode only |
| `SPF_DIFFICULTY_HARD` | Hard mode bonus |

### Crit Scaling Types

| Type | Description |
|------|-------------|
| `CRIT_SCALING_LINEAR` | Simple multiplier |
| `CRIT_SCALING_EXPONENTIAL` | Multiplier increases with roll |
| `CRIT_SCALING_BONUS_FLAT` | Flat bonus to damage |
| `CRIT_SCALING_BONUS_PERCENT` | Percentage bonus |
| `CRIT_SCALING_CRIT_CHANCE` | Crit chance affects multiplier |

---

## Enemies

### Enemy Definition Template

New enemies are defined in `src/doom/info.c` using mobjinfo_t:

```c
// Example: Goblin Scout
mobjinfo_t(
    MT_GOBLIN_SCOUT,        // Type
    "Goblin Scout",         // Name
    S_SCOUT_STND,          // Spawn state
    S_SCOUT_RUN,           // See state
    S_SCOUT_ATK,           // Attack state
    S_SCOUT_PAIN,          // Pain state
    S_SCOUT_DIE,           // Death state
    S_SCOUT_XDIE,          // XDeath state
    0,                      // Spawn health
    30,                     // Reaction time
    8,                      // Pain chance
    8 * FRACUNIT,           // Speed
    12 * FRACUNIT,          // Radius
    56 * FRACUNIT,          // Height
    100,                    // Mass
    0,                      // Damage (0 = uses weapon)
    0,                      // Active sound
    0,                      // Attack sound
    0                       // Spawn sound
)
```

### Enemy Flags

| Flag | Description |
|------|-------------|
| `MF_SOLID` | Blocks movement |
| `MF_SHOOTABLE` | Can be shot |
| `MF_NOGRAVITY` | Floats/flies |
| `MF_DROPOFF` | Can fall off ledges |
| `MF_PICKUP` | Picks up items |
| `MF_NOCLIP` | No collision |
| `MF_SLIDE` | Slides along walls |
| `MF_FLOAT` | Floating random movement |
| `MG_ALWAYSPICKUP` | Always picks up |
| `MF_COUNTKILL` | Counts toward kill percentage |
| `MF_COUNTITEM` | Counts toward item percentage |
| `MF_SPECIAL` | Triggers special on touch |

---

## Powerups

### Powerup Framework

Powerups use the standard Doom artifact system with custom thinkertimers:

```c
// Powerup types
typedef enum
{
    PWUP_NONE = 0,
    PWUP_CRIT,           // Critical Hit boost
    PWUP_DOUBLEDAMAGE,   // Double damage
    PWUP_FORTUNE,        // Guaranteed next-hit crit
    PWUP_INVULNERABILITY,
    PWUP_INVISIBILITY,
    PWUP_RADIATIONSUIT,
    PWUP_ALLMAP,
    PWUP_LIGHTAMP,
    // Add custom powerups here
} powerup_type_t;
```

### Adding Custom Powerups

1. Define the powerup type in the enum
2. Add pickup logic in `p_inter.c`
3. Add thinker timer in `p_tick.c`
4. Add HUD indicator in `st_stuff.c`
5. Add pickup sprite in WAD

---

## Status Effects

### Status Effect Types

| Effect | Duration | Visual |
|--------|----------|--------|
| `SFX_BURNING` | 5 seconds | Red overlay, particle sparks |
| `SFX_FROZEN` | 3 seconds | Blue tint, slow movement |
| `SFX_STUNNED` | 2 seconds | Dizzy stars, no attack |
| `SFX_DICECURSE` | 10 seconds | Purple tint, random damage variance |

### Implementing Status Effects

Status effects are implemented in the player thinker:

```c
// Status effect structure
typedef struct
{
    int type;           // Status effect type
    int duration;       // Remaining duration (ticks)
    int damage;         // DOT damage (if applicable)
    int source;         // Source actor (for credit)
} player_status_t;
```

---

## Map Theming

### Cavern/Mine Theme

For goblin-cave themed maps:

- **Textures**: Rock, timber, rails (use -merge for sprite replacements)
- **Lighting**: Low light, warm torch tones
- **Ambient**: Drips, distant rumble
- **Enemies**: Goblin-heavy (Scout, Sneak, Alchemist)
- **Hazards**: Cave-ins, bottomless pits

### Forge/Lava Theme

For dwarven-forge themed maps:

- **Textures**: Lava floors, anvils, chains, grates
- **Lighting**: Red/orange bias, bright sparks
- **Ambient**: Steam hisses, hammering
- **Enemies**: Dwarf-heavy (Defender, Marksman, Miner, Engineer)
- **Hazards**: Lava damage sectors (damage = 4/tick), steam vents

### Treasure Chamber Theme

For high-value reward rooms:

- **Textures**: Gold, gems, ornate stone
- **Lighting**: Bright with echo ambience
- **Enemies**: Elite presets, boss-scale challenges
- **Rewards**: Guaranteed powerup spawns
- **Hazards**: Pressure plates, drop floors

---

## Configuration Variables

### Gameplay CVARs

| Variable | Default | Description |
|----------|---------|-------------|
| `goblin_crit_chance` | 5 | Base crit chance (%) |
| `goblin_exploding` | 0 | Enable exploding dice |
| `goblin_advantage` | 0 | Enable advantage rolls |
| `goblin_combo_multiplier` | 1.0 | Crit combo multiplier |
| `goblin_luck` | 0 | Luck stat modifier |
| `rpg_mode` | 0 | Enable RPG progression |

### Debug Commands

| Command | Description |
|---------|-------------|
| `showfps` | Toggle FPS counter |
| `god` | Toggle god mode |
| `iddqd` | Toggle invincibility |
| `give all` | Give all weapons/ammo |
| `give powerup <n>` | Give powerup by ID |
| `showstats` | Show weapon damage stats |

---

## Sprite Naming

### Weapon Sprites

Weapon sprites follow Doom naming conventions:

```
P - Pistol (not used in GDR)
S - Shotgun
C - Chaingun
M - Missile
L - Plasma
B - BFG
K - Chainsaw
D - Super Shotgun
```

For dice weapons, extend with state suffixes:

```
D6B - d6 Blaster (ready)
D6A1- D6B - Attack frames
D6U  - d6 Blaster (raise)
D6D  - d6 Blaster (lower)
```

### Enemy Sprites

Enemy sprites use 8-character Doom format:

```
XXXXNNCC
XXXX = 4-char sprite name
NN   = Frame number (A0-A9, B0-B9, etc.)
CC   = Rotation (01-08)
```

Example: `SCOUAB5` = Goblin Scout frame B, rotation 5

---

## Sprite Replacement

### Critical: Use -merge Not -file

**Vanilla Doom cannot load sprites from PWAD via `-file`.** You must use `-merge`:

```bash
# WRONG - sprites won't load
./goblin-doom -file mymod.wad

# CORRECT - sprites will load
./goblin-doom -merge mymod.wad
```

This is a fundamental limitation of the Doom engine. The `-file` option only loads additional WAD data (maps, sounds, textures), not sprite replacements.

### Sprite Naming Convention

Doom sprites use an 8-character format: `XXXXNNCC`

| Position | Meaning | Examples |
|----------|---------|----------|
| XXXX | 4-char sprite name | TROO, PLAY, GOBL |
| NN | Frame (A0-A9, B0-B9) | A0, B3, C1 |
| CC | Rotation (01-08) | 01, 05, 08 |

Example: `GOBL A1` = Goblin sprite, frame A1, rotation 1

### Required Sprite Frames

For enemies, you typically need:

| Frame | Purpose |
|-------|---------|
| S**T**ND | Standing Idle |
| S**R**UN | Running |
| S**A**TTACK | Attacking |
| S**P**AIN | Pain reaction |
| S**D**IE | Death |
| S**X**DIE | Explosive death |

For weapons:

| Frame | Purpose |
|-------|---------|
| **B** | Ready/Idle |
| **A**1-**A**N | Attack frames |
| **U** | Raise weapon |
| **D** | Lower weapon |

### Sprite Dimensions

- Maximum width: 256 pixels
- Maximum height: 256 pixels
- Optimal size: 128x128 for enemies
- Use transparent backgrounds (Doom uses color index 247 as transparent)

### Palette Requirements

Sprites must use the Doom palette (256 colors). Use tools like SLADE3 to convert:

```bash
# Example with ImageMagick (requires palette)
convert input.png -remap doom.pal output.png
```

### Tools for Sprite Creation

1. **SLADE3** - Best for WAD editing and sprite management
2. **Doom Builder** - Map editing with sprite preview
3. **Photoshop/GIMP** - Create sprites, export with Doom palette

### Example: Replacing Imp Sprites

1. Create new sprites named:
   - TROOA1.png through TROOA8.png (attack)
   - TROOB1.png through TROOB8.png (death)
   - etc.

2. Add to WAD in a SPRITES lump or as loose PNGs

3. Load with:
   ```bash
   ./goblin-doom -merge mysprites.wad
   ```

### Common Issues

| Issue | Solution |
|-------|----------|
| Sprites not showing | Use `-merge`, not `-file` |
| Wrong colors | Convert to Doom palette |
| Black squares | Ensure transparent color is index 247 |
| Wrong size | Resize to power of 2 or standard Doom sizes |

---

## DEHACKED Reference

### Modifying Weapons via DEHACKED

```
[Weapons]
# Modify weapon damage, ammo usage, etc.
```

### Modifying Enemies

```
[Things]
# Modify enemy health, speed, etc.
```

---

## Best Practices

1. **Use -merge for sprites**: Vanilla Doom cannot load sprites via -file
2. **Test on multiple difficulties**: Balance spawn flags appropriately
3. **Validate deterministic behavior**: Use demo recording to verify sync
4. **Avoid floating-point in gameplay**: Use fixed-point math
5. **Keep actor IDs unique**: Don't conflict with existing Doom IDs
6. **Document custom content**: Include README in your WAD

---

## Tools

- **WAD Author**: https://wadauthor.sourceforge.net/
- **SLADE3**: https://slade.mancubus.net/
- **Doom Builder**: https://www.doombuilder.com/
- **DEHACKED**: Part of Chocolate Doom source

---

*Last updated for Goblin Dice Rollaz v0.1*
