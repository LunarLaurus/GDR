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
- [ ] Implement hash table for visplane lookup in r_plane.c
- [ ] Remove linear search in R_FindPlane function
- [ ] Optimize R_CheckPlane overlap scanning
- [ ] Pre-cache sprite patches in LRU cache r_things.c
- [ ] Optimize O(n) drawseg traversal in sprite column rendering
- [ ] Remove redundant clip array reinitialization per sprite
- [ ] Add SIMD vectorization to R_DrawColumn inner loop
- [ ] Add SIMD vectorization to R_DrawSpan loop
- [ ] Optimize R_DrawFuzzColumn cache access pattern
- [ ] Remove unused unrolled column loop in r_draw.c
- [ ] Remove unused unrolled span loop in r_draw.c
- [ ] Remove redundant xtoviewangle table recalculation
- [ ] Increase MAXVISPLANES from 128 or make dynamic
- [ ] Remove hardcoded &127 texture mask in r_draw.c
- [ ] Fix RANGECHECK in r_bsp.c line 543
- [ ] Optimize r_segs.c render loop branching
- [ ] Add sprite distance culling threshold
- [ ] Implement sprite billboarding optimization
- [ ] Add wall texture caching for repeated lookups
- [ ] Optimize floor/ceiling texture mapping
- [ ] Add parallel sector processing for large levels
- [ ] Implement mipmap texture streaming
- [ ] Add GPU texture compression support
- [ ] Optimize screen buffer blitting
- [ ] Add palette lookup caching
- [ ] Implement sky rendering optimization
- [ ] Add detail level adjustment for performance
- [ ] Profile and benchmark each rendering subsystem

### Phase 3: Code Cleanup
- [ ] Remove all #if 0 blocks in g_game.c
- [ ] Remove all #if 0 blocks in p_pspr.c
- [ ] Remove all #if 0 blocks in r_main.c
- [ ] Remove all #if 0 blocks in r_draw.c
- [ ] Remove all #if 0 blocks in p_doors.c
- [ ] Remove all #if 0 blocks in p_floor.c
- [ ] Remove all #if 0 blocks in p_maputl.c
- [ ] Remove commented dead code in r_segs.c
- [ ] Remove unused code in dstrings.c
- [ ] Remove or convert RANGECHECK in r_things.c
- [ ] Remove or convert RANGECHECK in r_draw.c
- [ ] Remove or convert RANGECHECK in r_plane.c
- [ ] Remove or convert RANGECHECK in p_sight.c
- [ ] Split weapon debug overlay from st_stuff.c to st_weapon_debug.c
- [ ] Create m_menu_gameopts.c for game settings submenu
- [ ] Create m_menu_accessibility.c for accessibility options
- [ ] Create p_ai_faction.c for faction AI system

### Phase 4: Rebranding
- [ ] Rename "Based on Chocolate Doom engine" banner in d_main.c
- [ ] Update CD-ROM config path in d_englsh.h
- [ ] Update "DOOM! -- Inferno" string in d_englsh.h
- [ ] Update "To continue the DOOM experience" string
- [ ] Rename usemodernfx to goblin_modern_fx in m_config.c
- [ ] Rename crosshair_* variables to dice_crosshair_*
- [ ] Rename crit_boost_bonus to goblin_crit_boost_bonus
- [ ] Rename exploding_dice_enabled to goblin_exploding_dice
- [ ] Rename show_weapon_stats to goblin_weapon_stats
- [ ] Rename status effect CVARs to goblin_status_*
- [ ] Rename colorblind_mode to goblin_colorblind_mode
- [ ] Rename reduce_motion to goblin_reduce_motion
- [ ] Update README.md goblin references
- [ ] Update AGENTS.md base engine reference
- [ ] Update DESIGN.md project description

### Phase 5: Remove Demo Compatibility
- [ ] Remove g_demo_test.c entirely
- [ ] Remove GDT_Init function references
- [ ] Remove GDT_RecordDemo function references
- [ ] Remove GDT_PlayAndVerifyDemo function references
- [ ] Remove GDTChecksums function
- [ ] Remove GDT_ComputeStateChecksum function
- [ ] Remove InitGameVersion demo detection
- [ ] Remove G_VanillaVersionCode function
- [ ] Remove vanilla_demo_limit flag and handling
- [ ] Remove longtics support
- [ ] Remove DOOM_191_VERSION handling
- [ ] Remove D_NonVanillaRecord function
- [ ] Remove D_NonVanillaPlayback function
- [ ] Remove -strictdemos command line option
- [ ] Remove NET_PACKET_TYPE_ACK deprecated handling
- [ ] Remove Lost Soul bounce compatibility in p_mobj.c
- [ ] Remove gameversion >= exe_ultimate checks
- [ ] Remove demo version mismatch error messages
- [ ] Remove demo buffer expansion logic
- [ ] Remove demo checksum computation
- [ ] Remove demo share functionality in g_replay.c
- [ ] Remove G_ExportDemo function
- [ ] Remove G_LoadSharedDemo function
- [ ] Remove G_ShareDemo function
- [ ] Remove demobuffer management code
- [ ] Remove demo lump reading/writing
- [ ] Remove IsDemoFile function dependencies
- [ ] Remove demo1/demo2/demo3 lump handling
- [ ] Remove D_CheckNetGame demo warnings
- [ ] Clean up g_game.c demo recording section

### Phase 6: Audio System Cleanup
- [ ] Remove redundant I_SoundIsPlaying check before stopping
- [ ] Optimize S_UpdateSounds channel iteration
- [ ] Remove redundant channel scanning in i_sdlsound.c
- [ ] Remove profanity comment in s_sound.c line 322
- [ ] Remove duplicate #include in s_sound.c line 40
- [ ] Remove empty I_InitMusic stub
- [ ] Remove empty I_ShutdownMusic stub
- [ ] Remove unused snd_sbport config variable
- [ ] Remove unused snd_sbirq config variable
- [ ] Remove unused snd_sbdma config variable
- [ ] Remove unused snd_mport config variable
- [ ] Fix CheckVolumeSeparation to be inline
- [ ] Remove PC speaker frequency table (i_pcsound.c)
- [ ] Remove unused I_PCS_UpdateSoundParams stub
- [ ] Clean up DOS sound configuration bindings
- [ ] Remove DOS-specific sound module comments
- [ ] Consolidate platform-specific sound detection
- [ ] Add sound channel pooling optimization
- [ ] Remove debug sound allocation logging
- [ ] Clean up music module list compilation

### Phase 7: Network System Cleanup
- [ ] Remove NET_StartSecureDemo function
- [ ] Remove NET_EndSecureDemo function
- [ ] Remove securedemo_start_message variable
- [ ] Remove unused NET_MASTER_PACKET_TYPE_GET_METADATA
- [ ] Remove NET_OLD_MAGIC_NUMBER handling
- [ ] Remove NET_TICDIFF_RAVEN legacy support
- [ ] Remove NET_TICDIFF_STRIFE legacy support
- [ ] Remove protocol_names unused slots
- [ ] Remove #ifdef DROP_PACKETS testing code
- [ ] Remove commented bandwidth monitoring code
- [ ] Consolidate NET_Log under DEBUG_NET flag
- [ ] Update master server address or remove queries
- [ ] Fix InitPetName to not reseed rand() every call
- [ ] Remove unused NET_PACKET_TYPE_ACK
- [ ] Clean up NET_MASTER_PACKET_TYPE_SIGN entries
- [ ] Remove unused packet type definitions
- [ ] Optimize reliable packet ACK handling
- [ ] Remove unused NET_ExpandTicNum complexity
- [ ] Simplify NET_Conn_ReliablePacket ACK logic
- [ ] Remove unused net_client_recv_t memmove code
- [ ] Clean up SendOneQuery rate limiting
- [ ] Reduce MAX_MODULES if too large
- [ ] Document network protocol for future reference
- [ ] Add network protocol version stub

### Phase 8: Input System Cleanup
- [ ] Remove empty I_Tactile function
- [ ] Remove unused shiftxform keyboard translation table
- [ ] Remove vanilla_keyboard_mapping if not needed
- [ ] Implement SDL_SetTextInputRect in i_input.c
- [ ] Fix XXX pointer undefined behavior in i_input.c
- [ ] Implement UpdateFocus in i_video.c
- [ ] Implement fullscreen toggle in i_video.c
- [ ] Document Windows stack trace functions
- [ ] Document Windows crash handler functions

### Phase 9: Configuration Cleanup
- [ ] Remove screenblocks duplicate of screensize
- [ ] Remove detaillevel legacy variable
- [ ] Remove Strife-only config variables
- [ ] Remove Heretic-only config variables
- [ ] Consolidate mouse_sensitivity bindings across games
- [ ] Remove duplicate config bindings
- [ ] Ensure goblin-doom.cfg is default config filename
- [ ] Remove default.cfg references where applicable
- [ ] Clean up string duplication in chat macro bindings

### Phase 10: Memory Leak Fixes
- [ ] Add V_DisableDiceIcon function to free saved_background
- [ ] Add V_DisableDiceIcon function to free dice_data
- [ ] Add free(gamedescription) in d_main.c shutdown
- [ ] Remove unused Z_ChangeUser function
- [ ] Remove unused Z_ZoneSize function
- [ ] Remove broken Z_DumpHeap implementation
- [ ] Remove broken Z_FileDumpHeap implementation

### Phase 11: TODO/FIXME Cleanup
- [ ] Resolve i_video.c line 328 SDL2-TODO
- [ ] Resolve i_video.c line 746 SDL2-TODO
- [ ] Resolve i_input.c line 294 SDL2-TODO
- [ ] Resolve i_endoom.c SDL2-TODO items
- [ ] Resolve i_video.c fullscreen toggle TODO
- [ ] Resolve net_server.c client rejection TODO
- [ ] Resolve net_dedicated.c socket polling TODO
- [ ] Resolve net_packet.c safe string TODO
- [ ] Resolve p_mobj.c NOP function pointer FIXME
- [ ] Resolve p_tick.c NOP FIXME
- [ ] Resolve d_main.c demo numbers FIXME
- [ ] Resolve viewangleoffset handling FIXME
- [ ] Remove all STRIFE-TODO commented code
- [ ] Remove all STRIFE-FIXME commented code
- [ ] Resolve d_event.h TODO line 117
- [ ] Resolve v_video.c TODO line 46
- [ ] Resolve i_musicpack.c TODO line 58
- [ ] Resolve i_oplmusic TODO items
- [ ] Resolve heretic TODO items
- [ ] Resolve hexen TODO items
- [ ] Resolve setup TODO items
- [ ] Resolve m_config.c TODO line 2866
- [ ] Resolve d_loop.c TODO items
- [ ] Resolve joystick.c XXX comment
- [ ] Remove STRIFE-TODO in p_spec.c lines 1097, 1099
- [ ] Remove STRIFE-TODO in p_switch.c lines 796-903
- [ ] Remove STRIFE-TODO in p_dialog.c lines 1023, 1163
- [ ] Remove STRIFE-TODO in p_enemy.c lines 908, 992, 1746
- [ ] Remove STRIFE-TODO in p_floor.c line 265
- [ ] Remove STRIFE-TODO in p_inter.c lines 633, 838, 1101
- [ ] Remove STRIFE-TODO in g_game.c Strife version
- [ ] Remove STRIFE-TODO in hu_stuff.c lines 384, 390
- [ ] Remove STRIFE-TODO in d_main.c Strife version
- [ ] Remove STRIFE-TODO in doomdata.h lines 140, 143
- [ ] Remove STRIFE-TODO in doomdef.h lines 90, 92, 94, 96
- [ ] Resolve p_mobj.c second FIXME (line 494)
- [ ] Resolve p_mobj.c third FIXME (line 485)
- [ ] Resolve p_saveg.c STRIFE-FIXME line 565
- [ ] Resolve m_saves.c STRIFE-FIXME line 435
- [ ] Resolve deh_ptr.c FIXME line 60
- [ ] Resolve deh_cheat.c STRIFE-FIXME line 55
- [ ] Resolve d_main.c Strife FIXME lines 606, 1896, 1903
- [ ] Resolve r_main.c FIXME for viewangleoffset (Hexen/Heretic)
- [ ] Resolve g_game.c Hexen FIXME line 108
- [ ] Resolve g_game.c Heretic FIXME line 128
- [ ] Resolve heretic d_net.c TODO items
- [ ] Resolve hexen h2_main.c TODO line 527
- [ ] Resolve gusconf.c TODO line 210
- [ ] Resolve setup multiplayer.c TODO line 248
- [ ] Resolve d_iwad.c STRIFE-FIXME line 50
- [ ] Resolve s_sound.c STRIFE-TODO line 592
- [ ] Resolve st_stuff.c Strife TODO lines 174, 803, 987

### Phase 12: Platform Code Consolidation
- [ ] Move Windows stack trace to platform abstraction
- [ ] Move Windows crash handler to platform abstraction
- [ ] Document Windows CD-ROM path handling
- [ ] Document macOS fullscreen modifier handling
- [ ] Remove DOS keyboard shift translation table
- [ ] Remove legacy keyboard mapping code

### Phase 13: Build System Cleanup
- [ ] Update configure.ac version if needed
- [ ] Ensure goblin-doom binary name is consistent

### Phase 14: New Dice Weapons
- [ ] Add d2 Flip of Fate weapon (binary damage, 50% 1dmg/50% 2dmg)
- [ ] Add d3 Skewered Luck weapon (guaranteed 1-3 damage, piercing)
- [ ] Add d7 Seven Veins weapon (ore fragment splash, 14% crit)
- [ ] Add d14 Fused Doubler weapon (dual dice, resonance explosion)
- [ ] Add d24 Hourglass Cannon weapon (slows enemies, freezes on crit)
- [ ] Add d30 Gear Grinder weapon (bonus vs armored dwarves)
- [ ] Add d48 Forge Hammer weapon (molten effect on crit, fire dmg)
- [ ] Add d50 Dwarf's Bane weapon (3x vs dwarf enemies)
- [ ] Add d60 Elemental Chaos weapon (fire/ice/lightning/poison)
- [ ] Add d100+1 Fumble Finder weapon (no fumbles, 4x on crit)
- [ ] Add d3 Quake Boulder weapon (shockwave, stun)
- [ ] Add Chain Reaction d12 weapon (sticks, chains to nearby)
- [ ] Add Crystal d8 Shatter weapon (shard spray, shield break)
- [ ] Add Splitting d20 weapon (splits into multiple dice)
- [ ] Add Greed's d6 Gold Rush weapon (damage stacks on kills)
- [ ] Add Mining Charge d10 weapon (proximity mines)
- [ ] Add Swarm d6 Dice Storm weapon (rapid fire spread)
- [ ] Add Runic Binding d12 weapon (random debuff on hit)

### Phase 15: New Dwarf Enemies
- [ ] Add Dwarf Flamethrower (area flame cone, low HP)
- [ ] Add Dwarf Thunderer (war drum stuns, moderate HP)
- [ ] Add Dwarf Ironclad (rotating minigun, very high HP)
- [ ] Add Dwarf Stonecutter (hurls rock fragments, high impact)
- [ ] Add Dwarf Thundermage (lightning arcs, medium range)
- [ ] Add Dwarf Warlord (boss, 300 HP, immunity aura)
- [ ] Add Dwarf Runesmith (reflective rune shields on ground)
- [ ] Add Dwarf Runebearer (crit resistance aura, support)
- [ ] Add Dwarf High Priest (heals wounded dwarves)
- [ ] Add Dwarf Commander (focus fire orders on player)
- [ ] Add Dwarf Steam Golem (steam cloud, fire vulnerable)
- [ ] Add Dwarf Scrap Drone (swarms of 3-5, low HP each)
- [ ] Add Dwarf Drill Tank (drills through thin walls)
- [ ] Add Dwarf Siege Engine (stationary explosive cannon)
- [ ] Add Dwarf Sky Miner (flying, hover attacks)
- [ ] Add Dwarf Mine Layer (plants proximity mines)
- [ ] Add Dwarf Tinkerer (repair drones for mechanicals)
- [ ] Add Dwarf Barrel Drone (explosive chaser)
- [ ] Add Dwarf Shadowblade (cloaked melee, invisible)
- [ ] Add Dwarf Obsidian Guardian (volcanic glass, fire weak)
- [ ] Add Dwarf Geologist (spawns rock Golems)
- [ ] Add Dwarf Alchemist (throws volatile potions)
- [ ] Add Dwarf Treasure Hunter (drops extra loot)
- [ ] Add Dwarf Juggernaut (slow, devastating melee)
- [ ] Add Dwarf Sapper (places explosives on walls)

### Phase 16: New Map Environments
- [ ] Add Molten Anvil forge map theme
- [ ] Add Barracks of Stone Guard map theme
- [ ] Add Great Hall of Maces (chandelier hazards)
- [ ] Add Dwarven Armory Vaults (auto-arming traps)
- [ ] Add Royal Treasury (pressure plate alarms)
- [ ] Add Veinfall Chasm (minecart traps)
- [ ] Add Flooded Shafts (electrocution hazard)
- [ ] Add Crystalline Grotto (exploding crystals)
- [ ] Add Abandoned Excavation (cave-in hazards)
- [ ] Add Deep Drill map theme (crushing drill)
- [ ] Add Crusher Press (timed piston hazards)
- [ ] Add Steam Vent Corridors (periodic bursts)
- [ ] Add Minecart Gauntlet (runaway carts)
- [ ] Add Lever Room of Doom (puzzle traps)
- [ ] Add Turret Foundry (spawning turrets)
- [ ] Add Warren Warrens (goblin ambush tunnels)
- [ ] Add Goblin Totem Chamber (totem buffs)
- [ ] Add Slop Kitchen (environmental kills)
- [ ] Add Scrap Metal Fortress (explosive barrels)
- [ ] Add Bone Pit (released cage monsters)

### Phase 17: New Powerups
- [ ] Add Lucky Seven powerup (7 guaranteed crits)
- [ ] Add Exploding Dice powerup (reroll max faces)
- [ ] Add Advantage Roll powerup (roll twice take best)
- [ ] Add Snake Eyes powerup (mark for double damage)
- [ ] Add Dwarven Weakness powerup (2x vs dwarves)
- [ ] Add Pickaxe Repellent powerup (dwarves flee)
- [ ] Add Gold Fever powerup (attack speed vs treasure)
- [ ] Add Greed Is Good powerup (ammo on dwarf kills)
- [ ] Add Dash Token powerup (dash with iframes)
- [ ] Add Ghost Step powerup (pass through enemies)
- [ ] Add Wall Walker powerup (ceiling traversal)
- [ ] Add Trap Sense powerup (highlight mines)
- [ ] Add Vitality Crystal powerup (permanent +25 HP)
- [ ] Add Luck Stone powerup (permanent +5% crit)
- [ ] Add Ammo Pouch powerup (permanent +50% ammo)
- [ ] Add Berserker Regen powerup (regen at low HP)
- [ ] Add Cursed D4 powerup (3x dmg, lose armor)
- [ ] Add Blood Pact powerup (infinite ammo, health drain)
- [ ] Add Madness Die powerup (random +/- on kills)
- [ ] Add Glass Cannon powerup (2x dmg, 50% HP)

### Phase 18: Code Quality Improvements
- [ ] Define DEFAULT_DICE_VOLUME constant in m_menu.c
- [ ] Define DEFAULT_SHAKE_SCALE and DEFAULT_DAMAGE_SCALE
- [ ] Define MAX_MENU_PARTICLES constant
- [ ] Define SHAKE_LOW, SHAKE_MEDIUM, SHAKE_HIGH constants
- [ ] Define CRIT_CHANCE_SCALE constant in p_inter.c
- [ ] Define movement speed constants in g_game.c
- [ ] Fix typo: indetermined -> undetermined in doomstat.c
- [ ] Fix inconsistent naming: standardize snake_case for goblin vars
- [ ] Add NULL check after malloc in m_menu.c
- [ ] Add NULL check after malloc in d_main.c
- [ ] Add NULL check after strdup in d_main.c
- [ ] Add NULL check after calloc in w_wad.c
- [ ] Add NULL check after malloc in p_saveg.c
- [ ] Make video_driver const char* in i_video.c
- [ ] Make window_position const char* in i_video.c
- [ ] Make snd_musiccmd const char* in i_sound.c
- [ ] Expand dice_str buffer from 4 to 8 bytes in m_menu.c
- [ ] Expand level name buffer in m_menu.c line 1931

### Phase 19: Additional Rendering Optimizations
- [ ] Add sprite occlusion culling for off-screen sprites
- [ ] Implement temporal AA for smooth sprite edges
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
- [ ] Add automated build test script
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

## Current Status

**V1 complete**. V2 Technical Improvements in progress - 30 phases, ~600 tasks.
