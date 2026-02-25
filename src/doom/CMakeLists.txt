add_library(doom STATIC
            am_map.c        am_map.h
            deh_ammo.c
            deh_bexstr.c
            deh_cheat.c
            deh_doom.c
            deh_frame.c
            deh_misc.c      deh_misc.h
            deh_ptr.c
            deh_sound.c
            deh_thing.c
            deh_weapon.c
            dice_predict.c  dice_predict.h
            dice_projectile_lag.c dice_projectile_lag.h
                            d_englsh.h
            d_items.c       d_items.h
            d_main.c        d_main.h
            d_net.c
            dmg_ovl.c       dmg_ovl.h
            dbg_ovl.c       dbg_ovl.h
                            doomdata.h
            doomdef.c       doomdef.h
            doomstat.c      doomstat.h
                            d_player.h
            dstrings.c      dstrings.h
                            d_textur.h
                            d_think.h
            f_finale.c      f_finale.h
            f_wipe.c        f_wipe.h
            g_game.c        g_game.h
            g_powerup.c     g_powerup.h
            g_achievement.c g_achievement.h
            g_rpg.c         g_rpg.h
            g_stats.c       g_stats.h
            g_status.c      g_status.h
            g_survival.c    g_survival.h
            g_timeattack.c  g_timeattack.h
            g_leaderboard.c g_leaderboard.h
            hu_lib.c        hu_lib.h
            hu_stuff.c      hu_stuff.h
            info.c          info.h
            m_menu.c        m_menu.h
            m_tips.c        m_tips.h
            m_random.c      m_random.h
            p_ai_faction.c   p_ai_faction.h
            p_ceilng.c
            p_doors.c
            p_enemy.c
            p_floor.c
            p_inter.c       p_inter.h
            p_lights.c
                            p_local.h
            p_map.c
            p_maputl.c
            p_mobj.c        p_mobj.h
            p_particles.c   p_particles.h
            p_plats.c
            p_pspr.c        p_pspr.h
            p_saveg.c       p_saveg.h
            p_setup.c       p_setup.h
            p_sight.c
            p_spec.c        p_spec.h
            p_switch.c
            p_telept.c
            p_tick.c        p_tick.h
            p_user.c
            r_bsp.c         r_bsp.h
            r_data.c        r_data.h
                            r_defs.h
            r_draw.c        r_draw.h
                            r_local.h
            r_main.c        r_main.h
            r_plane.c       r_plane.h
            r_segs.c        r_segs.h
            r_sky.c         r_sky.h
                            r_state.h
            r_things.c      r_things.h
            s_sound.c       s_sound.h
            sounds.c        sounds.h
            statdump.c      statdump.h
            st_lib.c        st_lib.h
            st_stuff.c      st_stuff.h
            st_weapon_debug.c st_weapon_debug.h
            m_menu_gameopts.c m_menu_gameopts.h
            m_menu_accessibility.c m_menu_accessibility.h
            wi_stuff.c      wi_stuff.h)

target_include_directories(doom PRIVATE "../" "${CMAKE_CURRENT_BINARY_DIR}/../../")

if (DEFINED EMSCRIPTEN)
    set(SDL_FLAGS "-s USE_SDL=2")
    if(ENABLE_SDL2_MIXER)
        set(SDL_FLAGS "${SDL_FLAGS} -s USE_SDL_MIXER=2")
    endif()
    if(ENABLE_SDL2_NET)
        set(SDL_FLAGS "${SDL_FLAGS} -s USE_SDL_NET=2")
    endif()
else()
    set(SDL_LINK_LIBS SDL2::SDL2)
    if(ENABLE_SDL2_MIXER)
        list(APPEND SDL_LINK_LIBS SDL2_mixer::SDL2_mixer)
    endif()
    if(ENABLE_SDL2_NET)
        list(APPEND SDL_LINK_LIBS SDL2_net::SDL2_net)
    endif()
endif()
target_link_libraries(doom ${SDL_LINK_LIBS})
set_target_properties(doom PROPERTIES COMPILE_FLAGS "${SDL_FLAGS}")
set_target_properties(doom PROPERTIES LINK_FLAGS "${SDL_FLAGS}")
