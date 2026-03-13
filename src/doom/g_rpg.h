//
// Copyright(C) 2024 Goblin Dice Rollaz
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
//     RPG Progression System header for Goblin Dice Rollaz.
//

#ifndef __G_RPG__
#define __G_RPG__

#include "doomdef.h"
#include "p_mobj.h"
#include "d_player.h"

void G_InitPlayerRPG(player_t* player);
void G_PlayerLevelUp(player_t* player);
void G_GiveExperience(player_t* player, int amount);
void G_AddPlayerXPForKill(mobj_t* victim, mobj_t* inflictor);
int G_GetPlayerDamageBonus(player_t* player);
int G_GetPlayerCritBonus(player_t* player);
int G_GetPlayerLevelCritBonus(player_t* player);
int G_GetPlayerMaxHealthBonus(player_t* player);
int G_GetPlayerSpeedBonus(player_t* player);
boolean G_AllocateStatPoint(player_t* player, int stat_type);
void G_RPGThink(player_t* player);
int G_GetPlayerXPProgress(player_t* player);

void G_InitPlayerWeaponMastery(player_t* player);
void G_AddWeaponKill(player_t* player, int weapon);
int G_GetWeaponMasteryLevel(player_t* player, int weapon);
int G_GetWeaponMasteryDamageBonus(player_t* player, int weapon);
int G_GetWeaponMasteryCritBonus(player_t* player, int weapon);
int G_GetWeaponMasteryXPProgress(player_t* player, int weapon);

#endif
