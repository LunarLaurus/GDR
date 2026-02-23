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
//     Tracks high scores for survival, time attack, and challenge modes.
//

#ifndef __G_LEADERBOARD__
#define __G_LEADERBOARD__

#include "doomdef.h"

#define LEADERBOARD_MAX_ENTRIES 10

#define LEADERBOARD_SURVIVAL 0
#define LEADERBOARD_TIMEATTACK 1
#define LEADERBOARD_CHALLENGE_CRITONLY 2
#define LEADERBOARD_CHALLENGE_NOPOWERUPS 3
#define LEADERBOARD_CHALLENGE_HARDCORE 4

typedef struct {
    char player_name[16];
    int score;
    int wave;
    int time;
} leaderboard_entry_t;

typedef struct {
    leaderboard_entry_t entries[LEADERBOARD_MAX_ENTRIES];
    int num_entries;
} leaderboard_t;

extern leaderboard_t survival_leaderboard;
extern leaderboard_t timeattack_leaderboard;
extern leaderboard_t challenge_critonly_leaderboard;
extern leaderboard_t challenge_nopowerups_leaderboard;
extern leaderboard_t challenge_hardcore_leaderboard;

void G_InitLeaderboard(void);
void G_LoadLeaderboard(void);
void G_SaveLeaderboard(void);

boolean G_AddSurvivalEntry(const char *name, int wave, int kills);
boolean G_AddTimeAttackEntry(const char *name, int total_time, int maps_completed);
boolean G_AddChallengeEntry(int challenge_type, const char *name, int score, int wave);

leaderboard_t* G_GetLeaderboard(int type);
const char* G_GetLeaderboardName(int type);

void G_ClearLeaderboard(int type);
void G_ClearAllLeaderboards(void);

#endif
