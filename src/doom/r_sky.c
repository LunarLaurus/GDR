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
//  Sky rendering. The DOOM sky is a texture map like any
//  wall, wrapping around. A 1024 columns equal 360 degrees.
//  The default sky map is 256 columns and repeats 4 times
//  on a 320 screen?
//  
//

#include <stdlib.h>

#include "config.h"
#include "doomdef.h"
#include "i_system.h"
#include "m_fixed.h"
#include "r_data.h"
#include "r_sky.h"

//
// sky mapping
//
int			skyflatnum;
int			skytexture;
int			skytexturemid;

#define SKY_ANGLE_CACHE_SIZE 1024
static byte* sky_column_cache[SKY_ANGLE_CACHE_SIZE];
static boolean sky_cache_initialized = false;

void R_PrecacheSkyColumns(void)
{
    int i;
    for (i = 0; i < SKY_ANGLE_CACHE_SIZE; i++)
    {
        sky_column_cache[i] = R_GetColumn(skytexture, i);
    }
    sky_cache_initialized = true;
}

void R_ClearSkyCache(void)
{
    sky_cache_initialized = false;
}

byte* R_GetCachedSkyColumn(int angle)
{
    if (!sky_cache_initialized || skytexture == 0)
    {
        return R_GetColumn(skytexture, angle);
    }
    return sky_column_cache[angle & (SKY_ANGLE_CACHE_SIZE - 1)];
}

//
// R_InitSkyMap
// Called whenever the view size changes.
//
void R_InitSkyMap (void)
{
  // skyflatnum = R_FlatNumForName ( SKYFLATNAME );
    skytexturemid = SCREENHEIGHT/2*FRACUNIT;
}

