//
// Copyright(C) 2024 Jack Hobhouse
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
//  Siege wave spawning system for large-scale battle AI (50+ enemies)
//

#ifndef __P_SIEGE__
#define __P_SIEGE__

#include "p_mobj.h"
#include "doomdata.h"

#define MAX_SIEGE_WAVES 16
#define MAX_WAVE_ENEMIES 64
#define SIEGE_WAVE_SPAWNER_BASE 9100
#define SIEGE_WAVE_COOLDOWN 120

typedef enum {
    SIEGE_INACTIVE,
    SIEGE_ASSAULT_ACTIVE,
    SIEGE_WAVE_COMPLETE,
    SIEGE_VICTORY,
    SIEGE_DEFEAT
} siege_state_t;

typedef struct {
    short thing_type;
    short enemy_type;
    short count;
    short delay;
    short spawn_interval;
    boolean ambush;
} siege_wave_t;

typedef struct {
    thinker_t thinker;
    mapthing_t spawnpoint;
    int wave_index;
    int enemies_spawned;
    int enemies_killed;
    int next_spawn_time;
    boolean active;
    boolean wave_complete;
    int total_waves;
    int current_wave;
} siege_wave_spawner_t;

void T_SiegeWaveSpawner(thinker_t* thinker);
void P_SpawnSiegeWave(mapthing_t* mthing);
void P_UpdateSiegeWaves(void);
void P_ShutdownSiegeWaves(void);
boolean P_SiegeActive(void);
int P_GetSiegeEnemiesRemaining(void);
void P_NotifySiegeWaveComplete(int wave);
siege_state_t P_GetSiegeState(void);
void P_SetSiegeState(siege_state_t state);
int P_GetCurrentWave(void);
int P_GetTotalWaves(void);
void P_AdvanceSiegeWave(void);
void P_CheckSiegeVictoryConditions(void);

#endif
