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
// DESCRIPTION:  Debug overlay implementation for damage calculations
//

#include "dbg_ovl.h"

#include <stdio.h>
#include <string.h>

#include "doomdef.h"
#include "doomstat.h"
#include "d_items.h"
#include "p_inter.h"
#include "g_game.h"
#include "v_video.h"
#include "w_wad.h"

#define DBG_OVL_MAX_HISTORY 8

typedef struct {
    int damage;
    int critChance;
    int critMultiplier;
    int diceRoll;
    int critRoll;
    boolean wasCrit;
    boolean guaranteedCrit;
    int targetType;
    char targetName[32];
} damage_record_t;

static damage_record_t damage_history[DBG_OVL_MAX_HISTORY];
static int history_index = 0;
static boolean dbg_overlay_visible = false;

void DBG_Init(void)
{
    int i;
    for (i = 0; i < DBG_OVL_MAX_HISTORY; i++)
    {
        damage_history[i].damage = 0;
        damage_history[i].critChance = 0;
        damage_history[i].critMultiplier = 0;
        damage_history[i].diceRoll = 0;
        damage_history[i].critRoll = 0;
        damage_history[i].wasCrit = false;
        damage_history[i].guaranteedCrit = false;
        damage_history[i].targetType = 0;
        damage_history[i].targetName[0] = '\0';
    }
}

void DBG_SetVisible(boolean visible)
{
    dbg_overlay_visible = visible;
}

void DBG_SetLastDamage(int damage, int critChance, int critMultiplier, 
                       int diceRoll, int critRoll, boolean wasCrit, 
                       boolean guaranteedCrit, int targetType)
{
    damage_record_t *rec = &damage_history[history_index];
    
    rec->damage = damage;
    rec->critChance = critChance;
    rec->critMultiplier = critMultiplier;
    rec->diceRoll = diceRoll;
    rec->critRoll = critRoll;
    rec->wasCrit = wasCrit;
    rec->guaranteedCrit = guaranteedCrit;
    rec->targetType = targetType;
    
    history_index = (history_index + 1) % DBG_OVL_MAX_HISTORY;
}

void DBG_SetLastHitTarget(const char* targetName)
{
    damage_record_t *rec = &damage_history[(history_index + DBG_OVL_MAX_HISTORY - 1) % DBG_OVL_MAX_HISTORY];
    strncpy(rec->targetName, targetName, sizeof(rec->targetName) - 1);
    rec->targetName[sizeof(rec->targetName) - 1] = '\0';
}

static void DBG_DrawChar(int x, int y, int c)
{
    static patch_t **hu_font;
    static boolean fonts_loaded = false;
    char namebuf[9];
    
    if (!fonts_loaded)
    {
        hu_font = (patch_t**)W_CacheLumpName("STCFN000", PU_STATIC);
        fonts_loaded = true;
    }
    
    if (c >= 0 && c < HU_FONTSIZE)
    {
        V_DrawPatchDirect(x, y, hu_font[c]);
    }
}

static void DBG_DrawString(int x, int y, const char *str)
{
    int i;
    int charWidth = 8;
    
    for (i = 0; str[i]; i++)
    {
        int c = str[i];
        if (c == ' ')
        {
            x += charWidth;
        }
        else if (c >= '0' && c <= '9')
        {
            DBG_DrawChar(x, y, c);
            x += charWidth;
        }
        else if (c >= 'A' && c <= 'Z')
        {
            DBG_DrawChar(x, y, c);
            x += charWidth;
        }
        else if (c >= 'a' && c <= 'z')
        {
            DBG_DrawChar(x, y, c);
            x += charWidth;
        }
        else if (c == ':' || c == '-' || c == '+' || c == '!')
        {
            DBG_DrawChar(x, y, c);
            x += charWidth;
        }
        else if (c == '%')
        {
            DBG_DrawChar(x, y, '%');
            x += charWidth;
        }
    }
}

static void DBG_DrawNumber(int x, int y, int num)
{
    char buf[16];
    int i;
    int len;
    
    if (num < 0)
    {
        DBG_DrawChar(x, y, '-');
        x += 8;
        num = -num;
    }
    
    if (num == 0)
    {
        DBG_DrawChar(x, y, '0');
        return;
    }
    
    len = 0;
    while (num > 0)
    {
        buf[len++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (i = len - 1; i >= 0; i--)
    {
        DBG_DrawChar(x, y, buf[i]);
        x += 8;
    }
}

void DBG_Drawer(void)
{
    int i;
    int y;
    int x;
    player_t *player;
    dice_weapon_info_t *dwi;
    
    if (!dbg_overlay_visible)
        return;
    
    if (!playeringame[consoleplayer])
        return;
    
    player = &players[consoleplayer];
    if (!player || !player->mo)
        return;
    
    x = 10;
    y = 50;
    
    DBG_DrawString(x, y, "=== DAMAGE DEBUG ===");
    y += 12;
    
    if (player->readyweapon >= 0 && player->readyweapon < NUMWEAPONS)
    {
        dwi = &dice_weapon_info[player->readyweapon];
        
        DBG_DrawString(x, y, "Weapon:");
        DBG_DrawNumber(x + 56, y, player->readyweapon);
        y += 10;
        
        if (dwi->die_type > 0)
        {
            DBG_DrawString(x, y, "Die:d");
            DBG_DrawNumber(x + 32, y, dwi->die_type);
            y += 10;
            
            DBG_DrawString(x, y, "MinDmg:");
            DBG_DrawNumber(x + 48, y, dwi->min_damage);
            y += 10;
            
            DBG_DrawString(x, y, "Crit%:");
            DBG_DrawNumber(x + 40, y, dwi->crit_chance);
            DBG_DrawChar(x + 64, y, '%');
            y += 10;
            
            DBG_DrawString(x, y, "CritMult:");
            DBG_DrawNumber(x + 56, y, dwi->crit_multiplier);
            y += 10;
            
            if (dwi->misfire_roll > 0)
            {
                DBG_DrawString(x, y, "Misfire:");
                DBG_DrawNumber(x + 56, y, dwi->misfire_roll);
                y += 10;
            }
            
            if (dwi->ricochet_bounces > 0)
            {
                DBG_DrawString(x, y, "Ricochet:");
                DBG_DrawNumber(x + 64, y, dwi->ricochet_bounces);
                y += 10;
            }
        }
        else
        {
            DBG_DrawString(x, y, "Non-dice weapon");
            y += 10;
        }
    }
    
    y += 8;
    DBG_DrawString(x, y, "--- Last Hits ---");
    y += 12;
    
    for (i = 0; i < 5; i++)
    {
        int idx = (history_index + DBG_OVL_MAX_HISTORY - 5 + i) % DBG_OVL_MAX_HISTORY;
        damage_record_t *rec = &damage_history[idx];
        
        if (rec->damage > 0 || rec->diceRoll > 0)
        {
            DBG_DrawNumber(x, y, rec->damage);
            
            if (rec->wasCrit)
            {
                if (rec->guaranteedCrit)
                {
                    DBG_DrawString(x + 24, y, "*G!");
                }
                else
                {
                    DBG_DrawString(x + 24, y, "*!");
                }
            }
            else
            {
                DBG_DrawString(x + 24, y, "  ");
            }
            
            DBG_DrawString(x + 48, y, "d");
            DBG_DrawNumber(x + 56, y, rec->diceRoll);
            
            if (rec->targetName[0])
            {
                int nameX = x + 80;
                int j;
                for (j = 0; rec->targetName[j] && j < 8; j++)
                {
                    DBG_DrawChar(nameX, y, rec->targetName[j]);
                    nameX += 8;
                }
            }
            
            y += 10;
        }
    }
}
