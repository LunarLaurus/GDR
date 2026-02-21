# Goblin Dice Rollaz Map Themes

## Overview

This document defines the three primary map themes for Goblin Dice Rollaz: Mine/Cavern, Forge/Lava, and Treasure Chamber. Each theme includes texture palettes, lighting rules, enemy weightings, environmental hazards, and room archetypes.

---

# Mine / Cavern Map Theme

## Theme Identity

The Mine/Cavern theme represents the underground lairs of the goblin tribes—dark, damp tunnels carved into mountain rock with rough-hewn timber supports and ancient mine cart rails.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| FLOOR0 | Cave Dirt | Dark brown, uneven cave floor |
| FLOOR1 | Mine Stone | Grey-brown stone with gravel |
| FLOOR2 | Wooden Plank | Warped wooden boards |
| FLOOR3 | Damp Rock | Wet-looking dark grey stone |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| ROCK1 | Rough Stone | Unhewn cave wall |
| ROCK2 | Layered Rock | Stratified rock face |
| ROCK3 | Mossy Stone | Stone with green patches |
| SUPPORT | Timber Beam | Vertical wood support |
| BRICK | Mine Brick | Red-brown brickwork |
| METAL | Rusted Rail | Iron mine rail |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| CEIL1 | Low Cave | Dark cave ceiling |
| CEIL2 | Timber Support | Exposed wooden beams |
| CEIL3 | Stone Arch | Rough stone overhead |

## Lighting Rules

- **Base light level**: 64 (dim, requires torch/powerup to see clearly)
- **Torch accent**: +32 light radius around light sources
- **Ambient occlusion**: -16 light in corners and alcoves
- **Color tint**: Warm orange bias (RGB: 255, 200, 150) for torchlit areas
- **Shadow zones**: 16 light level for unlit passages
- **Fog density**: Light fog effect at ranges > 640 units

### Light Source Types

1. **Torch** (point light): Radius 192, intensity 1.0, warm orange
2. **Campfire** (point light): Radius 256, intensity 1.2, flickering
3. **Glow Mushroom** (point light): Radius 128, intensity 0.8, green tint
4. **Cave Luminescence** (ambient): +8 global, blue-green tint

## Environmental Hazards

### Cave-In
- Trigger: Sector damage (5-15% health per second)
- Visual: Falling rock sprites, dust particles
- Audio: Rumbling sound
- Recovery: Exit hazard zone

### Pit Fall
- Trigger: Sector with floor 0, ceiling > floor + 64
- Damage: Instant death (fall damage equivalent)
- Visual: Darkness/fog below floor
- Prevention: Bridges, platforms, or marked paths

### Flooding
- Trigger: Timed sector with rising floor
- Speed: 0.5 to 2 units per second
- Effect: Pushes player, reduces movement speed 50%
- Damage: 1% per second while submerged

### Gas Vent
- Trigger: Periodic damage sector
- Cycle: 3 seconds active, 5 seconds dormant
- Damage: 2% per second while active
- Visual: Green mist sprite (future)

## Ambient Sound Profile

### Continuous
- Dripping water: Random interval 2-8 seconds
- Distant rumble: Low frequency, 30-second loop
- Wind howl: Occasional, 15-second interval

### Triggered
- Rock fall: On sector tag activation
- Mine cart: When riding mover object
- Goblin chatter: When enemies in adjacent sectors

### Background
- Cave echo: Reverb effect, 0.5 second decay
- Ambient music: Low, ominous drone

## Enemy Weighting

### Primary (60% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Goblin Scout | 25% |
| Goblin Grunt | 20% |
| Goblin Sneak | 15% |

### Secondary (30% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Miner | 15% |
| Goblin Shaman | 10% |
| Goblin Alchemist | 5% |

### Rare (10% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Goblin Totemist | 5% |
| Goblin King (mini-boss) | 3% |
| Dwarf Berserker | 2% |

### Encounter Density
- Small rooms: 1-3 enemies
- Medium rooms: 4-8 enemies
- Large chambers: 10-20 enemies
- Avoid spawn clusters > 25 (performance)

## Example Room Archetypes

### Mine Shaft
```
Sector configuration:
- Floor: 0 height
- Ceiling: 128 height  
- Size: 256x256 to 512x512
- Features: Central elevator, ladder (climb), platforms
- Enemy placement: Platforms and floor
- Light: Torch at floor level, shadow above 64
```

### Narrow Tunnel
```
Sector configuration:
- Floor: 0 height
- Ceiling: 80 height
- Size: 128x512 (corridor)
- Features: Mine cart rails, support beams
- Enemy placement: Ambush at tunnel end
- Light: Torch every 256 units
```

### Goblin Chamber
```
Sector configuration:
- Floor: 0 height (varied -32 to +32)
- Ceiling: 192 height
- Size: 512x512 to 1024x1024
- Features: Central fire pit, sleeping area, treasure pile
- Enemy placement: 5-15 enemies, varied
- Light: Campfire center (radius 256), shadows at edges
- Special: Powerup spawn in treasure pile
```

### Underground Lake
```
Sector configuration:
- Floor: -64 height (water)
- Ceiling: 128 height
- Size: 256x256 to 512x512
- Features: Pier/dock, boat (mover)
- Enemy placement: Enemies on piers, surface swimmers
- Light: Bioluminescent mushrooms, dim
- Special: Water damage (slow), no swimming ( Doom)
```

### Treasure Vault
```
Sector configuration:
- Floor: 0 height
- Ceiling: 96 height
- Size: 256x256
- Features: Locked door (tag triggered), gold textures
- Enemy placement: 3-5 guards
- Light: Gold glow (RGB: 255, 215, 0), +32 base
- Special: High-value loot drop guaranteed
```

## Mapper Checklist for Cavern Layouts

- [ ] All floor sectors use appropriate cave/stone textures
- [ ] Support beam textures on 50%+ of wall sectors
- [ ] Light levels verified: base 64, torch zones 96, shadows 16
- [ ] Torch objects placed every 256 units in corridors
- [ ] Campfire in rooms larger than 512x512
- [ ] Cave-in hazards marked in sector notes
- [ ] Pit hazards have visible floor markers
- [ ] Enemy spawn weights follow primary/secondary/rare ratios
- [ ] Ambient sound linedefs placed (drip, rumble, wind)
- [ ] Reverb zone linedef on all enclosed rooms
- [ ] No untextured sectors remaining
- [ ] Minimum 2 exits from any room > 256x256
- [ ] Choke points identified for gameplay flow

---

# Forge / Lava Map Theme

*(Document continues in map-themes-forge.md)*

---

# Treasure Chamber Map Theme

*(Document continues in map-themes-treasure.md)*
