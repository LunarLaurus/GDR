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
// DESCRIPTION:  Damage number overlay implementation
//

#include "dmg_ovl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomdef.h"
#include "doomstat.h"
#include "i_video.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#define DAMAGE_LIFETIME 35
#define CRIT_POPUP_LIFETIME 45
#define MAX_DAMAGE_NUMBERS 24
#define MAX_CRIT_POPUPS 8

static patch_t* dmg_font[10];
static damage_number_t damage_numbers[MAX_DAMAGE_NUMBERS];
static crit_popup_t crit_popups[MAX_CRIT_POPUPS];
static patch_t* hu_font[HU_FONTSIZE];

void DMG_Init(void)
{
    int i;
    char namebuf[9];

    for (i = 0; i < MAX_DAMAGE_NUMBERS; i++)
    {
        damage_numbers[i].active = false;
    }

    for (i = 0; i < MAX_CRIT_POPUPS; i++)
    {
        crit_popups[i].active = false;
    }

    for (i = 0; i < 10; i++)
    {
        DEH_snprintf(namebuf, 9, "STTNUM%d", i);
        dmg_font[i] = (patch_t*)W_CacheLumpName(namebuf, PU_STATIC);
    }

    for (i = 0; i < HU_FONTSIZE; i++)
    {
        DEH_snprintf(namebuf, 9, "STCFN%.3d", i);
        hu_font[i] = (patch_t*)W_CacheLumpName(namebuf, PU_STATIC);
    }
}

void DMG_AddDamage(int x, int y, int damage, boolean critical, int crit_roll, damage_type_t damage_type)
{
    int i;
    int oldest_index = -1;
    int oldest_lifetime = 999;

    for (i = 0; i < MAX_DAMAGE_NUMBERS; i++)
    {
        if (!damage_numbers[i].active)
        {
            break;
        }
        if (damage_numbers[i].lifetime < oldest_lifetime)
        {
            oldest_lifetime = damage_numbers[i].lifetime;
            oldest_index = i;
        }
    }

    if (i >= MAX_DAMAGE_NUMBERS)
    {
        if (oldest_index >= 0)
        {
            i = oldest_index;
        }
        else
        {
            return;
        }
    }

    damage_numbers[i].x = x;
    damage_numbers[i].y = y;
    damage_numbers[i].value = damage;
    damage_numbers[i].lifetime = DAMAGE_LIFETIME;
    damage_numbers[i].max_lifetime = DAMAGE_LIFETIME;
    damage_numbers[i].is_critical = critical;
    damage_numbers[i].crit_roll = crit_roll;
    damage_numbers[i].damage_type = damage_type;
    damage_numbers[i].active = true;

    if (critical && crit_roll > 0)
    {
        DMG_AddCritPopup(x, y - 20, crit_roll);
    }
}

void DMG_AddCritPopup(int x, int y, int roll)
{
    int i;
    int oldest_index = -1;
    int oldest_lifetime = 999;

    for (i = 0; i < MAX_CRIT_POPUPS; i++)
    {
        if (!crit_popups[i].active)
        {
            break;
        }
        if (crit_popups[i].lifetime < oldest_lifetime)
        {
            oldest_lifetime = crit_popups[i].lifetime;
            oldest_index = i;
        }
    }

    if (i >= MAX_CRIT_POPUPS)
    {
        if (oldest_index >= 0)
        {
            i = oldest_index;
        }
        else
        {
            return;
        }
    }

    crit_popups[i].x = x;
    crit_popups[i].y = y;
    crit_popups[i].roll = roll;
    crit_popups[i].lifetime = CRIT_POPUP_LIFETIME;
    crit_popups[i].max_lifetime = CRIT_POPUP_LIFETIME;
    crit_popups[i].active = true;
}

void DMG_Ticker(void)
{
    int i;
    for (i = 0; i < MAX_DAMAGE_NUMBERS; i++)
    {
        if (damage_numbers[i].active)
        {
            damage_numbers[i].lifetime--;
            damage_numbers[i].y -= 1;
            if (damage_numbers[i].lifetime <= 0)
            {
                damage_numbers[i].active = false;
            }
        }
    }

    for (i = 0; i < MAX_CRIT_POPUPS; i++)
    {
        if (crit_popups[i].active)
        {
            crit_popups[i].lifetime--;
            if (crit_popups[i].lifetime > crit_popups[i].max_lifetime - 10)
            {
                crit_popups[i].y -= 1;
            }
            else
            {
                crit_popups[i].y += 1;
            }
            if (crit_popups[i].lifetime <= 0)
            {
                crit_popups[i].active = false;
            }
        }
    }
}

static void DMG_DrawNumber(int x, int y, int num, boolean critical)
{
    int w;
    int h;
    int digit;
    int numdigits;
    int temp;
    boolean negative;

    if (num < 0)
    {
        negative = true;
        num = -num;
    }
    else
    {
        negative = false;
    }

    temp = num;
    numdigits = 1;
    while (temp >= 10)
    {
        temp /= 10;
        numdigits++;
    }

    w = SHORT(dmg_font[0]->width);
    h = SHORT(dmg_font[0]->height);

    x = x - (numdigits * w) / 2;

    if (!num)
    {
        V_DrawPatch(x, y, dmg_font[0]);
        return;
    }

    while (num && numdigits--)
    {
        digit = num % 10;
        V_DrawPatch(x, y, dmg_font[digit]);
        x += w;
        num /= 10;
    }
}

static void DMG_DrawCritPopup(int x, int y, int roll)
{
    int w;
    int h;
    int digit;
    int numdigits;
    int temp;

    temp = roll;
    numdigits = 1;
    while (temp >= 10)
    {
        temp /= 10;
        numdigits++;
    }

    w = SHORT(hu_font[0]->width);
    h = SHORT(hu_font[0]->height);

    x = x - (numdigits * w) / 2;

    if (!roll)
    {
        V_DrawPatch(x, y, hu_font[0]);
        return;
    }

    while (roll && numdigits--)
    {
        digit = roll % 10;
        V_DrawPatch(x, y, hu_font[digit]);
        x += w;
        roll /= 10;
    }
}

static void DMG_DrawDamageTypeIcon(int x, int y, damage_type_t dtype)
{
    byte *col;
    int i;
    static const byte colors[DAMAGETYPE_MAX][3] = {
        { 255, 255, 255 },
        { 255, 80, 0 },
        { 100, 200, 255 },
        { 192, 192, 192 },
        { 255, 140, 0 },
        { 128, 0, 128 }
    };

    if (dtype <= DAMAGETYPE_NORMAL || dtype >= DAMAGETYPE_MAX)
        return;

    col = colors[dtype];

    for (i = 0; i < 6; i++)
    {
        byte *dest = I_VideoBuffer + (y + i) * SCREENWIDTH + x;
        int j;
        for (j = 0; j < 6; j++)
        {
            if (x + j >= 0 && x + j < SCREENWIDTH && y + i >= 0 && y + i < SCREENHEIGHT)
            {
                dest[j] = I_GetColor(j, i, col[0], col[1], col[2]);
            }
        }
    }
}

void DMG_Drawer(void)
{
    int i;

    for (i = 0; i < MAX_CRIT_POPUPS; i++)
    {
        if (crit_popups[i].active)
        {
            DMG_DrawCritPopup(crit_popups[i].x,
                              crit_popups[i].y,
                              crit_popups[i].roll);
        }
    }

    for (i = 0; i < MAX_DAMAGE_NUMBERS; i++)
    {
        if (damage_numbers[i].active)
        {
            DMG_DrawNumber(damage_numbers[i].x, 
                          damage_numbers[i].y, 
                          damage_numbers[i].value,
                          damage_numbers[i].is_critical);

            if (damage_numbers[i].damage_type > DAMAGETYPE_NORMAL && 
                damage_numbers[i].damage_type < DAMAGETYPE_MAX)
            {
                DMG_DrawDamageTypeIcon(damage_numbers[i].x + 20,
                                      damage_numbers[i].y,
                                      damage_numbers[i].damage_type);
            }
        }
    }
}
