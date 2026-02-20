# AGENTS.md - Goblin Dice Rollaz

## Build Commands

### Linux/macOS (Docker)
```bash
cd GoblinDiceRollaz
docker build -t goblin-dice-rollaz .
```

### Local Build (when tools available)
```bash
cd GoblinDiceRollaz
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### GitHub Actions
Pushed to `.github/workflows/build.yml` - auto-runs on push to main.

## Project Structure
- `src/doom/` - Doom game engine code
- `src/` - Platform abstraction layer (SDL2)
- `data/` - Game assets (sprites, sounds, textures)
- `CMakeLists.txt` - Build configuration

## Key Files to Modify
- `CMakeLists.txt` - Project name/version
- `src/doom/d_main.c` - Game initialization, title
- `src/doom/m_menu.c` - Menu text
- `src/doom/info.c` - Monster/player definitions
- `src/doom/d_items.c` - Weapon definitions

## Testing
- Build must succeed before commit
- GitHub Actions validates Linux/Windows/Docker builds

## Notes
- Chocolate Doom is the base engine
- Need Doom IWAD file to run (commercial game data)
- Game content (sprites, sounds) requires WAD files
