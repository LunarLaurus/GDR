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
//     Time attack mode for Goblin Dice Rollaz.
//     Tracks total time across all levels for speedrun competition.
//

#ifndef __G_TIMEATTACK__
#define __G_TIMEATTACK__

#include "doomdef.h"

#define MAX_TIMEATTACK_MAPS 9

typedef struct {
    boolean active;
    int total_time;
    int level_times[MAX_TIMEATTACK_MAPS];
    int current_map;
    int maps_completed;
    boolean game_completed;
    int best_time;
} timeattack_data_t;

extern timeattack_data_t timeattack;

void G_InitTimeAttack(void);
void G_StartTimeAttack(void);
void G_TimeAttackTicker(void);
boolean G_IsTimeAttackMode(void);
int G_GetTimeAttackTotalTime(void);
int G_GetTimeAttackCurrentLevelTime(void);
void G_RecordLevelCompleteTime(int leveltime);
void G_EndTimeAttack(void);
void G_TimeAttackCompleteLevel(void);
int G_GetTimeAttackBestTime(void);
void G_SetTimeAttackBestTime(int time);

#endif
