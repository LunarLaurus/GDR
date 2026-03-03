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
// DESCRIPTION: Particle system for critical hit visual effects
//

#include "p_particles.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomdef.h"
#include "doomstat.h"
#include "m_random.h"
#include "r_main.h"
#include "v_video.h"
#include "z_zone.h"

extern int goblin_particle_distance;

#define PARTICLE_LIFETIME 20
#define PARTICLE_GRAVITY (FRACUNIT / 8)

static particle_t particles[MAX_PARTICLES];

void P_InitParticles(void)
{
    int i;
    for (i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = false;
    }
}

void P_SpawnCritParticles(fixed_t x, fixed_t y, fixed_t z, int damage, int crit_roll)
{
    int i;
    int count;
    int oldest_index = -1;
    int oldest_lifetime = 999;
    int color;

    if (!goblin_modern_fx)
        return;
    
    if (!crit_roll || crit_roll < 15)
        return;
    
    count = 5 + (crit_roll / 10);
    if (count > 20)
        count = 20;
    
    if (crit_roll >= 20)
        color = 184;
    else if (crit_roll >= 15)
        color = 231;
    else
        color = 247;
    
    for (i = 0; i < count; i++)
    {
        int j;
        int oldest = -1;
        int oldest_time = 999;
        
        for (j = 0; j < MAX_PARTICLES; j++)
        {
            if (!particles[j].active)
            {
                oldest = j;
                break;
            }
            if (particles[j].lifetime < oldest_time)
            {
                oldest_time = particles[j].lifetime;
                oldest = j;
            }
        }
        
        if (oldest < 0)
            break;
        
        particles[oldest].x = x + ((P_Random() - 128) * FRACUNIT / 32);
        particles[oldest].y = y + ((P_Random() - 128) * FRACUNIT / 32);
        particles[oldest].z = z + ((P_Random() - 128) * FRACUNIT / 32);
        
        particles[oldest].vx = ((P_Random() - 128) * FRACUNIT / 64);
        particles[oldest].vy = ((P_Random() - 128) * FRACUNIT / 64);
        particles[oldest].vz = (P_Random() * FRACUNIT / 32) + (FRACUNIT / 2);
        
        particles[oldest].lifetime = PARTICLE_LIFETIME + (P_Random() % 10);
        particles[oldest].max_lifetime = particles[oldest].lifetime;
        particles[oldest].color = color;
        particles[oldest].active = true;
    }
}

void P_SpawnProjectileTrail(fixed_t x, fixed_t y, fixed_t z, int color)
{
    int j;
    int oldest = -1;
    int oldest_time = 999;
    int i;
    
    for (j = 0; j < MAX_PARTICLES; j++)
    {
        if (!particles[j].active)
        {
            oldest = j;
            break;
        }
        if (particles[j].lifetime < oldest_time)
        {
            oldest_time = particles[j].lifetime;
            oldest = j;
        }
    }
    
    if (oldest < 0)
        return;
    
    particles[oldest].x = x + ((P_Random() - 128) * FRACUNIT / 64);
    particles[oldest].y = y + ((P_Random() - 128) * FRACUNIT / 64);
    particles[oldest].z = z + ((P_Random() - 128) * FRACUNIT / 64);
    
    particles[oldest].vx = 0;
    particles[oldest].vy = 0;
    particles[oldest].vz = 0;
    
    particles[oldest].lifetime = 10 + (P_Random() % 5);
    particles[oldest].max_lifetime = particles[oldest].lifetime;
    particles[oldest].color = color;
    particles[oldest].active = true;
}

void P_TickerParticles(void)
{
    int i;
    
    for (i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].z += particles[i].vz;
            particles[i].vz -= PARTICLE_GRAVITY;
            
            particles[i].lifetime--;
            
            if (particles[i].lifetime <= 0)
            {
                particles[i].active = false;
            }
        }
    }
}

void P_DrawParticles(void)
{
    int i;
    int max_distance;
    
    if (goblin_particle_distance > 0)
        max_distance = goblin_particle_distance * FRACUNIT;
    else
        max_distance = 128 * FRACUNIT;
    
    for (i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            int screen_x;
            int screen_y;
            fixed_t z_diff;
            
            z_diff = particles[i].z - viewz;
            
            if (z_diff <= 0 || z_diff > max_distance)
                continue;
            
            screen_x = (viewanglefrac + FixedMul(particles[i].x - viewx, projection)) >> FRACBITS;
            screen_y = (centeryfrac + FixedMul(particles[i].z - viewz, yscale)) >> FRACBITS;
            
            if (screen_x < 0 || screen_x >= SCREENWIDTH || screen_y < 0 || screen_y >= SCREENHEIGHT)
                continue;
            
            {
                byte alpha;
                byte col;
                int fade;
                
                fade = (particles[i].lifetime * 255) / particles[i].max_lifetime;
                alpha = (byte)fade;
                
                col = particles[i].color;
                
                V_DrawPixel(screen_x, screen_y, col, alpha);
                V_DrawPixel(screen_x - 1, screen_y, col, alpha / 2);
                V_DrawPixel(screen_x + 1, screen_y, col, alpha / 2);
                V_DrawPixel(screen_x, screen_y - 1, col, alpha / 2);
                V_DrawPixel(screen_x, screen_y + 1, col, alpha / 2);
            }
        }
    }
}
