# Enemy Definition Template

This document provides a template for adding custom enemies to Goblin Dice Rollaz.

## Overview

To add a new enemy, you need to modify several files:
- `src/doom/info.h` - Enemy type enum (`mobjtype_t`)
- `src/doom/info.c` - Enemy definition (mobjinfo struct)
- `src/doom/d_items.c` - Pickup/item definitions (if enemy drops items)
- `src/doom/p_enemy.c` - AI behavior extensions
- `src/doom/p_inter.c` - Damage/interaction logic
- `src/doom/d_englsh.h` - Enemy name strings (optional)

---

## Step 1: Add Enemy Type Enum

In `src/doom/info.h`, add your enemy to the `mobjtype_t` enum:

```c
// Add after existing enemies (e.g., after MT_GOBLIN_TOTEMIST):
MT_CUSTOM_ENEMY,       // Custom enemy template
NUMMOBJTYPES
```

**Important**: Add your enemy type BEFORE `NUMMOBJTYPES`.

---

## Step 2: Define Enemy Info

In `src/doom/info.c`, add to the `mobjinfo[]` array. Copy an existing enemy definition as template:

```c
{           // MT_CUSTOM_ENEMY
    8090,           // doomednum (unique mapthing ID, 8000+ for custom)
    S_CUST_STND,    // spawnstate (sprite state)
    50,             // spawnhealth (hit points)
    S_CUST_RUN1,    // seestate (alert animation)
    sfx_posit2,     // seesound (alert sound)
    8,              // reactiontime (AI think interval)
    sfx_pistol,     // attacksound (attack sound)
    S_CUST_PAIN,    // painstate (pain animation)
    100,            // painchance (0-100, likelihood to enter pain state)
    sfx_popain,     // painsound (pain sound)
    S_CUST_ATK1,    // meleestate (melee attack state, 0 if ranged only)
    S_CUST_ATK1,    // missilestate (ranged attack state, 0 if melee only)
    S_CUST_DIE1,    // deathstate (death animation)
    S_CUST_XDIE1,   // xdeathstate (gib animation)
    sfx_podth2,     // deathsound
    12,             // speed (movement speed in fracunits)
    16*FRACUNIT,    // radius (collision radius)
    40*FRACUNIT,    // height (collision height)
    200,            // mass (for momentum calculations)
    0,              // damage (contact damage, 0 for most enemies)
    sfx_posact,     // activesound (ambient sound)
    MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL,  // flags
    S_CUST_RAISE1,  // raisestate (resurrection animation, or S_NULL)
    20,             // crit_resistance (0-100, reduces crit chance)
    60,             // aggression (0-100, AI aggression level)
    0,              // shield_reduction (0-100, frontal damage reduction %)
    0,              // rear_weakness (0-100, rear damage bonus %)
    0               // phase2_health (0 = no boss phases)
},
```

---

## mobjinfo Field Reference

| Field | Type | Description |
|-------|------|-------------|
| `doomednum` | int | Map editor spawn ID (8000+ for custom) |
| `spawnstate` | int | State for idle/standing animation |
| `spawnhealth` | int | Starting hit points |
| `seestate` | int | State when player detected |
| `seesound` | int | Sound played when detecting player |
| `reactiontime` | int | AI think interval (tics) |
| `attacksound` | int | Sound when attacking |
| `painstate` | int | Pain animation state |
| `painchance` | int | 0-100 probability of pain reaction |
| `painsound` | int | Pain cry sound |
| `meleestate` | int | Melee attack state (0 = no melee) |
| `missilestate` | int | Ranged attack state (0 = no ranged) |
| `deathstate` | int | Death animation state |
| `xdeathstate` | int | Gib/explosion state |
| `deathsound` | int | Death sound |
| `speed` | int | Movement speed (FRACUNIT = ~1.0) |
| `radius` | int | Collision radius |
| `height` | int | Collision height |
| `mass` | int | Mass for knockback calculations |
| `damage` | int | Contact damage per frame |
| `activesound` | int | Ambient/idle sound |
| `flags` | int | MF_* behavior flags |
| `raisestate` | int | Resurrection state |
| `crit_resistance` | int | Goblin Dice Rollaz: Reduces crit chance (0-100) |
| `aggression` | int | Goblin Dice Rollaz: AI aggression (0-100) |
| `shield_reduction` | int | Goblin Dice Rollaz: Frontal damage reduction % |
| `rear_weakness` | int | Goblin Dice Rollaz: Rear damage bonus % |
| `phase2_health` | int | Goblin Dice Rollaz: Boss phase threshold |

### Common Flags

| Flag | Description |
|------|-------------|
| `MF_SOLID` | Blocks movement |
| `MF_SHOOTABLE` | Can be damaged |
| `MF_NOGRAVITY` | Flying unit |
| `MF_DROPOFF` | Can fall off ledges |
| `MF_PICKUP` | Can pick up items |
| `MF_NOTDMATCH` | Not in deathmatch |
| `MF_COUNTKILL` | Counts toward kill total |
| `MF_COUNTITEM` | Counts toward item total |
| `MF_SPECIAL` | Triggers `A_ItemPicker` on touch |
| `MF_MISSILE` | Is a projectile |

---

## Step 3: Define State Animations

In `src/doom/info.c`, add state definitions for your enemy. States map to sprite frames in your WAD.

```c
// Example states for custom enemy
S_CUST_STND,   S_CUST_STND,   0,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_RUN1,   S_CUST_RUN2,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_RUN2,   S_CUST_RUN3,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_RUN3,   S_CUST_RUN4,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_RUN4,   S_CUST_RUN1,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_ATK1,   S_CUST_ATK2,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_ATK2,   S_CUST_ATK3,   8,      A_FaceTarget, 0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_ATK3,   S_CUST_RUN1,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_PAIN,   S_CUST_RUN1,   4,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_DIE1,   S_CUST_DIE2,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_DIE2,   S_CUST_DIE3,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_DIE3,   S_CUST_DIE4,   8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
S_CUST_DIE4,   S_NULL,        8,      0,      0,  S_NULL, 0,  0, 0, 0, 0, 0,
```

### State Format

```
S_STATENAME,  nextstate,  tics,  action1,  action2,  action3,  misc1, misc2, misc3, misc4, misc5, misc6
```

---

## Step 4: Add AI Behavior (Optional)

For custom AI behaviors, modify `src/doom/p_enemy.c`:

### Faction Definition

```c
// Add to P_GetFaction() switch statement:
case MT_CUSTOM_ENEMY:
    return FACTION_GOBLIN;  // or FACTION_DWARF
```

### Tactic Definition

```c
// Add to P_GetTactic() switch statement:
case MT_CUSTOM_ENEMY:
    return TACTIC_FLANK;  // or TACTIC_HOLD, TACTIC_CHARGE, TACTIC_RANGED
```

### Custom Attack Logic

```c
// In P_MissileAttack() or P_MeleeAttack():
case MT_CUSTOM_ENEMY:
    // Custom attack code
    break;
```

### Leader/Morale

```c
// In P_CanBeLeader():
case MT_CUSTOM_ENEMY:
    return true;  // Can be squad leader

// In P_CanFollow():
case MT_CUSTOM_ENEMY:
    return true;  // Can follow leader
```

---

## Step 5: Enemy Stat Guidelines

| Enemy Type | Health | Speed | Pain Chance | Notes |
|------------|--------|-------|-------------|-------|
| Basic Grunt | 20-30 | 8-10 | 100 | Fodder |
| Ranged | 15-25 | 6-8 | 50 | Keep distance |
| Heavy | 50-80 | 4-6 | 20 | Tank |
| Elite | 40-60 | 10-12 | 30 | Fast, dangerous |
| Boss | 200+ | 4-8 | 10 | Multi-phase |

### Difficulty Scaling

| Difficulty | Health Multiplier | Damage Multiplier |
|------------|-------------------|-------------------|
| ITYTD | 0.5x | 0.5x |
| HNTR | 1.0x | 1.0x |
| UV | 1.0x | 1.0x |
| NM | 1.5x | 1.5x |

---

## Example: Creating a "Dwarf Flamethrower"

### 1. Add Enum (info.h)
```c
MT_DWARF_FLAMER,   // Dwarf with flame weapon
```

### 2. Add Definition (info.c)
```c
{           // MT_DWARF_FLAMER
    8025,           // doomednum
    S_SPOS_STND,    // reuse Imp animations
    45,             // spawnhealth
    S_SPOS_RUN1,    // seestate
    sfx_posit2,     // seesound
    10,             // reactiontime
    0,              // attacksound (handled in AI)
    S_SPOS_PAIN,    // painstate
    80,             // painchance
    sfx_popain,     // painsound
    S_SPOS_ATK1,    // meleestate (backup if too close)
    S_SPOS_ATK1,    // missilestate (flame attack)
    S_SPOS_DIE1,    // deathstate
    S_SPOS_XDIE1,   // xdeathstate
    sfx_podth2,     // deathsound
    6,              // speed (slow, stationary)
    16*FRACUNIT,    // radius
    40*FRACUNIT,    // height
    250,            // mass (heavy)
    0,              // damage
    sfx_posact,     // activesound
    MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL,
    S_SPOS_RAISE1,
    25,             // crit_resistance
    40,             // aggression (defensive)
    0,              // shield_reduction
    0,              // rear_weakness
    0               // phase2_health
},
```

### 3. Add AI Behavior (p_enemy.c)

In `P_MissileAttack()`:
```c
case MT_DWARF_FLAMER:
    P_SpawnMissileAngle(actor, MT_FLAME_PROJECTILE, actor->angle, 0);
    break;
```

---

## Troubleshooting

### Enemy Not Spawning
- Verify `doomednum` is unique (8000+)
- Check enum is before `NUMMOBJTYPES`
- Ensure state numbers are valid

### Enemy Doesn't Take Damage
- Check `MF_SHOOTABLE` flag is set
- Verify `spawnhealth` > 0

### AI Not Working
- Confirm faction is defined in `P_GetFaction()`
- Check `reactiontime` is not 0

### Custom Attack Not Firing
- Verify `missilestate` or `meleestate` is not 0
- Ensure attack logic is in `p_enemy.c`

### Critical Hits Not Working
- Set `crit_resistance` appropriately (0-100)
- Remember base crit is 10% (roll of 20 on d20)

---

## References

- See `src/doom/info.c` for full enemy implementations (MT_DWARF_*, MT_GOBLIN_*)
- See `src/doom/info.h` for mobjtype_t enum
- See `src/doom/p_enemy.c` for AI behavior
- See `src/doom/p_inter.c` for damage calculations
- See `DESIGN.md` for combat mechanics
