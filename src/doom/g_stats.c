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
//     Dice roll statistics tracking implementation.
//

#include "doomdef.h"
#include "dstrings.h"
#include "g_stats.h"
#include "d_items.h"

global_dice_stats_t global_dice_stats;
dice_weapon_stats_t weapon_stats[MAX_DICE_WEAPONS];

static const char* weapon_names[NUMWEAPONS] = {
    "Fist",
    "Pistol",
    "d6 Blaster",
    "d20 Cannon",
    "Shotgun",
    "Chaingun",
    "Missile Launcher",
    "Plasma Rifle",
    "BFG 9000",
    "Chainsaw",
    "Super Shotgun",
    "d12",
    "Percentile (d100)",
    "d4",
    "d8",
    "d10",
    "Twin d6",
    "Arcane d20",
    "Cursed Die"
};

void G_ResetDiceStats(void)
{
    int i;
    
    global_dice_stats.total_dice_rolls = 0;
    global_dice_stats.total_crits = 0;
    global_dice_stats.total_misfires = 0;
    global_dice_stats.total_damage_dealt = 0;
    global_dice_stats.highest_single_hit = 0;
    global_dice_stats.highest_roll_achieved = 0;
    global_dice_stats.kills = 0;
    global_dice_stats.deaths = 0;
    
    for (i = 0; i < MAX_DICE_WEAPONS; i++)
    {
        weapon_stats[i].weapon = -1;
        weapon_stats[i].die_type = 0;
        weapon_stats[i].total_rolls = 0;
        weapon_stats[i].crit_rolls = 0;
        weapon_stats[i].misfire_rolls = 0;
        weapon_stats[i].total_damage = 0;
        weapon_stats[i].highest_roll = 0;
        weapon_stats[i].highest_damage = 0;
        weapon_stats[i].lowest_damage = 0;
        weapon_stats[i].total_roll_sum = 0;
    }
    
    for (i = 0; i < NUMWEAPONS; i++)
    {
        dice_weapon_info_t *dwi = &dice_weapon_info[i];
        if (dwi->die_type > 0 && i < MAX_DICE_WEAPONS)
        {
            weapon_stats[i].weapon = i;
            weapon_stats[i].die_type = dwi->die_type;
            weapon_stats[i].lowest_damage = 999999;
        }
    }
}

void G_TrackDiceRoll(int weapon, int die_type, int roll, int damage, boolean is_crit, boolean is_misfire)
{
    dice_weapon_stats_t *ws;
    
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return;
    
    if (weapon >= MAX_DICE_WEAPONS)
        return;
    
    ws = &weapon_stats[weapon];
    
    if (ws->weapon == -1)
    {
        ws->weapon = weapon;
        ws->die_type = die_type;
        ws->lowest_damage = 999999;
    }
    
    ws->total_rolls++;
    ws->total_roll_sum += roll;
    ws->total_damage += damage;
    
    if (roll > ws->highest_roll)
        ws->highest_roll = roll;
    
    if (damage > ws->highest_damage)
        ws->highest_damage = damage;
    
    if (damage < ws->lowest_damage)
        ws->lowest_damage = damage;
    
    if (is_crit)
        ws->crit_rolls++;
    
    if (is_misfire)
        ws->misfire_rolls++;
    
    global_dice_stats.total_dice_rolls++;
    global_dice_stats.total_damage_dealt += damage;
    
    if (is_crit)
        global_dice_stats.total_crits++;
    
    if (is_misfire)
        global_dice_stats.total_misfires++;
    
    if (roll > global_dice_stats.highest_roll_achieved)
        global_dice_stats.highest_roll_achieved = roll;
    
    if (damage > global_dice_stats.highest_single_hit)
        global_dice_stats.highest_single_hit = damage;
}

void G_TrackKill(int killer_weapon)
{
    (void)killer_weapon;
    global_dice_stats.kills++;
}

void G_TrackDeath(void)
{
    global_dice_stats.deaths++;
}

const char* G_GetWeaponName(int weapon)
{
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return "Unknown";
    return weapon_names[weapon];
}

void G_PrintWeaponStats(int weapon)
{
    dice_weapon_stats_t *ws;
    double avg_roll;
    double avg_damage;
    
    if (weapon < 0 || weapon >= NUMWEAPONS)
    {
        DEH_printf("Invalid weapon number.\n");
        return;
    }
    
    if (weapon >= MAX_DICE_WEAPONS)
    {
        DEH_printf("Weapon index out of range.\n");
        return;
    }
    
    ws = &weapon_stats[weapon];
    
    if (ws->total_rolls == 0)
    {
        DEH_printf("No statistics for %s.\n", G_GetWeaponName(weapon));
        return;
    }
    
    DEH_printf("\n=== %s Statistics ===\n", G_GetWeaponName(weapon));
    DEH_printf("d%d\n", ws->die_type);
    DEH_printf("Total Rolls:    %d\n", ws->total_rolls);
    DEH_printf("Critical Hits:  %d (%.1f%%)\n", ws->crit_rolls, 
               (ws->crit_rolls * 100.0) / ws->total_rolls);
    DEH_printf("Misfires:      %d\n", ws->misfire_rolls);
    DEH_printf("Total Damage:  %d\n", ws->total_damage);
    
    avg_roll = (double)ws->total_roll_sum / ws->total_rolls;
    avg_damage = (double)ws->total_damage / ws->total_rolls;
    
    DEH_printf("Avg Roll:      %.2f\n", avg_roll);
    DEH_printf("Avg Damage:    %.2f\n", avg_damage);
    DEH_printf("Highest Roll:  %d\n", ws->highest_roll);
    DEH_printf("Highest Dmg:   %d\n", ws->highest_damage);
    DEH_printf("Lowest Dmg:    %d\n", ws->lowest_damage == 999999 ? 0 : ws->lowest_damage);
    DEH_printf("\n");
}

void G_PrintDiceStats(void)
{
    int i;
    int dice_weapon_count = 0;
    double avg_damage;
    
    DEH_printf("\n");
    DEH_printf("======================================\n");
    DEH_printf("   GOBLIN DICE ROLLAZ - DICE STATS   \n");
    DEH_printf("======================================\n");
    DEH_printf("\n");
    
    DEH_printf("--- Global Statistics ---\n");
    DEH_printf("Total Dice Rolls:    %d\n", global_dice_stats.total_dice_rolls);
    DEH_printf("Total Criticals:     %d\n", global_dice_stats.total_crits);
    DEH_printf("Total Misfires:       %d\n", global_dice_stats.total_misfires);
    DEH_printf("Total Damage Dealt:  %d\n", global_dice_stats.total_damage_dealt);
    DEH_printf("Highest Single Hit:  %d\n", global_dice_stats.highest_single_hit);
    DEH_printf("Highest Roll:        %d\n", global_dice_stats.highest_roll_achieved);
    DEH_printf("Kills:               %d\n", global_dice_stats.kills);
    DEH_printf("Deaths:              %d\n", global_dice_stats.deaths);
    
    if (global_dice_stats.total_dice_rolls > 0)
    {
        avg_damage = (double)global_dice_stats.total_damage_dealt / global_dice_stats.total_dice_rolls;
        DEH_printf("Avg Damage per Roll: %.2f\n", avg_damage);
        DEH_printf("Crit Rate:           %.1f%%\n", 
                   (global_dice_stats.total_crits * 100.0) / global_dice_stats.total_dice_rolls);
    }
    
    DEH_printf("\n--- Per-Weapon Statistics ---\n");
    
    for (i = 0; i < NUMWEAPONS; i++)
    {
        dice_weapon_info_t *dwi = &dice_weapon_info[i];
        
        if (dwi->die_type > 0 && weapon_stats[i].total_rolls > 0)
        {
            dice_weapon_count++;
            DEH_printf("\n%s (d%d):\n", G_GetWeaponName(i), dwi->die_type);
            DEH_printf("  Rolls: %d | Crits: %d (%.1f%%) | Avg Dmg: %.1f\n",
                       weapon_stats[i].total_rolls,
                       weapon_stats[i].crit_rolls,
                       (weapon_stats[i].crit_rolls * 100.0) / weapon_stats[i].total_rolls,
                       (double)weapon_stats[i].total_damage / weapon_stats[i].total_rolls);
        }
    }
    
    if (dice_weapon_count == 0)
    {
        DEH_printf("\nNo dice roll statistics recorded yet.\n");
        DEH_printf("Start playing to generate statistics!\n");
    }
    
    DEH_printf("\n======================================\n");
    DEH_printf("\n");
}
