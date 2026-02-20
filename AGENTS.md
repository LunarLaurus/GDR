# AGENTS.md – Goblin Dice Rollaz

This document defines operational guidance for autonomous agents working on Goblin Dice Rollaz.

Base engine: Chocolate Doom  
Language: C  
Build system: CMake  
Rendering/Input: SDL2  

---

## Build Commands

### GitHub Actions (CI)

Workflow file: `.github/workflows/build.yml`

Triggers:
- Push to `main`
- Pull request

CI validates:
- Linux build
- Windows build
- Docker build

All PRs must pass CI before merge.

---

## Running the Game

Requires a Doom IWAD (commercial game data not included, will refactor to own game later).

Example:

```bash
./goblin-doom -iwad DOOM.WAD
```

Using merged content:

```bash
./goblin-doom -merge custom.wad
```

Configuration file:
- `goblin-doom.cfg`

---

## Project Structure

```
GoblinDiceRollaz/
│
├── src/
│   ├── doom/              # Core Doom engine logic
│   ├── i_*.c              # Platform abstraction
│   ├── m_*.c              # Utility modules
│   └── CMakeLists.txt
│
├── data/                  # Assets (optional runtime data)
├── specs/                 # Feature specifications
├── IMPLEMENTATION_PLAN.md # Task tracking
├── CMakeLists.txt
└── Dockerfile
```

---

## Core Engine Areas

### Gameplay Systems
- `src/doom/p_*` – Physics, combat, actors
- `src/doom/info.c` – Actor definitions
- `src/doom/d_items.c` – Weapons and pickups
- `src/doom/p_inter.c` – Damage handling

### UI / HUD
- `src/doom/st_stuff.c` – Status bar
- `src/doom/m_menu.c` – Menus
- `src/doom/d_englsh.h` – Game strings

### Initialization
- `src/doom/d_main.c` – Entry and game startup

### Rendering
- `src/doom/r_*` – Software renderer

---

## Key Files to Modify

- `CMakeLists.txt` – Project name, versioning
- `src/doom/d_main.c` – Game identity
- `src/doom/m_menu.c` – Menu modifications
- `src/doom/info.c` – Enemy definitions
- `src/doom/d_items.c` – Weapons
- `src/doom/p_inter.c` – Damage logic (crit system)
- `src/doom/st_stuff.c` – HUD changes

Modify only what is required for the current task.

---

## Coding Guidelines

- Follow Chocolate Doom coding style.
- Prefer fixed-point math over floating point.
- Maintain deterministic RNG behavior.
- Do not introduce platform-specific logic into gameplay code.
- Keep demo compatibility intact.
- Avoid refactoring unless explicitly required by a task.

---

## Git Workflow

- One task = one commit.
- Commit only relevant files.
- Do not reformat unrelated code.
- Ensure build succeeds locally before commit.

Commit message format:

```
feat: complete Phase X – <task description>
```

or

```
fix: resolve <short description>
```

---

## Testing Requirements

Before committing:

- Project builds successfully (Release).
- No new warnings introduced.
- CI pipeline passes.

If gameplay logic is modified:
- Validate no crashes on startup.
- Test at least one level load.
- Verify deterministic behavior (no obvious desync).

---

## Content & WAD Notes

- Vanilla Doom cannot load sprites from PWAD via `-file`.
- Use `-merge` for TC-style content.
- Sprite replacements must follow Doom sprite naming conventions.
- Actor IDs must not collide with existing ones.

---

## Determinism Rules

- Do not use floating point for damage calculations.
- Do not use non-deterministic system time.
- Do not introduce randomness outside Doom’s RNG table.
- All new RNG must use existing Doom RNG mechanisms.

---

## Safety Constraints

Agents must NOT:

- Remove existing completed features.
- Reorder IMPLEMENTATION_PLAN.md tasks.
- Modify CI configuration unless explicitly required.
- Introduce external dependencies without approval.

---

## Platform builds

### Linux/macOS (Docker – Preferred for CI Parity)

```bash
cd GoblinDiceRollaz
docker build -t goblin-dice-rollaz .
```

Run container interactively (if needed):

```bash
docker run --rm -it goblin-dice-rollaz
```

---

### Local Build (Native Toolchain)

Release build:

```bash
cd GoblinDiceRollaz
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

Debug build:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

Clean rebuild:

```bash
rm -rf build
```

---

### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

---

## Definition of Success

A valid contribution:

- Implements exactly one task.
- Builds successfully.
- Passes CI.
- Updates IMPLEMENTATION_PLAN.md accordingly.
- Preserves engine philosophy.

End of agent instructions.