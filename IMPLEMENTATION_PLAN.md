# IMPLEMENTATION_PLAN.md - Goblin Dice Rollaz

## Priority Tasks

### Phase 1: Determinism Fixes
- [x] Replace floating-point cos() with finecosine[] in p_pspr.c knockback
- [x] Replace floating-point sin() with finesine[] in p_pspr.c knockback
- [x] Replace rand() with M_Random() in m_menu.c particle system
- [x] Audit p_inter.c for floating-point damage calculations
- [x] Audit p_mobj.c for floating-point physics
- [x] Verify all RNG usage uses P_Random/M_Random tables

### Phase 2: Rendering Performance
- [x] Implement hash table for visplane lookup in r_plane.c
- [x] Remove linear search in R_FindPlane function
- [x] Optimize R_CheckPlane overlap scanning
- [x] Pre-cache sprite patches in LRU cache r_things.c
- [x] Optimize O(n) drawseg traversal in sprite column rendering
- [x] Remove redundant clip array reinitialization per sprite
- [x] Add SIMD vectorization to R_DrawColumn inner loop
- [x] Add SIMD vectorization to R_DrawSpan loop
- [x] Optimize R_DrawFuzzColumn cache access pattern
- [x] Remove unused unrolled column loop in r_draw.c
- [x] Remove unused unrolled span loop in r_draw.c
- [x] Remove redundant xtoviewangle table recalculation
- [x] Increase MAXVISPLANES from 128 or make dynamic
- [x] Remove hardcoded &127 texture mask in r_draw.c
- [x] Fix RANGECHECK in r_bsp.c line 543
- [x] Optimize r_segs.c render loop branching
- [x] Add sprite distance culling threshold
- [x] Implement sprite billboarding optimization
- [x] Add wall texture caching for repeated lookups
- [x] Optimize floor/ceiling texture mapping
- [x] Add parallel sector processing for large levels
- [x] Implement mipmap texture streaming
- [x] Add GPU texture compression support
- [x] Optimize screen buffer blitting
- [x] Add palette lookup caching
- [x] Implement sky rendering optimization
- [x] Add detail level adjustment for performance
- [x] Profile and benchmark each rendering subsystem

### Phase 3: Code Cleanup
- [x] Remove all #if 0 blocks in g_game.c
- [x] Remove all #if 0 blocks in p_pspr.c
- [x] Remove all #if 0 blocks in r_main.c
- [x] Remove all #if 0 blocks in r_draw.c
- [x] Remove all #if 0 blocks in p_doors.c
- [x] Remove all #if 0 blocks in p_floor.c
- [x] Remove all #if 0 blocks in p_maputl.c
- [x] Remove commented dead code in r_segs.c
- [x] Remove unused code in dstrings.c
- [x] Remove or convert RANGECHECK in r_things.c
- [x] Remove or convert RANGECHECK in r_draw.c
- [x] Remove or convert RANGECHECK in r_plane.c
- [x] Remove or convert RANGECHECK in p_sight.c
- [x] Split weapon debug overlay from st_stuff.c to st_weapon_debug.c
- [x] Create m_menu_gameopts.c for game settings submenu
- [x] Create m_menu_accessibility.c for accessibility options
- [x] Create p_ai_faction.c for faction AI system

### Phase 4: Rebranding
- [x] Rename "Based on Chocolate Doom engine" banner in d_main.c
- [x] Update CD-ROM config path in d_englsh.h
- [x] Update "DOOM! -- Inferno" string in d_englsh.h
- [x] Update "To continue the DOOM experience" string
- [x] Rename usemodernfx to goblin_modern_fx in m_config.c
- [x] Rename crosshair_* variables to dice_crosshair_*
- [x] Rename crit_boost_bonus to goblin_crit_boost_bonus
- [x] Rename exploding_dice_enabled to goblin_exploding_dice
- [x] Rename show_weapon_stats to goblin_weapon_stats
- [x] Rename status effect CVARs to goblin_status_*
- [x] Rename colorblind_mode to goblin_colorblind_mode
- [x] Rename reduce_motion to goblin_reduce_motion
- [x] Update README.md goblin references
- [x] Update AGENTS.md base engine reference
- [x] Update DESIGN.md project description

### Phase 5: Remove Demo Compatibility
- [x] Remove g_demo_test.c entirely
- [x] Remove GDT_Init function references
- [x] Remove GDT_RecordDemo function references
- [x] Remove GDT_PlayAndVerifyDemo function references
- [x] Remove GDTChecksums function
- [x] Remove GDT_ComputeStateChecksum function
- [x] Remove InitGameVersion demo detection
- [x] Remove G_VanillaVersionCode function
- [x] Remove vanilla_demo_limit flag and handling
- [x] Remove longtics support
- [x] Remove DOOM_191_VERSION handling
- [x] Remove D_NonVanillaRecord function
- [x] Remove D_NonVanillaPlayback function
- [x] Remove -strictdemos command line option
- [x] Remove NET_PACKET_TYPE_ACK deprecated handling
- [x] Remove Lost Soul bounce compatibility in p_mobj.c
- [x] Remove gameversion >= exe_ultimate checks
- [x] Remove demo version mismatch error messages
- [x] Remove demo buffer expansion logic
- [x] Remove demo checksum computation
- [x] Remove demo share functionality in g_replay.c
- [x] Remove G_ExportDemo function
- [x] Remove G_LoadSharedDemo function
- [x] Remove G_ShareDemo function
- [x] Remove demobuffer management code
- [x] Remove demo lump reading/writing
- [x] Remove IsDemoFile function dependencies
- [x] Remove demo1/demo2/demo3 lump handling
- [x] Remove D_CheckNetGame demo warnings
- [x] Clean up g_game.c demo recording section

### Phase 6: Audio System Cleanup
- [x] Remove redundant I_SoundIsPlaying check before stopping
- [x] Optimize S_UpdateSounds channel iteration
- [x] Remove redundant channel scanning in i_sdlsound.c
- [x] Remove profanity comment in s_sound.c line 322
- [x] Remove duplicate #include in s_sound.c line 40
- [x] Remove empty I_InitMusic stub
- [x] Remove empty I_ShutdownMusic stub
- [x] Remove unused snd_sbport config variable
- [x] Remove unused snd_sbirq config variable
- [x] Remove unused snd_sbdma config variable
- [x] Remove unused snd_mport config variable
- [x] Fix CheckVolumeSeparation to be inline
- [x] Remove PC speaker frequency table (i_pcsound.c)
- [x] Remove unused I_PCS_UpdateSoundParams stub
- [x] Clean up DOS sound configuration bindings
- [x] Remove DOS-specific sound module comments
- [x] Consolidate platform-specific sound detection
- [x] Add sound channel pooling optimization
- [x] Remove debug sound allocation logging
- [x] Clean up music module list compilation

### Phase 7: Network System Cleanup
- [x] Remove NET_StartSecureDemo function
- [x] Remove NET_EndSecureDemo function
- [x] Remove securedemo_start_message variable
- [x] Remove unused NET_MASTER_PACKET_TYPE_GET_METADATA
- [x] Remove NET_OLD_MAGIC_NUMBER handling
- [x] Remove NET_TICDIFF_RAVEN legacy support
- [x] Remove NET_TICDIFF_STRIFE legacy support
- [x] Remove protocol_names unused slots
- [x] Remove #ifdef DROP_PACKETS testing code
- [x] Remove commented bandwidth monitoring code
- [x] Consolidate NET_Log under DEBUG_NET flag
- [x] Update master server address or remove queries
- [x] Fix InitPetName to not reseed rand() every call
- [x] Remove unused NET_PACKET_TYPE_ACK
- [x] Clean up NET_MASTER_PACKET_TYPE_SIGN entries
- [x] Remove unused packet type definitions
- [x] Optimize reliable packet ACK handling
- [x] Remove unused NET_ExpandTicNum complexity
- [x] Simplify NET_Conn_ReliablePacket ACK logic
- [x] Remove unused net_client_recv_t memmove code
  - [x] Resolve blocker: memmove code in NET_SV_AdvanceWindow and NET_CL_AdvanceWindow is actively used for network sliding window - cannot be removed without breaking networking
  - [x] Clarification needed: Task describes removing "unused" code, but memmove is essential networking - should task be closed as invalid?
- [x] Clean up SendOneQuery rate limiting
- [x] Reduce MAX_MODULES if too large
- [x] Document network protocol for future reference
- [x] Add network protocol version stub

### Phase 8: Input System Cleanup
- [x] Remove empty I_Tactile function
- [x] Remove unused shiftxform keyboard translation table
- [x] Remove vanilla_keyboard_mapping if not needed
- [x] Implement SDL_SetTextInputRect in i_input.c
- [x] Fix XXX pointer undefined behavior in i_input.c
- [x] Implement UpdateFocus in i_video.c
- [x] Implement fullscreen toggle in i_video.c
- [x] Document Windows stack trace functions
- [x] Document Windows crash handler functions

### Phase 9: Configuration Cleanup
- [x] Remove screenblocks duplicate of screensize
- [x] Remove detaillevel legacy variable
- [x] Remove Strife-only config variables
- [x] Remove Heretic-only config variables
- [x] Consolidate mouse_sensitivity bindings across games
- [x] Remove duplicate config bindings
- [x] Ensure goblin-doom.cfg is default config filename
- [x] Remove default.cfg references where applicable
- [x] Clean up string duplication in chat macro bindings

### Phase 10: Memory Leak Fixes
- [x] Add V_DisableDiceIcon function to free saved_background
- [x] Add V_DisableDiceIcon function to free dice_data
- [x] Add free(gamedescription) in d_main.c shutdown
- [x] Remove unused Z_ChangeUser function
- [x] Remove unused Z_ZoneSize function
- [x] Remove broken Z_DumpHeap implementation
- [x] Remove broken Z_FileDumpHeap implementation

### Phase 11: TODO/FIXME Cleanup
- [x] Resolve i_video.c line 328 SDL2-TODO
- [x] Resolve i_video.c line 746 SDL2-TODO
- [x] Resolve i_input.c line 294 SDL2-TODO
- [x] Resolve i_endoom.c SDL2-TODO items
- [x] Resolve i_video.c fullscreen toggle TODO
- [x] Resolve net_server.c client rejection TODO
- [x] Resolve net_dedicated.c socket polling TODO
- [x] Resolve net_packet.c safe string TODO
- [x] Resolve p_mobj.c NOP function pointer FIXME
- [x] Resolve p_tick.c NOP FIXME
- [x] Resolve d_main.c demo numbers FIXME
- [x] Resolve viewangleoffset handling FIXME
- [ ] Remove all STRIFE-TODO commented code
  - [ ] Resolve blocker: All STRIFE-TODO in src/strife/ will be removed with Phase 36 directory removal
- [x] Remove all STRIFE-FIXME commented code
  - [x] Removed from src/d_iwad.c (non-Strife code)
  - [ ] Resolve blocker: Rest in src/strife/ will be removed with Phase 36 directory removal
- [x] Resolve d_event.h TODO line 117
- [x] Resolve v_video.c TODO line 46
- [ ] Resolve i_musicpack.c TODO line 58
  - [ ] Resolve blocker: Cannot resolve until SDL2_mixer minimum version is bumped to 2.6.0+
- [x] Resolve i_oplmusic TODO items
- [x] Resolve heretic TODO items
- [x] Resolve hexen TODO items
- [x] Resolve setup TODO items
- [x] Resolve m_config.c TODO line 2866
- [x] Resolve d_loop.c TODO items
- [x] Resolve joystick.c XXX comment
- [ ] Remove STRIFE-TODO in p_spec.c lines 1097, 1099
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in p_switch.c lines 796-903
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in p_dialog.c lines 1023, 1163
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in p_enemy.c lines 908, 992, 1746
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in p_floor.c line 265
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in p_inter.c lines 633, 838, 1101
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in g_game.c Strife version
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in hu_stuff.c lines 384, 390
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in d_main.c Strife version
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in doomdata.h lines 140, 143
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Remove STRIFE-TODO in doomdef.h lines 90, 92, 94, 96
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve p_mobj.c second FIXME (line 494)
  - [ ] Resolve blocker: src/strife/p_mobj.c will be removed in Phase 36
- [ ] Resolve p_mobj.c third FIXME (line 485)
  - [ ] Resolve blocker: src/strife/p_mobj.c will be removed in Phase 36
- [ ] Resolve p_saveg.c STRIFE-FIXME line 565
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve m_saves.c STRIFE-FIXME line 435
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve deh_ptr.c FIXME line 60
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve deh_cheat.c STRIFE-FIXME line 55
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve d_main.c Strife FIXME lines 606, 1896, 1903
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve r_main.c FIXME for viewangleoffset (Hexen/Heretic)
  - [ ] Resolve blocker: src/heretic/ and src/hexen/ will be removed in Phase 37
- [ ] Resolve g_game.c Hexen FIXME line 108
  - [ ] Resolve blocker: Hexen code will be removed in Phase 37
- [ ] Resolve g_game.c Heretic FIXME line 128
  - [ ] Resolve blocker: Heretic code will be removed in Phase 37
- [ ] Resolve heretic d_net.c TODO items
  - [ ] Resolve blocker: src/heretic/ will be removed in Phase 37
- [ ] Resolve hexen h2_main.c TODO line 527
  - [ ] Resolve blocker: Hexen code will be removed in Phase 37
- [x] Resolve gusconf.c TODO line 210
- [x] Resolve setup multiplayer.c TODO line 248
- [x] Resolve d_iwad.c STRIFE-FIXME line 50
- [ ] Resolve s_sound.c STRIFE-TODO line 592
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36
- [ ] Resolve st_stuff.c Strife TODO lines 174, 803, 987
  - [ ] Resolve blocker: src/strife/ will be removed in Phase 36

### Phase 12: Platform Code Consolidation
- [x] Move Windows stack trace to platform abstraction
- [x] Move Windows crash handler to platform abstraction
- [x] Document Windows CD-ROM path handling
- [x] Document macOS fullscreen modifier handling
- [x] Remove DOS keyboard shift translation table
- [x] Remove legacy keyboard mapping code

### Phase 13: Build System Cleanup
- [x] Update configure.ac version if needed
- [x] Ensure goblin-doom binary name is consistent

### Phase 14: New Dice Weapons
- [x] Add d2 Flip of Fate weapon (binary damage, 50% 1dmg/50% 2dmg)
- [x] Add d3 Skewered Luck weapon (guaranteed 1-3 damage, piercing)
- [x] Add d7 Seven Veils weapon (ore fragment splash, 14% crit)
- [x] Add d14 Fused Doubler weapon (dual dice, resonance explosion)
- [x] Add d24 Hourglass Cannon weapon (slows enemies, freezes on crit)
- [x] Add d30 Gear Grinder weapon (bonus vs armored dwarves)
- [x] Add d48 Forge Hammer weapon (molten effect on crit, fire dmg)
- [x] Add d50 Dwarf's Bane weapon (3x vs dwarf enemies)
- [x] Add d60 Elemental Chaos weapon (fire/ice/lightning/poison)
- [x] Add d100+1 Fumble Finder weapon (no fumbles, 4x on crit)
- [x] Add d3 Quake Boulder weapon (shockwave, stun)
- [x] Add Chain Reaction d12 weapon (sticks, chains to nearby)
- [x] Add Crystal d8 Shatter weapon (shard spray, shield break)
- [x] Add Splitting d20 weapon (splits into multiple dice)
- [x] Add Greed's d6 Gold Rush weapon (damage stacks on kills)
- [x] Add Mining Charge d10 weapon (proximity mines)
- [x] Add Swarm d6 Dice Storm weapon (rapid fire spread)
- [x] Add Runic Binding d12 weapon (random debuff on hit)

### Phase 15: New Dwarf Enemies
- [x] Add Dwarf Flamethrower (area flame cone, low HP)
- [x] Add Dwarf Thunderer (war drum stuns, moderate HP)
- [x] Add Dwarf Ironclad (rotating minigun, very high HP)
- [x] Add Dwarf Stonecutter (hurls rock fragments, high impact)
- [x] Add Dwarf Thundermage (lightning arcs, medium range)
- [x] Add Dwarf Warlord (boss, 300 HP, immunity aura)
- [x] Add Dwarf Runesmith (reflective rune shields on ground)
- [x] Add Dwarf Runebearer (crit resistance aura, support)
- [x] Add Dwarf High Priest (heals wounded dwarves)
- [x] Add Dwarf Commander (focus fire orders on player)
- [x] Add Dwarf Steam Golem (steam cloud, fire vulnerable)
- [x] Add Dwarf Scrap Drone (swarms of 3-5, low HP each)
- [x] Add Dwarf Drill Tank (drills through thin walls)
- [x] Add Dwarf Siege Engine (stationary explosive cannon)
- [x] Add Dwarf Sky Miner (flying, hover attacks)
- [x] Add Dwarf Mine Layer (plants proximity mines)
- [x] Add Dwarf Tinkerer (repair drones for mechanicals)
- [x] Add Dwarf Barrel Drone (explosive chaser)
- [x] Add Dwarf Shadowblade (cloaked melee, invisible)
- [x] Add Dwarf Obsidian Guardian (volcanic glass, fire weak)
- [x] Add Dwarf Geologist (spawns rock Golems)
- [x] Add Dwarf Alchemist (throws volatile potions)
- [x] Add Dwarf Treasure Hunter (drops extra loot)
- [x] Add Dwarf Juggernaut (slow, devastating melee)
- [x] Add Dwarf Sapper (places explosives on walls)

### Phase 16: New Map Environments
- [x] Add Molten Anvil forge map theme
- [x] Add Barracks of Stone Guard map theme
- [x] Add Great Hall of Maces (chandelier hazards)
- [x] Add Dwarven Armory Vaults (auto-arming traps)
- [x] Add Royal Treasury (pressure plate alarms)
- [x] Add Veinfall Chasm (minecart traps)
- [x] Add Flooded Shafts (electrocution hazard)
- [x] Add Crystalline Grotto (exploding crystals)
- [x] Add Abandoned Excavation (cave-in hazards)
- [x] Add Deep Drill map theme (crushing drill)
- [x] Add Crusher Press (timed piston hazards)
- [x] Add Steam Vent Corridors (periodic bursts)
- [x] Add Minecart Gauntlet (runaway carts)
- [x] Add Lever Room of Doom (puzzle traps)
- [x] Add Turret Foundry (spawning turrets)
- [x] Add Warren Warrens (goblin ambush tunnels)
- [x] Add Goblin Totem Chamber (totem buffs)
- [x] Add Slop Kitchen (environmental kills)
- [x] Add Scrap Metal Fortress (explosive barrels)
- [x] Add Bone Pit (released cage monsters)

### Phase 17: New Powerups
- [x] Add Lucky Seven powerup (7 guaranteed crits)
- [x] Add Exploding Dice powerup (reroll max faces)
- [x] Add Advantage Roll powerup (roll twice take best)
- [x] Add Snake Eyes powerup (mark for double damage)
- [x] Add Dwarven Weakness powerup (2x vs dwarves)
- [x] Add Pickaxe Repellent powerup (dwarves flee)
- [x] Add Gold Fever powerup (attack speed vs treasure)
- [x] Add Greed Is Good powerup (ammo on dwarf kills)
- [x] Add Dash Token powerup (dash with iframes)
- [x] Add Ghost Step powerup (pass through enemies)
- [x] Add Wall Walker powerup (ceiling traversal)
- [x] Add Trap Sense powerup (highlight mines)
- [x] Add Vitality Crystal powerup (permanent +25 HP)
- [x] Add Luck Stone powerup (permanent +5% crit)
- [x] Add Ammo Pouch powerup (permanent +50% ammo)
- [x] Add Berserker Regen powerup (regen at low HP)
- [ ] Add Cursed D4 powerup (3x dmg, lose armor)
- [ ] Add Blood Pact powerup (infinite ammo, health drain)
- [ ] Add Madness Die powerup (random +/- on kills)
- [x] Add Glass Cannon powerup (2x dmg, 50% HP)

### Phase 18: Code Quality Improvements
- [x] Define DEFAULT_DICE_VOLUME constant in m_menu.c
- [x] Define DEFAULT_SHAKE_SCALE and DEFAULT_DAMAGE_SCALE
- [x] Define MAX_MENU_PARTICLES constant
- [x] Define SHAKE_LOW, SHAKE_MEDIUM, SHAKE_HIGH constants
- [x] Define CRIT_CHANCE_SCALE constant in p_inter.c
- [x] Define movement speed constants in g_game.c
- [x] Fix typo: indetermined -> undetermined in doomstat.c
- [x] Fix inconsistent naming: standardize snake_case for goblin vars
- [x] Add NULL check after malloc in m_menu.c
  - [x] Resolve blocker: m_menu.c uses static arrays, no malloc exists
- [x] Add NULL check after malloc in d_main.c
- [x] Add NULL check after strdup in d_main.c
- [x] Add NULL check after calloc in w_wad.c
  - [x] Resolve blocker: w_wad.c uses Z_Malloc, no calloc exists
- [x] Add NULL check after malloc in p_saveg.c
- [x] Make video_driver const char* in i_video.c
- [x] Make window_position const char* in i_video.c
- [x] Make snd_musiccmd const char* in i_sound.c
- [x] Expand dice_str buffer from 4 to 8 bytes in m_menu.c
- [ ] Expand level name buffer in m_menu.c line 1931
  - [ ] Resolve blocker: Line numbers shifted, cannot find specific "level name buffer" to expand

### Phase 19: Additional Rendering Optimizations
- [x] Add sprite occlusion culling for off-screen sprites
- [x] Implement temporal AA for smooth sprite edges
- [ ] Add texture LOD switching for distant walls
- [ ] Optimize colormap lookups with caching
- [ ] Add hardware acceleration detection and use
- [ ] Implement variable draw distance for particles
- [ ] Add batch rendering for similar sprites
- [ ] Profile and optimize texture upload bandwidth

### Phase 20: Additional Audio Features
- [ ] Add 3D positional audio for dwarf voice cues
- [ ] Add reverb zones for cave environments
- [ ] Implement dynamic music intensity based on combat
- [ ] Add dice roll sound variation per die type
- [ ] Add critical hit layered sound effects
- [ ] Implement audio LOD for distant sounds
- [ ] Add volume slider for dice roll sounds in menu
- [ ] Implement weapon-specific sound profiles

### Phase 21: Save System Improvements
- [ ] Add compressed save games
- [ ] Add save game checksum validation
- [ ] Add auto-save on player death
- [ ] Add continue from last save option
- [ ] Add save game comments with timestamp
- [ ] Implement cloud save support stub
- [ ] Add cross-version save compatibility

### Phase 22: Modding Support
- [ ] Add custom dice weapon DEHACKED template
- [ ] Add custom enemy definition template
- [ ] Document WAD sprite replacement requirements
- [ ] Add mod loader error reporting
- [ ] Add mod load order configuration
- [ ] Implement mod compatibility checker

### Phase 23: Testing Infrastructure
- [x] Add automated build test script
- [ ] Add basic gameplay smoke test
- [ ] Add deterministic replay test
- [ ] Add memory leak detection test
- [ ] Add save/load roundtrip test
- [ ] Add config file parsing test
- [ ] Add network basic functionality test

### Phase 24: Performance Profiling
- [ ] Add FPS counter debug overlay
- [ ] Add frame time profiling
- [ ] Add memory usage display
- [ ] Add draw call counter
- [ ] Add sprite batch count display
- [ ] Add network latency display
- [ ] Add think time profiler

### Phase 25: Enemy Behavior AI Expansion
- [ ] Add dwarf formation movement (shield wall)
- [ ] Add dwarf call for help behavior
- [ ] Add dwarf pursuit stamina (give up after distance)
- [ ] Add goblin cowardice when leader dies
- [ ] Add goblin flanking coordination
- [ ] Add dwarf weapon switching on low ammo
- [ ] Add enemy dodge rolling behavior
- [ ] Add surprise attack detection zones
- [ ] Add pathfinding around hazards
- [ ] Add dynamic difficulty scaling AI

### Phase 26: Player Movement Enhancements
- [ ] Add dodge roll ability
- [ ] Add mantling/climbing small obstacles
- [ ] Add crouch/slide mechanics
- [ ] Add wall jump in narrow passages
- [ ] Add ledge grab/climb ability
- [ ] Add swimming physics for flooded areas
- [ ] Add ladder climbing mechanics
- [ ] Add momentum-based movement
- [ ] Add air control improvements

### Phase 27: Combat System Expansion
- [ ] Add combo system (consecutive hits increase damage)
- [ ] Add parry/timing window for defense
- [ ] Add weak point targeting (headshots)
- [ ] Add armor penetration mechanics
- [ ] Add elemental combo reactions (fire + ice = steam)
- [ ] Add stealth backstab damage bonus
- [ ] Add rage meter for berserker mode
- [ ] Add weapon durability (optional)
- [ ] Add dismemberment on critical hits (with appropriate content)

### Phase 28: HUD and UI Improvements
- [ ] Add damage type indicator icons
- [ ] Add enemy health bar on hover
- [ ] Add ammo indicator for next weapon
- [ ] Add dice roll result display
- [ ] Add combo counter UI element
- [ ] Add minimap with enemy positions
- [ ] Add mission objective display
- [ ] Add tutorial overlay system
- [ ] Add kill feed for multiplayer
- [ ] Add death recap screen

### Phase 29: Level Design Tools
- [ ] Add spawner entity for dice weapons
- [ ] Add spawner entity for powerups
- [ ] Add scripted event triggers
- [ ] Add environment damage sectors
- [ ] Add moving platform templates
- [ ] Add secret detection triggers
- [ ] Add music change triggers
- [ ] Add lighting change triggers
- [ ] Add cinematic cutscene support
- [ ] Add checkpoint system

### Phase 30: Multiplayer Features
- [ ] Add class selection system
- [ ] Add loadout customization
- [ ] Add team deathmatch mode
- [ ] Add capture the flag mode
- [ ] Add king of the hill mode
- [ ] Add lobby browser
- [ ] Add party system
- [ ] Add match statistics
- [ ] Add vote kick functionality
- [ ] Add admin tools

### Phase 31: Integrate Strife Features
- [ ] Import Strife dialog system into core engine
- [ ] Import Strife inventory system (key items, quests)
- [ ] Import Strife NPC interaction system
- [ ] Import Strife quest tracking system
- [ ] Import Strife quest journal UI
- [ ] Import Strife multi-key binding system
- [ ] Import Strife shop/merchant system
- [ ] Import Strife weapon dropping/pickup system
- [ ] Import Strife companion/follower system
- [ ] Import Strife climbing/ledge grab mechanics
- [ ] Import Strife view bobbing system
- [ ] Import Strife macrotized textures
- [ ] Import Strife macGuffin pickups

### Phase 32: Integrate Heretic Features
- [ ] Import Heretic artifact/pickup system
- [ ] Import Heretic Tome of Power powerup
- [ ] Import Heretic Wings of Wrath flight
- [ ] Import Heretic ethereal ammo system
- [ ] Import Heretic staff weapon mechanics
- [ ] Import Heretic magic system (mana)
- [ ] Import Heretic dragon attack patterns
- [ ] Import Heretic iron lich phases
- [ ] Import Heretic water/lava physics
- [ ] Import Heretic day/night cycle lighting
- [ ] Import Heretic spell casting UI
- [ ] Import Heretic scroll pickup system

### Phase 33: Integrate Hexen Features
- [ ] Import Hexen class system (Corvus, Crusader, Necromancer)
- [ ] Import Hexen class-specific abilities
- [ ] Import Hexen mana/fetish system
- [ ] Import Hexen class switching mechanic
- [ ] Import Hexen artifact class restrictions
- [ ] Import Hexen teleport sphere system
- [ ] Import Hexen puzzle key system
- [ ] Import Hexen class-based starting items
- [ ] Import Hexen experience/leveling (optional)
- [ ] Import Hexen class stat modifiers
- [ ] Import Hexen class weapon proficiencies
- [ ] Import Hexen ring of telefrag

### Phase 34: Unified Engine Core
- [ ] Create unified actor spawner system
- [ ] Create unified weapon definition framework
- [ ] Create unified enemy type registry
- [ ] Create unified powerup system
- [ ] Create unified inventory/item system
- [ ] Create unified quest system
- [ ] Create unified dialog system
- [ ] Create unified shop/merchant system
- [ ] Create unified NPC interaction system
- [ ] Create unified class system
- [ ] Create unified save/load format
- [ ] Create unified config system
- [ ] Create unified mod loading framework
- [ ] Create unified content pack system
- [ ] Create unified game mode selector
- [ ] Create unified difficulty system
- [ ] Create unified achievement system
- [ ] Create unified statistics tracking
- [ ] Create unified story/mission system

### Phase 35: Deprecate Game-Specific Directories
- [ ] Mark src/strife/ as deprecated
- [ ] Mark src/heretic/ as deprecated
- [ ] Mark src/hexen/ as deprecated
- [ ] Update build system to hide deprecated games
- [ ] Update config defaults to goblin-doom only
- [ ] Update IWAD detection for main game only
- [ ] Remove game selection from menu (default to goblin)
- [ ] Add deprecation warnings to build logs
- [ ] Update documentation to note deprecation
- [ ] Create migration guide for mods

### Phase 36: Remove Strife Code
- [ ] Remove src/strife/ directory entirely
- [ ] Remove Strife-specific functions from g_game.c
- [ ] Remove Strife-specific functions from d_main.c
- [ ] Remove Strife-specific functions from doomstat.c
- [ ] Remove Strife-specific functions from d_net.c
- [ ] Remove Strife-specific sound module
- [ ] Remove Strife-specific savegamet
- [ ] Remove Strife-specific HUD rendering
- [ ] Remove Strife-specific map format support
- [ ] Remove Strife-specific enemy types
- [ ] Remove Strife-specific weapon types
- [ ] Remove Strife-specific powerup types
- [ ] Clean up #ifdef Strife from all files
- [ ] Update CHANGELOG with Strife removal

### Phase 37: Remove Heretic/Hexen Code
- [ ] Remove src/heretic/ directory entirely
- [ ] Remove src/hexen/ directory entirely
- [ ] Remove Heretic-specific functions from g_game.c
- [ ] Remove Hexen-specific functions from g_game.c
- [ ] Remove Heretic-specific functions from d_main.c
- [ ] Remove Hexen-specific functions from d_main.c
- [ ] Remove Heretic-specific sound modules
- [ ] Remove Hexen-specific sound modules
- [ ] Remove Heretic-specific enemy types
- [ ] Remove Hexen-specific enemy types
- [ ] Remove Heretic weapon types
- [ ] Remove Hexen weapon types
- [ ] Remove Heretic powerup types
- [ ] Remove Hexen powerup types
- [ ] Remove class system from Hexen (move to unified)
- [ ] Remove mana system (move to unified)
- [ ] Clean up #ifdef Heretic from all files
- [ ] Clean up #ifdef Hexen from all files
- [ ] Update CHANGELOG with Heretic/Hexen removal

### Phase 38: Finalize Unified Engine
- [ ] Remove configure.ac game selection options
- [ ] Remove CMake game-specific build options
- [ ] Remove setup tool game selection
- [ ] Remove doom/id/ references to other games
- [ ] Update VERSION to Goblin Dice Rollaz only
- [ ] Update copyright headers
- [ ] Remove unused doom_build.h entries
- [ ] Final cleanup of any remaining #ifdef GAME
- [ ] Verify build produces single executable
- [ ] Test all features still work
- [ ] Update all documentation for unified engine
- [ ] Create unified engine README

### Phase 39: New Faction - Skeletons
- [ ] Add Skeleton Warrior enemy (basic undead dwarf)
- [ ] Add Skeleton Archer enemy (ranged attacks)
- [ ] Add Skeleton Mage enemy (dark magic attacks)
- [ ] Add Skeleton Champion enemy (elite warrior)
- [ ] Add Skeleton King boss (summons minions)
- [ ] Add Skeleton Priest enemy (heals undead)
- [ ] Add Bone Golem enemy (constructed from dwarf bones)
- [ ] Add Ghost Dwarf enemy (ethereal, phase through walls)
- [ ] Add Lich Dwarf enemy (powerful necromancer)
- [ ] Add Skeleton Dog enemy (fast swarm)
- [ ] Add skeleton faction AI (fear fire, crave blood)
- [ ] Add skeleton weakness to holy/light damage
- [ ] Add skeleton drop table (bones, soul gems)

### Phase 40: Additional Dwarf Content
- [ ] Add Dwarf Veterans (experienced soldiers)
- [ ] Add Dwarf Pyromaniac (explosives expert)
- [ ] Add Dwarf Beastmaster (tames cave creatures)
- [ ] Add Dwarf Courier (runs messages, low HP)
- [ ] Add Dwarf Scholar (defensive magic)
- [ ] Add Dwarf Ghost Hunter (anti-undead specialist)
- [ ] Add Dwarf War Wagon (mobile fort)
- [ ] Add Dwarf Ballista (stationary anti-air)
- [ ] Add Elite Dwarf Guard (boss bodyguards)
- [ ] Add Dwarf Queen (heals, buffs, royal)
- [ ] Add dwarf veteran weapon upgrades
- [ ] Add dwarf scar patterns for identification
- [ ] Add dwarf clan banners (territory markers)

### Phase 41: Elemental System
- [ ] Define elemental damage types (Fire, Ice, Lightning, Poison, Holy, Dark)
- [ ] Add elemental damage to weapon definitions
- [ ] Add elemental weakness/resistance to enemy definitions
- [ ] Implement elemental combo reactions (Fire + Ice = Steam blast)
- [ ] Add elemental aura pickup items
- [ ] Add elemental weapon modifications
- [ ] Add elemental resistance status effect
- [ ] Add burning damage over time
- [ ] Add frozen/slowed status
- [ ] Add lightning chain to nearby enemies
- [ ] Add poison damage and weakening
- [ ] Add holy damage vs undead
- [ ] Add dark damage vs living
- [ ] Add elemental particle effects
- [ ] Add elemental damage numbers (color-coded)

### Phase 42: Crafting System
- [ ] Add material pickup system (ores, bones, herbs)
- [ ] Add crafting menu UI
- [ ] Add crafting recipes for weapons
- [ ] Add crafting recipes for armor
- [ ] Add crafting recipes for powerups
- [ ] Add material drop tables for enemies
- [ ] Add material spawners in levels
- [ ] Add crafting station locations
- [ ] Add material inventory UI
- [ ] Add rare/legendary material types
- [ ] Add disassembly of unwanted items
- [ ] Add crafting skill level (improves yields)
- [ ] Add recipe discovery system
- [ ] Add crafting material exchange

### Phase 43: Skill Trees
- [ ] Define skill tree categories (Combat, Magic, Survival)
- [ ] Add Combat skill tree (damage, speed, crit)
- [ ] Add Magic skill tree (mana efficiency, spell power)
- [ ] Add Survival skill tree (health, defense, recovery)
- [ ] Add skill point acquisition system
- [ ] Add skill point rewards from kills
- [ ] Add skill point rewards from quests
- [ ] Add skill reset option
- [ ] Add skill tree UI display
- [ ] Add passive ability unlocks
- [ ] Add ultimate abilities at tree completion
- [ ] Add skill respecing items
- [ ] Add skill-based difficulty scaling

### Phase 44: Rogue-like Mode
- [ ] Add procedural level generation
- [ ] Add permadeath system
- [ ] Add per-run character progression
- [ ] Add rogue-like run start loadout selection
- [ ] Add roguelike item pickups
- [ ] Add curse items (risk/reward)
- [ ] Add floor curse system (difficulty scaling)
- [ ] Add boss rush mode
- [ ] Add daily challenge runs
- [ ] Add high score tracking
- [ ] Add run statistics
- [ ] Add unlockable characters/classes
- [ ] Add meta-progression between runs

### Phase 45: Puzzle Dungeons
- [ ] Add pressure plate triggers
- [ ] Add switch/lever puzzles
- [ ] Add timing-based puzzles
- [ ] Add sequence puzzles
- [ ] Add environment interaction puzzles
- [ ] Add rune inscription puzzles
- [ ] Add hidden switch discovery
- [ ] Add puzzle solution rewards
- [ ] Add puzzle hint system
- [ ] Add puzzle room design templates
- [ ] Add trap disarming mechanics
- [ ] Add timed escape sequences
- [ ] Add puzzle completion scoring

### Phase 46: Siege Battles
- [x] Add large-scale battle AI (50+ enemies)
- [x] Add siege weapon interactions
- [ ] Add defensive position combat
- [ ] Add wave-based assault mode
- [ ] Add objective-based siege (capture point)
- [ ] Add allied NPC soldiers
- [ ] Add destructible environment
- [ ] Add siege engine vehicles
- [ ] Add breach point combat
- [ ] Add defend the champion mode
- [ ] Add siege victory conditions
- [ ] Add siege defeat conditions
- [ ] Add siege arena builder

## Current Status

**V1 complete**. V2 Technical Improvements in progress - 46 phases, ~750 tasks.
