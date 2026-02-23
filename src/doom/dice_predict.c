//
// Copyright(C) 2026 Goblin Dice Rollaz
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
// DESCRIPTION:  Client-side prediction system for dice roll results
//

#include "doomdef.h"
#include "dice_predict.h"
#include "doomstat.h"
#include "g_game.h"
#include "m_misc.h"
#include "net_defs.h"

static predicted_damage_t predicted_damages[MAX_PREDICTED_DAMAGE];
static int next_predict_id = 0;
static int desync_count = 0;

extern int net_sync_debug;
extern int netgame;

void PREDICT_Init(void)
{
    int i;
    for (i = 0; i < MAX_PREDICTED_DAMAGE; i++)
    {
        predicted_damages[i].pending = false;
        predicted_damages[i].validated = false;
        predicted_damages[i].desynced = false;
    }
    next_predict_id = 0;
    desync_count = 0;
}

int PREDICT_RecordDamage(int x, int y, int damage, boolean critical, 
                        int crit_roll, int weapon)
{
    int i;
    int slot = -1;
    
    for (i = 0; i < MAX_PREDICTED_DAMAGE; i++)
    {
        if (!predicted_damages[i].pending)
        {
            slot = i;
            break;
        }
    }
    
    if (slot == -1)
    {
        slot = next_predict_id % MAX_PREDICTED_DAMAGE;
    }
    
    predicted_damages[slot].id = next_predict_id++;
    predicted_damages[slot].target_x = x;
    predicted_damages[slot].target_y = y;
    predicted_damages[slot].predicted_damage = damage;
    predicted_damages[slot].actual_damage = damage;
    predicted_damages[slot].predicted_critical = critical;
    predicted_damages[slot].actual_critical = critical;
    predicted_damages[slot].predicted_crit_roll = crit_roll;
    predicted_damages[slot].actual_crit_roll = crit_roll;
    predicted_damages[slot].weapon = weapon;
    predicted_damages[slot].tic = gametic;
    predicted_damages[slot].pending = true;
    predicted_damages[slot].validated = false;
    predicted_damages[slot].desynced = false;
    
    if (net_sync_debug && netgame)
    {
        DEH_printf("[PREDICT] Recorded damage: id=%d, damage=%d, crit=%d, roll=%d, weapon=%d\n",
                   predicted_damages[slot].id, damage, critical, crit_roll, weapon);
    }
    
    return predicted_damages[slot].id;
}

void PREDICT_ValidateDamage(int predict_id, int actual_damage, 
                            boolean actual_critical, int actual_crit_roll)
{
    int i;
    
    for (i = 0; i < MAX_PREDICTED_DAMAGE; i++)
    {
        if (predicted_damages[i].pending && predicted_damages[i].id == predict_id)
        {
            predicted_damages[i].actual_damage = actual_damage;
            predicted_damages[i].actual_critical = actual_critical;
            predicted_damages[i].actual_crit_roll = actual_crit_roll;
            predicted_damages[i].validated = true;
            
            if (predicted_damages[i].predicted_damage != actual_damage ||
                predicted_damages[i].predicted_critical != actual_critical ||
                predicted_damages[i].predicted_crit_roll != actual_crit_roll)
            {
                predicted_damages[i].desynced = true;
                desync_count++;
                
                if (net_sync_debug && netgame)
                {
                    DEH_printf("[PREDICT] DESYNC DETECTED: id=%d\n", predict_id);
                    DEH_printf("  Predicted: damage=%d, crit=%d, roll=%d\n",
                               predicted_damages[i].predicted_damage,
                               predicted_damages[i].predicted_critical,
                               predicted_damages[i].predicted_crit_roll);
                    DEH_printf("  Actual:    damage=%d, crit=%d, roll=%d\n",
                               actual_damage, actual_critical, actual_crit_roll);
                    DEH_printf("  Total desyncs: %d\n", desync_count);
                }
            }
            return;
        }
    }
    
    if (net_sync_debug && netgame)
    {
        DEH_printf("[PREDICT] Warning: No matching prediction found for id=%d\n", predict_id);
    }
}

void PREDICT_Clear(void)
{
    int i;
    for (i = 0; i < MAX_PREDICTED_DAMAGE; i++)
    {
        predicted_damages[i].pending = false;
        predicted_damages[i].validated = false;
        predicted_damages[i].desynced = false;
    }
}

int PREDICT_GetPendingCount(void)
{
    int i;
    int count = 0;
    for (i = 0; i < MAX_PREDICTED_DAMAGE; i++)
    {
        if (predicted_damages[i].pending)
            count++;
    }
    return count;
}

int PREDICT_GetDesyncCount(void)
{
    return desync_count;
}

void PREDICT_Ticker(void)
{
    int i;
    int old_tic;
    
    if (!netgame)
        return;
    
    old_tic = gametic - (BACKUPTICS / 2);
    
    for (i = 0; i < MAX_PREDICTED_DAMAGE; i++)
    {
        if (predicted_damages[i].pending && predicted_damages[i].tic < old_tic)
        {
            if (!predicted_damages[i].validated)
            {
                if (net_sync_debug)
                {
                    DEH_printf("[PREDICT] Prediction timeout: id=%d (tic=%d, current=%d)\n",
                               predicted_damages[i].id, predicted_damages[i].tic, gametic);
                }
            }
            
            predicted_damages[i].pending = false;
        }
    }
}
