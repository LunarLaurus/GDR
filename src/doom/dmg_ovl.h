//
// Copyright(C) 2026 Goblin Dice Rollaz
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
// DESCRIPTION:  Damage number overlay system
//

#ifndef __DMG_OVL_H__
#define __DMG_OVL_H__

#include "doomdef.h"

#define MAX_DAMAGE_NUMBERS 32
#define MAX_CRIT_POPUPS 8

typedef struct damage_number_s {
    int x;
    int y;
    int value;
    int lifetime;
    int max_lifetime;
    boolean is_critical;
    int crit_roll;
    boolean active;
} damage_number_t;

typedef struct crit_popup_s {
    int x;
    int y;
    int roll;
    int lifetime;
    int max_lifetime;
    boolean active;
} crit_popup_t;

void DMG_Init(void);
void DMG_AddDamage(int x, int y, int damage, boolean critical, int crit_roll);
void DMG_AddCritPopup(int x, int y, int roll);
void DMG_Ticker(void);
void DMG_Drawer(void);

#endif
