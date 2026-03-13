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

#ifndef __P_PARTICLES_H__
#define __P_PARTICLES_H__

#include "doomdef.h"
#include "m_fixed.h"

#define MAX_PARTICLES 256

typedef struct particle_s {
    fixed_t x;
    fixed_t y;
    fixed_t z;
    fixed_t vx;
    fixed_t vy;
    fixed_t vz;
    int lifetime;
    int max_lifetime;
    int color;
    boolean active;
} particle_t;

void P_InitParticles(void);
void P_SpawnCritParticles(fixed_t x, fixed_t y, fixed_t z, int damage, int crit_roll);
void P_SpawnProjectileTrail(fixed_t x, fixed_t y, fixed_t z, int color);
void P_TickerParticles(void);
void P_DrawParticles(void);

#endif
