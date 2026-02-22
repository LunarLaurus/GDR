# Dwarven War Machine Boss Specification

## Overview
The Dwarven War Machine serves as a mid-campaign boss encounter, appearing in forge/lava themed maps. It is a heavy mechanical construct - a war engine piloted by a dwarf engineer, featuring projectile-heavy combat and mechanical destruction visuals.

## Boss Profile

### Basic Stats
- **Name**: Dwarven War Machine
- **Map Spawn ID**: 90
- **Health**: 4000 (Phase 1), 3500 (Phase 2) = 7500 total
- **Speed**: 4 (slow, methodical movement)
- **Radius**: 64 * FRACUNIT
- **Height**: 96 * FRACUNIT
- **Mass**: 3000
- **Crit Resistance**: 40%
- **Pain Chance**: 5%
- **Reward**: 3000 exp, "Bomb Pack" ammo refill

## Phase Breakdown

### Phase 1: "Industrial Terror" (100%-50% HP)
- **Health**: 4000
- **Duration**: Until 50% HP
- **Behavior**: Slow methodical advance, heavy projectile output
- **Attacks**:
  - **Cannon Barrage**: Primary ranged attack, fires explosive cannonballs
    - Damage: 40-80 (2d20 + 20)
    - Radius: 128 units (AoE splash)
    - Cooldown: 3 seconds
    - Projectile speed: 12
  - **Machine Gun Sweep**: Rapid-fire projectile stream
    - Damage: 10-15 per hit (1d6 + 9)
    - Duration: 2 seconds
    - Cooldown: 8 seconds
  - **Smoke Screen**: Emits obscuring smoke, brief invisibility
    - Duration: 3 seconds
    - Cooldown: 20 seconds

### Phase 2: "Overdrive" (50%-0% HP)
- **Health**: 3500
- **Duration**: Until death
- **Transition**: Engine glows bright orange, smoke trails increase
- **Behavior**: Faster movement, more aggressive, starts self-repair
- **Attacks**:
  - **Flame Jets**: Front-facing cone attack
    - Damage: 20-40 (2d10) per tick
    - Duration: 2 seconds
    - Cooldown: 6 seconds
  - **Rocket Barrage**: 12 explosive rockets in spread
    - Damage: 50-100 per rocket (5d20)
    - Total potential: 600-1200 if all hit
    - Cooldown: 10 seconds
  - **Emergency Repair**: Self-heals 500 HP
    - Duration: 4 seconds (vulnerable)
    - Cooldown: 30 seconds
    - Interruptible by damage
  - **Charge Attack**: Ram attack with windup
    - Damage: 100-150 (5d30)
    - Warning: 1 second charge animation
    - Cooldown: 15 seconds

## Technical Implementation

### Actor Definition
```
MT_WAR_MACHINE - New mobj type
- doomednum: 90
- spawnstate: S_WARMC_STND
- spawnhealth: 7500
- seestate: S_WARMC_RUN1
- seesound: sfx_warmchl
- painstate: S_WARMC_PAIN
- painchance: 5
- meleestate: S_WARMC_CHARGE1
- missilestate: S_WARMC_ATK1
- deathstate: S_WARMC_DIE1
- deathsound: sfx_warmcdt
- speed: 4
- radius: 64*FRACUNIT
- height: 96*FRACUNIT
- damage: 0
- flags: MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL|MF_NOGRAV
- crit_resistance: 40
```

### Phase Management
- Track current phase in custom actor field `phase`
- Check health thresholds in damage handler
- Trigger phase transition when crossing 3750 HP
- Phase 2 flags: increased speed, damage output

### State Definitions Required
```
S_WARMC_STND    - Idle/engine idle animation
S_WARMC_RUN1-4  - Movement frames (tracked tread animation)
S_WARMC_PAIN    - Pain reaction (metal clang)
S_WARMC_ATK1-4  - Cannon barrage windup/fire
S_WARMC_GUN1-4  - Machine gun sweep
S_WARMC_SMOKE1-3 - Smoke screen emission
S_WARMC_FLAME1-4 - Flame jets attack
S_WARMC_ROCK1-5 - Rocket barrage
S_WARMC_REPAIR1-4 - Self-repair sequence
S_WARMC_CHARGE1-3 - Charge attack windup/execute
S_WARMC_DIE1-8  - Explosion sequence
S_WARMC_DIE9    - Wreckage remain (non-solid)
```

### Sound Effects
- `sfx_warmchl` - Engine start/see player
- `sfx_warmcpn` - Cannon fire
- `sfx_warmcgun` - Machine gun
- `sfx_warmcpain` - Pain (metal hit)
- `sfx_warmcdt` - Death explosion
- `sfx_warmcflm` - Flame jets
- `sfx_warmcrok` - Rocket launch
- `sfx_warmcrep` - Repair sound

### Map Placement
- Boss arena: Minimum 1536x1536 units
- Required: Open area for charge attack
- Recommended: Lava pits, steam vents
- Spawn point: Arena center
- Navigation: Ensure no dead-ends for charge

## Difficulty Scaling

### Nightmare Mode
- +50% HP (11250 total)
- +25% damage output
- -33% repair interval

### Ultra-Violence
- Standard values as defined

## Reward System

### Bomb Pack (MT_BOMB_PACK)
- **Type**: Ammo pickup
- **Effect**: Grants 50 heavy dice ammo
- **Quantity**: Single use
- **Sprite**: Bundle of explosive charges
- **Spawn ID**: 91

## Multiplayer Considerations
- Boss targets closest player
- Damage shared across all players
- Phase transitions sync'd deterministically
- All players receive reward on kill
- Charge attack targets random player

## Sprite Requirements
- Idle engine pose (4 frames)
- Movement tread animation (4 frames)
- Attack animations per type (4-8 frames)
- Death explosion sequence (8 frames)
- Smoke/vapor trail overlays

## Weak Points (Optional Advanced)
- **Main Cannon**: 25% bonus damage when shot from front
- **Engine Vents**: 25% bonus damage when shot from rear (if exposed)
- **Pilot Cockpit**: 50% bonus damage (small hitbox)

## Environmental Interactions
- Immune to lava damage (fireproof)
- Can trigger steam vent sector damage
- Destroys destructible pillars
- Can activate locked doors via proximity

## Design Notes
- Inspired by the Spiderdemon but more mobile
- Projectile variety distinguishes from Goblin King
- Phase 2 "berserk" mode creates tension
- Self-repair creates tactical choice: interrupt or DPS
