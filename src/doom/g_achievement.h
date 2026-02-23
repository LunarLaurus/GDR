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
//     Achievement system for Goblin Dice Rollaz.
//     Tracks player accomplishments and displays them in-game.
//

#ifndef __G_ACHIEVEMENT__
#define __G_ACHIEVEMENT__

#include "doomdef.h"

#define MAX_ACHIEVEMENTS 32

typedef enum {
    ACH_CATEGORY_COMBAT,
    ACH_CATEGORY_DICE,
    ACH_CATEGORY_EXPLORATION,
    ACH_CATEGORY_SKILL,
    ACH_CATEGORY_CHALLENGE,
    ACH_CATEGORY_COUNT
} achievement_category_t;

typedef enum {
    ACH_TYPE_KILLS,
    ACH_TYPE_DEATHS,
    ACH_TYPE_CRIT_RATE,
    ACH_TYPE_HIGH_ROLL,
    ACH_TYPE_DAMAGE,
    ACH_TYPE_SINGLE_DAMAGE,
    ACH_TYPE_LEVELS,
    ACH_TYPE_BOSSES,
    ACH_TYPE_TIME,
    ACH_TYPE_COMBO,
    ACH_TYPE_MISFIRE,
    ACH_TYPE_COUNT
} achievement_type_t;

typedef struct {
    const char* id;
    const char* name;
    const char* description;
    achievement_category_t category;
    achievement_type_t type;
    int threshold;
    boolean unlocked;
    int progress;
} achievement_t;

extern achievement_t achievements[MAX_ACHIEVEMENTS];
extern int achievement_count;

// Goblin Dice Rollaz: Achievement toast notifications
#define MAX_TOAST_QUEUE 4
#define TOAST_DURATION 180  // 3 seconds at 60fps

typedef struct {
    const char *name;
    const char *description;
    int duration;
    boolean active;
} achievement_toast_t;

extern achievement_toast_t toast_queue[MAX_TOAST_QUEUE];
extern int toast_count;

void G_InitAchievements(void);
void G_ResetAchievements(void);
void G_UnlockAchievement(const char* id);
void G_QueueAchievementToast(const char *name, const char *description);
void G_UpdateToasts(void);
void G_DrawToasts(void);
boolean G_HasAchievement(const char* id);
void G_TrackAchievementProgress(achievement_type_t type, int value);
void G_UpdateAchievements(void);
void G_PrintAchievements(void);
int G_GetUnlockedAchievementCount(void);
const char* G_GetAchievementName(const char* id);
void G_SaveAchievements(void);
void G_LoadAchievements(void);

#endif
