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
// DESCRIPTION:  Debug overlay for damage calculations
//

#ifndef __DBG_OVL_H__
#define __DBG_OVL_H__

#include "doomdef.h"

void DBG_Init(void);
void DBG_Drawer(void);
void DBG_SetLastDamage(int damage, int critChance, int critMultiplier, 
                       int diceRoll, int critRoll, boolean wasCrit, 
                       boolean guaranteedCrit, int targetType);
void DBG_SetLastHitTarget(const char* targetName);
void DBG_SetVisible(boolean visible);

#endif
