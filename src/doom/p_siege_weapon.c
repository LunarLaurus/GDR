//
// Copyright(C) 2024 Goblin Dice Rollaz
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU General Public License for more details.
//
// DESCRIPTION:
//  Siege weapon interaction system implementation.
//

#include "p_siege_weapon.h"
#include "p_mobj.h"
#include "p_user.h"
#include "p_inter.h"
#include "g_game.h"
#include "s_sound.h"
#include "m_random.h"
#include "doomstat.h"
#include "info.h"
#include "d_player.h"

static siege_weapon_t* siege_weapons[MAX_SIEGE_WEAPONS];
static int num_siege_weapons = 0;

#define SIEGE_MOUNT_DISTANCE (64 * FRACUNIT)
#define SIEGE_FIRE_COOLDOWN 30

typedef enum
{
    SIEGE_CANNON,
    SIEGE_BALLISTA,
    SIEGE_TREBUCHET,
    SIEGE_CATAPULT
} siege_type_t;

void P_InitSiegeWeapons(void)
{
    int i;
    for (i = 0; i < MAX_SIEGE_WEAPONS; i++)
    {
        siege_weapons[i] = NULL;
    }
    num_siege_weapons = 0;
}

void P_ShutdownSiegeWeapons(void)
{
    int i;
    for (i = 0; i < num_siege_weapons; i++)
    {
        if (siege_weapons[i])
        {
            Z_Free(siege_weapons[i]);
            siege_weapons[i] = NULL;
        }
    }
    num_siege_weapons = 0;
}

void P_SpawnSiegeWeapon(fixed_t x, fixed_t y, angle_t angle, int siege_type)
{
    siege_weapon_t* siege;
    mobj_t* mo;

    if (num_siege_weapons >= MAX_SIEGE_WEAPONS)
    {
        I_Error("P_SpawnSiegeWeapon: Too many siege weapons (max %d)", MAX_SIEGE_WEAPONS);
        return;
    }

    siege = Z_Malloc(sizeof(siege_weapon_t), PU_LEVEL, NULL);
    memset(siege, 0, sizeof(siege_weapon_t));

    mo = P_SpawnMobj(x, y, ONFLOORZ, MT_BARREL);
    if (!mo)
    {
        Z_Free(siege);
        return;
    }

    mo->flags &= ~MF_SHOOTABLE;
    mo->flags &= ~MF_TELESTOMP;
    mo->flags |= MF_SOLID;
    mo->flags |= MF_NOTDMATCH;
    mo->radius = 24 * FRACUNIT;
    mo->height = 48 * FRACUNIT;

    siege->weapon_mobj = mo;
    siege->fire_cooldown = 0;
    siege->player_mounted = false;
    siege->current_angle = angle;
    siege->min_angle = angle - ANG45;
    siege->max_angle = angle + ANG45;

    switch (siege_type)
    {
        case SIEGE_CANNON:
            siege->ammo_type = am_cell;
            siege->ammo_per_shot = 10;
            siege->damage = 100;
            siege->missile_type = MT_BFG;
            siege->missile_speed = 25 * FRACUNIT;
            break;
        case SIEGE_BALLISTA:
            siege->ammo_type = am_misl;
            siege->ammo_per_shot = 5;
            siege->damage = 60;
            siege->missile_type = MT_MISC0;
            siege->missile_speed = 30 * FRACUNIT;
            break;
        case SIEGE_TREBUCHET:
            siege->ammo_type = am_shell;
            siege->ammo_per_shot = 8;
            siege->damage = 150;
            siege->missile_type = MT_MISC1;
            siege->missile_speed = 20 * FRACUNIT;
            break;
        case SIEGE_CATAPULT:
        default:
            siege->ammo_type = am_clip;
            siege->ammo_per_shot = 20;
            siege->damage = 80;
            siege->missile_type = MT_MISC2;
            siege->missile_speed = 22 * FRACUNIT;
            break;
    }

    siege_weapons[num_siege_weapons++] = siege;
}

boolean P_TryMountSiegeWeapon(player_t* player)
{
    int i;
    siege_weapon_t* siege;
    fixed_t dx, dy;
    fixed_t dist;
    mobj_t* mo;

    if (!player || !player->mo)
        return false;

    if (player->mounted_siege_weapon)
        return false;

    mo = player->mo;

    for (i = 0; i < num_siege_weapons; i++)
    {
        siege = siege_weapons[i];
        if (!siege || !siege->weapon_mobj)
            continue;

        dx = siege->weapon_mobj->x - mo->x;
        dy = siege->weapon_mobj->y - mo->y;
        dist = P_AproxDistance(dx, dy);

        if (dist <= SIEGE_MOUNT_DISTANCE)
        {
            player->mounted_siege_weapon = siege->weapon_mobj;
            siege->player_mounted = true;
            player->siege_weapon_angle = siege->current_angle;

            player->playerstate = PST_LIVE;

            P_SetMobjState(mo, S_NULL);

            if (player == &players[consoleplayer])
            {
                S_StartSound(NULL, sfx_none);
            }

            return true;
        }
    }

    return false;
}

void P_DismountSiegeWeapon(player_t* player)
{
    int i;
    siege_weapon_t* siege = NULL;

    if (!player || !player->mounted_siege_weapon)
        return;

    for (i = 0; i < num_siege_weapons; i++)
    {
        if (siege_weapons[i]->weapon_mobj == player->mounted_siege_weapon)
        {
            siege = siege_weapons[i];
            break;
        }
    }

    if (siege)
    {
        siege->player_mounted = false;
    }

    player->mounted_siege_weapon = NULL;
    player->siege_weapon_angle = 0;

    if (player->mo)
    {
        player->mo->x += 64 * finecosine[player->mo->angle >> ANGLETOFINESHIFT];
        player->mo->y += 64 * finesine[player->mo->angle >> ANGLETOFINESHIFT];
    }
}

void P_UpdateSiegeWeapons(void)
{
    int i;
    siege_weapon_t* siege;

    for (i = 0; i < num_siege_weapons; i++)
    {
        siege = siege_weapons[i];
        if (!siege)
            continue;

        if (siege->fire_cooldown > 0)
            siege->fire_cooldown--;
    }
}

boolean P_CanPlayerUseSiegeWeapon(player_t* player)
{
    int i;
    fixed_t dx, dy;
    fixed_t dist;
    mobj_t* mo;

    if (!player || !player->mo)
        return false;

    if (player->mounted_siege_weapon)
        return true;

    mo = player->mo;

    for (i = 0; i < num_siege_weapons; i++)
    {
        siege_weapon_t* siege = siege_weapons[i];
        if (!siege || !siege->weapon_mobj)
            continue;

        dx = siege->weapon_mobj->x - mo->x;
        dy = siege->weapon_mobj->y - mo->y;
        dist = P_AproxDistance(dx, dy);

        if (dist <= SIEGE_MOUNT_DISTANCE)
            return true;
    }

    return false;
}

void P_FireSiegeWeapon(player_t* player)
{
    int i;
    siege_weapon_t* siege = NULL;
    mobj_t* missile;
    mobj_t* mo;

    if (!player || !player->mounted_siege_weapon)
        return;

    for (i = 0; i < num_siege_weapons; i++)
    {
        if (siege_weapons[i]->weapon_mobj == player->mounted_siege_weapon)
        {
            siege = siege_weapons[i];
            break;
        }
    }

    if (!siege)
        return;

    if (siege->fire_cooldown > 0)
        return;

    if (player->ammo[siege->ammo_type] < siege->ammo_per_shot)
    {
        if (player == &players[consoleplayer])
        {
            player->message = "No ammo for siege weapon";
        }
        return;
    }

    player->ammo[siege->ammo_type] -= siege->ammo_per_shot;
    siege->fire_cooldown = SIEGE_FIRE_COOLDOWN;

    mo = player->mounted_siege_weapon;

    missile = P_SpawnMobj(
        mo->x,
        mo->y + siege->current_angle,
        mo->z + 32 * FRACUNIT,
        siege->missile_type
    );

    if (missile)
    {
        missile->target = mo;
        missile->angle = siege->current_angle;
        missile->momx = FixedMul(siege->missile_speed, finecosine[missile->angle >> ANGLETOFINESHIFT]);
        missile->momy = FixedMul(siege->missile_speed, finesine[missile->angle >> ANGLETOFINESHIFT]);
        missile->momz = 0;
        missile->damage = siege->damage;

        S_StartSound(missile, missile->info->seesound);
    }

    if (player == &players[consoleplayer])
    {
        S_StartSound(NULL, sfx_shotgn);
    }
}

siege_weapon_t* P_GetSiegeWeaponAtPosition(fixed_t x, fixed_t y)
{
    int i;
    siege_weapon_t* siege;
    fixed_t dx, dy;
    fixed_t dist;

    for (i = 0; i < num_siege_weapons; i++)
    {
        siege = siege_weapons[i];
        if (!siege || !siege->weapon_mobj)
            continue;

        dx = siege->weapon_mobj->x - x;
        dy = siege->weapon_mobj->y - y;
        dist = P_AproxDistance(dx, dy);

        if (dist <= SIEGE_MOUNT_DISTANCE)
            return siege;
    }

    return NULL;
}
