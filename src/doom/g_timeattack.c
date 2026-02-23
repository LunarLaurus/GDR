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

#include "doomdef.h"
#include "p_local.h"
#include "info.h"
#include "doomstat.h"
#include "g_timeattack.h"
#include "m_random.h"

timeattack_data_t timeattack;

void G_InitTimeAttack(void)
{
    int i;

    timeattack.active = false;
    timeattack.total_time = 0;
    timeattack.current_map = 0;
    timeattack.maps_completed = 0;
    timeattack.game_completed = false;
    timeattack.best_time = 0;

    for (i = 0; i < MAX_TIMEATTACK_MAPS; i++)
    {
        timeattack.level_times[i] = 0;
    }
}

void G_StartTimeAttack(void)
{
    G_InitTimeAttack();
    timeattack.active = true;
    timeattack.total_time = 0;
    timeattack.current_map = gamemap - 1;

    if (timeattack.current_map >= 0 && timeattack.current_map < MAX_TIMEATTACK_MAPS)
    {
        timeattack.level_times[timeattack.current_map] = 0;
    }
}

void G_TimeAttackTicker(void)
{
    if (!timeattack.active || timeattack.game_completed)
        return;

    if (gamestate == GS_LEVEL)
    {
        timeattack.total_time++;
    }
}

boolean G_IsTimeAttackMode(void)
{
    return timeattack.active;
}

int G_GetTimeAttackTotalTime(void)
{
    return timeattack.total_time;
}

int G_GetTimeAttackCurrentLevelTime(void)
{
    int current = gamemap - 1;

    if (current >= 0 && current < MAX_TIMEATTACK_MAPS)
    {
        return timeattack.level_times[current];
    }
    return 0;
}

void G_RecordLevelCompleteTime(int leveltime)
{
    int current = gamemap - 1;

    if (current >= 0 && current < MAX_TIMEATTACK_MAPS)
    {
        timeattack.level_times[current] = leveltime;
        timeattack.maps_completed = current + 1;
    }
}

void G_TimeAttackCompleteLevel(void)
{
    if (!timeattack.active)
        return;

    if (gamemap >= MAX_TIMEATTACK_MAPS || gamemap == 9)
    {
        timeattack.game_completed = true;

        if (timeattack.best_time == 0 || timeattack.total_time < timeattack.best_time)
        {
            timeattack.best_time = timeattack.total_time;
        }
    }
}

void G_EndTimeAttack(void)
{
    timeattack.active = false;
}

int G_GetTimeAttackBestTime(void)
{
    return timeattack.best_time;
}

void G_SetTimeAttackBestTime(int time)
{
    timeattack.best_time = time;
}
