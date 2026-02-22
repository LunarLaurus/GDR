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
//     RPG Progression System for Goblin Dice Rollaz.
//     Experience tracking, leveling, and stat bonuses.
//

#include "doomdef.h"
#include "d_player.h"
#include "p_local.h"
#include "doomstat.h"
#include "m_menu.h"
#include "g_game.h"

#define XP_PER_LEVEL_BASE 100
#define XP_PER_LEVEL_SCALE 50
#define STAT_POINTS_PER_LEVEL 2
#define MAX_LEVEL 50
#define MAX_STAT_BONUS 10

static int xp_to_next_level(int level)
{
    return XP_PER_LEVEL_BASE + (level * XP_PER_LEVEL_SCALE);
}

void G_InitPlayerRPG(player_t* player)
{
    if (!player)
        return;

    player->experience = 0;
    player->level = 1;
    player->stat_points = 0;
    player->strength_bonus = 0;
    player->dexterity_bonus = 0;
    player->vitality_bonus = 0;
    player->luck_bonus = 0;
}

void G_PlayerLevelUp(player_t* player)
{
    if (!player || !rpg_mode)
        return;

    while (player->experience >= xp_to_next_level(player->level) && player->level < MAX_LEVEL)
    {
        player->experience -= xp_to_next_level(player->level);
        player->level++;
        player->stat_points += STAT_POINTS_PER_LEVEL;

        if (player == &players[consoleplayer])
        {
            static char lvupmsg[128];
            snprintf(lvupmsg, sizeof(lvupmsg), "LEVEL UP! You are now level %d!", player->level);
            player->message = lvupmsg;
        }
    }

    if (player == &players[consoleplayer] && player->stat_points > 0)
    {
        if (gamestate == GS_LEVEL && !menuactive)
        {
            M_SetupNextMenu(&LevelUpDef);
        }
    }
}

void G_GiveExperience(player_t* player, int amount)
{
    if (!player || !rpg_mode || amount <= 0)
        return;

    player->experience += amount;
    G_PlayerLevelUp(player);
}

void G_AddPlayerXPForKill(mobj_t* victim, mobj_t* inflictor)
{
    int base_xp;
    int level_bonus;
    player_t* player;

    if (!victim || !inflictor || !rpg_mode)
        return;

    if (!inflictor->player)
        return;

    player = inflictor->player;

    if (victim->flags & MF_FRIENDLY)
        return;

    base_xp = 10;

    if (victim->info->spawnhealth > 0)
    {
        base_xp = victim->info->spawnhealth / 3;
        if (base_xp < 5)
            base_xp = 5;
        if (base_xp > 50)
            base_xp = 50;
    }

    if (victim->flags & MF_BOSS)
    {
        base_xp *= 5;
    }

    level_bonus = player->level / 2;
    base_xp += level_bonus;

    G_GiveExperience(player, base_xp);
}

int G_GetPlayerDamageBonus(player_t* player)
{
    if (!player || !rpg_mode)
        return 0;

    return player->strength_bonus;
}

int G_GetPlayerCritBonus(player_t* player)
{
    if (!player || !rpg_mode)
        return 0;

    return player->luck_bonus;
}

int G_GetPlayerMaxHealthBonus(player_t* player)
{
    if (!player || !rpg_mode)
        return 0;

    return player->vitality_bonus * 5;
}

int G_GetPlayerSpeedBonus(player_t* player)
{
    if (!player || !rpg_mode)
        return 0;

    return player->dexterity_bonus;
}

boolean G_AllocateStatPoint(player_t* player, int stat_type)
{
    if (!player || !rpg_mode)
        return false;

    if (player->stat_points <= 0)
        return false;

    switch (stat_type)
    {
    case 0:
        if (player->strength_bonus < MAX_STAT_BONUS)
        {
            player->strength_bonus++;
            player->stat_points--;
            return true;
        }
        break;
    case 1:
        if (player->dexterity_bonus < MAX_STAT_BONUS)
        {
            player->dexterity_bonus++;
            player->stat_points--;
            return true;
        }
        break;
    case 2:
        if (player->vitality_bonus < MAX_STAT_BONUS)
        {
            player->vitality_bonus++;
            player->stat_points--;
            return true;
        }
        break;
    case 3:
        if (player->luck_bonus < MAX_STAT_BONUS)
        {
            player->luck_bonus++;
            player->stat_points--;
            return true;
        }
        break;
    }

    return false;
}

void G_RPGThink(player_t* player)
{
    if (!player || !rpg_mode)
        return;

    if (player->crit_combo_timer > 0)
    {
        player->crit_combo_timer--;
        if (player->crit_combo_timer == 0)
        {
            player->crit_combo = 0;
        }
    }
}

int G_GetPlayerXPProgress(player_t* player)
{
    int current_xp_needed;

    if (!player || !rpg_mode)
        return 0;

    current_xp_needed = xp_to_next_level(player->level);
    if (current_xp_needed <= 0)
        return 100;

    return (player->experience * 100) / current_xp_needed;
}
