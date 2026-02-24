# IMPLEMENTATION_PLAN.md - Goblin Dice Rollaz

## Priority Tasks

### Phase 1: Determinism Fixes
- [ ] Replace floating-point cos() with finecosine[] in p_pspr.c knockback
- [ ] Replace floating-point sin() with finesine[] in p_pspr.c knockback
- [ ] Replace rand() with M_Random() in m_menu.c particle system
- [ ] Audit p_inter.c for floating-point damage calculations
- [ ] Audit p_mobj.c for floating-point physics
- [ ] Verify all RNG usage uses P_Random/M_Random tables

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
- [ ] Consolidate NET_Log functions under DEBUG_NET flag
- [ ] Update master server address or remove master server queries
- [ ] Fix InitPetName to not reseed rand() every call
- [ ] Remove unused NET_PACKET_TYPE_ACK

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

## Current Status

**V1 complete**. V2 Technical Improvements in progress - 13 phases, ~175 tasks.

(End of file)
