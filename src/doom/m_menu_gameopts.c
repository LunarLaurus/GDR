//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Game options menu for Goblin Dice Rollaz.
//

#include <stdlib.h>

#include "doomdef.h"
#include "doomkeys.h"
#include "dstrings.h"

#include "d_main.h"
#include "deh_main.h"

#include "i_system.h"
#include "i_video.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "r_local.h"

#include "g_game.h"

#include "doomstat.h"

#include "m_menu.h"
#include "m_menu_gameopts.h"

#include "st_stuff.h"

extern int dice_sound_volume;
extern int crit_notification;
extern float screen_shake_intensity;
extern float damage_number_scale;
extern int particle_effects;

enum
{
    gamesettings_dicevol,
    gamesettings_critnotif,
    gamesettings_screenshake,
    gamesettings_damagenum,
    gamesettings_particles,
    gamesettings_end
} gamesettings_e;

menuitem_t GameSettingsMenu[]=
{
    {2,"M_DICEVOL", M_DiceSoundVol, 'd'},
    {1,"M_CRITNOT", M_CritNotification, 'c'},
    {2,"M_SCRNSHK", M_ScreenShake, 's'},
    {2,"M_DMGSCAL", M_DamageNumberScale, 'n'},
    {1,"M_PARTCL", M_ParticleEffects, 'p'}
};

menu_t  GameSettingsDef =
{
    gamesettings_end,
    &MainDef,
    GameSettingsMenu,
    M_DrawGameSettings,
    60,37,
    0
};

void M_GameSettings(int choice)
{
    choice = 0;
    M_SetupNextMenu(&GameSettingsDef);
}

void M_DiceSoundVol(int choice)
{
    switch(choice)
    {
      case 0:
	if (dice_sound_volume)
	    dice_sound_volume--;
	break;
      case 1:
	if (dice_sound_volume < 15)
	    dice_sound_volume++;
	break;
    }
}

void M_CritNotification(int choice)
{
    choice = 0;
    crit_notification = 1 - crit_notification;
    
    if (!crit_notification)
	players[consoleplayer].message = "Crit Notifications OFF";
    else
	players[consoleplayer].message = "Crit Notifications ON";
}

void M_ScreenShake(int choice)
{
    if (reduce_motion)
        return;

    switch(choice)
    {
      case 0:
	if (screen_shake_intensity > 0.0f)
	    screen_shake_intensity -= 0.1f;
	break;
      case 1:
	if (screen_shake_intensity < 2.0f)
	    screen_shake_intensity += 0.1f;
	break;
    }
}

void M_DamageNumberScale(int choice)
{
    switch(choice)
    {
      case 0:
	if (damage_number_scale > 0.5f)
	    damage_number_scale -= 0.1f;
	break;
      case 1:
	if (damage_number_scale < 2.0f)
	    damage_number_scale += 0.1f;
	break;
    }
}

void M_ParticleEffects(int choice)
{
    choice = 0;
    particle_effects = 1 - particle_effects;
    
    if (!particle_effects)
	players[consoleplayer].message = "Particle Effects OFF";
    else
	players[consoleplayer].message = "Particle Effects ON";
}

void M_DrawGameSettings(void)
{
    V_DrawPatchDirect(108, 15, W_CacheLumpName(DEH_String("M_OPTTTL"),
                                               PU_CACHE));
    
    // Dice sound volume slider
    M_WriteText(GameSettingsDef.x - 80, GameSettingsDef.y, "Dice Vol");
    M_DrawThermo(GameSettingsDef.x, GameSettingsDef.y + LINEHEIGHT * 0,
		  16, dice_sound_volume);
    
    // Crit notification toggle
    M_WriteText(GameSettingsDef.x - 80, GameSettingsDef.y + LINEHEIGHT * 1, "Crit Notif");
    V_DrawPatchDirect(GameSettingsDef.x + 120, 
                      GameSettingsDef.y + LINEHEIGHT * 1,
                      W_CacheLumpName(DEH_String(crit_notification ? "M_YES" : "M_NO"),
                                      PU_CACHE));
    
    // Screen shake slider
    M_WriteText(GameSettingsDef.x - 80, GameSettingsDef.y + LINEHEIGHT * 2, "Shake");
    M_DrawThermo(GameSettingsDef.x, GameSettingsDef.y + LINEHEIGHT * 2,
		  20, (int)(screen_shake_intensity * 10));
    
    // Damage number scale slider
    M_WriteText(GameSettingsDef.x - 80, GameSettingsDef.y + LINEHEIGHT * 3, "Dmg Scale");
    M_DrawThermo(GameSettingsDef.x, GameSettingsDef.y + LINEHEIGHT * 3,
		  15, (int)(damage_number_scale * 10));
    
    // Particle effects toggle
    M_WriteText(GameSettingsDef.x - 80, GameSettingsDef.y + LINEHEIGHT * 4, "Particles");
    V_DrawPatchDirect(GameSettingsDef.x + 120, 
                      GameSettingsDef.y + LINEHEIGHT * 4,
                      W_CacheLumpName(DEH_String(particle_effects ? "M_YES" : "M_NO"),
                                      PU_CACHE));
}
