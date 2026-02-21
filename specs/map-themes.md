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

## Theme Identity

The Treasure Chamber theme represents the legendary vaults of the dwarven kingdoms—opulent chambers filled with gold, precious gems, and ancient artifacts. These are the most dangerous rooms in the game, serving as both the ultimate reward and the ultimate challenge for adventurers.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| GOLD1 | Polished Gold | Shiny golden floor tiles |
| GOLD2 | Coin Carpet | Floor covered in scattered coins |
| GEM1 | Crystal Floor | Semi-transparent gem-embedded stone |
| GEM2 | Emerald Tile | Deep green emerald mosaic |
| TREASURE1 | Rich Carpet | Red velvet floor covering |
| TREASURE2 | Marble Inlay | White and grey marble pattern |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| VAULT1 | Gold Wall | Solid gold plate decoration |
| VAULT2 | Gem Encrusted | Wall with embedded gems |
| VAULT3 | Ancient Stone | Weathered but ornate stone |
| PILLAR1 | Gold Pillar | Cylindrical gold support |
| PILLAR2 | Gem Pillar | Crystal-embedded column |
| DOOR_GOLD | Treasure Door | Ornate golden door |
| CHEST | Treasure Chest | Large chest graphic texture |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| VAULT_C1 | Dome Ceiling | Curved gold dome |
| VAULT_C2 | Beam Ceiling | Crossed golden beams |
| VAULT_C3 | Mosaic Ceiling | Artistically tiled ceiling |

## Lighting Rules

- **Base light level**: 128 (bright, celebratory atmosphere)
- **Gold reflection**: +32 light near gold surfaces
- **Gem glow**: +48 light radius around gem formations, multi-color tint
- **Ambient warmth**: Golden yellow bias (RGB: 255, 220, 150)
- **Highlight spots**: 192 light level on treasure displays
- **Shadow corners**: 64 light level (still brighter than other themes)
- **Chandelier**: Point light radius 384, intensity 1.5, warm white

### Light Source Types

1. **Chandelier** (point light): Radius 384, intensity 1.5, warm white
2. **Gem Formation** (point light): Radius 128, multi-color based on gem type
3. **Treasure Glow** (point light): Radius 96, golden yellow
4. **Torch Sconce** (point light): Radius 192, orange (less common here)

## Environmental Hazards

### Pressure Plate Trap
- Trigger: Player enters tagged sector
- Effect: Arrow projectiles from walls (8 directions)
- Damage: 10% per hit, 3-second cooldown
- Visual: Floor tile depression animation
- Audio: Click/whoosh sound
- Counter: Disarm via switch in adjacent alcove

### Pit Drop Trap
- Trigger: Floor sector with tag activation
- Effect: Floor collapses, player falls (sector below)
- Damage: 25% fall damage + trap damage
- Visual: Crumbling floor animation
- Recovery: Climb out via side ledges or respawn
- Counter: Detect via switch that reveals safe path

### Treasure Curse
- Trigger: Picking up high-value treasure (tagged special item)
- Effect: Random status effect (Burning, Frozen, or Dice Curse)
- Duration: 15 seconds
- Visual: Purple particle effect around player
- Audio: Mysterious cackle/curse sound
- Counter: None (accept risk for reward)

### Gas Chamber
- Trigger: Sector activation via switch
- Effect: Knockback + 5% damage per second
- Visual: Green gas sprite filling sector
- Audio: Hissing sound
- Counter: Exit before full fill (2-second window)

## Ambient Sound Profile

### Continuous
- Echoing footsteps: When player moves, reverb 1.0s decay
- Distant dripping: Water in distant caves, 10-second interval
- Gem hum: Subtle magical resonance, constant low tone

### Triggered
- Treasure pickup: Satisfying chime
- Trap activation: Mechanical click/mechanism sound
- Curse trigger: Evil laugh/eerie whoosh

### Background
- Grand ambience: Deep, resonant echo
- Subtle magical drone: Mystical undertone
- No heavy music (vaults should feel still and silent)

## Enemy Weighting

### Primary (40% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Defender | 20% |
| Armored Dwarf | 20% |

### Secondary (40% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Captain | 15% |
| Dwarf Marksman | 15% |
| Dwarf Miner | 10% |

### Rare (20% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Bombardier | 8% |
| Goblin Sneak (invader) | 7% |
| Goblin King (mini-boss) | 5% |

### Elite Encounters
- Treasure chambers always spawn at least 1 elite enemy
- 50% chance of mini-boss (Goblin King or Dwarf Captain)
- Boss variant appears in chambers with 3+ elite spawn points

### Encounter Density
- Small vault: 2-4 enemies (guarding specific treasure)
- Medium vault: 5-8 enemies (standard treasure room)
- Grand vault: 10-15 enemies (major loot location)
- legendary vault: 15-25 enemies + boss (end-game reward)

## Powerup Spawn Weighting

### Guaranteed Drops
- Every treasure chamber contains at least 1 powerup
- Small vault: 1 guaranteed (common)
- Medium vault: 2 guaranteed (1 common, 1 uncommon)
- Grand vault: 3 guaranteed (1 common, 1 uncommon, 1 rare)
- Legendary vault: 4 guaranteed (1 of each tier)

### Rarity Tiers

| Tier | Powerups | Spawn Rate |
|------|----------|------------|
| Common | Ammo, Health, Armor | 60% |
| Uncommon | Double Damage, Critical Hit | 30% |
| Rare | Dice Fortune, Max Health, Max Armor | 9% |
| Legendary | All Rare + Boss Key | 1% |

### Special Treasure Items
- Goblin Crown: Triggers boss fight, spawns Goblin King
- Dwarven Crown: Triggers boss fight, spawns Dwarven War Machine
- These replace powerups in standard legendary vaults Archetypes



## Example Room### Small Vault
```
Sector configuration:
- Floor: 0 height
- Ceiling: 96 height
- Size: 256x256
- Features: Single treasure display, 1-2 guardians
- Enemy placement: Guards near treasure
- Light: Chandelier center (radius 384), bright
- Special: Guaranteed 1 common powerup
```

### Standard Treasure Room
```
Sector configuration:
- Floor: 0 height (varied platforms)
- Ceiling: 128 height
- Size: 512x512
- Features: Multiple treasure piles, ornate pillars
- Enemy placement: 5-8 guards, formation
- Light: Multiple chandeliers, gem glow
- Special: 2 powerups, pressure plate trap
```

### Grand Vault
```
Sector configuration:
- Floor: -64 to 32 height (varied)
- Ceiling: 192 height
- Size: 512x512 to 1024x1024
- Features: Central massive treasure, multiple chambers
- Enemy placement: 10-15 guards, wave spawns
- Light: Massive chandelier, golden glow throughout
- Special: 3 powerups, multiple trap types
```

### Legendary Vault
```
Sector configuration:
- Floor: Multiple levels (-128 to 64)
- Ceiling: 256 height (dome)
- Size: 768x768 to 1024x1024
- Features: Crown pedestals, boss arena
- Enemy placement: 15-25 enemies + boss
- Light: Intense golden glow, pulsing gem light
- Special: Boss encounter, 4 powerups, crown treasure
```

### Treasure Alcove
```
Sector configuration:
- Floor: 0 height
- Ceiling: 64 height
- Size: 128x128
- Features: Single treasure item on pedestal
- Enemy placement: None (optional challenge)
- Light: Spot light on treasure
- Special: Quick grab and go, escape required
```

## Shared Theme Documentation Template

For future theme expansion, use this template:

```
# [Theme Name] Map Theme

## Theme Identity
[One paragraph describing the theme's atmosphere and story]

## Texture Palette
### Floor/Wall/Ceiling Tables
[Include at least 3 variants of each]

## Lighting Rules
- Base: [number]
- Ambient bias: [RGB values]
- Special effects: [descriptions]

## Environmental Hazards
### [Hazard Name]
- Trigger: [what activates it]
- Effect: [what happens]
- Counter: [how to avoid]

## Enemy Weighting
[Primary/Secondary/Rare tables]

## Room Archetypes
[At least 3 example room types]

## Mapper Checklist
[5-10 verification items]
```

## Example WAD Structure

To demonstrate all themes, an example WAD should include:

```
TREASURE.WAD
├── MAP01: Mine Entrance (tutorial)
├── MAP02-MAP05: Mine/Cavern levels
├── MAP06-MAP09: Forge/Lava levels
├── MAP10: Grand Vault (treasure)
├── MAP11-MAP14: Mixed themes
├── MAP15: Legendary Vault (finale)
├── SNDINFO: Theme ambient sounds
├── ACS: Trap and hazard scripts
└── DECORATE: Custom actors
```

## Encounter Pacing Guidelines

### Per Theme

| Theme | Combat Pacing | Rest Opportunities |
|-------|---------------|---------------------|
| Mine/Cavern | Medium (ambushes) | Many alcoves |
| Forge/Lava | Fast (formations) | Limited |
| Treasure Chamber | Intense (waves) | After clear |

### Treasure Chamber Specific
- Wave-based spawning (2-3 waves per room)
- 5-second delay between waves
- Treasure becomes accessible after final wave
- No respawning once all enemies cleared

### Flow Between Themes
- Mine → Forge: Descend deeper
- Forge → Treasure: Find vault door
- Treasure → Mine: Escape sequence

## Internal Review Checklist for Map Consistency

### General
- [ ] All sectors properly textured (no gaps)
- [ ] No floating or intersecting geometry
- [ ] All doors and switches functional
- [ ] Path connectivity verified (all areas reachable)
- [ ] Proper line-of-sight for combat

### Treasure Chamber Specific
- [ ] Lighting consistent with theme (bright, golden)
- [ ] Treasure display areas properly lit (spotlights)
- [ ] Trap triggers tested and working
- [ ] Powerup spawns match weighting rules
- [ ] Elite/boss encounters properly configured
- [ ] Sound environment has proper echo
- [ ] Minimum enemy count per vault size met
- [ ] Difficulty scaling applied per skill level

### Theme Transitions
- [ ] Visual transition between themes feels natural
- [ ] No abrupt lighting changes without context
- [ ] Enemy type changes match new theme

### Balance Verification
- [ ] Powerup value matches room difficulty
- [ ] Ammo drops sufficient for encounter length
- [ ] Health pickups placed after major fights
- [ ] No impossible encounters without options

### Performance
- [ ] No excessive sector/line counts
- [ ] Texture usage within limits
- [ ] Lighting calculations performant
