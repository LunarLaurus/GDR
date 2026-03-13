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
//	Here is a core component: drawing the floors and ceilings,
//	 while maintaining a per column clipping list only.
//	Moreover, the sky areas have to be determined.
//


#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include "i_system.h"
#include "z_zone.h"
#include "w_wad.h"

#include "doomdef.h"
#include "doomstat.h"

#include "r_local.h"
#include "r_sky.h"



planefunction_t		floorfunc;
planefunction_t		ceilingfunc;

//
// opening
//

// Here comes the obnoxious "visplane".
#define MAXVISPLANES	256
visplane_t		visplanes[MAXVISPLANES];
visplane_t*		lastvisplane;
visplane_t*		floorplane;
visplane_t*		ceilingplane;

static visplane_t*	visplane_hash[MAXVISPLANES];

//
// Hash function for visplane lookup
//
static inline unsigned int VisplaneHash(fixed_t height, int picnum, int lightlevel)
{
    unsigned int hash;
    
    hash = (unsigned int)((height >> FRACBITS) * 73856093u);
    hash ^= (unsigned int)(picnum * 19349663u);
    hash ^= (unsigned int)(lightlevel * 83492791u);
    
    return hash & (MAXVISPLANES - 1);
}

// ?
#define MAXOPENINGS	SCREENWIDTH*64
short			openings[MAXOPENINGS];
short*			lastopening;


//
// Clip values are the solid pixel bounding the range.
//  floorclip starts out SCREENHEIGHT
//  ceilingclip starts out -1
//
short			floorclip[SCREENWIDTH];
short			ceilingclip[SCREENWIDTH];

//
// spanstart holds the start of a plane span
// initialized to 0 at start
//
int			spanstart[SCREENHEIGHT];
int			spanstop[SCREENHEIGHT];

//
// texture mapping
//
lighttable_t**		planezlight;
fixed_t			planeheight;

fixed_t			yslope[SCREENHEIGHT];
fixed_t			distscale[SCREENWIDTH];
fixed_t			basexscale;
fixed_t			baseyscale;

fixed_t			cachedheight[SCREENHEIGHT];
fixed_t			cacheddistance[SCREENHEIGHT];
fixed_t			cachedxstep[SCREENHEIGHT];
fixed_t			cachedystep[SCREENHEIGHT];

int				ds_mipmap_level;
byte*			ds_mipmap_source;



//
// R_InitPlanes
// Only at game startup.
//
void R_InitPlanes (void)
{
  // Doh!
}


//
// R_MapPlane
//
// Uses global vars:
//  planeheight
//  ds_source
//  basexscale
//  baseyscale
//  viewx
//  viewy
//  ds_mipmap_level
//  ds_mipmap_source
//
// BASIC PRIMITIVE
//
void
R_MapPlane
( int		y,
  int		x1,
  int		x2 )
{
    angle_t	angle;
    fixed_t	distance;
    fixed_t	length;
    unsigned index;
	
    if (planeheight != cachedheight[y])
    {
	cachedheight[y] = planeheight;
	distance = cacheddistance[y] = FixedMul (planeheight, yslope[y]);
	ds_xstep = cachedxstep[y] = FixedMul (distance,basexscale);
	ds_ystep = cachedystep[y] = FixedMul (distance,baseyscale);
    }
    else
    {
	distance = cacheddistance[y];
	ds_xstep = cachedxstep[y];
	ds_ystep = cachedystep[y];
    }
	
    length = FixedMul (distance,distscale[x1]);
    angle = (viewangle + xtoviewangle[x1])>>ANGLETOFINESHIFT;
    ds_xfrac = viewx + FixedMul(finecosine[angle], length);
    ds_yfrac = -viewy - FixedMul(finesine[angle], length);

    if (fixedcolormap)
	ds_colormap = fixedcolormap;
    else
    {
	index = distance >> LIGHTZSHIFT;
	
	if (index >= MAXLIGHTZ )
	    index = MAXLIGHTZ-1;

	ds_colormap = planezlight[index];
    }
	
    ds_y = y;
    ds_x1 = x1;
    ds_x2 = x2;

    // high or low detail
    spanfunc ();	
}


//
// R_ClearPlanes
// At begining of frame.
//
void R_ClearPlanes (void)
{
    int		i;
    angle_t	angle;
    
    for (i = 0; i < MAXVISPLANES; i++)
    {
        visplane_hash[i] = NULL;
    }
    
    // opening / clipping determination
    for (i=0 ; i<viewwidth ; i++)
    {
	floorclip[i] = viewheight;
	ceilingclip[i] = -1;
    }

    lastvisplane = visplanes;
    lastopening = openings;
    
    // texture calculation
    memset (cachedheight, 0, sizeof(cachedheight));

    ds_mipmap_level = 0;
    ds_mipmap_source = NULL;

    // left to right mapping
    angle = (viewangle-ANG90)>>ANGLETOFINESHIFT;
	
    // scale will be unit scale at SCREENWIDTH/2 distance
    basexscale = FixedDiv (finecosine[angle],centerxfrac);
    baseyscale = -FixedDiv (finesine[angle],centerxfrac);
}




//
// R_FindPlane
//
visplane_t*
R_FindPlane
( fixed_t	height,
  int		picnum,
  int		lightlevel )
{
    visplane_t*	check;
    unsigned int hash;
    
    if (picnum == skyflatnum)
    {
	height = 0;			// all skys map together
	lightlevel = 0;
    }
    
    hash = VisplaneHash(height, picnum, lightlevel);
    check = visplane_hash[hash];
    
    while (check != NULL)
    {
        if (height == check->height
	    && picnum == check->picnum
	    && lightlevel == check->lightlevel)
	{
	    return check;
	}
	check = check->hash_next;
    }
    
    if (lastvisplane - visplanes == MAXVISPLANES)
    {
        I_Error ("R_FindPlane: no more visplanes");
    }
		
    check = lastvisplane++;
    
    check->height = height;
    check->picnum = picnum;
    check->lightlevel = lightlevel;
    check->minx = SCREENWIDTH;
    check->maxx = -1;
    check->hash_next = visplane_hash[hash];
    visplane_hash[hash] = check;
    
    memset (check->top,0xff,sizeof(check->top));
		
    return check;
}


//
// R_CheckPlane
//
// Optimized overlap scanning using wider word comparisons
//
visplane_t*
R_CheckPlane
( visplane_t*	pl,
  int		start,
  int		stop )
{
    int		intrl;
    int		intrh;
    int		unionl;
    int		unionh;
    int		x;
    byte*	p;
    unsigned int check;
    
    if (start < pl->minx)
    {
	intrl = pl->minx;
	unionl = start;
    }
    else
    {
	unionl = pl->minx;
	intrl = start;
    }
	
    if (stop > pl->maxx)
    {
	intrh = pl->maxx;
	unionh = stop;
    }
    else
    {
	unionh = pl->maxx;
	intrh = stop;
    }

    // Fast overlap check: scan using 32-bit comparisons
    // We need to find any byte != 0xff (which indicates empty)
    p = &pl->top[intrl];
    x = intrl;

    // Check unaligned bytes before 4-byte boundary
    while (x < intrh && (((unsigned int)p) & 3))
    {
        if (*p != 0xff)
            goto overlapfound;
        p++;
        x++;
    }

    // Check 4 bytes at a time using fast 32-bit comparison
    // If any byte != 0xff, the dword != 0xffffffff
    while (x + 4 <= intrh)
    {
        check = *(unsigned int*)p;
        // Check if any byte is not 0xff
        // Each byte should be 0xff (255). Combined: 0xffffffff if all 0xff
        // If any byte != 0xff, then the dword != 0xffffffff
        if (check != 0xffffffff)
        {
            // Found non-0xff, check which byte
            if (p[0] != 0xff) goto overlapfound;
            if (p[1] != 0xff) { p++; goto overlapfound; }
            if (p[2] != 0xff) { p += 2; goto overlapfound; }
            if (p[3] != 0xff) { p += 3; goto overlapfound; }
        }
        p += 4;
        x += 4;
    }

    // Check remaining bytes
    while (x <= intrh)
    {
        if (*p != 0xff)
            goto overlapfound;
        p++;
        x++;
    }

overlapfound:

    if (x > intrh)
    {
	pl->minx = unionl;
	pl->maxx = unionh;

	// use the same one
	return pl;		
    }
	
    // make a new visplane
    lastvisplane->height = pl->height;
    lastvisplane->picnum = pl->picnum;
    lastvisplane->lightlevel = pl->lightlevel;
    
    if (lastvisplane - visplanes == MAXVISPLANES)
	I_Error ("R_CheckPlane: no more visplanes");

    pl = lastvisplane++;
    pl->minx = start;
    pl->maxx = stop;

    memset (pl->top,0xff,sizeof(pl->top));
		
    return pl;
}


//
// R_MakeSpans
//
void
R_MakeSpans
( int		x,
  int		t1,
  int		b1,
  int		t2,
  int		b2 )
{
    while (t1 < t2 && t1<=b1)
    {
	R_MapPlane (t1,spanstart[t1],x-1);
	t1++;
    }
    while (b1 > b2 && b1>=t1)
    {
	R_MapPlane (b1,spanstart[b1],x-1);
	b1--;
    }
	
    while (t2 < t1 && t2<=b2)
    {
	spanstart[t2] = x;
	t2++;
    }
    while (b2 > b1 && b2>=t2)
    {
	spanstart[b2] = x;
	b2--;
    }
}



//
// R_DrawPlanes
// At the end of each frame.
//
// Parallel sector processing: For large levels with many sectors (visplanes),
// we can parallelize the plane processing. However, due to the software renderer's
// global state (ds_* variables) and screen buffer writes, true parallelization
// requires thread-local storage or major refactoring.
//
// For now, we use OpenMP to parallelize the plane iteration when available.
// The actual drawing still happens serially due to global state dependencies.
//
void R_DrawPlanes (void)
{
    visplane_t*		pl;
    int			light;
    int			x;
    int			stop;
    int			angle;
    int                 lumpnum;
    int                 numplanes;
    int                 i;
				
    numplanes = lastvisplane - visplanes;

#if defined(HAVE_OPENMP) && defined(OPENMP_PARALLEL_PLANES)
    #pragma omp parallel for private(pl, light, x, stop, angle, lumpnum) schedule(dynamic)
    for (i = 0; i < numplanes; i++)
    {
        pl = &visplanes[i];
#else
    for (pl = visplanes ; pl < lastvisplane ; pl++)
    {
#endif
	if (pl->minx > pl->maxx)
	    continue;

	// Goblin Dice Rollaz: Track plane draw calls
	if (r_render_profile_enabled)
	{
	    r_plane_count++;
	}

	
	// sky flat
	if (pl->picnum == skyflatnum)
	{
	    dc_iscale = pspriteiscale>>detailshift;
	    
	    // Sky is allways drawn full bright,
	    //  i.e. colormaps[0] is used.
	    // Because of this hack, sky is not affected
	    //  by INVUL inverse mapping.
	    dc_colormap = colormaps;
	    dc_texturemid = skytexturemid;
	    for (x=pl->minx ; x <= pl->maxx ; x++)
	    {
		dc_yl = pl->top[x];
		dc_yh = pl->bottom[x];

		if (dc_yl <= dc_yh)
		{
		    angle = (viewangle + xtoviewangle[x])>>ANGLETOSKYSHIFT;
		    dc_x = x;
		dc_source = R_GetCachedSkyColumn(angle);
		    colfunc ();
		}
	    }
	    continue;
	}
	
	// regular flat
        lumpnum = firstflat + flattranslation[pl->picnum];
	
	planeheight = abs(pl->height-viewz);

	ds_mipmap_level = 0;
	ds_mipmap_source = NULL;

	if (planeheight > 256*FRACUNIT)
	{
	    byte* mip = R_GetFlatMipmap(pl->picnum, planeheight);
	    if (mip)
	    {
		ds_mipmap_source = mip;
		if (planeheight > 512*FRACUNIT)
		    ds_mipmap_level = 2;
		else
		    ds_mipmap_level = 1;
	    }
	}

	ds_source = W_CacheLumpNum(lumpnum, PU_CACHE);
	
	light = (pl->lightlevel >> LIGHTSEGSHIFT)+extralight;

	if (light >= LIGHTLEVELS)
	    light = LIGHTLEVELS-1;

	if (light < 0)
	    light = 0;

	planezlight = zlight[light];

	pl->top[pl->maxx+1] = 0xff;
	pl->top[pl->minx-1] = 0xff;
	
        stop = pl->maxx + 1;

	for (x=pl->minx ; x<= stop ; x++)
	{
	    R_MakeSpans(x,pl->top[x-1],
			pl->bottom[x-1],
			pl->top[x],
			pl->bottom[x]);
	}
    } /* end of per-plane loop (both OpenMP and non-OpenMP) */
}
