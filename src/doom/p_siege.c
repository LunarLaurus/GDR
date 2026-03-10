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

#include "doomdef.h"
#include "p_siege.h"
#include "p_mobj.h"
#include "p_tick.h"
#include "g_game.h"
#include "s_sound.h"
#include "m_random.h"
#include "doomstat.h"
#include "info.h"

static siege_wave_spawner_t* siege_spawners[MAX_SIEGE_WAVES];
static int num_siege_spawners = 0;
static boolean siege_active = false;
static siege_state_t siege_state = SIEGE_INACTIVE;
static int current_wave_index = 0;
static int total_waves_configured = 0;
static int wave_cooldown_timer = 0;
static boolean wave_advance_pending = false;

static const siege_wave_t siege_waves[] = {
    { 9100, 8020, 10, 60, 15, false },
    { 9101, 8020, 15, 120, 12, false },
    { 9102, 8021, 20, 180, 10, true },
    { 9103, 8100, 12, 240, 15, false },
    { 9104, 8101, 25, 300, 8, false },
    { 9105, 8020, 30, 360, 8, false },
    { 9106, 8104, 18, 420, 10, false },
    { 9107, 8021, 35, 480, 6, true },
    { 9108, 8100, 40, 540, 6, false },
    { 9109, 8101, 50, 600, 5, false },
};

static const int num_siege_waves = sizeof(siege_waves) / sizeof(siege_waves[0]);

void P_SpawnSiegeWave(mapthing_t* mthing)
{
    siege_wave_spawner_t* spawner;
    int i;

    if (num_siege_spawners >= MAX_SIEGE_WAVES)
    {
        I_Error("P_SpawnSiegeWave: Too many siege spawners (max %d)", MAX_SIEGE_WAVES);
        return;
    }

    for (i = 0; i < num_siege_waves; i++)
    {
        if (siege_waves[i].thing_type == mthing->type)
            break;
    }

    if (i == num_siege_waves)
    {
        I_Error("P_SpawnSiegeWave: Unknown siege wave type %d", mthing->type);
        return;
    }

    spawner = Z_Malloc(sizeof(siege_wave_spawner_t), PU_LEVEL, NULL);
    memset(spawner, 0, sizeof(siege_wave_spawner_t));

    spawner->thinker.function = T_SiegeWaveSpawner;
    spawner->spawnpoint = *mthing;
    spawner->wave_index = i;
    spawner->enemies_spawned = 0;
    spawner->enemies_killed = 0;
    spawner->next_spawn_time = leveltime + siege_waves[i].delay;
    spawner->active = true;
    spawner->wave_complete = false;
    spawner->total_waves = num_siege_waves;
    spawner->current_wave = 0;

    P_AddThinker(&spawner->thinker);
    siege_spawners[num_siege_spawners++] = spawner;
    siege_active = true;
    siege_state = SIEGE_ASSAULT_ACTIVE;
    
    if (total_waves_configured == 0)
        total_waves_configured = num_siege_waves;
}

static mobj_t* P_SpawnSiegeEnemy(fixed_t x, fixed_t y, int enemy_type, boolean ambush)
{
    mobj_t* mobj;
    int i;
    int bit;
    mobjtype_t mobjtype = -1;

    for (i = 0; i < NUMMOBJTYPES; i++)
    {
        if (mobjinfo[i].doomednum == enemy_type)
        {
            mobjtype = i;
            break;
        }
    }

    if (mobjtype == -1)
        return NULL;

    if (!netgame && (gameskill == sk_baby))
        bit = 1;
    else if (!netgame && (gameskill == sk_nightmare))
        bit = 4;
    else if (!netgame)
        bit = 1 << (gameskill - 1);
    else
        bit = 4;

    if (!(bit & 1) && gameskill <= sk_easy)
        return NULL;

    mobj = P_SpawnMobj(x, y, ONFLOORZ, mobjtype);

    if (!mobj)
        return NULL;

    mobj->spawnpoint.x = mobj->x >> FRACBITS;
    mobj->spawnpoint.y = mobj->y >> FRACBITS;

    if (mobj->tics > 0)
        mobj->tics = 1 + (P_Random() % mobj->tics);

    if (mobj->flags & MF_COUNTKILL)
        totalkills++;

    if (ambush)
        mobj->flags |= MF_AMBUSH;

    return mobj;
}

void T_SiegeWaveSpawner(thinker_t* thinker)
{
    siege_wave_spawner_t* spawner = (siege_wave_spawner_t*)thinker;
    const siege_wave_t* wave;
    fixed_t spawn_x, spawn_y;
    int angle;
    int formation_offset;
    int enemies_alive;

    if (!spawner->active)
        return;

    if (wave_cooldown_timer > 0)
    {
        wave_cooldown_timer--;
        return;
    }

    if (wave_advance_pending)
    {
        wave_advance_pending = false;
        spawner->wave_index++;
        spawner->enemies_spawned = 0;
        spawner->enemies_killed = 0;
        spawner->wave_complete = false;
        spawner->active = true;
        
        if (spawner->wave_index >= num_siege_waves)
        {
            siege_state = SIEGE_VICTORY;
            spawner->active = false;
            return;
        }
        
        wave = &siege_waves[spawner->wave_index];
        spawner->next_spawn_time = leveltime + wave->delay;
        current_wave_index = spawner->wave_index;
    }

    if (spawner->wave_index >= num_siege_waves)
    {
        spawner->wave_complete = true;
        spawner->active = false;
        return;
    }

    wave = &siege_waves[spawner->wave_index];

    enemies_alive = P_GetSiegeEnemiesRemaining();
    
    if (spawner->enemies_spawned >= wave->count && enemies_alive == 0)
    {
        spawner->wave_complete = true;
        P_NotifySiegeWaveComplete(spawner->wave_index);
        
        if (spawner->wave_index < num_siege_waves - 1)
        {
            siege_state = SIEGE_WAVE_COMPLETE;
            wave_cooldown_timer = SIEGE_WAVE_COOLDOWN;
            wave_advance_pending = true;
        }
        else
        {
            siege_state = SIEGE_VICTORY;
            spawner->active = false;
        }
        return;
    }

    if (leveltime < spawner->next_spawn_time)
        return;

    if (spawner->enemies_spawned >= wave->count)
        return;

    angle = (spawner->spawnpoint.angle / 45) * ANG45;
    formation_offset = spawner->enemies_spawned * 64;

    spawn_x = (spawner->spawnpoint.x << FRACBITS) +
              (formation_offset * finecosine[angle >> ANGLETOFINESHIFT]);
    spawn_y = (spawner->spawnpoint.y << FRACBITS) +
              (formation_offset * finesine[angle >> ANGLETOFINESHIFT]);

    P_SpawnSiegeEnemy(spawn_x, spawn_y, wave->enemy_type, wave->ambush);
    spawner->enemies_spawned++;
    spawner->next_spawn_time = leveltime + wave->spawn_interval;
}

void P_UpdateSiegeWaves(void)
{
    int i;
    int total_active = 0;

    for (i = 0; i < num_siege_spawners; i++)
    {
        if (siege_spawners[i]->active)
            total_active++;
    }

    siege_active = total_active > 0;
}

void P_ShutdownSiegeWaves(void)
{
    int i;

    for (i = 0; i < num_siege_spawners; i++)
    {
        if (siege_spawners[i])
        {
            P_RemoveThinker(&siege_spawners[i]->thinker);
            Z_Free(siege_spawners[i]);
            siege_spawners[i] = NULL;
        }
    }

    num_siege_spawners = 0;
    siege_active = false;
    siege_state = SIEGE_INACTIVE;
    current_wave_index = 0;
    total_waves_configured = 0;
    wave_cooldown_timer = 0;
    wave_advance_pending = false;
}

boolean P_SiegeActive(void)
{
    return siege_active;
}

int P_GetSiegeEnemiesRemaining(void)
{
    int i;
    int total = 0;
    mobj_t* mo;

    for (mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
    {
        if (mo->flags & MF_COUNTKILL && !(mo->flags & MF_CORPSE))
            total++;
    }

    return total;
}

void P_NotifySiegeWaveComplete(int wave)
{
}

siege_state_t P_GetSiegeState(void)
{
    return siege_state;
}

void P_SetSiegeState(siege_state_t state)
{
    siege_state = state;
}

int P_GetCurrentWave(void)
{
    return current_wave_index + 1;
}

int P_GetTotalWaves(void)
{
    return total_waves_configured > 0 ? total_waves_configured : num_siege_waves;
}

void P_AdvanceSiegeWave(void)
{
    wave_advance_pending = true;
}

void P_CheckSiegeVictoryConditions(void)
{
    if (siege_state == SIEGE_ASSAULT_ACTIVE || siege_state == SIEGE_WAVE_COMPLETE)
    {
        if (P_GetSiegeEnemiesRemaining() == 0 && !wave_advance_pending && wave_cooldown_timer == 0)
        {
            int i;
            boolean all_waves_complete = true;
            
            for (i = 0; i < num_siege_spawners; i++)
            {
                if (siege_spawners[i] && siege_spawners[i]->wave_index < num_siege_waves - 1)
                {
                    all_waves_complete = false;
                    break;
                }
            }
            
            if (all_waves_complete)
            {
                siege_state = SIEGE_VICTORY;
            }
        }
    }
}
