# Spawn Group Presets for Map Designers

## Overview

Spawn group presets allow map designers to place pre-configured enemy formations using a single mapthing. Instead of manually placing each enemy, designers can place a group spawner that creates multiple enemies in a tactical formation.

## Technical Implementation

### Group Spawner Thing IDs

Group spawners use reserved thing IDs in the 9000-9099 range:

| ID | Name | Description |
|----|------|-------------|
| 9000 | Goblin Squad | 3-4 goblins in formation |
| 9001 | Goblin Patrol | 2 goblins in line |
| 9002 | Goblin Ambush | 3 goblins with sneak AI |
| 9010 | Dwarf Team | 3 dwarves in formation |
| 9011 | Dwarf Patrol | 2 dwarves in line |
| 9012 | Dwarf Defense | 2 defenders + 1 marksman |
| 9020 | Mixed Force | 2 goblins + 2 dwarves |
| 9021 | Elite Guard | 1 captain + 2 berserkers |
| 9030 | Wave Spawner | Used for scripted waves |

### Mapthing Fields Used

When placing a group spawner, the standard mapthing fields control behavior:

- **angle**: Formation orientation (0, 45, 90, etc.)
- **options**: Skill level flags (same as regular things)

### Formation Patterns

The angle field determines formation orientation:

| Angle Value | Formation |
|-------------|-----------|
| 0 | Facing East |
| 45 | Facing Northeast |
| 90 | Facing North |
| 135 | Facing Northwest |
| 180 | Facing West |
| 225 | Facing Southwest |
| 270 | Facing South |
| 315 | Facing Southeast |

Enemies spawn at fixed offsets from the group center, rotated by the angle value.

## Spawn Offsets

### Goblin Squad (9000)

```
      G
     G G
     C
```
- Center: group leader (Scout)
- Offset 1: +32px forward, +16px left (Scout)
- Offset 2: +32px forward, +16px right (Scout)
- Offset 3: +64px forward (Scout)

### Goblin Ambush (9002)

All goblins spawn with MF_AMBUSH flag (surprise attack):
- Center: Sneak
- Offsets: Surrounding positions

### Dwarf Defense (9012)

```
   M
 D D
```
- Marksman (M): rear position, ranged
- Defenders (D): front positions, blocking

## Skill Scaling

Group spawners respect skill level options:

| Options Value | Spawns On |
|---------------|-----------|
| 1 | Easy only |
| 2 | Medium only |
| 3 | Easy + Medium |
| 4 | Hard only |
| 5 | Easy + Hard |
| 6 | Medium + Hard |
| 7 | All difficulties |

## Placement Guidelines

### Room Size Recommendations

| Group Type | Minimum Room Size |
|------------|-------------------|
| Goblin Squad | 256x256 |
| Dwarf Team | 384x256 |
| Elite Guard | 512x512 |
| Wave Spawner | Arena-sized |

### Positioning Tips

1. Place group spawners at center of intended engagement area
2. Ensure adequate spacing from walls (128px minimum)
3. For ambush groups, place near cover/doorways
4. Consider sight lines when placing ranged groups

## Examples

### Simple Room (MAP01)

```
Place at room center:
- Thing 9000 (Goblin Squad) at (512, 512), angle 0
- This spawns 4 goblins in formation facing east
```

### Corridor Ambush

```
Place at corridor junction:
- Thing 9002 (Goblin Ambush) at (256, 256), angle 90
- This spawns 3 goblins with ambush flag facing north
```

### Defensive Position

```
Place at chokepoint:
- Thing 9012 (Dwarf Defense) at (384, 384), angle 0
- Spawns 2 defenders + 1 marksman
```

## Debug Commands

```
spawngroup <id>  // Test spawn a group at player position
groupstats       // Show active group spawns
```

## Custom Group Definition

Advanced mappers can define custom groups in code by editing `p_mobj.c`:

```c
// Group definitions array
typedef struct {
    short type;              // Thing ID (9000-9099)
    short count;             // Number of enemies
    short enemy_type[8];     // Enemy thing IDs
    short offset_x[8];      // X offsets from center
    short offset_y[8];      // Y offsets from center
    boolean ambush;         // All have MF_AMBUSH
} spawn_group_t;
```

## See Also

- `src/doom/p_mobj.c` - P_SpawnMapThing function
- `src/doom/doomdata.h` - mapthing_t structure
- `specs/spawn-tuning.md` - Difficulty-based spawn tuning
- `specs/wad-sprites.md` - Sprite naming conventions
