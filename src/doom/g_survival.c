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

#include "doomdef.h"
#include "p_local.h"
#include "info.h"
#include "doomstat.h"
#include "g_survival.h"
#include "g_leaderboard.h"
#include "m_random.h"

survival_data_t survival;

void G_InitSurvival(void)
{
    survival.wave_number = 0;
    survival.enemies_remaining = 0;
    survival.enemies_spawned = 0;
    survival.wave_total_enemies = 0;
    survival.wave_delay_tics = SURVIVAL_INITIAL_DELAY;
    survival.wave_in_progress = false;
    survival.wave_complete = false;
    survival.game_over = false;
    survival.total_kills = 0;
    survival.highest_wave_reached = 0;
    survival.spawn_points_collected = 0;
}

boolean G_IsSurvivalMode(void)
{
    return (survival.wave_number > 0 || survival.wave_in_progress);
}

void G_StartSurvivalWave(void)
{
    int base_enemies;
    int wave_bonus;
    int player_count = 0;
    int i;
    char wave_msg[128];

    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i])
            player_count++;
    }

    if (player_count == 0)
        player_count = 1;

    survival.wave_number++;
    survival.highest_wave_reached = survival.wave_number;
    survival.wave_in_progress = true;
    survival.wave_complete = false;

    base_enemies = 4 + (survival.wave_number * 2);
    wave_bonus = (survival.wave_number / 3);
    survival.wave_total_enemies = (base_enemies + wave_bonus) * player_count;

    if (survival.wave_total_enemies > MAX_SURVIVAL_WAVE_ENEMIES)
        survival.wave_total_enemies = MAX_SURVIVAL_WAVE_ENEMIES;

    survival.enemies_spawned = 0;
    survival.enemies_remaining = survival.wave_total_enemies;

    if (players[consoleplayer].mo)
    {
        snprintf(wave_msg, sizeof(wave_msg), "WAVE %d - %d ENEMIES",
                 survival.wave_number, survival.wave_total_enemies);
        players[consoleplayer].message = wave_msg;
    }
}

static mobjtype_t G_GetSurvivalEnemyType(int wave)
{
    int idx;
    
    if (wave < 3)
    {
        idx = P_Random() % 3;
        switch(idx)
        {
            case 0: return MT_DWARF;
            case 1: return MT_GOBLIN_SCOUT;
            default: return MT_DWARF;
        }
    }
    else if (wave < 5)
    {
        idx = P_Random() % 5;
        switch(idx)
        {
            case 0: return MT_DWARF;
            case 1: return MT_DWARF_BERSERKER;
            case 2: return MT_GOBLIN_SCOUT;
            case 3: return MT_GOBLIN_SNEAK;
            default: return MT_DWARF;
        }
    }
    else if (wave < 8)
    {
        idx = P_Random() % 8;
        switch(idx)
        {
            case 0: return MT_DWARF;
            case 1: return MT_DWARF_BERSERKER;
            case 2: return MT_DWARF_ENGINEER;
            case 3: return MT_DWARF_DEFENDER;
            case 4: return MT_GOBLIN_SCOUT;
            case 5: return MT_GOBLIN_SNEAK;
            case 6: return MT_GOBLIN_ALCHEMIST;
            default: return MT_DWARF;
        }
    }
    else
    {
        idx = P_Random() % 24;
        switch(idx)
        {
            case 0: return MT_DWARF;
            case 1: return MT_DWARF_BERSERKER;
            case 2: return MT_DWARF_ENGINEER;
            case 3: return MT_DWARF_DEFENDER;
            case 4: return MT_DWARF_MARKSMAN;
            case 5: return MT_DWARF_MINER;
            case 6: return MT_DWARF_FLAMETHROWER;
            case 7: return MT_DWARF_THUNDERER;
            case 8: return MT_DWARF_IRONCLAD;
            case 9: return MT_DWARF_STONECUTTER;
            case 10: return MT_DWARF_THUNDERMAGE;
            case 11: return MT_DWARF_RUNESMITH;
            case 12: return MT_DWARF_RUNEBEARER;
            case 13: return MT_DWARF_HIGHPRIEST;
            case 14: return MT_DWARF_COMMANDER;
            case 15: return MT_DWARF_TINKERER;
            case 16: return MT_DWARF_BARRELELITE;
            case 17: return MT_DWARF_SHADOWBLADE;
            case 18: return MT_GOBLIN_SCOUT;
            case 19: return MT_GOBLIN_SNEAK;
            case 20: return MT_GOBLIN_ALCHEMIST;
            case 21: return MT_GOBLIN_TOTEMIST;
            case 22: return MT_GOBLIN_SHAMAN;
            default: return MT_DWARF;
        }
    }
}

void G_SpawnSurvivalEnemy(void)
{
    mobj_t* player;
    mobjtype_t enemy_type;
    fixed_t spawn_x;
    fixed_t spawn_y;
    angle_t angle;
    int spawn_attempts;
    mobj_t* mo;

    if (!survival.wave_in_progress || survival.enemies_spawned >= survival.wave_total_enemies)
        return;

    player = &players[consoleplayer].mo;
    if (!player || player->health <= 0)
        return;

    enemy_type = G_GetSurvivalEnemyType(survival.wave_number);

    for (spawn_attempts = 0; spawn_attempts < 10; spawn_attempts++)
    {
        angle = P_Random() << 24;
        spawn_x = player->x + (P_Random() % 16 + 12) * FRACUNIT * finesine[angle >> ANGLETOFINESHIFT];
        spawn_y = player->y + (P_Random() % 16 + 12) * FRACUNIT * finecosine[angle >> ANGLETOFINESHIFT];

        if (!P_CheckPosition(NULL, spawn_x, spawn_y))
            continue;

        mo = P_SpawnMobj(spawn_x, spawn_y, ONFLOORZ, enemy_type);
        if (mo)
        {
            survival.enemies_spawned++;
            return;
        }
    }
}

void G_SurvivalEnemyKilled(void)
{
    char complete_msg[128];

    if (!survival.wave_in_progress)
        return;

    survival.enemies_remaining--;
    survival.total_kills++;

    if (survival.enemies_remaining <= 0 && survival.enemies_spawned >= survival.wave_total_enemies)
    {
        survival.wave_in_progress = false;
        survival.wave_complete = true;
        survival.wave_delay_tics = SURVIVAL_WAVE_DELAY;

        if (players[consoleplayer].mo)
        {
            snprintf(complete_msg, sizeof(complete_msg), "WAVE %d COMPLETE! +%d KILLS",
                     survival.wave_number, survival.wave_total_enemies);
            players[consoleplayer].message = complete_msg;
        }
    }
}

void G_SurvivalTicker(void)
{
    int active_enemies = 0;
    mobj_t* mo;
    int i;
    char complete_msg[128];
    char gameover_msg[128];

    if (survival.game_over)
        return;

    if (!G_IsSurvivalMode() && gamestate == GS_LEVEL)
    {
        if (survival.wave_delay_tics > 0)
        {
            survival.wave_delay_tics--;
            return;
        }
        G_StartSurvivalWave();
    }

    if (survival.wave_in_progress)
    {
        if (survival.enemies_spawned < survival.wave_total_enemies)
        {
            if ((leveltime % TICRATE) == 0)
            {
                G_SpawnSurvivalEnemy();
            }
        }
    }

    if (survival.wave_complete && survival.wave_delay_tics > 0)
    {
        survival.wave_delay_tics--;
        if (survival.wave_delay_tics == 0)
        {
            survival.wave_in_progress = true;
            G_StartSurvivalWave();
        }
    }

    if (players[consoleplayer].mo && players[consoleplayer].mo->health <= 0)
    {
        survival.game_over = true;
        if (players[consoleplayer].mo)
        {
            snprintf(gameover_msg, sizeof(gameover_msg), "GAME OVER! REACHED WAVE %d - %d TOTAL KILLS",
                     survival.wave_number, survival.total_kills);
            players[consoleplayer].message = gameover_msg;
        }
    }
}

void G_EndSurvivalGame(void)
{
    if (survival.highest_wave_reached > 0)
    {
        G_AddSurvivalEntry("Player", survival.highest_wave_reached, survival.total_kills);
    }
    survival.wave_number = 0;
    survival.wave_in_progress = false;
    survival.game_over = true;
}

int G_GetSurvivalWaveNumber(void)
{
    return survival.wave_number;
}

int G_GetSurvivalEnemiesRemaining(void)
{
    return survival.enemies_remaining;
}

int G_GetSurvivalTotalKills(void)
{
    return survival.total_kills;
}

void G_SurvivalSpawnRandomEnemy(void)
{
    G_SpawnSurvivalEnemy();
}
