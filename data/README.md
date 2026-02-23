# Goblin Dice Rollaz - Demo WAD Bundle

This directory contains the demo WAD bundle for Goblin Dice Rollaz.

## Contents

- `demo-wad-spec.md` - Full specification for the demo WAD
- `build_demo_wad.sh` - Build script (Unix/Linux)
- `build_demo_wad.ps1` - Build script (Windows PowerShell)
- `maps/` - Map definitions
  - `demo_maps.txt` - Map geometry and sectors
- `things/` - Thing spawn definitions
  - `weapons.txt` - Weapon and ammo pickups
  - `powerups.txt` - Powerup locations
  - `enemies.txt` - Enemy spawn points

## Quick Start

### Prerequisites

1. **Goblin Dice Rollaz** built from source
2. A **Doom IWAD** (DOOM.WAD, DOOM2.WAD, etc.)
3. **SLADE3** or **Doom Builder 2** (for WAD creation)

### Building the WAD

#### Option 1: SLADE3 (Recommended)

1. Download SLADE3 from https://slade.mancubus.net/
2. Open SLADE3
3. File → New → New PWAD
4. Import:
   - Map data from `maps/demo_maps.txt`
   - Thing data from `things/*.txt`
5. Save as `goblin_demo.wad`

#### Option 2: Doom Builder 2

1. Download Doom Builder 2
2. Map → New Map → Select "Doom 2" engine
3. Import thing definitions from the `things/` directory
4. Build the map (F5)
5. Save as `goblin_demo.wad`

#### Option 3: PowerShell (Windows)

```powershell
cd data
.\build_demo_wad.ps1
```

### Running the Demo

```bash
# With Goblin Dice Rollaz executable
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD

# Start at specific map
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD -warp 1

# Map 2: Dwarven Forge
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD -warp 2

# Map 3: Treasure Vault
./goblin-doom -merge goblin_demo.wad -iwad DOOM.WAD -warp 3
```

## Demo Content

### Map 1: Goblin Cavern

- **Theme**: Underground mine/cave
- **Enemies**: Goblin Warriors, Scouts, Sneaks
- **Weapons**: d4, d6, d8, d10 ammo pickups
- **Objective**: Fight through the goblin encampment

### Map 2: The Dwarven Forge

- **Theme**: Active forge with lava
- **Enemies**: Dwarf Defenders, Marksmen, Miners
- **Weapons**: d12, d20 ammo pickups
- **Hazards**: Lava pit (sector damage)
- **Objective**: Reach the d20 cannon in the back

### Map 3: The Treasure Vault

- **Theme**: High-value treasure chamber
- **Enemies**: Elite Captain, Armored Dwarves, Berserkers
- **Weapons**: All dice weapons available
- **Powerups**: Critical Hit, Double Damage, Fortune
- **Objective**: Defeat the elite encounter

## Customization

### Adjusting Enemy Counts

Edit `things/enemies.txt` to modify spawn counts:

```text
# Add more goblins
THING: 8100,XXXX,YYYY,0,ANGLE,7

# Remove enemies by deleting lines
```

### Adding Custom Weapons

1. Define weapon in engine code (`src/doom/d_items.c`)
2. Add pickup thing in `things/weapons.txt`
3. Rebuild the WAD

## Troubleshooting

### "Sprite not found" errors

- This is expected without custom sprites
- The game will use Doom's default sprites

### Map won't load

- Ensure you're using the `-merge` flag, not `-file`
- Verify your Doom IWAD is valid

### Enemies not spawning

- Check thing IDs match engine definitions
- Verify skill level allows spawn (flag 5 = nightmare only)

## License

This demo WAD is provided as part of Goblin Dice Rollaz.
See project root for licensing information.
