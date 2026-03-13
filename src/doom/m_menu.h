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
//   Menu widget stuff, episode selection and such.
//    


#ifndef __M_MENU__
#define __M_MENU__



#include "d_event.h"

//
// MENUS
//
// Called by main loop,
// saves config file and calls I_Quit when user exits.
// Even when the menu is not displayed,
// this can resize the view and change game parameters.
// Does all the real work of the menu interaction.
boolean M_Responder (event_t *ev);


// Called by main loop,
// only used for menu (skull cursor) animation.
void M_Ticker (void);

// Called by main loop,
// draws the menus directly into the screen buffer.
void M_Drawer (void);

// Called by D_DoomMain,
// loads the config file.
void M_Init (void);

// Called by intro code to force menu up upon a keypress,
// does nothing if menu is already up.
void M_StartControlPanel (void);


// Goblin Dice Rollaz: detailLevel removed - always high detail
extern int screenblocks;

extern boolean inhelpscreens;
extern int showMessages;

// Goblin Dice Rollaz: RPG Progression Mode toggle
extern int rpg_mode;

// Goblin Dice Rollaz: Game Settings
extern int dice_sound_volume;
extern int crit_notification;
extern float screen_shake_intensity;
extern float damage_number_scale;
extern int particle_effects;

// Goblin Dice Rollaz: Accessibility Settings
extern int goblin_colorblind_mode;
extern float hud_scale;
extern int goblin_reduce_motion;
extern int screen_reader_enabled;

// Menu type definitions (shared with split menu files)
#define LINEHEIGHT 16

typedef struct
{
    // 0 = no cursor here, 1 = ok, 2 = arrows ok
    short   status;

    char    name[10];

    // choice = menu item #.
    // if status = 2,
    //   choice=0:leftarrow,1:rightarrow
    void    (*routine)(int choice);

    // hotkey in menu
    char    alphaKey;
} menuitem_t;

typedef struct menu_s
{
    short           numitems;   // # of menu items
    struct menu_s*  prevMenu;   // previous menu
    menuitem_t*     menuitems;  // menu items
    void            (*routine)(); // draw routine
    short           x;
    short           y;          // x,y of menu
    short           lastOn;     // last item user was on in menu
} menu_t;

// Main menu definition (used as prevMenu by submenus)
extern menu_t MainDef;

// Leaderboard close function
void M_CloseLeaderboard(int choice);

// Goblin Dice Rollaz: Level Up menu
void M_SetupNextMenu(menu_t *menudef);
extern menu_t LevelUpDef;
void M_LevelUp(int choice);

// Thermometer bar drawing (used by options/gameopts/accessibility menus)
void M_DrawThermo(int x, int y, int thermWidth, int thermDot);

// Text drawing function for intermission and other screens
void M_WriteText(int x, int y, const char *string);


#endif
