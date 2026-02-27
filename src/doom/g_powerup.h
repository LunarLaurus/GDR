//
// Copyright(C) 2024 Goblin Dice Rollaz
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
//     Shared powerup framework for Goblin Dice Rollaz.
//     Provides centralized powerup definitions and helper functions
//     to simplify adding new powerups.
//

#ifndef __G_POWERUP__
#define __G_POWERUP__

#include "doomdef.h"

#define POWERUP_FLAG_TIMED        0x0001
#define POWERUP_FLAG_ONESHOT      0x0002
#define POWERUP_FLAG_EXCLUSIVE    0x0004
#define POWERUP_FLAG_STACKABLE    0x0008
#define POWERUP_FLAG_PERMANENT    0x0010
#define POWERUP_FLAG_GUARANTEED_CRITS 0x0020

typedef struct
{
    int                 powerup_id;
    const char*         name;
    int                 default_duration;
    int                 flags;
    const char*         pickup_message;
    int                 color;
    int                 sound;
    int                 mobj_type;
    spritenum_t         sprite;
    int                 spawn_rarity;
} powerup_info_t;

extern powerup_info_t powerups[NUMPOWERS];

boolean G_PowerupIsActive(player_t* player, int powerup_id);
int     G_PowerupTimeRemaining(player_t* player, int powerup_id);
void    G_PowerupActivate(player_t* player, int powerup_id, int duration);
void    G_PowerupDeactivate(player_t* player, int powerup_id);
boolean G_PowerupCanPickup(player_t* player, int powerup_id);
void    G_PowerupOnHit(player_t* player, int powerup_id);
int     G_GetActivePowerupCount(player_t* player);
void    G_PowerupShareWithNearbyPlayers(player_t* activator, int powerup_id);
int     G_GetAndConsumeGuaranteedCrits(player_t* player);

#define POWERUP(c) powerups[c]

#endif
