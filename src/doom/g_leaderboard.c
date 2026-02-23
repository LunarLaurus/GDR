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
//     Leaderboard system for Goblin Dice Rollaz challenge modes.
//

#include "g_leaderboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomstat.h"
#include "m_misc.h"

leaderboard_t survival_leaderboard;
leaderboard_t timeattack_leaderboard;
leaderboard_t challenge_critonly_leaderboard;
leaderboard_t challenge_nopowerups_leaderboard;
leaderboard_t challenge_hardcore_leaderboard;

static const char* leaderboard_filenames[] = {
    "survival.lb",
    "timeattack.lb",
    "challenge_critonly.lb",
    "challenge_nopowerups.lb",
    "challenge_hardcore.lb"
};

static const char* leaderboard_names[] = {
    "SURVIVAL",
    "TIME ATTACK",
    "CRIT ONLY",
    "NO POWERUPS",
    "HARDCORE"
};

void G_InitLeaderboard(void)
{
    memset(&survival_leaderboard, 0, sizeof(leaderboard_t));
    memset(&timeattack_leaderboard, 0, sizeof(leaderboard_t));
    memset(&challenge_critonly_leaderboard, 0, sizeof(leaderboard_t));
    memset(&challenge_nopowerups_leaderboard, 0, sizeof(leaderboard_t));
    memset(&challenge_hardcore_leaderboard, 0, sizeof(leaderboard_t));

    G_LoadLeaderboard();
}

void G_LoadLeaderboard(void)
{
    int i;
    FILE *f;
    char *filepath;

    for (i = 0; i <= LEADERBOARD_CHALLENGE_HARDCORE; i++)
    {
        if (savegamedir && *savegamedir)
        {
            filepath = M_StringJoin(savegamedir, leaderboard_filenames[i], NULL);
        }
        else
        {
            filepath = M_StringJoin("goblin_dice_rollaz_", leaderboard_filenames[i], NULL);
        }

        f = M_fopen(filepath, "rb");
        if (f)
        {
            leaderboard_t *lb = G_GetLeaderboard(i);
            fread(lb, sizeof(leaderboard_t), 1, f);
            fclose(f);
        }
    }
}

void G_SaveLeaderboard(void)
{
    int i;
    FILE *f;
    char *filepath;

    for (i = 0; i <= LEADERBOARD_CHALLENGE_HARDCORE; i++)
    {
        if (savegamedir && *savegamedir)
        {
            filepath = M_StringJoin(savegamedir, leaderboard_filenames[i], NULL);
        }
        else
        {
            filepath = M_StringJoin("goblin_dice_rollaz_", leaderboard_filenames[i], NULL);
        }

        f = M_fopen(filepath, "wb");
        if (f)
        {
            leaderboard_t *lb = G_GetLeaderboard(i);
            fwrite(lb, sizeof(leaderboard_t), 1, f);
            fclose(f);
        }
    }
}

boolean G_AddSurvivalEntry(const char *name, int wave, int kills)
{
    int i;
    leaderboard_entry_t *entry;
    leaderboard_t *lb = &survival_leaderboard;
    int score = wave * 1000 + kills * 10;

    for (i = 0; i < lb->num_entries; i++)
    {
        if (score > lb->entries[i].score)
        {
            break;
        }
    }

    if (i >= LEADERBOARD_MAX_ENTRIES)
    {
        return false;
    }

    if (lb->num_entries < LEADERBOARD_MAX_ENTRIES)
    {
        lb->num_entries++;
    }

    if (i < lb->num_entries - 1)
    {
        memmove(&lb->entries[i + 1], &lb->entries[i], 
                (lb->num_entries - 1 - i) * sizeof(leaderboard_entry_t));
    }

    entry = &lb->entries[i];
    memset(entry, 0, sizeof(leaderboard_entry_t));
    strncpy(entry->player_name, name, 15);
    entry->player_name[15] = '\0';
    entry->score = score;
    entry->wave = wave;
    entry->time = kills;

    G_SaveLeaderboard();
    return true;
}

boolean G_AddTimeAttackEntry(const char *name, int total_time, int maps_completed)
{
    int i;
    leaderboard_entry_t *entry;
    leaderboard_t *lb = &timeattack_leaderboard;
    int score = maps_completed * 10000 - total_time;

    for (i = 0; i < lb->num_entries; i++)
    {
        if (score > lb->entries[i].score)
        {
            break;
        }
    }

    if (i >= LEADERBOARD_MAX_ENTRIES)
    {
        return false;
    }

    if (lb->num_entries < LEADERBOARD_MAX_ENTRIES)
    {
        lb->num_entries++;
    }

    if (i < lb->num_entries - 1)
    {
        memmove(&lb->entries[i + 1], &lb->entries[i], 
                (lb->num_entries - 1 - i) * sizeof(leaderboard_entry_t));
    }

    entry = &lb->entries[i];
    memset(entry, 0, sizeof(leaderboard_entry_t));
    strncpy(entry->player_name, name, 15);
    entry->player_name[15] = '\0';
    entry->score = score;
    entry->wave = maps_completed;
    entry->time = total_time;

    G_SaveLeaderboard();
    return true;
}

boolean G_AddChallengeEntry(int challenge_type, const char *name, int score, int wave)
{
    int i;
    leaderboard_entry_t *entry;
    leaderboard_t *lb;

    switch (challenge_type)
    {
        case LEADERBOARD_CHALLENGE_CRITONLY:
            lb = &challenge_critonly_leaderboard;
            break;
        case LEADERBOARD_CHALLENGE_NOPOWERUPS:
            lb = &challenge_nopowerups_leaderboard;
            break;
        case LEADERBOARD_CHALLENGE_HARDCORE:
            lb = &challenge_hardcore_leaderboard;
            break;
        default:
            return false;
    }

    for (i = 0; i < lb->num_entries; i++)
    {
        if (score > lb->entries[i].score)
        {
            break;
        }
    }

    if (i >= LEADERBOARD_MAX_ENTRIES)
    {
        return false;
    }

    if (lb->num_entries < LEADERBOARD_MAX_ENTRIES)
    {
        lb->num_entries++;
    }

    if (i < lb->num_entries - 1)
    {
        memmove(&lb->entries[i + 1], &lb->entries[i], 
                (lb->num_entries - 1 - i) * sizeof(leaderboard_entry_t));
    }

    entry = &lb->entries[i];
    memset(entry, 0, sizeof(leaderboard_entry_t));
    strncpy(entry->player_name, name, 15);
    entry->player_name[15] = '\0';
    entry->score = score;
    entry->wave = wave;

    G_SaveLeaderboard();
    return true;
}

leaderboard_t* G_GetLeaderboard(int type)
{
    switch (type)
    {
        case LEADERBOARD_SURVIVAL:
            return &survival_leaderboard;
        case LEADERBOARD_TIMEATTACK:
            return &timeattack_leaderboard;
        case LEADERBOARD_CHALLENGE_CRITONLY:
            return &challenge_critonly_leaderboard;
        case LEADERBOARD_CHALLENGE_NOPOWERUPS:
            return &challenge_nopowerups_leaderboard;
        case LEADERBOARD_CHALLENGE_HARDCORE:
            return &challenge_hardcore_leaderboard;
        default:
            return NULL;
    }
}

const char* G_GetLeaderboardName(int type)
{
    if (type < 0 || type > LEADERBOARD_CHALLENGE_HARDCORE)
    {
        return "UNKNOWN";
    }
    return leaderboard_names[type];
}

void G_ClearLeaderboard(int type)
{
    leaderboard_t *lb = G_GetLeaderboard(type);
    if (lb)
    {
        memset(lb, 0, sizeof(leaderboard_t));
        G_SaveLeaderboard();
    }
}

void G_ClearAllLeaderboards(void)
{
    int i;
    for (i = 0; i <= LEADERBOARD_CHALLENGE_HARDCORE; i++)
    {
        G_ClearLeaderboard(i);
    }
}
