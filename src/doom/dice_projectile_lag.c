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
// DESCRIPTION:  Lag compensation system for projectile hits
//

#include "doomdef.h"
#include "dice_projectile_lag.h"
#include "doomstat.h"
#include "g_game.h"
#include "m_misc.h"
#include "net_defs.h"
#include "d_items.h"
#include "p_local.h"

static projectile_record_t projectile_records[MAX_PROJECTILE_RECORDS];
static int next_projectile_id = 0;
static int lag_record_count = 0;

extern int net_sync_debug;
extern int netgame;

int sv_lag_compensation = 1;
int lag_compensation_window = LAG_COMPENSATION_TICS;

void PLAG_Init(void)
{
    int i;
    for (i = 0; i < MAX_PROJECTILE_RECORDS; i++)
    {
        projectile_records[i].id = -1;
        projectile_records[i].projectile = NULL;
        projectile_records[i].target = NULL;
        projectile_records[i].hit_registered = false;
        projectile_records[i].validated = false;
    }
    next_projectile_id = 0;
    lag_record_count = 0;
}

int PLAG_RecordProjectile(mobj_t *proj, mobj_t *target, int weapon)
{
    int slot = -1;
    int i;
    
    if (!sv_lag_compensation || !netgame)
        return -1;
    
    for (i = 0; i < MAX_PROJECTILE_RECORDS; i++)
    {
        if (projectile_records[i].id < 0)
        {
            slot = i;
            break;
        }
    }
    
    if (slot == -1)
    {
        slot = next_projectile_id % MAX_PROJECTILE_RECORDS;
    }
    
    projectile_records[slot].id = next_projectile_id++;
    projectile_records[slot].projectile = proj;
    projectile_records[slot].target = target;
    projectile_records[slot].spawn_x = proj->x;
    projectile_records[slot].spawn_y = proj->y;
    projectile_records[slot].spawn_z = proj->z;
    
    if (target)
    {
        projectile_records[slot].target_x = target->x;
        projectile_records[slot].target_y = target->y;
    }
    else
    {
        projectile_records[slot].target_x = proj->x + proj->momx * 100;
        projectile_records[slot].target_y = proj->y + proj->momy * 100;
    }
    
    projectile_records[slot].angle = proj->angle;
    projectile_records[slot].speed = proj->momx ? proj->momx : proj->momy;
    projectile_records[slot].spawn_tic = gametic;
    projectile_records[slot].weapon = weapon;
    projectile_records[slot].hit_registered = false;
    projectile_records[slot].validated = false;
    
    lag_record_count++;
    
    if (net_sync_debug)
    {
        DEH_printf("[PLAG] Recorded projectile: id=%d, weapon=%d, pos=(%d,%d), target=(%d,%d)\n",
                   projectile_records[slot].id, weapon,
                   proj->x >> FRACBITS, proj->y >> FRACBITS,
                   projectile_records[slot].target_x >> FRACBITS,
                   projectile_records[slot].target_y >> FRACBITS);
    }
    
    return projectile_records[slot].id;
}

void PLAG_MarkHit(int record_id)
{
    int i;
    
    if (!sv_lag_compensation || !netgame)
        return;
    
    for (i = 0; i < MAX_PROJECTILE_RECORDS; i++)
    {
        if (projectile_records[i].id == record_id)
        {
            projectile_records[i].hit_registered = true;
            return;
        }
    }
}

void PLAG_Clear(void)
{
    int i;
    for (i = 0; i < MAX_PROJECTILE_RECORDS; i++)
    {
        projectile_records[i].id = -1;
        projectile_records[i].projectile = NULL;
        projectile_records[i].target = NULL;
        projectile_records[i].hit_registered = false;
        projectile_records[i].validated = false;
    }
    lag_record_count = 0;
}

void PLAG_Ticker(void)
{
    int i;
    int old_tic;
    
    if (!sv_lag_compensation || !netgame)
        return;
    
    old_tic = gametic - lag_compensation_window;
    
    for (i = 0; i < MAX_PROJECTILE_RECORDS; i++)
    {
        if (projectile_records[i].id >= 0)
        {
            if (projectile_records[i].spawn_tic < old_tic)
            {
                projectile_records[i].id = -1;
                lag_record_count--;
            }
            else if (projectile_records[i].projectile && 
                     projectile_records[i].projectile->flags & MF_MISSILE)
            {
                if (!projectile_records[i].validated)
                {
                    if (projectile_records[i].hit_registered)
                    {
                        projectile_records[i].validated = true;
                        
                        if (net_sync_debug)
                        {
                            DEH_printf("[PLAG] Validated hit: id=%d, tics_ago=%d\n",
                                       projectile_records[i].id,
                                       gametic - projectile_records[i].spawn_tic);
                        }
                    }
                }
            }
        }
    }
}

boolean PLAG_ValidateHit(mobj_t *projectile, mobj_t *target)
{
    int i;
    int tics_since_fire;
    fixed_t predicted_x, predicted_y;
    fixed_t dx, dy;
    fixed_t dist;
    
    if (!sv_lag_compensation || !netgame)
        return true;
    
    if (!projectile || !target)
        return true;
    
    for (i = 0; i < MAX_PROJECTILE_RECORDS; i++)
    {
        if (projectile_records[i].projectile == projectile)
        {
            tics_since_fire = gametic - projectile_records[i].spawn_tic;
            
            if (tics_since_fire > lag_compensation_window)
            {
                if (net_sync_debug)
                {
                    DEH_printf("[PLAG] Rejecting hit: projectile too old (%d tics)\n",
                               tics_since_fire);
                }
                return false;
            }
            
            if (projectile_records[i].target)
            {
                predicted_x = projectile_records[i].target_x;
                predicted_y = projectile_records[i].target_y;
                
                dx = target->x - predicted_x;
                dy = target->y - predicted_y;
                dist = P_AproxDistance(dx, dy);
                
                if (dist > 64 * FRACUNIT)
                {
                    if (net_sync_debug)
                    {
                        DEH_printf("[PLAG] Rejecting hit: target too far from predicted pos (dist=%d)\n",
                                   dist >> FRACBITS);
                    }
                    return false;
                }
            }
            
            if (net_sync_debug)
            {
                DEH_printf("[PLAG] Validating hit: projectile=%d, target_offset=%d\n",
                           projectile_records[i].id, dist >> FRACBITS);
            }
            
            return true;
        }
    }
    
    return true;
}

void PLAG_NetArchivePlayers(void)
{
    PLAG_Clear();
}
