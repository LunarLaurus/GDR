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
//     Achievement system implementation for Goblin Dice Rollaz.
//

#include "doomdef.h"
#include "g_achievement.h"
#include "g_stats.h"
#include "d_items.h"
#include "v_video.h"
#include "hu_stuff.h"

achievement_t achievements[MAX_ACHIEVEMENTS];
int achievement_count;

// Goblin Dice Rollaz: Achievement toast notifications
achievement_toast_t toast_queue[MAX_TOAST_QUEUE];
int toast_count = 0;

static achievement_t default_achievements[] = {
    {"ACH_KILL_10", "First Blood", "Kill 10 enemies", ACH_CATEGORY_COMBAT, ACH_TYPE_KILLS, 10, 0, 0},
    {"ACH_KILL_50", "Goblin Slayer", "Kill 50 enemies", ACH_CATEGORY_COMBAT, ACH_TYPE_KILLS, 50, 0, 0},
    {"ACH_KILL_100", "Monster Hunter", "Kill 100 enemies", ACH_CATEGORY_COMBAT, ACH_TYPE_KILLS, 100, 0, 0},
    {"ACH_KILL_500", "Death Dealer", "Kill 500 enemies", ACH_CATEGORY_COMBAT, ACH_TYPE_KILLS, 500, 0, 0},
    {"ACH_KILL_1000", "Grim Reaper", "Kill 1000 enemies", ACH_CATEGORY_COMBAT, ACH_TYPE_KILLS, 1000, 0, 0},
    
    {"ACH_BOSS_1", "Boss Slayer", "Defeat 1 boss", ACH_CATEGORY_COMBAT, ACH_TYPE_BOSSES, 1, 0, 0},
    {"ACH_BOSS_5", "Arena Champion", "Defeat 5 bosses", ACH_CATEGORY_COMBAT, ACH_TYPE_BOSSES, 5, 0, 0},
    {"ACH_BOSS_10", "Legendary Hero", "Defeat 10 bosses", ACH_CATEGORY_COMBAT, ACH_TYPE_BOSSES, 10, 0, 0},
    
    {"ACH_CRIT_10", "Lucky Streak", "Land 10 critical hits", ACH_CATEGORY_DICE, ACH_TYPE_CRIT_RATE, 10, 0, 0},
    {"ACH_CRIT_50", "Critical Master", "Land 50 critical hits", ACH_CATEGORY_DICE, ACH_TYPE_CRIT_RATE, 50, 0, 0},
    {"ACH_CRIT_100", "Fate's Favorite", "Land 100 critical hits", ACH_CATEGORY_DICE, ACH_TYPE_CRIT_RATE, 100, 0, 0},
    {"ACH_CRIT_500", "Dice God", "Land 500 critical hits", ACH_CATEGORY_DICE, ACH_TYPE_CRIT_RATE, 500, 0, 0},
    
    {"ACH_HIGH_ROLL_18", "Rolling High", "Roll an 18 or higher on a d20", ACH_CATEGORY_DICE, ACH_TYPE_HIGH_ROLL, 18, 0, 0},
    {"ACH_HIGH_ROLL_20", "Natural 20!", "Roll a perfect 20 on a d20", ACH_CATEGORY_DICE, ACH_TYPE_HIGH_ROLL, 20, 0, 0},
    {"ACH_HIGH_ROLL_100", "Century Roll", "Roll 100 on a percentile die", ACH_CATEGORY_DICE, ACH_TYPE_HIGH_ROLL, 100, 0, 0},
    
    {"ACH_DAMAGE_1000", "Heavy Hitter", "Deal 1000 total damage", ACH_CATEGORY_DICE, ACH_TYPE_DAMAGE, 1000, 0, 0},
    {"ACH_DAMAGE_10000", "Devastator", "Deal 10000 total damage", ACH_CATEGORY_DICE, ACH_TYPE_DAMAGE, 10000, 0, 0},
    {"ACH_DAMAGE_50000", "Obliterator", "Deal 50000 total damage", ACH_CATEGORY_DICE, ACH_TYPE_DAMAGE, 50000, 0, 0},
    {"ACH_SINGLE_100", "One Shot", "Deal 100+ damage in a single hit", ACH_CATEGORY_DICE, ACH_TYPE_SINGLE_DAMAGE, 100, 0, 0},
    {"ACH_SINGLE_200", "Massive Damage", "Deal 200+ damage in a single hit", ACH_CATEGORY_DICE, ACH_TYPE_SINGLE_DAMAGE, 200, 0, 0},
    
    {"ACH_LEVEL_5", "Adventurer", "Complete 5 levels", ACH_CATEGORY_EXPLORATION, ACH_TYPE_LEVELS, 5, 0, 0},
    {"ACH_LEVEL_10", "Dungeon Delver", "Complete 10 levels", ACH_CATEGORY_EXPLORATION, ACH_TYPE_LEVELS, 10, 0, 0},
    {"ACH_LEVEL_20", "Master Explorer", "Complete 20 levels", ACH_CATEGORY_EXPLORATION, ACH_TYPE_LEVELS, 20, 0, 0},
    
    {"ACH_SURVIVE_1", "Survivor", "Win 1 survival wave", ACH_CATEGORY_SKILL, ACH_TYPE_LEVELS, 1, 0, 0},
    {"ACH_SURVIVE_10", "Wave Runner", "Win 10 survival waves", ACH_CATEGORY_SKILL, ACH_TYPE_LEVELS, 10, 0, 0},
    {"ACH_SURVIVE_25", "Last Stand", "Win 25 survival waves", ACH_CATEGORY_SKILL, ACH_TYPE_LEVELS, 25, 0, 0},
    
    {"ACH_COMBO_3", "Hot Streak", "Get 3 consecutive critical hits", ACH_CATEGORY_DICE, ACH_TYPE_COMBO, 3, 0, 0},
    {"ACH_COMBO_5", "Unstoppable", "Get 5 consecutive critical hits", ACH_CATEGORY_DICE, ACH_TYPE_COMBO, 5, 0, 0},
    {"ACH_COMBO_10", "Legendary Luck", "Get 10 consecutive critical hits", ACH_CATEGORY_DICE, ACH_TYPE_COMBO, 10, 0, 0},
    
    {"ACH_CHALLENGE_CRIT", "Crit Only", "Complete a level in Crit Only mode", ACH_CATEGORY_CHALLENGE, ACH_TYPE_LEVELS, 1, 0, 0},
    {"ACH_CHALLENGE_NOPOW", "Purist", "Complete a level with no powerups", ACH_CATEGORY_CHALLENGE, ACH_TYPE_LEVELS, 1, 0, 0},
    {"ACH_CHALLENGE_HARD", "Hardcore", "Complete a level in Hardcore mode", ACH_CATEGORY_CHALLENGE, ACH_TYPE_LEVELS, 1, 0, 0},
};

void G_InitAchievements(void)
{
    int i;
    
    for (i = 0; i < MAX_ACHIEVEMENTS; i++)
    {
        achievements[i].id = 0;
        achievements[i].name = 0;
        achievements[i].description = 0;
        achievements[i].category = ACH_CATEGORY_COUNT;
        achievements[i].type = ACH_TYPE_COUNT;
        achievements[i].threshold = 0;
        achievements[i].unlocked = 0;
        achievements[i].progress = 0;
    }
    
    achievement_count = sizeof(default_achievements) / sizeof(achievement_t);
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        achievements[i] = default_achievements[i];
    }
}

void G_ResetAchievements(void)
{
    int i;
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        achievements[i].unlocked = 0;
        achievements[i].progress = 0;
    }
}

void G_UnlockAchievement(const char* id)
{
    int i;
    
    if (!id)
        return;
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].id && !strcmp(achievements[i].id, id))
        {
            if (!achievements[i].unlocked)
            {
                achievements[i].unlocked = 1;
                achievements[i].progress = achievements[i].threshold;
                
                DEH_printf("[ACHIEVEMENT UNLOCKED: %s]\n", achievements[i].name);
                
                G_QueueAchievementToast(achievements[i].name, achievements[i].description);
            }
            return;
        }
    }
}

void G_QueueAchievementToast(const char *name, const char *description)
{
    int i;
    
    if (!name)
        return;
    
    for (i = 0; i < MAX_TOAST_QUEUE; i++)
    {
        if (!toast_queue[i].active)
        {
            toast_queue[i].name = name;
            toast_queue[i].description = description;
            toast_queue[i].duration = TOAST_DURATION;
            toast_queue[i].active = true;
            toast_count++;
            return;
        }
    }
}

void G_UpdateToasts(void)
{
    int i;
    
    for (i = 0; i < MAX_TOAST_QUEUE; i++)
    {
        if (toast_queue[i].active)
        {
            toast_queue[i].duration--;
            if (toast_queue[i].duration <= 0)
            {
                toast_queue[i].active = false;
                toast_count--;
            }
        }
    }
}

void G_DrawToasts(void)
{
    int i;
    int y_offset;
    int char_y;
    const char *ch;
    int c;
    int x;
    extern patch_t *hu_font[HU_FONTSIZE];
    extern void M_WriteText(int x, int y, const char *string);
    
    y_offset = 20;
    
    for (i = 0; i < MAX_TOAST_QUEUE; i++)
    {
        if (toast_queue[i].active)
        {
            int alpha = 255;
            if (toast_queue[i].duration < 30)
                alpha = (toast_queue[i].duration * 255) / 30;
            
            V_DrawFilledBox(SCREENWIDTH - 280, y_offset, 270, 50, 0);
            V_DrawBox(SCREENWIDTH - 280, y_offset, 270, 50, 84);
            
            char_y = y_offset + 8;
            x = SCREENWIDTH - 275;
            
            if (toast_queue[i].name)
            {
                ch = toast_queue[i].name;
                while (*ch)
                {
                    c = toupper(*ch++) - HU_FONTSTART;
                    if (c >= 0 && c < HU_FONTSIZE && hu_font[c])
                    {
                        V_DrawPatchDirect(x, char_y, hu_font[c]);
                        x += SHORT(hu_font[c]->width);
                    }
                    else
                    {
                        x += 4;
                    }
                }
            }
            
            char_y = y_offset + 26;
            x = SCREENWIDTH - 275;
            
            if (toast_queue[i].description)
            {
                ch = toast_queue[i].description;
                while (*ch)
                {
                    c = toupper(*ch++) - HU_FONTSTART;
                    if (c >= 0 && c < HU_FONTSIZE && hu_font[c])
                    {
                        V_DrawPatchDirect(x, char_y, hu_font[c]);
                        x += SHORT(hu_font[c]->width);
                    }
                    else
                    {
                        x += 4;
                    }
                }
            }
            
            y_offset += 55;
        }
    }
}

boolean G_HasAchievement(const char* id)
{
    int i;
    
    if (!id)
        return 0;
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].id && !strcmp(achievements[i].id, id))
        {
            return achievements[i].unlocked;
        }
    }
    
    return 0;
}

void G_TrackAchievementProgress(achievement_type_t type, int value)
{
    int i;
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].type == type)
        {
            if (!achievements[i].unlocked)
            {
                achievements[i].progress = value;
                
                if (value >= achievements[i].threshold)
                {
                    G_UnlockAchievement(achievements[i].id);
                }
            }
        }
    }
}

void G_UpdateAchievements(void)
{
    int i;
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].unlocked)
            continue;
        
        switch (achievements[i].type)
        {
            case ACH_TYPE_KILLS:
                G_TrackAchievementProgress(ACH_TYPE_KILLS, global_dice_stats.kills);
                break;
            case ACH_TYPE_CRIT_RATE:
                G_TrackAchievementProgress(ACH_TYPE_CRIT_RATE, global_dice_stats.total_crits);
                break;
            case ACH_TYPE_HIGH_ROLL:
                G_TrackAchievementProgress(ACH_TYPE_HIGH_ROLL, global_dice_stats.highest_roll_achieved);
                break;
            case ACH_TYPE_DAMAGE:
                G_TrackAchievementProgress(ACH_TYPE_DAMAGE, global_dice_stats.total_damage_dealt);
                break;
            case ACH_TYPE_SINGLE_DAMAGE:
                G_TrackAchievementProgress(ACH_TYPE_SINGLE_DAMAGE, global_dice_stats.highest_single_hit);
                break;
            default:
                break;
        }
    }
}

void G_PrintAchievements(void)
{
    int i;
    int unlocked = 0;
    
    DEH_printf("\n");
    DEH_printf("======================================\n");
    DEH_printf("   GOBLIN DICE ROLLAZ - ACHIEVEMENTS  \n");
    DEH_printf("======================================\n");
    DEH_printf("\n");
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].id)
        {
            if (achievements[i].unlocked)
            {
                DEH_printf("[*] %s\n", achievements[i].name);
                DEH_printf("    %s\n\n", achievements[i].description);
                unlocked++;
            }
        }
    }
    
    DEH_printf("\n--- Locked Achievements ---\n\n");
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].id && !achievements[i].unlocked)
        {
            DEH_printf("[ ] %s\n", achievements[i].name);
            DEH_printf("    %s\n", achievements[i].description);
            DEH_printf("    Progress: %d / %d\n\n", 
                       achievements[i].progress, 
                       achievements[i].threshold);
        }
    }
    
    DEH_printf("\nUnlocked: %d / %d\n", unlocked, achievement_count);
    DEH_printf("======================================\n");
    DEH_printf("\n");
}

int G_GetUnlockedAchievementCount(void)
{
    int i;
    int count = 0;
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].unlocked)
            count++;
    }
    
    return count;
}

const char* G_GetAchievementName(const char* id)
{
    int i;
    
    if (!id)
        return "Unknown";
    
    for (i = 0; i < achievement_count && i < MAX_ACHIEVEMENTS; i++)
    {
        if (achievements[i].id && !strcmp(achievements[i].id, id))
        {
            return achievements[i].name;
        }
    }
    
    return "Unknown";
}

void G_SaveAchievements(void)
{
}

void G_LoadAchievements(void)
{
}
