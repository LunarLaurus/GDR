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
#include "d_items.h"
#include "p_local.h"

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

// Goblin Dice Rollaz: Server-authoritative damage recalculation
// Recalculates damage server-side to validate client predictions
int PREDICT_CalculateServerDamage(int weapon, player_t *player, mobj_t *target)
{
    int damage = 0;
    int crit_roll = 0;
    int dice_roll = 0;
    int misfire = 0;
    boolean is_critical = false;
    int effectiveCritChance = crit_chance_default;
    int effectiveCritMultiplier = crit_multiplier_default;
    
    if (!player || weapon < 0 || weapon >= NUMWEAPONS)
        return 0;
    
    damage = P_CalculateDiceDamage(weapon, 0, &crit_roll, &misfire, &dice_roll, player);
    
    if (damage <= 0)
        return 0;
    
    dice_weapon_info_t *dwi = &dice_weapon_info[weapon];
    if (dwi->die_type > 0)
    {
        effectiveCritChance = dwi->crit_chance;
        effectiveCritMultiplier = dwi->crit_multiplier;
    }
    
    if (player->powers[pw_critboost])
    {
        effectiveCritChance += crit_boost_bonus;
    }
    
    if (rpg_mode)
    {
        effectiveCritChance += G_GetPlayerCritBonus(player);
        effectiveCritChance += G_GetPlayerLevelCritBonus(player);
    }
    
    if (target && target->info)
    {
        int targetCritResistance = target->info->crit_resistance;
        effectiveCritChance -= targetCritResistance;
    }
    
    if (effectiveCritChance < 0)
        effectiveCritChance = 0;
    
    if ((P_Random() % 100) < effectiveCritChance)
    {
        damage *= effectiveCritMultiplier;
        is_critical = true;
        crit_roll = (P_Random() % 20) + 1;
    }
    
    if (player->powers[pw_doubledamage])
    {
        damage *= 2;
    }
    
    if (damage < min_damage_cap)
        damage = min_damage_cap;
    
    if (net_sync_debug && netgame)
    {
        DEH_printf("[SERVER] Recalculated damage: weapon=%d, damage=%d, crit=%d, roll=%d\n",
                   weapon, damage, is_critical, crit_roll);
    }
    
    return damage;
}

// Goblin Dice Rollaz: Server-authoritative validation check
// Called periodically to validate client damage predictions
void PREDICT_ValidateServerDamage(int predict_id, int weapon, player_t *player, mobj_t *target)
{
    if (!sv_authoritative_damage || !netgame)
        return;
    
    if (sv_validate_frequency > 0 && (sv_damage_validate_count++ % sv_validate_frequency) != 0)
        return;
    
    int server_damage = PREDICT_CalculateServerDamage(weapon, player, target);
    
    PREDICT_ValidateDamage(predict_id, server_damage, false, 0);
}
