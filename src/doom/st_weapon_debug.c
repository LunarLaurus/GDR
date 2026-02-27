//
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
//      Weapon debug overlay for Goblin Dice Rollaz.
//

#include <stdio.h>

#include "i_system.h"
#include "i_video.h"
#include "z_zone.h"
#include "w_wad.h"

#include "deh_main.h"
#include "d_items.h"
#include "doomdef.h"

#include "g_game.h"
#include "m_menu.h"

#include "st_weapon_debug.h"
#include "st_lib.h"
#include "r_local.h"

int goblin_weapon_stats = 0;
static patch_t *st_weaponstat_font[HU_FONTSIZE];

extern float hud_scale;
extern dice_weapon_info_t dice_weapon_info[NUMWEAPONS];

static int ST_CalculateAvgDamage(int weapon)
{
    dice_weapon_info_t *dwi;
    int i;
    int total = 0;
    int count = 0;
    int avg_damage;
    
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return 0;
    
    dwi = &dice_weapon_info[weapon];
    
    if (dwi->die_type == 0)
        return 0;
    
    for (i = 0; i < 7; i++)
    {
        if (dwi->damage_table[i] > 0)
        {
            total += dwi->damage_table[i];
            count++;
        }
    }
    
    if (count == 0)
        return 0;
    
    avg_damage = total / count;
    
    if (dwi->die_type == 100)
    {
        if (dwi->gamble_shot)
        {
            avg_damage = (avg_damage * (100 - dwi->misfire_roll)) / 100;
        }
    }
    
    return avg_damage;
}

void ST_PrintWeaponStats(void)
{
    int i;
    dice_weapon_info_t *dwi;
    const char *weapon_names[NUMWEAPONS] = {
        "Fist",
        "D6 Blaster",
        "D20 Cannon",
        "D12",
        "Shotgun",
        "Chaingun",
        "Missile",
        "Plasma",
        "BFG",
        "Chainsaw",
        "SuperShotgun",
        "Percentile",
        "D4",
        "D8",
        "D10",
        "Twin D6",
        "Arcane D20",
        "Cursed",
        "D2 Flip",
        "D3 Skewered",
        "D7 Veils",
        "D14 Fused",
        "D24 Hourglass",
        "D30 Gear",
        "D48 Forge",
        "D50 Bane",
        "D60 Chaos",
        "D100+1 Fumble",
        "D3 Quake",
        "D12 Chain",
        "Crystal D8"
    };

    DEH_printf("\n=== Dice Weapon Stats ===\n");
    DEH_printf("%-12s %-8s %-6s %-6s %-8s %-6s %-10s\n", 
                "Weapon", "Die", "Crit%", "Mult", "MinDmg", "Roll", "Scaling");
    DEH_printf("------------------------------------------------------------\n");

    for (i = 0; i < NUMWEAPONS; i++)
    {
        dwi = &dice_weapon_info[i];

        if (dwi->die_type == 0)
            continue;

        DEH_printf("%-12s d%-3d %-6d %-6d %-8d %-6d ",
                   weapon_names[i],
                   dwi->die_type,
                   dwi->crit_chance,
                   dwi->crit_multiplier,
                   dwi->min_damage,
                   dwi->crit_roll);

        switch (dwi->crit_scaling_type)
        {
            case CRIT_SCALING_LINEAR:
                DEH_printf("Linear(%d)\n", dwi->crit_scaling_param);
                break;
            case CRIT_SCALING_EXPONENTIAL:
                DEH_printf("Exp(%d)\n", dwi->crit_scaling_param);
                break;
            case CRIT_SCALING_BONUS_FLAT:
                DEH_printf("Flat+%d\n", dwi->crit_scaling_param);
                break;
            case CRIT_SCALING_BONUS_PERCENT:
                DEH_printf("Percent+%d\n", dwi->crit_scaling_param);
                break;
            default:
                DEH_printf("Unknown\n");
                break;
        }
    }

    DEH_printf("\nDamage Tables (roll -> damage):\n");
    for (i = 0; i < NUMWEAPONS; i++)
    {
        dwi = &dice_weapon_info[i];

        if (dwi->die_type == 0)
            continue;

        DEH_printf("%-12s: ", weapon_names[i]);
        if (dwi->die_type == 100)
        {
            DEH_printf("1-15=%d, 16-35=%d, 36-55=%d, 56-75=%d, 76-90=%d, 91-99=%d, 100=%d(crit)\n",
                       dwi->damage_table[0], dwi->damage_table[1], dwi->damage_table[2],
                       dwi->damage_table[3], dwi->damage_table[4], dwi->damage_table[5],
                       dwi->damage_table[6]);
        }
        else if (dwi->die_type == 6)
        {
            int bucket = dwi->die_type / 6;
            DEH_printf("1-%d=%d, %d-%d=%d, %d-%d=%d, %d=crit(%d)\n",
                       bucket, dwi->damage_table[0],
                       bucket+1, bucket*2, dwi->damage_table[1],
                       bucket*2+1, bucket*3, dwi->damage_table[2],
                       dwi->die_type, dwi->damage_table[6]);
        }
        else
        {
            int bucket = dwi->die_type / 6;
            DEH_printf("1-%d=%d, %d-%d=%d, %d-%d=%d, %d-%d=%d, %d-%d=%d, %d=crit(%d)\n",
                       bucket, dwi->damage_table[0],
                       bucket+1, bucket*2, dwi->damage_table[1],
                       bucket*2+1, bucket*3, dwi->damage_table[2],
                       bucket*3+1, bucket*4, dwi->damage_table[3],
                       bucket*4+1, bucket*5, dwi->damage_table[4],
                       bucket*5+1, dwi->die_type-1, dwi->damage_table[5],
                       dwi->die_type, dwi->damage_table[6]);
        }
    }

    DEH_printf("\nSpecial Flags:\n");
    for (i = 0; i < NUMWEAPONS; i++)
    {
        dwi = &dice_weapon_info[i];
        if (dwi->die_type == 0)
            continue;

        if (dwi->gamble_shot || dwi->ricochet_bounces > 0 || dwi->misfire_roll > 0)
        {
            DEH_printf("%-12s: ", weapon_names[i]);
            if (dwi->gamble_shot)
                DEH_printf("GambleShot ");
            if (dwi->ricochet_bounces > 0)
                DEH_printf("Ricochet(%d) ", dwi->ricochet_bounces);
            if (dwi->misfire_roll > 0)
                DEH_printf("Misfire(1-%d) ", dwi->misfire_roll);
            DEH_printf("\n");
        }
    }

    DEH_printf("\nSpawn Weights:\n");
    for (i = 0; i < NUMWEAPONS; i++)
    {
        dwi = &dice_weapon_info[i];
        if (dwi->die_type == 0 || dwi->spawn_weight == 0)
            continue;
        DEH_printf("%-12s: weight=%d\n", weapon_names[i], dwi->spawn_weight);
    }

    DEH_printf("=== End Weapon Stats ===\n\n");
}

void ST_DrawWeaponStats(int x, int y)
{
    player_t *plyr;
    int weapon;
    dice_weapon_info_t *dwi;
    int min_dmg, max_dmg, avg_dmg, crit_pct;
    char buf[64];
    int i;
    char namebuf[9];
    int scaled_x, scaled_y;
    int line_height;
    
    if (hud_scale != 1.0f)
    {
        scaled_x = (int)(x * hud_scale);
        scaled_y = (int)(y * hud_scale);
        line_height = (int)(12 * hud_scale);
    }
    else
    {
        scaled_x = x;
        scaled_y = y;
        line_height = 12;
    }
    
    if (!players)
        return;
    
    plyr = &players[consoleplayer];
    if (!plyr)
        return;
    
    weapon = plyr->readyweapon;
    
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return;
    
    dwi = &dice_weapon_info[weapon];
    
    if (dwi->die_type == 0)
        return;
    
    for (i = 0; i < HU_FONTSIZE; i++)
    {
        DEH_snprintf(namebuf, 9, "STCFN%.3d", i);
        st_weaponstat_font[i] = (patch_t*)W_CacheLumpName(namebuf, PU_STATIC);
    }
    
    min_dmg = dwi->damage_table[0];
    max_dmg = dwi->damage_table[6];
    avg_dmg = ST_CalculateAvgDamage(weapon);
    crit_pct = dwi->crit_chance;
    
    DEH_snprintf(buf, sizeof(buf), "%dd%d", 1, dwi->die_type);
    for (i = 0; buf[i]; i++)
    {
        int c = buf[i];
        if (c >= ' ' && c < HU_FONTSIZE)
            V_DrawPatchDirect(scaled_x, scaled_y, st_weaponstat_font[c]);
        scaled_x += SHORT(st_weaponstat_font[c]->width) * hud_scale;
    }
    
    scaled_y += line_height;
    
    DEH_snprintf(buf, sizeof(buf), "DMG:%d-%d", min_dmg, max_dmg);
    scaled_x = hud_scale != 1.0f ? (int)(x * hud_scale) : x;
    for (i = 0; buf[i]; i++)
    {
        int c = buf[i];
        if (c >= ' ' && c < HU_FONTSIZE)
            V_DrawPatchDirect(scaled_x, scaled_y, st_weaponstat_font[c]);
        scaled_x += SHORT(st_weaponstat_font[c]->width) * hud_scale;
    }
    
    scaled_y += line_height;
    
    DEH_snprintf(buf, sizeof(buf), "AVG:%d", avg_dmg);
    scaled_x = hud_scale != 1.0f ? (int)(x * hud_scale) : x;
    for (i = 0; buf[i]; i++)
    {
        int c = buf[i];
        if (c >= ' ' && c < HU_FONTSIZE)
            V_DrawPatchDirect(scaled_x, scaled_y, st_weaponstat_font[c]);
        scaled_x += SHORT(st_weaponstat_font[c]->width) * hud_scale;
    }
    
    scaled_y += line_height;
    
    DEH_snprintf(buf, sizeof(buf), "CRIT:%d%%", crit_pct);
    scaled_x = hud_scale != 1.0f ? (int)(x * hud_scale) : x;
    for (i = 0; buf[i]; i++)
    {
        int c = buf[i];
        if (c >= ' ' && c < HU_FONTSIZE)
            V_DrawPatchDirect(scaled_x, scaled_y, st_weaponstat_font[c]);
        scaled_x += SHORT(st_weaponstat_font[c]->width) * hud_scale;
    }
    
    scaled_y += line_height;
    
    DEH_snprintf(buf, sizeof(buf), "x%d", dwi->crit_multiplier);
    scaled_x = hud_scale != 1.0f ? (int)(x * hud_scale) : x;
    for (i = 0; buf[i]; i++)
    {
        int c = buf[i];
        if (c >= ' ' && c < HU_FONTSIZE)
            V_DrawPatchDirect(scaled_x, scaled_y, st_weaponstat_font[c]);
        scaled_x += SHORT(st_weaponstat_font[c]->width) * hud_scale;
    }
}
