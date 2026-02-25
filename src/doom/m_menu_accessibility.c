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
//      Accessibility menu for Goblin Dice Rollaz.
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
#include "m_menu_accessibility.h"

#include "st_stuff.h"

extern int goblin_colorblind_mode;
extern int goblin_reduce_motion;
extern int screen_reader_enabled;
extern float hud_scale;
extern float screen_shake_intensity;

enum
{
    accessibility_colorblind,
    accessibility_hudscale,
    accessibility_reducemotion,
    accessibility_screenreader,
    accessibility_end
} accessibility_e;

menuitem_t AccessibilityMenu[]=
{
    {1,"M_COLBLND", M_ColorblindMode, 'c'},
    {2,"M_HUDSCAL", M_HUDScale, 'h'},
    {1,"M_REDMOT", M_ReduceMotion, 'r'},
    {1,"M_SCRNRD", M_ScreenReaderMode, 's'}
};

menu_t  AccessibilityDef =
{
    accessibility_end,
    &MainDef,
    AccessibilityMenu,
    M_DrawAccessibility,
    60,37,
    0
};

void M_Accessibility(int choice)
{
    choice = 0;
    M_SetupNextMenu(&AccessibilityDef);
}

void M_ColorblindMode(int choice)
{
    choice = 0;
    goblin_colorblind_mode = (goblin_colorblind_mode + 1) % 3;
    
    if (goblin_colorblind_mode == 0)
        players[consoleplayer].message = "Colorblind Mode: OFF";
    else if (goblin_colorblind_mode == 1)
        players[consoleplayer].message = "Colorblind Mode: Red-Green";
    else
        players[consoleplayer].message = "Colorblind Mode: Blue-Yellow";
}

void M_HUDScale(int choice)
{
    switch(choice)
    {
      case 0:
          if (hud_scale > 0.5f)
              hud_scale -= 0.1f;
          break;
      case 1:
          if (hud_scale < 2.0f)
              hud_scale += 0.1f;
          break;
    }
}

void M_ReduceMotion(int choice)
{
    choice = 0;
    goblin_reduce_motion = 1 - goblin_reduce_motion;
    
    if (goblin_reduce_motion)
    {
        players[consoleplayer].message = "Reduce Motion: ON";
        screen_shake_intensity = 0.0f;
    }
    else
    {
        players[consoleplayer].message = "Reduce Motion: OFF";
        screen_shake_intensity = 1.0f;
    }
}

void M_ScreenReaderMode(int choice)
{
    choice = 0;
    screen_reader_enabled = 1 - screen_reader_enabled;
    
    if (screen_reader_enabled)
    {
        players[consoleplayer].message = "Screen Reader: ON";
    }
    else
    {
        players[consoleplayer].message = "Screen Reader: OFF";
    }
}

void M_DrawAccessibility(void)
{
    V_DrawPatchDirect(72, 15, W_CacheLumpName(DEH_String("M_OPTTTL"),
                                               PU_CACHE));
    
    M_WriteText(AccessibilityDef.x - 80, AccessibilityDef.y, "Colorblind");
    if (goblin_colorblind_mode == 0)
        M_WriteText(AccessibilityDef.x + 120, AccessibilityDef.y, "OFF");
    else if (goblin_colorblind_mode == 1)
        M_WriteText(AccessibilityDef.x + 120, AccessibilityDef.y, "R/G");
    else
        M_WriteText(AccessibilityDef.x + 120, AccessibilityDef.y, "B/Y");
    
    M_WriteText(AccessibilityDef.x - 80, AccessibilityDef.y + LINEHEIGHT * 1, "HUD Scale");
    M_DrawThermo(AccessibilityDef.x, AccessibilityDef.y + LINEHEIGHT * 1,
                 15, (int)(hud_scale * 10));
    
    M_WriteText(AccessibilityDef.x - 80, AccessibilityDef.y + LINEHEIGHT * 2, "Reduce Mot");
    V_DrawPatchDirect(AccessibilityDef.x + 120, 
                      AccessibilityDef.y + LINEHEIGHT * 2,
                      W_CacheLumpName(DEH_String(goblin_reduce_motion ? "M_YES" : "M_NO"),
                                      PU_CACHE));
    
    M_WriteText(AccessibilityDef.x - 80, AccessibilityDef.y + LINEHEIGHT * 3, "Screen Rd");
    V_DrawPatchDirect(AccessibilityDef.x + 120, 
                      AccessibilityDef.y + LINEHEIGHT * 3,
                      W_CacheLumpName(DEH_String(screen_reader_enabled ? "M_YES" : "M_NO"),
                                      PU_CACHE));
}
