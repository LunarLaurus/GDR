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
- [ ] Test that Linux build job works (blocked: requires git push)
- [ ] Test that Windows build job works (blocked: requires git push)
- [ ] Test that Docker build works (blocked: requires git push)
- [ ] Fix any CI failures (blocked: requires git push)

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
- [ ] Add balance pass for rarity and placement
- [ ] Implement shared powerup framework for future buffs
- [ ] Add global powerup HUD slot system
- [ ] Add powerup debug command for testing
- [ ] Add powerup-only test map for tuning

### Phase 13: Map Themes
- [ ] Document mine/cavern map theme
- [ ] Define texture palette (rock, timber supports, rails)
- [ ] Define lighting rules (low light, warm torches)
- [ ] Define ambient sound profile (drips, rumble)
- [ ] Define primary enemy weighting (goblin-heavy)
- [ ] Define environmental hazards (cave-ins, pits)
- [ ] Provide example room archetypes (shafts, tunnels, chambers)
- [ ] Create mapper checklist for cavern layouts
- [ ] Document forge/lava map theme
- [ ] Define lava floor damage sector rules
- [ ] Define animated lava textures
- [ ] Define red/orange lighting bias
- [ ] Define dwarf-heavy enemy weighting
- [ ] Define environmental hazards (steam vents, molten spills)
- [ ] Define industrial props (anvils, chains, grates)
- [ ] Provide arena-style encounter layout examples
- [ ] Create mapper checklist for forge maps
- [ ] Document treasure chamber map theme
- [ ] Define gold/gem texture variants
- [ ] Define trap sector mechanics (pressure plates, drops)
- [ ] Define high-value powerup spawn weighting
- [ ] Define elite enemy spawn presets
- [ ] Define brighter lighting and echo ambience
- [ ] Provide vault room archetype examples
- [ ] Create mapper checklist for treasure rooms
- [ ] Create shared theme documentation template
- [ ] Provide example WAD demonstrating all themes
- [ ] Define encounter pacing guidelines per theme
- [ ] Add internal review checklist for map consistency

### Phase 13.5: Arsenal & Enemy Expansion (Deep Content Pass)
- [ ] Refactor weapon system to support shared dice-roll backend (centralized roll + crit resolver)
- [ ] Add unique firing animations per die type (distinct silhouettes and recoil timing)
- [ ] Implement per-weapon crit modifiers (e.g., d4 high crit chance, d12 high crit multiplier)
- [ ] Add alternate fire modes (if engine constraints allow)
- [ ] d4: rapid burst throw
- [ ] d12: charged smash (long windup, bonus crit chance)
- [ ] d100: “Gamble Shot” (wider roll variance)
- [ ] Add ammo types per die category (Light Dice, Heavy Dice, Arcane Dice)
- [ ] Implement ammo pickup sprites themed to carved bone / rune dice
- [ ] Add weapon tier balancing pass (DPS, ammo efficiency, crit frequency)
- [ ] Add weapon selection UI icons for each die type
- [ ] Add weapon-specific sound profiles (distinct roll sounds per die)
- [ ] Add misfire mechanic for high-risk weapons (low percentile roll penalty)
- [ ] Add “Exploding Max Roll” support (optional rule toggle)
- [ ] Add weapon stat debug overlay (damage range, crit %, average roll)
- [ ] Add weapon spawn flags for map balancing
- [ ] Document all dice weapons in DESIGN.md with damage math breakdown
- [ ] Add d8 mid-tier weapon (balanced fire rate and crit rate)
- [ ] Add d10 ricochet weapon (projectiles bounce once)
- [ ] Add twin d6 scatter weapon (close-range burst)
- [ ] Add arcane d20 beam (continuous roll tick damage)
- [ ] Add cursed die weapon (high damage, self-risk mechanic)
- [ ] Refactor enemy definition templates for easier expansion (shared dwarf base struct)
- [ ] Implement per-enemy stat table (HP, speed, aggression, crit resistance)
- [ ] Add difficulty scaling hooks per enemy
- [ ] Dwarf Defender (shielded, frontal damage reduction)
- [ ] Dwarf Marksman (slow, high-damage ranged unit)
- [ ] Dwarf Miner (throws pickaxes, medium range)
- [ ] Elite Dwarf Captain (buffs nearby dwarves)
- [ ] Dwarf Bombardier (engineer subclass with timed explosives)
- [ ] Armored Dwarf (high HP, weak rear hitbox)
- [ ] Goblin Scout (low HP, high mobility)
- [ ] Goblin Sneak (ambush behavior, delayed aggro)
- [ ] Goblin Alchemist (throws volatile potion projectiles)
- [ ] Goblin Totemist (deploys buff/debuff totems)
- [ ] Goblin Shaman (complete spell kit)
- [ ] Firebolt projectile
- [ ] Freeze hex (slow effect)
- [ ] Minor heal on allies
- [ ] Randomized “Chaos Spell” ability
- [ ] Teleport reposition logic
- [ ] Spell cooldown system
- [ ] Add faction coordination (goblins prioritize flanking, dwarves hold formation)
- [ ] Add morale system (retreat if leader dies)
- [ ] Add crit resistance values per enemy type
- [ ] Add weak point logic (headshot or rear bonus multiplier)
- [ ] Add spawn group presets for map designers
- [ ] Unique death animations per elite unit
- [ ] Critical hit reaction animations
- [ ] Elemental status visual overlays (burn/freeze/glow)
- [ ] Add mini bestiary documentation in README or `/docs`

## Phase 14: Spell & Status Effect System
- [ ] Implement generic status effect framework (duration-based effects in thinker loop)
- [ ] Add “Burning” effect (damage over time, fire visuals)
- [ ] Add “Frozen” effect (movement speed reduction)
- [ ] Add “Stunned” effect (temporary attack suppression)
- [ ] Add “Dice Curse” effect (randomized stat variance)
- [ ] Add HUD status effect indicators
- [ ] Expose status parameters via configurable constants

## Phase 15: Advanced Enemy AI Behaviors
- [ ] Add tactical retreat logic for ranged dwarves
- [ ] Add group aggression triggers (pack behavior)
- [ ] Add engineer turret deployment logic
- [ ] Add shaman support logic (buff nearby enemies)
- [ ] Add per-enemy reaction time variance
- [ ] Implement difficulty-scaled AI parameters

## Phase 16: Boss Encounters
- [ ] Design Goblin King boss (multi-phase fight)
- [ ] Design Dwarven War Machine boss (projectile-heavy)
- [ ] Implement phase transition triggers
- [ ] Add boss-specific music cues
- [ ] Add boss health bar overlay
- [ ] Add scripted arena lock system (vanilla-compatible)

## Phase 17: Dice Mechanics Expansion
- [ ] Add “Exploding Dice” mechanic (max roll triggers reroll)
- [ ] Add “Advantage/Disadvantage” system (roll twice, take best/worst)
- [ ] Add combo multiplier system for consecutive crits
- [ ] Add luck stat affecting roll distribution
- [ ] Add configurable crit scaling curves

## Phase 18: Progression System (Optional Mode)
- [ ] Add optional RPG progression mode toggle
- [ ] Implement experience tracking
- [ ] Add level-up stat selection screen
- [ ] Add permanent crit chance scaling
- [ ] Add weapon mastery modifiers
- [ ] Ensure vanilla mode remains unaffected

## Phase 19: Audio Overhaul
- [ ] Add dice roll sound library (per die type)
- [ ] Add crit impact layered sound
- [ ] Add dwarf voice cues
- [ ] Add goblin chatter ambient sounds
- [ ] Add reverb zones for caves
- [ ] Add audio toggle options in setup tool

## Phase 20: Visual Effects Layer
- [ ] Add dynamic screen shake on high rolls
- [ ] Add particle sparks for crit hits
- [ ] Add lava heat shimmer effect
- [ ] Add magic projectile trail rendering
- [ ] Add low-health screen tint
- [ ] Add optional modern FX toggle

## Phase 21: Multiplayer Enhancements
- [ ] Ensure dice mechanics sync deterministically over netplay
- [ ] Add crit event broadcast messages
- [ ] Add optional PvP dice arena mode
- [ ] Add co-op shared crit buffs
- [ ] Add net desync debug logging

## Phase 22: Modding & Extensibility
- [ ] Document dice weapon DEHACKED mappings
- [ ] Expose crit system parameters to config
- [ ] Add custom dice weapon template example
- [ ] Add enemy definition template
- [ ] Create example goblin-themed TC pack
- [ ] Write Modding Guide in `/docs`

## Phase 23: Performance & Determinism Audit
- [ ] Profile thinker loop under heavy dice spam
- [ ] Validate deterministic RNG across platforms
- [ ] Audit floating-point usage (avoid nondeterminism)
- [ ] Benchmark sprite-heavy scenes
- [ ] Test low-spec hardware compatibility
- [ ] Add reproducible demo test suite

## Phase 24: Release Engineering
- [ ] Create versioning scheme (SemVer or similar)
- [ ] Add changelog automation
- [ ] Add release packaging scripts (Linux/Windows/macOS)
- [ ] Create demo WAD bundle
- [ ] Publish first tagged release
- [ ] Draft public roadmap

## Phase 25: Polish & Identity Lock
- [ ] Finalize logo and title screen art
- [ ] Replace remaining Doom legacy strings
- [ ] Audit UI consistency
- [ ] Balance all dice weapons numerically
- [ ] Final playtest pass (solo + co-op)
- [ ] Freeze feature set for v1.0

## Current Status
**Phase 1-8 complete**. Phase 9-13 ready for implementation.
