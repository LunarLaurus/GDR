# IMPLEMENTATION_PLAN.md - Goblin Dice Rollaz

## Priority Tasks

### Phase 1: Engine Setup (Complete)
- [x] Clone Chocolate Doom source
- [x] Set up CMakeLists.txt with project name
- [x] Create Dockerfile
- [x] Create GitHub Actions workflow

### Phase 2: Game Identity (Complete)
- [x] Update d_main.c with "Goblin Dice Rollaz" title
- [x] Update banner/copyright text
- [x] Update quit message
- [x] Update screenshot filename

### Phase 3: Fix CMakeLists.txt Binary Names
- [x] Rename binary outputs from chocolate-doom to goblin-doom in src/CMakeLists.txt
- [x] Update PROGRAM_PREFIX to use goblin-

### Phase 4: GitHub Actions CI/CD
- [x] Fix build.yml workflow - ensure it builds correctly

### Phase 5: Game Content - Text Changes
- [x] Update menu "Quit DOOM" to "Quit Game" in m_menu.c
- [x] Update episode text to goblin theme in d_englsh.h
- [x] Update skill level names if needed

### Phase 6: Prepare for WAD-based Content
- [x] Document how WAD files work for sprites
- [x] Create spec for goblin sprite replacement
- [x] Create spec for dwarf enemy sprites
- [x] Create spec for dice weapon sprites

### Phase 7: Gameplay Features
- [x] Add d6 blaster weapon definition to d_items.c
- [x] Add d20 cannon weapon definition
- [x] Add dwarf enemy definitions to info.c
- [x] Add critical hit system (percentile-based damage)
- [x] Update README.md with Goblin Dice Rollaz identity
- [x] Update config file references (goblin-doom.cfg)

### Phase 8: UI/UX Enhancements
- [x] Add damage number overlay system
- [x] Add dice roll popup effects for critical hits
- [x] Modify status bar for dice theme
- [x] Add crit chance HUD indicator

### Phase 9: Engine Improvements
- [x] Add debug console command to show/hide FPS counter
- [x] Add cheat code for infinite ammo (debug mode)
- [x] Add console variable for mouse sensitivity scaling
- [x] Optimize sprite rendering for modern hardware

### Phase 10: Additional Weapons
- [x] Add d12 weapon (heavy impact weapon)
- [x] Add percentile dice weapon (rolls 1-100 for damage)
- [x] Add d4 throwing knives weapon

### Phase 11: New Enemies
- [x] Add dwarf berserker enemy (fast, melee)
- [x] Add dwarf engineer enemy (uses explosives)
- [x] Add goblin shaman enemy (casts spells)

### Phase 12: Powerups
- [x] Add Critical Hit powerup (increases crit chance for duration)
- [x] Define crit bonus percentage and duration values
- [x] Implement timed powerup thinker logic
- [x] Ensure crit bonus stacks safely with existing crit system
- [x] Add HUD timer indicator for active crit buff
- [x] Add visual screen tint while active
- [x] Add activation and expiration sound effects
- [x] Add CVAR to configure crit powerup strength
- [x] Add pickup sprite and map editor spawn ID
- [x] Balance spawn frequency per difficulty level
- [x] Add Double Damage powerup
- [x] Apply 2x multiplier post-dice-roll calculation
- [x] Ensure compatibility with crit multiplier logic
- [x] Add distinct visual effect (screen glow or weapon aura)
- [x] Add unique pickup sprite and sound
- [x] Add duration timer and HUD display
- [x] Prevent unintended stacking with other damage buffs
- [x] Add difficulty-based spawn tuning
- [x] Add multiplayer deterministic sync validation
- [x] Add Dice Fortune powerup (guaranteed crit on next hit)
- [x] Implement one-shot guaranteed crit flag
- [x] Persist effect across weapon swaps
- [x] Clear effect on successful hit
- [x] Add HUD indicator ("Next Hit: CRIT")
- [x] Add expiration fail-safe on player death
- [x] Add unique pickup sprite variant
- [x] Add audio cue when guaranteed crit triggers
- [x] Add balance pass for rarity and placement
- [x] Implement shared powerup framework for future buffs
- [x] Add global powerup HUD slot system
- [x] Add powerup debug command for testing
- [x] Add powerup-only test map for tuning

### Phase 13: Map Themes
- [x] Document mine/cavern map theme
- [x] Define texture palette (rock, timber supports, rails)
- [x] Define lighting rules (low light, warm torches)
- [x] Define ambient sound profile (drips, rumble)
- [x] Define primary enemy weighting (goblin-heavy)
- [x] Define environmental hazards (cave-ins, pits)
- [x] Provide example room archetypes (shafts, tunnels, chambers)
- [x] Create mapper checklist for cavern layouts
- [x] Document forge/lava map theme
- [x] Define lava floor damage sector rules
- [x] Define animated lava textures
- [x] Define red/orange lighting bias
- [x] Define dwarf-heavy enemy weighting
- [x] Define environmental hazards (steam vents, molten spills)
- [x] Define industrial props (anvils, chains, grates)
- [x] Provide arena-style encounter layout examples
- [x] Create mapper checklist for forge maps
- [x] Document treasure chamber map theme
- [x] Define gold/gem texture variants
- [x] Define trap sector mechanics (pressure plates, drops)
- [x] Define high-value powerup spawn weighting
- [x] Define elite enemy spawn presets
- [x] Define brighter lighting and echo ambience
- [x] Provide vault room archetype examples
- [x] Create mapper checklist for treasure rooms
- [x] Create shared theme documentation template
- [x] Provide example WAD demonstrating all themes
- [x] Define encounter pacing guidelines per theme
- [x] Add internal review checklist for map consistency

### Phase 13.5: Arsenal & Enemy Expansion (Deep Content Pass)
- [x] Refactor weapon system to support shared dice-roll backend (centralized roll + crit resolver)
- [x] Add unique firing animations per die type (distinct silhouettes and recoil timing)
- [x] Implement per-weapon crit modifiers (e.g., d4 high crit chance, d12 high crit multiplier)
- [x] Add alternate fire modes (if engine constraints allow)
- [x] d4: rapid burst throw
- [x] d12: charged smash (long windup, bonus crit chance)
- [x] d100: “Gamble Shot” (wider roll variance)
- [x] Add ammo types per die category (Light Dice, Heavy Dice, Arcane Dice)
- [x] Implement ammo pickup sprites themed to carved bone / rune dice
- [x] Add weapon tier balancing pass (DPS, ammo efficiency, crit frequency)
- [x] Add weapon selection UI icons for each die type
- [x] Add weapon-specific sound profiles (distinct roll sounds per die)
- [x] Add misfire mechanic for high-risk weapons (low percentile roll penalty)
- [x] Add "Exploding Max Roll" support (optional rule toggle)
- [x] Add weapon stat debug overlay (damage range, crit %, average roll)
- [x] Add weapon spawn flags for map balancing
- [x] Document all dice weapons in DESIGN.md with damage math breakdown
- [x] Add d8 mid-tier weapon (balanced fire rate and crit rate)
- [x] Add d10 ricochet weapon (projectiles bounce once)
- [x] Add twin d6 scatter weapon (close-range burst)
- [x] Add arcane d20 beam (continuous roll tick damage)
- [x] Add cursed die weapon (high damage, self-risk mechanic)
- [x] Refactor enemy definition templates for easier expansion (shared dwarf base struct)
- [x] Implement per-enemy stat table (HP, speed, aggression, crit resistance)
- [x] Add difficulty scaling hooks per enemy
- [x] Dwarf Defender (shielded, frontal damage reduction)
- [x] Dwarf Marksman (slow, high-damage ranged unit)
- [x] Dwarf Miner (throws pickaxes, medium range)
- [x] Elite Dwarf Captain (buffs nearby dwarves)
- [x] Dwarf Bombardier (engineer subclass with timed explosives)
- [x] Armored Dwarf (high HP, weak rear hitbox)
- [x] Goblin Scout (low HP, high mobility)
- [x] Goblin Sneak (ambush behavior, delayed aggro)
- [x] Goblin Alchemist (throws volatile potion projectiles)
- [x] Goblin Totemist (deploys buff/debuff totems)
- [x] Goblin Shaman (complete spell kit)
- [x] Firebolt projectile
- [x] Freeze hex (slow effect)
- [x] Minor heal on allies
- [x] Randomized "Chaos Spell" ability
- [x] Teleport reposition logic
- [x] Spell cooldown system
- [x] Add faction coordination (goblins prioritize flanking, dwarves hold formation)
- [x] Add morale system (retreat if leader dies)
- [x] Add crit resistance values per enemy type
- [x] Add weak point logic (headshot or rear bonus multiplier)
- [x] Add spawn group presets for map designers
- [ ] Unique death animations per elite unit (blocked: requires sprite assets)
- [ ] Resolve blocker: Cannot implement - no sprite artist available in current environment
- [ ] Critical hit reaction animations (blocked: requires sprite assets)
- [x] Elemental status visual overlays (burn/freeze/glow)
- [x] Add mini bestiary documentation in README or `/docs`

## Phase 14: Spell & Status Effect System
- [x] Implement generic status effect framework (duration-based effects in thinker loop)
- [x] Add "Burning" effect (damage over time, fire visuals)
- [x] Add "Frozen" effect (movement speed reduction)
- [x] Add "Stunned" effect (temporary attack suppression)
- [x] Add "Dice Curse" effect (randomized stat variance)
- [x] Add HUD status effect indicators
- [x] Expose status parameters via configurable constants

## Phase 15: Advanced Enemy AI Behaviors
- [x] Add tactical retreat logic for ranged dwarves
- [x] Add group aggression triggers (pack behavior)
- [x] Add engineer turret deployment logic
- [x] Add shaman support logic (buff nearby enemies)
- [x] Add per-enemy reaction time variance
- [x] Implement difficulty-scaled AI parameters

## Phase 16: Boss Encounters
- [x] Design Goblin King boss (multi-phase fight)
- [x] Design Dwarven War Machine boss (projectile-heavy)
- [x] Implement phase transition triggers
- [x] Add boss-specific music cues
- [x] Add boss health bar overlay
- [x] Add scripted arena lock system (vanilla-compatible)

## Phase 17: Dice Mechanics Expansion
- [x] Add "Exploding Dice" mechanic (max roll triggers reroll)
- [x] Add "Advantage/Disadvantage" system (roll twice, take best/worst)
- [x] Add combo multiplier system for consecutive crits
- [x] Add luck stat affecting roll distribution
- [x] Add configurable crit scaling curves

## Phase 18: Progression System (Optional Mode)
- [x] Add optional RPG progression mode toggle
- [x] Implement experience tracking
- [x] Add level-up stat selection screen
- [x] Add permanent crit chance scaling
- [x] Add weapon mastery modifiers
- [x] Ensure vanilla mode remains unaffected

## Phase 19: Audio Overhaul
- [x] Add dice roll sound library (per die type)
- [x] Add crit impact layered sound
- [x] Add dwarf voice cues
- [x] Add goblin chatter ambient sounds
- [x] Add reverb zones for caves
- [x] Add audio toggle options in setup tool

## Phase 20: Visual Effects Layer
- [x] Add dynamic screen shake on high rolls
- [x] Add particle sparks for crit hits
- [x] Add lava heat shimmer effect
- [x] Add magic projectile trail rendering
- [x] Add low-health screen tint
- [x] Add optional modern FX toggle

## Phase 21: Multiplayer Enhancements
- [x] Ensure dice mechanics sync deterministically over netplay
- [x] Add crit event broadcast messages
- [x] Add optional PvP dice arena mode
- [x] Add co-op shared crit buffs
- [x] Add net desync debug logging

## Phase 22: Modding & Extensibility
- [x] Document dice weapon DEHACKED mappings
- [x] Expose crit system parameters to config
- [x] Add custom dice weapon template example
- [x] Add enemy definition template
- [x] Create example goblin-themed TC pack
- [x] Write Modding Guide in `/docs`

## Phase 23: Performance & Determinism Audit
- [x] Profile thinker loop under heavy dice spam
- [x] Validate deterministic RNG across platforms
- [x] Audit floating-point usage (avoid nondeterminism)
- [x] Benchmark sprite-heavy scenes
- [x] Test low-spec hardware compatibility
- [x] Add reproducible demo test suite

## Phase 24: Release Engineering
- [x] Create versioning scheme (SemVer or similar)
- [x] Add changelog automation
- [x] Add release packaging scripts (Linux/Windows/macOS)
- [x] Create demo WAD bundle
- [ ] Publish first tagged release
- [ ] Resolve blocker: Requires manual GitHub release creation and push to remote
- [x] Draft public roadmap

## Phase 26: Quality of Life Improvements
- [x] Add weapon quick-switch keybindings (1-9 number keys)
- [x] Add reload/replace weapon function
- [x] Add auto-switch to new weapon on pickup
- [x] Add weapon cycle forward/backward keys
- [x] Add show/hide HUD toggle
- [x] Add damage log console command for review
- [x] Add kill count display in end-level stats

## Phase 27: Additional Game Modes
- [x] Add endless survival mode (waves of enemies)
- [x] Add time attack mode (speedrun)
- [x] Add challenge mode presets (crit only, no powerups, etc.)

## Phase 28: Configuration & Settings
- [x] Add in-game settings menu accessible via ESC
- [x] Add dice roll sound volume slider
- [x] Add critical hit notification text toggle
- [x] Add screen shake intensity slider
- [x] Add damage number scale slider
- [x] Add particle effects toggle

## Phase 29: Save System Enhancements
- [x] Add additional save slots (beyond 6)
- [x] Add quicksave/quickload slots
- [x] Add save game comments
- [x] Add auto-save on level transition

## Phase 30: Debug Tools & Developer Features
- [x] Add console command to list all dice weapons and stats
- [x] Add debug overlay showing damage calculations
- [x] Add configurable minimum damage cap (prevent 0-damage rolls)
- [x] Add screenshot hotkey with dice roll overlay capture
- [ ] Add demo recording/playback validation
- [ ] Add network packet debug logging for multiplayer
- [ ] Add weapon balance tuning constants file
- [ ] Add environment variable for IWAD path override
- [ ] Add startup crash handler with stack trace
- [ ] Add mod loader error reporting system

## Current Status
**Phase 1-8 complete**. Phase 9-13 ready for implementation.
