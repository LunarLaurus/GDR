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
//  Dice load indicator - Goblin Dice Rollaz themed loading animation.
//

#include "doomtype.h"
#include "i_swap.h"
#include "i_video.h"
#include "m_random.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "v_diceicon.h"

#include <string.h>

static pixel_t *dice_data;
static pixel_t *saved_background;

static int dice_icon_xoffs = 0;
static int dice_icon_yoffs = 0;

static boolean dice_drawn;
static int dice_animation_frame;
static int dice_last_update;

static const int dice_animation_speed = 8;

static void DrawDiceFace(pixel_t *buffer, int x, int y, int face, int size);

static void CopyRegion(pixel_t *dest, int dest_pitch,
                       pixel_t *src, int src_pitch,
                       int w, int h)
{
    pixel_t *s, *d;
    int y;

    s = src; d = dest;
    for (y = 0; y < h; ++y)
    {
        memcpy(d, s, w * sizeof(*d));
        s += src_pitch;
        d += dest_pitch;
    }
}

static void DrawDiceFace(pixel_t *buffer, int x, int y, int face, int size)
{
    int i;
    int dot_size = size / 5;
    int margin = dot_size;
    int inner_size = size - (margin * 2);
    
    uint8_t bg_color = 0;
    uint8_t dot_color = 224;
    
    for (i = 0; i < size * size; i++)
    {
        int px = i % size;
        int py = i / size;
        int dx = px - margin;
        int dy = py - margin;
        int cx = inner_size / 2;
        int cy = inner_size / 2;
        int is_dot = 0;
        
        if (dx < 0 || dx >= inner_size || dy < 0 || dy >= inner_size)
        {
            buffer[y * DICE_ICON_W + x + px] = bg_color;
            continue;
        }
        
        switch (face)
        {
            case 1:
                if ((dx == cx && dy == cy))
                    is_dot = 1;
                break;
            case 2:
                if ((dx <= cx/2 && dy <= cy/2) || (dx >= cx + cx/2 && dy >= cy + cy/2))
                    is_dot = 1;
                break;
            case 3:
                if ((dx == cx && dy == cy) ||
                    (dx <= cx/2 && dy <= cy/2) ||
                    (dx >= cx + cx/2 && dy >= cy + cy/2))
                    is_dot = 1;
                break;
            case 4:
                if ((dx <= cx/2 && dy <= cy/2) ||
                    (dx >= cx + cx/2 && dy <= cy/2) ||
                    (dx <= cx/2 && dy >= cy + cy/2) ||
                    (dx >= cx + cx/2 && dy >= cy + cy/2))
                    is_dot = 1;
                break;
            case 5:
                if ((dx == cx && dy == cy) ||
                    (dx <= cx/2 && dy <= cy/2) ||
                    (dx >= cx + cx/2 && dy <= cy/2) ||
                    (dx <= cx/2 && dy >= cy + cy/2) ||
                    (dx >= cx + cx/2 && dy >= cy + cy/2))
                    is_dot = 1;
                break;
            case 6:
                if ((dx <= cx/2) || (dx >= cx + cx/2))
                    is_dot = 1;
                break;
            default:
                if ((dx == cx && dy == cy))
                    is_dot = 1;
                break;
        }
        
        buffer[y * DICE_ICON_W + x + px] = is_dot ? dot_color : bg_color;
    }
}

void V_EnableDiceIcon(void)
{
    pixel_t *tmpscreen;
    int frame;
    
    dice_icon_xoffs = SCREENWIDTH - DICE_ICON_W - 8;
    dice_icon_yoffs = SCREENHEIGHT - DICE_ICON_H - 8;
    
    if (dice_icon_xoffs < 0)
        dice_icon_xoffs = 0;
    if (dice_icon_yoffs < 0)
        dice_icon_yoffs = 0;
    
    if (saved_background != NULL)
    {
        Z_Free(saved_background);
        saved_background = NULL;
    }
    
    if (dice_data != NULL)
    {
        Z_Free(dice_data);
        dice_data = NULL;
    }
    
    saved_background = Z_Malloc(DICE_ICON_W * DICE_ICON_H * sizeof(*saved_background),
                               PU_STATIC, NULL);
    dice_data = Z_Malloc(DICE_ICON_W * DICE_ICON_H * sizeof(*dice_data),
                        PU_STATIC, NULL);
    
    tmpscreen = Z_Malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(*tmpscreen),
                         PU_STATIC, NULL);
    memset(tmpscreen, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(*tmpscreen));
    V_UseBuffer(tmpscreen);
    
    frame = (M_Random() % 6) + 1;
    DrawDiceFace(dice_data, 0, 0, frame, DICE_ICON_H);
    dice_animation_frame = frame;
    dice_last_update = 0;
    
    V_RestoreBuffer();
    Z_Free(tmpscreen);
}

void V_DrawDiceIcon(void)
{
    pixel_t *screen_region;
    int current_time;
    
    if (dice_data == NULL)
        return;
    
    screen_region = I_VideoBuffer + dice_icon_yoffs * SCREENWIDTH + dice_icon_xoffs;
    
    current_time = dice_last_update + 1;
    if (current_time >= dice_animation_speed)
    {
        dice_animation_frame = (dice_animation_frame % 6) + 1;
        DrawDiceFace(dice_data, 0, 0, dice_animation_frame, DICE_ICON_H);
        dice_last_update = 0;
    }
    else
    {
        dice_last_update = current_time;
    }
    
    if (saved_background != NULL)
    {
        CopyRegion(saved_background, DICE_ICON_W,
                   screen_region, SCREENWIDTH,
                   DICE_ICON_W, DICE_ICON_H);
    }
    
    CopyRegion(screen_region, SCREENWIDTH,
               dice_data, DICE_ICON_W,
               DICE_ICON_W, DICE_ICON_H);
    dice_drawn = true;
}

void V_RestoreDiceBackground(void)
{
    pixel_t *screen_region;
    
    if (!dice_drawn)
        return;
    
    if (saved_background == NULL)
        return;
    
    screen_region = I_VideoBuffer + dice_icon_yoffs * SCREENWIDTH + dice_icon_xoffs;
    
    CopyRegion(screen_region, SCREENWIDTH,
               saved_background, DICE_ICON_W,
               DICE_ICON_W, DICE_ICON_H);
    
    dice_drawn = false;
}

void V_DisableDiceIcon(void)
{
    if (saved_background != NULL)
    {
        Z_Free(saved_background);
        saved_background = NULL;
    }
    
    if (dice_data != NULL)
    {
        Z_Free(dice_data);
        dice_data = NULL;
    }
}
