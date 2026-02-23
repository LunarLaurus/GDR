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


extern int detailLevel;
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
extern int colorblind_mode;
extern float hud_scale;
extern int reduce_motion;

// Goblin Dice Rollaz: Level Up menu
typedef struct menu_s menu_t;
void M_SetupNextMenu(menu_t *menudef);
extern menu_t LevelUpDef;
void M_LevelUp(int choice);


#endif
