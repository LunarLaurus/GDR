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
//     Endless survival mode for Goblin Dice Rollaz.
//     Wave-based enemy spawning with progressive difficulty.
//

#ifndef __G_SURVIVAL__
#define __G_SURVIVAL__

#include "doomdef.h"

#define MAX_SURVIVAL_WAVE_ENEMIES 64
#define SURVIVAL_WAVE_DELAY (5 * TICRATE)
#define SURVIVAL_INITIAL_DELAY (10 * TICRATE)
#define MAX_SURVIVAL_MAPS 9

typedef struct {
    int wave_number;
    int enemies_remaining;
    int enemies_spawned;
    int wave_total_enemies;
    int wave_delay_tics;
    boolean wave_in_progress;
    boolean wave_complete;
    boolean game_over;
    int total_kills;
    int highest_wave_reached;
    int spawn_points_collected;
} survival_data_t;

extern survival_data_t survival;

void G_InitSurvival(void);
void G_SurvivalTicker(void);
void G_StartSurvivalWave(void);
void G_SpawnSurvivalEnemy(void);
void G_SurvivalEnemyKilled(void);
boolean G_IsSurvivalMode(void);
void G_EndSurvivalGame(void);
int G_GetSurvivalWaveNumber(void);
int G_GetSurvivalEnemiesRemaining(void);
int G_GetSurvivalTotalKills(void);
void G_SurvivalSpawnRandomEnemy(void);

#endif
