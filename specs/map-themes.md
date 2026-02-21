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

## Theme Identity

The Forge/Lava theme represents the deep dwarven forges and volcanic chambers where molten metal flows and ancient war machines are constructed—a harsh, fiery environment of industrial machinery and natural geothermal power.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| FLAOR1 | Molten Rock | Glowing red-orange cracked stone |
| FLAOR2 | Iron Grate | Metal floor with gaps |
| FLAOR3 | Cooled Lava | Dark red/black hardened lava |
| FLAOR4 | Metal Plate | Industrial steel flooring |
| FLAOR5 | Ash Floor | Grey powdery volcanic ash |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| LAVA1 | Magma Wall | Glowing lava flow on vertical surface |
| LAVA2 | Cooling Rock | Dark red cooling basalt |
| FORGE1 | Stone Forge | Heat-stained grey stone |
| FORGE2 | Brick Forge | Firebrick red industrial brick |
| ANVIL | Anvil Block | Black iron anvil surface |
| CHAIN | Hanging Chain | Vertical chain texture |
| GEAR | Large Gear | Bronze/copper gear decoration |
| COOLER | Cooling Rack | Water-cooled metal bars |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| FORGE_C1 | Smoke Ceiling | Dark with soot stains |
| FORGE_C2 | Beam Ceiling | Exposed iron support beams |
| FORGE_C3 | Vent Ceiling | Industrial ventilation grates |

## Lighting Rules

- **Base light level**: 96 (forge glow provides ambient light)
- **Lava glow**: +48 light radius around lava sectors, red tint (RGB: 255, 100, 50)
- **Forge light**: +64 light radius around active forges, orange tint (RGB: 255, 180, 100)
- **Ambient orange bias**: All lighting in forge theme shifted warm
- **Shadow zones**: 32 light level in corners (still brighter than caverns)
- **Heat shimmer**: Visual distortion effect near lava (future)

### Light Source Types

1. **Molten Lava** (sector light): Radius variable, intensity based on sector size, red-orange
2. **Forge Fire** (point light): Radius 256, intensity 1.4, orange-yellow
3. **Lantern** (point light): Radius 192, intensity 1.0, warm yellow
4. **Ember** (point light): Radius 64, intensity 0.6, flickering red

## Environmental Hazards

### Lava Floor
- Trigger: Sector with lava texture, floor height < 0
- Damage: 10% health per second
- Visual: Animated lava texture, rising heat particles
- Prevention: Only crossable via bridges, platforms, or with powerup
- Note: Player cannot stand on lava sectors

### Steam Vent
- Trigger: Periodic linedef action
- Cycle: 2 seconds active, 4 seconds dormant
- Damage: 5% per second while active
- Visual: White steam sprite, obscuration effect
- Audio: Hissing sound

### Molten Splash
- Trigger: Random timed event in lava-adjacent sectors
- Damage: 15% (one-time hit)
- Visual: Projectile sprite from lava surface
- Warning: Subtle screen flash before launch

### Hot Metal
- Trigger: Sector with "heated floor" flag
- Damage: 2% per second (persistent)
- Visual: Orange glow on floor texture
- Mitigation: Move quickly through affected areas

## Ambient Sound Profile

### Continuous
- Forge hammering: Rhythmic, 1-3 second intervals
- Lava bubbling: Constant low rumble
- Bellows/fan: Periodic whooshing, 5-second cycle

### Triggered
- Steam hiss: On vent activation
- Metal clanging: On mover object movement
- Dwarf voice: Combat chatter when enemies present

### Background
- Factory ambience: Mechanical drone, echoes
- No ambient music (industrial soundscape)

## Enemy Weighting

### Primary (60% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Miner | 25% |
| Armored Dwarf | 20% |
| Dwarf Marksman | 15% |

### Secondary (30% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Engineer | 15% |
| Dwarf Defender | 10% |
| Dwarf Berserker | 5% |

### Rare (10% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Captain | 5% |
| Dwarf Bombardier | 3% |
| Goblin Alchemist (invader) | 2% |

### Encounter Density
- Small rooms: 2-4 enemies (dwarves hold formation)
- Medium rooms: 5-10 enemies
- Large chambers: 12-25 enemies
- Arena fights: Up to 30 enemies

## Example Room Archetypes

### Main Forge
```
Sector configuration:
- Floor: 0 height
- Ceiling: 192 height
- Size: 512x512 to 1024x1024
- Features: Central anvil, magma pit, storage barrels
- Enemy placement: 6-15 enemies, formation positions
- Light: Multiple forge fires, lava glow
- Special: Destructible props (barrels explode)
```

### Lava River
```
Sector configuration:
- Floor: -32 height (lava)
- Ceiling: 128 height
- Size: 128x512 (corridor)
- Features: Bridge platform (solid), lava sides
- Enemy placement: Defenders on bridge
- Light: Lava glow from below, embers rising
- Special: Fall = death, bridge = only path
```

### Armory
```
Sector configuration:
- Floor: 0 height
- Ceiling: 96 height
- Size: 256x256 to 512x256
- Features: Weapon racks, armor stands
- Enemy placement: 4-8 guards, stationary posts
- Light: Lanterns, warm orange tint
- Special: High-tier weapon pickup spawn
```

### magma Chamber
```
Sector configuration:
- Floor: -64 height (deep lava pool)
- Ceiling: 256 height
- Size: 512x512
- Features: Central island, lava falls
- Enemy placement: Boss arena setup
- Light: Intense lava glow, pulsing
- Special: Boss encounter space
```

### Cooling Room
```
Sector configuration:
- Floor: 0 height (water layer)
- Ceiling: 128 height
- Size: 256x256
- Features: Cooling racks, steam vents
- Enemy placement: 3-5 enemies
- Light: Dim, blue-white tint
- Special: Steam provides concealment
```

## Mapper Checklist for Forge Maps

- [ ] All floor sectors use appropriate forge/lava textures
- [ ] Lava sectors properly tagged for damage (no standing allowed)
- [ ] Light levels verified: base 96, forge zones 160+, lava 144
- [ ] Lava animations present on all lava textures
- [ ] Steam vent linedefs configured with timing
- [ ] Forge fire objects placed in working forges
- [ ] Metal hazard sectors marked in sector notes
- [ ] Enemy spawn weights follow dwarf-heavy ratios
- [ ] Ambient sound linedefs placed (hammer, bubble)
- [ ] No untextured sectors remaining
- [ ] Minimum 2 exits from any forge room
- [ ] Exploding barrel placements noted
- [ ] Boss arena check if room > 768x768

---

# Treasure Chamber Map Theme

*(Document continues in map-themes-treasure.md)*
