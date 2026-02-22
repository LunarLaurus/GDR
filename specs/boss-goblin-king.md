# Goblin King Boss Specification

## Overview
The Goblin King serves as the primary finale boss for Goblin Dice Rollaz, appearing at the end of the final episode. He is a large, throne-room boss with multiple combat phases that escalate in difficulty.

## Boss Profile

### Basic Stats
- **Name**: Goblin King
- **Map Spawn ID**: 88
- **Health**: 5000 (Phase 1), 4000 (Phase 2), 3000 (Phase 3) = 12000 total
- **Speed**: 8 (slow, throne-room bound)
- **Radius**: 48 * FRACUNIT
- **Height**: 120 * FRACUNIT
- **Mass**: 1500
- **Crit Resistance**: 25%
- **Pain Chance**: 10%
- **Reward**: 5000 exp, exclusive "Crown of Fortune" pickup

## Phase Breakdown

### Phase 1: "The Throne" (100%-66% HP)
- **Health**: 5000
- **Duration**: Until 66% HP
- **Behavior**: Stationary throne combat
- **Attacks**:
  - **Dice Barrage**: Fires 5 random die-type projectiles in spread pattern
    - Cooldown: 4 seconds
    - Damage: Based on die type rolled (1-20)
  - **Summon Minions**: Spawns 3-4 goblin underlings
    - Cooldown: 12 seconds
    - Types: MT_GOBLIN_SCOUT, MT_GOBLIN_SNEAK
  - **Royal Roar**: AoE knockback + brief player stun (1 second)
    - Cooldown: 15 seconds

### Phase 2: "The Mad Rush" (66%-33% HP)
- **Health**: 4000
- **Duration**: Until 33% HP  
- **Transition**: Health bar flashes orange, screen shakes
- **Behavior**: Mobile combat, throne destroyed
- **Attacks**:
  - **Dice Smash**: Melee attack, ground slam creating shockwave
    - Damage: 25-50 (dice roll based)
    - Radius: 256 units
    - Cooldown: 3 seconds
  - **Scattered Barrage**: 8 projectiles in all directions
    - Cooldown: 5 seconds
  - **Goblin Frenzy**: Buffs all nearby goblins (+50% damage, +30% speed)
    - Duration: 10 seconds
    - Cooldown: 20 seconds

### Phase 3: "Dice God Unleashed" (33%-0% HP)
- **Health**: 3000
- **Duration**: Until death
- **Transition**: Visual transformation (crown glows red), music intensifies
- **Behavior**: Aggressive pursuit, erratic movement
- **Attacks**:
  - **Catastrophic Roll**: Massive damage attack, charges up over 2 seconds
    - Warning: Screen flashes, target reticle appears
    - Damage: 100-200 (rolls 2d100)
    - Unblockable, must dodge
    - Cooldown: 8 seconds
  - **Reality Warp**: Teleports to random position in arena
    - Cooldown: 6 seconds
  - **Chaos Rain**: Random projectiles rain from ceiling for 5 seconds
    - Damage: 10-30 each
    - Coverage: Full arena

## Technical Implementation

### Actor Definition
```
MT_GOBLIN_KING - New mobj type
- doomednum: 88
- spawnstate: S_KING_STND
- spawnhealth: 12000
- seestate: S_KING_RUN1
- seesound: sfx_kingsit
- painstate: S_KING_PAIN
- painchance: 10
- meleestate: S_KING_SMASH
- missilestate: S_KING_ATK1
- deathstate: S_KING_DIE1
- deathsound: sfx_kingdth
- speed: 8
- radius: 48*FRACUNIT
- height: 120*FRACUNIT
- damage: 0
- flags: MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL
- crit_resistance: 25
```

### Phase Management
- Store current phase in `player->bossphase` or custom actor field
- Check health thresholds in `A_BossPain` or damage handler
- Trigger phase transition when crossing thresholds:
  - 8000 HP → Phase 2
  - 4000 HP → Phase 3

### State Definitions Required
```
S_KING_STND    - Idle/throne pose
S_KING_RUN1-4  - Movement frames  
S_KING_PAIN    - Pain reaction
S_KKING_ATK1-4 - Dice barrage windup/fire
S_KING_SMASH1-3 - Melee smash attack
S_KING_TELEPORT1-4 - Teleport effect
S_KING_CHARGE1-2 - Catastrophic roll charge
S_KING_RAIN1-4 - Chaos rain attack
S_KING_DIE1-8 - Death sequence (explosion, gibs)
S_KING_RAISE1-4 - Resurrection (if applicable)
```

### Sound Effects
- `sfx_kingsit` - Boss alert/see player
- `sfx_kingpain` - Pain cry
- `sfx_kingdth` - Death explosion
- `sfx_kingsmash` - Ground smash
- `sfx_kingtele` - Teleport whoosh
- `sfx_kingroar` - Phase transition roar

### Map Placement
- Boss arena: Minimum 2048x2048 units
- Required: Central throne platform, pillar cover
- Recommended: Pit hazards (lava/damage floor)
- Spawn point: Center of arena on throne

## Difficulty Scaling

### Nightmare Mode
- +50% HP (18000 total)
- -25% phase transition delay (faster)
- +25% damage output

### Ultra-Violence
- Standard values as defined

## Reward System

### Crown of Fortune (MT_CROWN_OF_FORTUNE)
- **Type**: Powerup
- **Effect**: +25% crit chance for 30 seconds
- **Duration**: Single pickup, non-stacking
- **Sprite**: Golden crown with gem
- **Spawn ID**: 89

## Multiplayer Considerations
- Boss targets closest player
- Damage shared across all players
- Phase transitions sync'd deterministically
- All players receive reward on kill

## Sprite Requirements
- Idle throne pose (8 frames)
- Walking/running (4 frames)
- Attack animations per type (4-8 frames each)
- Death explosion sequence (8 frames)
- Phase transition transformation
