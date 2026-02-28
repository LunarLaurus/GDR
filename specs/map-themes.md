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

## Barracks of Stone Guard Map Theme

## Theme Identity

The Barracks of Stone Guard theme represents the fortified living quarters and training halls of the elite dwarven warriors—stone-walled corridors lined with weapon racks, armor stands, and ceremonial banners. These are structured military environments with strategic chokepoints and defensive positions.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| BARRACK1 | Stone Floor | Grey flagstone tiles |
| BARRACK2 | Wooden Plank | Warped combat training boards |
| BARRACK3 | Dusty Stone | Worn grey stone with debris |
| BARRACK4 | Iron Plate | Reinforced metal floor sections |
| RUNWAY1 | Training Track | Long worn path in training area |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| STONE1 | Cut Stone | Precision hewn wall blocks |
| STONE2 | Weathered Stone | Aged stone with cracks |
| BANNER1 | Clan Banner | Red dwarf clan flag |
| BANNER2 | War Banner | Black battle standard |
| RACK1 | Weapon Rack | Wall-mounted armory |
| ARMOR1 | Armor Stand | Wall-mounted plate display |
| TORCH1 | Torch Sconce | Wall-mounted torch holder |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| BARRACK_C1 | Timber Beam | Exposed wooden support beams |
| BARRACK_C2 | Stone Arch | Curved stone ceiling |
| BARRACK_C3 | Flag Ceiling | Flat stone ceiling with flags |

## Lighting Rules

- **Base light level**: 80 (military efficiency, bright but functional)
- **Torch light**: +48 light radius around wall sconces, warm orange
- **Lantern posts**: +64 light radius in open areas
- **Chandelier**: Central point light, radius 320, intensity 1.4, warm white
- **Ambient bias**: Neutral warm (RGB: 255, 240, 220)
- **Shadow zones**: 48 light level in alcoves and storage areas

### Light Source Types

1. **Wall Torch** (point light): Radius 192, intensity 1.0, flickering orange
2. **Chandelier** (point light): Radius 320, intensity 1.4, warm white
3. **Lantern Post** (point light): Radius 256, intensity 1.2, steady yellow
4. **Banner Glow** (ambient): +16, red tint near clan banners

## Environmental Hazards

### Chandelier Drop
- Trigger: Sector tag activation or player proximity to weakened chain
- Damage: 25% health (crush), knockback
- Visual: Falling chandelier sprite, glass shatter particles
- Audio: Loud crash, screaming metal
- Counter: Shoot chain to drop early, avoid marked zones

### Trap Door
- Trigger: Step on tagged floor sector
- Effect: Player falls to sector below (64 unit drop)
- Damage: 15% fall damage + trap damage
- Visual: Floor panel opens downward
- Counter: Look for slightly different floor texture, move carefully

### Wall Spike
- Trigger: Proximity to tagged wall section
- Damage: 10% per hit, rapid fire
- Visual: Spike sprites emerge from wall
- Audio: Metal pneumatic punch sound
- Counter: Stay center of corridors, avoid walls in spike zones

### Armor Trap
- Trigger: Picking up or touching armor on rack
- Effect: Triggers nearby alarm, spawns reinforcements
- Visual: Armor falls, alarm sound
- Audio: Clanging alarm
- Counter: Don't touch displayed armor, find alternate path

## Ambient Sound Profile

### Continuous
- Distant marching: Rhythmic dwarf footsteps, 4-second interval
- Training clashing: Weapons sparring, random 3-8 seconds
- Echoing halls: General barracks ambience, constant

### Triggered
- Chandelier creak: Before drop hazard triggers
- Trap activation: Click/clunk sounds
- Alarm: When armor traps triggered

### Background
- Military drum: Slow beat, authoritative
- Dwarf chanting: Battle songs, distant
- No ambient music (barracks are active, noisy)

## Enemy Weighting

### Primary (55% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Defender | 25% |
| Armored Dwarf | 20% |
| Dwarf Marksman | 10% |

### Secondary (35% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Captain | 15% |
| Dwarf Veteran | 10% |
| Dwarf Engineer | 5% |
| Dwarf Commander | 5% |

### Rare (10% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Warlord | 5% |
| Dwarf Elite Guard | 3% |
| Dwarf Ironclad | 2% |

### Encounter Density
- Corridors: 1-2 enemies (guarding posts)
- Barracks rooms: 3-6 enemies (living quarters)
- Training halls: 8-15 enemies (group training)
- Command center: 5-10 enemies (elite defense)

## Example Room Archetypes

### Barracks Corridor
```
Sector configuration:
- Floor: 0 height
- Ceiling: 96 height
- Size: 128x512 (corridor)
- Features: Weapon racks, armor stands, torch sconces
- Enemy placement: Guards at posts
- Light: Torch every 128 units, moderate
- Special: Wall spike traps in narrow sections
```

### Sleeping Quarters
```
Sector configuration:
- Floor: 0 height
- Ceiling: 80 height
- Size: 256x256 to 512x256
- Features: Bunks, personal items, storage chests
- Enemy placement: 3-6 sleeping or alerted guards
- Light: Lanterns at corners, dim
- Special: Don't wake sleeping enemies (stealth optional)
```

### Training Hall
```
Sector configuration:
- Floor: 0 height
- Ceiling: 128 height
- Size: 512x512
- Features: Sparring rings, dummy targets, weapon racks
- Enemy placement: 8-15 enemies in formation
- Light: Chandelier center, bright throughout
- Special: Chandelier drop hazard, wave combat
```

### Command Center
```
Sector configuration:
- Floor: 0 height (raised platform)
- Ceiling: 144 height
- Size: 256x256
- Features: War table, maps, commander throne, clan banners
- Enemy placement: Commander + elite guard retinue
- Light: Chandelier, multiple lanterns, bright
- Special: Boss encounter potential, high-value target
```

### Armory Vault
```
Sector configuration:
- Floor: 0 height
- Ceiling: 96 height
- Size: 128x128 to 256x256
- Features: Weapons on display, locked cases
- Enemy placement: 2-4 guards, stationary posts
- Light: Spotlight on weapons, dim corners
- Special: Armor trap on weapon pickup, alarm triggers
```

## Mapper Checklist for Barracks Layouts

- [ ] All floor sectors use appropriate barracks textures
- [ ] Weapon and armor props placed on racks/stands
- [ ] Light levels verified: base 80, torch zones 128, chandelier 192
- [ ] Chandelier hazards marked in sector notes
- [ ] Trap door sectors identified and textured subtly different
- [ ] Wall spike linedefs configured in narrow corridors
- [ ] Enemy spawn weights follow military hierarchy
- [ ] Command center placed as map highlight/objective
- [ ] Ambient sound linedefs placed (marching, training)
- [ ] No untextured sectors remaining
- [ ] Minimum 2 exits from barracks rooms
- [ ] Armory vault marked for high-value loot

---

# Great Hall of Maces Map Theme

## Theme Identity

The Great Hall of Maces represents the grand ceremonial chambers and throne rooms of dwarven strongholds—massive vaulted spaces dominated by towering stone columns, flickering torchlight, and elaborate iron chandeliers hanging from chains high above. These halls serve as throne rooms, grand ballrooms, and assembly chambers where the dwarven lords hold court and conduct grand councils.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| HALL1 | Polished Stone | Worn but grand grey stone floor |
| HALL2 | Checkered Marble | Black and white tile pattern |
| HALL3 | Red Carpet | Worn crimson carpet runner |
| HALL4 | Bronze Plate | Bronze inlaid stone sections |
| HALL5 | Grand Tile | Ornate carved floor tiles |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| COLUMN1 | Stone Column | Massive cylindrical pillar |
| COLUMN2 | Carved Column | Ornately decorated pillar |
| WALL1 | Grand Stone | Hammered dwarven stonework |
| WALL2 | Banner Wall | Wall covered in clan banners |
| WALL3 | Torch Wall | Wall with torch sconces |
| WALL4 | Arched Wall | Wall with decorative arches |
| TRIM1 | Bronze Trim | Bronze decorative trim |
| TRIM2 | Gold Trim | Gold inlaid trim (rare) |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| CEIL_H1 | High Vault | Massive vaulted ceiling |
| CEIL_H2 | Beam Ceiling | Exposed massive wooden beams |
| CEIL_H3 | Domed Ceiling | Ornate dome center |
| CHAIN1 | Chain Link | Hanging chain texture |
| CHAIN2 | Rope Coils | Coiled rope decorations |

## Lighting Rules

- **Base light level**: 96 (grand but slightly dim)
- **Chandelier light**: +64 light radius per chandelier, warm white with flicker
- **Torch sconce**: +48 light radius, warm orange, slight flicker
- **Column torch**: +32 light radius, positioned on pillars
- **Ambient bias**: Warm golden (RGB: 255, 220, 180)
- **Shadow zones**: 48 light level in corners and behind columns

### Light Source Types

1. **Iron Chandelier** (point light): Radius 384, intensity 1.4, warm white, 3-frame flicker animation
2. **Wall Torch** (point light): Radius 192, intensity 1.0, flickering orange
3. **Pillar Torch** (point light): Radius 160, intensity 1.1, warm yellow
4. **Magical Sconce** (point light): Radius 128, intensity 0.8, blue-white (rare)

## Environmental Hazards

### Chandelier Drop (Primary Hazard)
- Trigger: Player shoots chain, or walks under weakened/damaged chandelier
- Damage: 35% health (crush), severe knockback, screen shake
- Visual: Massive iron chandelier falls, glass crystal shatter particles, dust cloud
- Audio: Massive crash, screaming metal, glass breaking
- Warning: Chain creaks loudly 3 seconds before drop
- Counter: Shoot chandeliers to drop early (controlled), avoid standing under them
- Special: Some chandeliers can be shot to swing and hit multiple enemies

### Falling Chain
- Trigger: Chandelier drop disconnects chain from ceiling
- Damage: 15% health (slash), knockback
- Visual: Chain falls with momentum
- Counter: Stay clear of chandelier drop zones

### Trap Floor Tiles
- Trigger: Step on specific floor tiles (tagged)
- Effect: Spike pit opens below (64 unit drop)
- Damage: 20% fall damage + 10% trap damage
- Visual: Floor tiles collapse downward
- Counter: Look for subtle texture differences, move carefully
- Detection: Pressing use near suspicious tiles highlights danger zone

### Hanging Banner Fall
- Trigger: Banner chain shot or time-based decay
- Damage: 10% health, knockback
- Visual: Heavy banner falls from wall
- Counter: Don't shoot banners unnecessarily

### Hidden Alarm Trigger
- Trigger: Activating certain floor tiles or picking up throne objects
- Effect: Spawns reinforcements from secret doors
- Audio: Loud alarm gong
- Counter: Identify alarm objects before touching

## Ambient Sound Profile

### Continuous
- Torch crackling: Constant, slight variation
- Distant echoes: Footsteps reverberate, 0.8s decay
- Draft howling: Subtle wind through high ceilings
- Chain creaking: Occasional, 10-20 second intervals

### Triggered
- Chandelier warning: Chain creak 3 seconds before drop
- Chandelier crash: On impact
- Trap activation: Stone grinding, mechanical click
- Alarm gong: On hidden trigger activation

### Background
- Grand hall reverb: Long echo, 1.2s decay
- Distant dwarf voices: Mumbled conversation
- Occasional horn: Royal summons, 30-second intervals
- Ambient: Orchestral dwarf music, distant and faint

## Enemy Weighting

### Primary (45% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Commander | 15% |
| Dwarf Defender | 15% |
| Dwarf Elite Guard | 15% |

### Secondary (40% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Captain | 15% |
| Dwarf Warlord | 10% |
| Dwarf Marksman | 10% |
| Dwarf Runesmith | 5% |

### Rare (15% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Queen | 5% |
| Dwarf High Priest | 5% |
| Dwarf Warlord (Elite) | 5% |

### Encounter Density
- Entry hall: 2-4 guards (honor guard)
- Main hall: 6-12 enemies (throne room defense)
- Side chambers: 3-6 enemies (personal quarters)
- Grand ballroom: 15-25 enemies (full assembly)

### Boss Encounters
- Throne Room Boss: Always spawns Dwarf Queen or enhanced Warlord
- Chamber can support 30+ enemies for final encounters

## Example Room Archetypes

### Main Throne Hall
```
Sector configuration:
- Floor: 0 height
- Ceiling: 256 height
- Size: 512x512 to 1024x1024
- Features: Central throne, columns, chandeliers (2-4), carpet runner
- Enemy placement: Throne guardian, honor guard, wave spawns
- Light: Multiple chandeliers, torch sconces, bright
- Special: Chandelier hazards, throne object alarm, boss arena potential
```

### Grand Ballroom
```
Sector configuration:
- Floor: 0 height (varied 0 to 32)
- Ceiling: 192 height
- Size: 768x768 to 1024x1024
- Features: Dance floor, columns, multiple chandeliers (4-6)
- Enemy placement: 15-25 enemies, wave-based spawning
- Light: Multiple chandeliers, bright throughout
- Special: Multiple chandelier hazards, wave combat, no cover
```

### Side Chamber
```
Sector configuration:
- Floor: 0 height
- Ceiling: 128 height
- Size: 256x256 to 512x256
- Features: Personal quarters, smaller chandelier (1), banner wall
- Enemy placement: 3-6 enemies, guards and elites
- Light: Chandelier center, moderate
- Special: Intimate combat, chandelier single hazard
```

### Corridor of Columns
```
Sector configuration:
- Floor: 0 height
- Ceiling: 144 height
- Size: 256x512 to 512x1024
- Features: Row of columns (4-8), wall torches, banner
- Enemy placement: Guards between columns
- Light: Torch sconces, moderate shadows
- Special: Column cover for combat, trap floor tiles
```

### Entrance Hall
```
Sector configuration:
- Floor: 0 height
- Ceiling: 160 height
- Size: 256x256
- Features: Grand doors, two chandeliers, guard posts
- Enemy placement: 2-4 honor guard
- Light: Two chandeliers, bright entry
- Special: Introduction to chandelier hazards
```

### Royal Treasury Alcove
```
Sector configuration:
- Floor: -32 height (elevated platform)
- Ceiling: 96 height
- Size: 256x256
- Features: Treasure display, smaller chandelier, throne object
- Enemy placement: 4-8 elite guards
- Light: Chandelier, treasure glow
- Special: Alarm trigger, high-value loot, boss spawn potential
```

## Mapper Checklist for Great Hall Layouts

- [ ] All floor sectors use appropriate grand hall textures
- [ ] Columns placed every 128-256 units for visual anchor
- [ ] Light levels verified: base 96, chandelier zones 160+, torch 144
- [ ] Chandelier hazards marked in sector notes
- [ ] Chandelier chains identified and tagged for shootability
- [ ] Trap floor tiles subtly textured (checkered, carved)
- [ ] Throne or alarm objects identified
- [ ] Enemy spawn weights follow hierarchy (commanders present)
- [ ] Side chambers connected for flanking routes
- [ ] Ambient sound linedefs placed (echo, torch, chain)
- [ ] Reverb zone linedef on all large rooms
- [ ] No untextured sectors remaining
- [ ] Minimum 2 exits from main hall
- [ ] Boss arena check if room > 768x768 and meets encounter requirements
- [ ] Carpet runner texture on main paths

## Unique Features

### Chandelier Combat Interaction
- Chandeliers can be shot to damage enemies below (35% to enemies)
- Enemies will avoid standing directly under chandeliers
- Some chandeliers have health and can be partially damaged before falling
- Lighting changes when chandelier falls (loss of light source)

### Dynamic Environment
- Chandelier chains can swing if shot at angle
- Falling chandeliers can knock down nearby columns (destructible)
- Chain links remain as debris after chandelier falls

### Treasure Integration
- Grand halls often connect to treasure chambers
- Boss encounters often require clearing hall first
- Chandelier hazards can be weaponized by player

---

# Dwarven Armory Vaults Map Theme

## Theme Identity

The Dwarven Armory Vaults represent the deepest, most heavily fortified weapon storage facilities of the dwarven kingdoms—massive underground chambers lined with racks of weapons, armor displays, and deadly automated defenses. These vaults are designed to keep treasures secure and intruders dead. The combination of valuable weapons and built-in defensive systems makes these areas extremely dangerous.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| ARMOR1 | Stone Floor | Grey-blue stone with iron reinforcements |
| ARMOR2 | Metal Grate | Heavy iron floor grating |
| ARMOR3 | Trap Floor | Floor with hidden pit triggers |
| ARMOR4 | Armor Plate | Polished steel floor sections |
| ARMOR5 | Oil Stain | Dark oily floor patches |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| WEAPON1 | Weapon Rack | Wall-mounted sword rack |
| WEAPON2 | Pike Rack | Long weapon display |
| WEAPON3 | Shield Wall | Shield decoration |
| ARMOR_W1 | Plate Rack | Armor stand display |
| ARMOR_W2 | Helm Rack | Helmet display |
| DEFENSE1 | Spike Wall | Defensive spike strips |
| DEFENSE2 | Barricade | Wooden/metal barricade |
| DOOR_ARM | Armory Door | Heavy reinforced door |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| CEIL_A1 | Low Vault | Low stone ceiling |
| CEIL_A2 | Beam Ceiling | Exposed support beams |
| CEIL_A3 | Grate Ceiling | Metal ventilation grate |
| CEIL_A4 | Chain Ceiling | Hanging chains and mechanisms |

## Lighting Rules

- **Base light level**: 72 (moderate, military efficiency)
- **Torch light**: +48 light radius around wall sconces, warm orange
- **Lantern posts**: +64 light radius in weapon display areas
- **Weapon glow**: +32 light near magical/legendary weapons
- **Ambient bias**: Cool blue-grey (RGB: 200, 210, 230)
- **Shadow zones**: 32 light level in trap areas and dead ends
- **Trap activation**: Bright flash (+64) when trap triggers

### Light Source Types

1. **Wall Torch** (point light): Radius 192, intensity 1.0, flickering orange
2. **Lantern Post** (point light): Radius 256, intensity 1.2, steady yellow
3. **Weapon Glow** (point light): Radius 96, intensity 0.8, magical color based on weapon
4. **Alarm Light** (point light): Radius 192, intensity 1.5, red (when triggered)

## Environmental Hazards

### Auto-Arming Trap (Primary Hazard)
- Trigger: Player enters room without disarming switch, or touches weapon rack
- Effect: Crossbow bolts fire from wall panels (8 directions), 3-second burst
- Damage: 8% per hit, rapid fire (5 bolts per second)
- Visual: Panel doors open, crossbows extend, bolt sprites
- Audio: Mechanical twang, alarm klaxon
- Counter: Find and shoot disarming switch first, or destroy wall panels
- Special: Some traps have 5-second arming delay - find switch quickly

### Falling Armor Trap
- Trigger: Touching armor display without triggering alarm first
- Damage: 20% health (crush), knockback
- Visual: Armor suit falls from ceiling mount
- Audio: Crash and clanging
- Counter: Shoot armor stand first to trigger safely, or don't touch displays

### Pit Trap
- Trigger: Step on tagged floor tiles
- Effect: Floor collapses, player falls to sector below
- Damage: 25% fall damage + trap damage (spikes)
- Visual: Floor tiles give way
- Counter: Look for subtly different floor texture (slightly raised), press use to check

### Flame Jet
- Trigger: Proximity to tagged wall section
- Damage: 15% per second while in jet
- Visual: Orange flame sprite, heat distortion
- Audio: Whoosh of flame
- Counter: Stay center of corridors, watch for wall vents

### Alarm Gong
- Trigger: Picking up weapons from racks (unless disarmed)
- Effect: Spawns reinforcements from sealed doors, all traps activate
- Visual: Gong sprite, red lighting
- Audio: Loud metallic gong, alarm klaxon
- Counter: Find alarm switch before touching weapons

### Dart Corridor
- Trigger: Walking down tagged corridor without disarming
- Damage: 5% per dart, rapid fire from both walls
- Visual: Small dart sprites, wall panels opening
- Audio: Soft twang, clicking
- Counter: Run quickly through, or find side passages

## Ambient Sound Profile

### Continuous
- Creaking metal: Armor stands settling, 10-20 second intervals
- Distant mechanical: Hidden mechanisms, constant low hum
- Dripping oil: Occasional, random 5-15 seconds
- Chain rattling: Occasional, 20-30 seconds

### Triggered
- Trap arming: Mechanical clunk, 3-second warning
- Trap fire: Crossbow twang, flame whoosh
- Alarm: Loud gong, klaxon
- Armor fall: Crash and clang

### Background
- Military precision: Everything organized, sterile
- Echo: Medium reverb, 0.6s decay
- No ambient music (defensive, alert atmosphere)

## Enemy Weighting

### Primary (50% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Defender | 20% |
| Armored Dwarf | 15% |
| Dwarf Marksman | 15% |

### Secondary (35% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Captain | 15% |
| Dwarf Ironclad | 10% |
| Dwarf Engineer | 10% |

### Rare (15% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Warlord | 6% |
| Dwarf Tinkerer | 5% |
| Dwarf Elite Guard | 4% |

### Encounter Density
- Entry vault: 2-4 enemies (guards at door)
- Main armory: 6-12 enemies (weapon distribution)
- Deep vault: 8-15 enemies (elite defense)
- Vault complex: 15-25 enemies (full lockdown)

## Example Room Archetypes

### Entry Vault
```
Sector configuration:
- Floor: 0 height
- Ceiling: 128 height
- Size: 256x256
- Features: Entry door, weapon racks on walls, armor displays
- Enemy placement: 2-4 guards at posts
- Light: Wall torches, moderate
- Special: Intro to trap mechanics, easy disarming switch
```

### Main Armory
```
Sector configuration:
- Floor: 0 height
- Ceiling: 144 height
- Size: 512x512
- Features: Central weapon island, multiple racks, armor stands
- Enemy placement: 6-12 enemies, formation
- Light: Multiple lanterns, bright
- Special: Multiple trap types, weapon pickups valuable but risky
```

### Crossbow Corridor
```
Sector configuration:
- Floor: 0 height
- Ceiling: 80 height
- Size: 128x512 (corridor)
- Features: Wall panels, dart holes, spike strips on walls
- Enemy placement: None initially, spawns on alarm
- Light: Dim torches at ends, trap areas dark
- Special: Dart trap primary hazard, must run or disarm
```

### Armor Hall
```
Sector configuration:
- Floor: 0 height
- Ceiling: 160 height
- Size: 512x256
- Features: Full armor suits on stands, weapon racks
- Enemy placement: 4-8 guards
- Light: Lanterns at intervals, weapon glow
- Special: Falling armor trap, alarm gong hazard
```

### Deep Vault
```
Sector configuration:
- Floor: -32 height (elevated platform)
- Ceiling: 192 height
- Size: 256x256
- Features: Legendary weapons, heavy armor, multiple traps
- Enemy placement: 8-15 elite guards
- Light: Multiple light sources, magical glow on treasures
- Special: Multiple simultaneous traps, boss potential, high-value loot
```

### Reinforcement Chamber
```
Sector configuration:
- Floor: 0 height
- Ceiling: 112 height
- Size: 256x256
- Features: Sealed doors (open on alarm), extra weapon racks
- Enemy placement: Initially hidden, spawns on alarm
- Light: Dim until alarm, then bright red
- Special: Alarm triggers door opening and enemy spawns
```

## Mapper Checklist for Armory Layouts

- [ ] All floor sectors use appropriate armory textures
- [ ] Weapon racks placed on walls with proper spacing
- [ ] Armor displays positioned for falling trap
- [ ] Light levels verified: base 72, weapon glow 104, alarm 144
- [ ] Auto-arming trap linedefs configured with timing
- [ ] Disarming switch locations marked (should be findable but not obvious)
- [ ] Trap floor tiles subtly different texture
- [ ] Wall vent areas marked for flame jet
- [ ] Alarm gong objects identified and tagged
- [ ] Enemy spawn weights follow military hierarchy
- [ ] Deep vault areas marked for boss potential
- [ ] Ambient sound linedefs placed (mechanical, creaking)
- [ ] No untextured sectors remaining
- [ ] Minimum 2 exits from main vault areas
- [ ] Disarm puzzles solvable without killing all enemies first

## Unique Features

### Trap Disarming System
- Each trap zone has a linked switch somewhere in the room
- Switches are visible but may require exploration to reach
- Destroying switch permanently disables that trap
- Some rooms have master switch that disables all traps

### Weapon Pickup Risk/Reward
- Picking up weapons from racks triggers alarm unless disarmed
- Weapons in armory are higher quality than standard drops
- Consider risk vs reward before grabbing weapons
- Some weapons are "bait" - trap triggers with no actual benefit

### Dynamic Defense
- On alarm, sealed doors open and reinforcements enter
- Traps remain active during combat (double danger)
- Enemies know trap locations and avoid them
- Player can use traps against enemies if clever

### Automation Theme
- All traps are mechanical (dwarven engineering)
- No magical traps in standard armory
- Some legendary armories may have golem defenders
- Tinkerer dwarves can repair traps mid-combat

## Treasure Integration

- Armory vaults often connect to treasure chambers
- Legendary weapons require clearing vault first
- Disarmed traps can be re-armed by player for defense
- Some weapons are cursed - picking them up is dangerous

---

# Veinfall Chasm Map Theme

## Theme Identity

The Veinfall Chasm represents the abandoned deep mine shafts and ore transport tunnels of the dwarven kingdoms—vertical drops connected by rickety minecart rails, crumbling rope bridges, and precarious升降 platforms. These chasms were once the primary vein of the mountain's wealth, now fallen into disrepair and infested with creatures that thrive in the darkness.

## Texture Palette

### Floor Textures

| Texture | Name | Description |
|---------|------|-------------|
| VEIN1 | Cracked Stone | Jagged cracked floor with ore veins |
| VEIN2 | Wooden Deck | Rotting wooden platform planks |
| VEIN3 | Metal Grate | Rusted metal walkway sections |
| VEIN4 | Gravel | Loose stone and gravel |
| VEIN5 | Rail Track | Metal rails for minecart movement |

### Wall Textures

| Texture | Name | Description |
|---------|------|-------------|
| CLIFF1 | Vertical Shaft | Rough vertical cave wall |
| CLIFF2 | Layered Ore | Rock face with visible ore layers |
| CLIFF3 | Timber Frame | Wooden support beam structure |
| ROPE1 | Rope Bridge | Twisted rope and plank bridge |
| ROPE2 | Rope Coil | Hanging rope coils |
| SUPPORT1 | Broken Beam | Snapped wooden support |
| SUPPORT2 | Chain Link | Hanging chain links |

### Ceiling Textures

| Texture | Name | Description |
|---------|------|-------------|
| CEIL_V1 | Open Shaft | Dark vertical shaft above |
| CEIL_V2 | Beam Ceiling | Exposed wooden beams |
| CEIL_V3 | Rock Ceiling | Rough cave rock overhead |

## Lighting Rules

- **Base light level**: 48 (very dark, dangerous)
- **Minecart lamp**: +64 light radius, warm yellow, slight flicker
- **Ore glow**: +32 light radius near ore veins, green-blue tint
- **Chasm depth**: -8 light per 64 units depth (darker lower)
- **Ambient bias**: Cool blue-grey (RGB: 180, 190, 210)
- **Shadow zones**: 16 light level in deep pits
- **Fog**: Dense fog effect at ranges > 512 units

### Light Source Types

1. **Minecart Lantern** (point light): Radius 192, intensity 1.0, flickering warm yellow
2. **Ore Vein Glow** (point light): Radius 96, intensity 0.6, blue-green (radioactive ore)
3. **Rope Bridge Lantern** (point light): Radius 128, intensity 0.8, warm orange
4. **Emergency Lamp** (point light): Radius 64, intensity 0.5, red (warning areas)

## Environmental Hazards

### Minecart Trap (Primary Hazard)
- Trigger: Player steps on track activation zone or shoots cart release
- Effect: Unsecured minecart launches along track, crushing anything in path
- Damage: 40% health (crush), severe knockback
- Visual: Cart sprites moving along predefined path, sparks flying
- Audio: Rumbling, metal wheels on rail, crashing
- Warning: Chain release clanks 2 seconds before launch
- Counter: Stay off tracks when cart visible, shoot release mechanism early
- Special: Some carts carry explosives (delayed explosion on impact)

### Rope Bridge Collapse
- Trigger: Player weight on damaged bridge section (health-based threshold)
- Effect: Bridge section falls, player falls if on it
- Damage: 30% fall damage to level below
- Visual: Ropes snap, planks scatter, dust cloud
- Audio: Rope snapping, screaming wood, player yell
- Counter: Look for frayed ropes, test bridges carefully, move quickly across
- Special: Can shoot bridge to drop enemies instead

### Falling Rock/Cave-In
- Trigger: Timed events or player proximity to unstable sections
- Damage: 15-25% health (impact), knockback
- Visual: Rock sprites falling, dust particles
- Audio: Rumbling, cracking stone
- Warning: Cracking sounds 3-5 seconds before fall
- Counter: Don't linger under cracked ceiling sections, watch for warning signs

###升降 Platform Failure
- Trigger: Player steps on worn升降 platform
- Effect: Platform drops suddenly, player falls if weight exceeds threshold
- Damage: 20% fall damage + 10% impact
- Visual: Platform tilts and falls, chains snapping
- Audio: Metal groaning, chain snapping, crash
- Counter: Look for rust and damage on platforms, test before full weight
- Special: Some platforms have working counterweights (solvable puzzle)

### Pit Fall
- Trigger: Walking into unmarked chasm edges
- Damage: Instant death (fall damage > 100%)
- Visual: Darkness below, no floor visible
- Counter: Watch minimap, look for floor texture changes, use lighting

### Electrocution (Flooded Shafts variant)
- Trigger: Entering flooded electrical area
- Damage: 5% per second while in water
- Visual: Sparks, flickering lights, blue glow
- Audio: Electrical hum, crackling
- Counter: Find dry path, destroy power source

## Ambient Sound Profile

### Continuous
- Dripping water: Constant, varying intervals
- Distant rumble: Low frequency from deep in chasm
- Wind howling: Through vertical shafts, 10-second intervals
- Creaking wood: Rope bridges and supports, occasional
- Cart wheels: Distant, when other carts move

### Triggered
- Cart launch: Rumbling start, then whooshing
- Bridge collapse: Snap, crash, debris
- Rock fall: Cracking, then impact
- Platform failure: Groan, snap, crash
- Player fall: Yell, then impact

### Background
- Cave echo: Medium decay, 0.5s
- Water dripping: Reverb echo
- Ambient: Eerie wind, distant creature sounds

## Enemy Weighting

### Primary (55% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Goblin Scout | 20% |
| Goblin Grunt | 20% |
| Goblin Sneak | 15% |

### Secondary (35% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Dwarf Miner | 15% |
| Goblin Shaman | 10% |
| Dwarf Sapper | 10% |

### Rare (10% spawn rate)

| Enemy | Spawn Weight |
|-------|-------------|
| Goblin King (mini-boss) | 4% |
| Dwarf Geologist | 3% |
| Cave Troll | 3% |

### Encounter Density
- Narrow ledges: 1-2 enemies (ambush positions)
- Platform areas: 3-5 enemies
- Chamber junctions: 5-8 enemies
- Main junction: 8-15 enemies

## Example Room Archetypes

### Vertical Shaft
```
Sector configuration:
- Floor: -256 to 0 height (vertical)
- Ceiling: 256 height
- Size: 256x256 to 512x512
- Features: 升降 platforms, rope bridges, minecart tracks
- Enemy placement: Platforms and ledges
- Light: Minecart lanterns, ore glow, very dim
- Special: Minecart trap routes through center
```

### Minecart Junction
```
Sector configuration:
- Floor: 0 height
- Ceiling: 128 height
- Size: 512x512
- Features: Multiple track directions, cart storage, switches
- Enemy placement: 5-10 enemies, control cart routes
- Light: Multiple lanterns, moderate
- Special: Player can control cart direction via switches
```

### Rope Bridge Crossing
```
Sector configuration:
- Floor: 0 height (bridge), -128 below (pit)
- Ceiling: 96 height
- Size: 128x512 (bridge corridor)
- Features: Rope bridge, support beams, frayed sections
- Enemy placement: 2-4 enemies crossing or defending
- Light: Bridge lanterns, dim, flickering
- Special: Bridge collapse hazard, can cut ropes to drop enemies
```

### ore Vein Chamber
```
Sector configuration:
- Floor: 0 height with ore formations
- Ceiling: 192 height
- Size: 256x256 to 512x256
- Features: Glowing ore veins, crystal formations, mineral deposits
- Enemy placement: 3-6 enemies mining or guarding
- Light: Ore glow, blue-green, low visibility
- Special: Radiation damage near bright ore, valuable pickups
```

### Abandoned Equipment Bay
```
Sector configuration:
- Floor: 0 height
- Ceiling: 144 height
- Size: 512x256
- Features: Old carts, broken machinery, tool racks
- Enemy placement: 4-8 enemies salvaging or hiding
- Light: Emergency lamps, dim red
- Special: Explosive barrels, can use machinery as weapons
```

###升降 Platform Hub
```
Sector configuration:
- Floor: Multiple levels (-128, -64, 0, 64)
- Ceiling: 160 height
- Size: 256x256 vertical
- Features: Multiple升降 platforms, chains, pulleys
- Enemy placement: 3-6 enemies on different levels
- Light: Platform lamps, moderate
- Special: Platform failure hazards, can control platform speeds
```

## Mapper Checklist for Veinfall Chasm Layouts

- [ ] All floor sectors use appropriate veinfall/ore textures
- [ ] Rope bridges properly flagged for collapse mechanics
- [ ] Minecart track paths defined with waypoints
- [ ] Minecart release switches placed and tagged
- [ ]升降 platforms tagged with weight thresholds
- [ ] Cave-in zones marked in sector notes
- [ ] Pit hazards have visible edge indicators
- [ ] Ore glow sectors properly tagged for light
- [ ] Chasm depth lighting applied correctly
- [ ] Enemy spawn weights follow primary/secondary/rare ratios
- [ ] Vertical gameplay flow verified (up/down paths)
- [ ] No untextured sectors remaining
- [ ] Minimum 2 routes through any chasm section
- [ ] Fall damage zones have recovery paths
- [ ] Minecart routes avoid dead-end traps

## Unique Features

### Minecart Physics
- Carts follow predefined track paths
- Speed increases on downward slopes
- Carts can be boarded by player (ride along)
- Player can steer cart at junctions (shoot switch)
- Carts carry momentum between track sections

### Vertical Gameplay Focus
- Multiple vertical layers in most rooms
- 升降 platforms connect levels
- Rope bridges span gaps
- Falling from upper levels is major hazard
- Combat across multiple vertical planes

### Environmental Interaction
- Rope bridges can be shot to drop enemies
- Minecart tracks can be sabotaged
-升降 platform chains can be cut
- Unstable rocks can be knocked down
- Player can ride minecarts through levels

### Treasure Integration
- Veinfall Chasm connects to Royal Treasury (descent)
- ore Vein Chambers have high-value mineral pickups
- Abandoned equipment may contain hidden weapons
- Cart routes sometimes lead to secret areas

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
