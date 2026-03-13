//
// Copyright(C) 2024 Goblin Dice Rollaz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// DESCRIPTION:
//  Player related functions header.
//  GDR STUB: Created to satisfy p_siege_weapon.c include.
//

#ifndef __P_USER__
#define __P_USER__

#include "d_player.h"

void P_PlayerThink(player_t* player);
void P_CalcHeight(player_t* player);
void P_DeathThink(player_t* player);
void P_MovePlayer(player_t* player);

#endif
