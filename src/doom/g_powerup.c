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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//     Shared powerup framework implementation for Goblin Dice Rollaz.
//

#include "doomdef.h"
#include "dstrings.h"
#include "d_englsh.h"
#include "sounds.h"
#include "s_sound.h"
#include "p_local.h"
#include "info.h"
#include "doomstat.h"

powerup_info_t powerups[NUMPOWERS] = {
    {
        pw_invulnerability,
        "Invulnerability",
        INVULNTICS,
        POWERUP_FLAG_TIMED,
        DEH_String(GOTINVUL),
        CRTRIPCOLORMAP,
        sfx_getpow,
        MT_INVULNERABILITY,
        SPR_PINV,
        100
    },
    {
        pw_strength,
        "Strength",
        1,
        POWERUP_FLAG_PERMANENT,
        DEH_String(GOTBERSERK),
        0,
        sfx_getpow,
        MT_STRENGTH,
        SPR_PSTR,
        100
    },
    {
        pw_invisibility,
        "Invisibility",
        INVISTICS,
        POWERUP_FLAG_TIMED,
        DEH_String(GOTINVIS),
        0,
        sfx_getpow,
        MT_INVISIBILITY,
        SPR_PINS,
        100
    },
    {
        pw_ironfeet,
        "Iron Feet",
        IRONTICS,
        POWERUP_FLAG_TIMED,
        DEH_String(GOTSUIT),
        0,
        sfx_getpow,
        MT_IRONFEET,
        SPR_SUIT,
        100
    },
    {
        pw_allmap,
        "Allmap",
        1,
        POWERUP_FLAG_PERMANENT,
        DEH_String(GOTMAP),
        0,
        sfx_getpow,
        MT_ALLMAP,
        SPR_PMAP,
        100
    },
    {
        pw_infrared,
        "Infrared",
        INFRATICS,
        POWERUP_FLAG_TIMED,
        DEH_String(GOTVISOR),
        0,
        sfx_getpow,
        MT_INFRARED,
        SPR_PVIS,
        100
    },
    {
        pw_critboost,
        "Critical Hit Boost",
        CRITBOOSTTICS,
        POWERUP_FLAG_TIMED | POWERUP_FLAG_EXCLUSIVE,
        DEH_String(GOTCRITS),
        CRITCOLORMAP,
        sfx_critup,
        MT_CRITBOOST,
        SPR_PCRT,
        100
    },
    {
        pw_doubledamage,
        "Double Damage",
        DOUBLEDAMAGETICS,
        POWERUP_FLAG_TIMED | POWERUP_FLAG_EXCLUSIVE,
        DEH_String(GOTDOUBLEDAMAGE),
        DOUBLEDAMAGECOLORMAP,
        sfx_doubup,
        MT_DOUBLEDAMAGE,
        SPR_PDMD,
        80
    },
    {
        pw_dicefortune,
        "Dice Fortune",
        1,
        POWERUP_FLAG_ONESHOT,
        "DICE FORTUNE!",
        CRITCOLORMAP,
        sfx_critup,
        MT_DICEFORTUNE,
        SPR_PFTR,
        50
    },
    {
        pw_luckyseven,
        "Lucky Seven",
        7,
        POWERUP_FLAG_ONESHOT | POWERUP_FLAG_GUARANTEED_CRITS,
        "LUCKY SEVEN!",
        CRITCOLORMAP,
        sfx_critup,
        MT_NADA,
        SPR_PCRT,
        25
    }
};

boolean G_PowerupIsActive(player_t* player, int powerup_id)
{
    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return false;
    return player->powers[powerup_id] > 0;
}

int G_PowerupTimeRemaining(player_t* player, int powerup_id)
{
    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return 0;
    return player->powers[powerup_id];
}

void G_PowerupActivate(player_t* player, int powerup_id, int duration)
{
    powerup_info_t* pu;

    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return;

    pu = &powerups[powerup_id];

    if (!(pu->flags & POWERUP_FLAG_PERMANENT) && duration <= 0)
    {
        duration = pu->default_duration;
    }

    if (pu->flags & POWERUP_FLAG_EXCLUSIVE)
    {
        int i;
        for (i = 0; i < NUMPOWERS; i++)
        {
            if (i != powerup_id && (powerups[i].flags & POWERUP_FLAG_EXCLUSIVE))
            {
                player->powers[i] = 0;
            }
        }
    }

    player->powers[powerup_id] = duration;

    if (pu->sound != sfx_None && player->mo)
    {
        S_StartSound(player->mo, pu->sound);
    }
}

void G_PowerupDeactivate(player_t* player, int powerup_id)
{
    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return;
    player->powers[powerup_id] = 0;
}

boolean G_PowerupCanPickup(player_t* player, int powerup_id)
{
    powerup_info_t* pu;
    int i;

    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return false;

    // Goblin Dice Rollaz: Challenge mode - no powerups
    if (CHALLENGE_NO_POWERUPS())
        return false;

    pu = &powerups[powerup_id];

    if (pu->flags & POWERUP_FLAG_ONESHOT)
    {
        return player->powers[powerup_id] == 0;
    }

    if (pu->flags & POWERUP_FLAG_EXCLUSIVE)
    {
        for (i = 0; i < NUMPOWERS; i++)
        {
            if (i != powerup_id && (powerups[i].flags & POWERUP_FLAG_EXCLUSIVE))
            {
                if (player->powers[i] > 0)
                    return false;
            }
        }
    }

    if (pu->flags & POWERUP_FLAG_TIMED)
    {
        if (player->powers[powerup_id] > 0)
            return false;
    }

    return true;
}

void G_PowerupOnHit(player_t* player, int powerup_id)
{
    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return;

    if (powerups[powerup_id].flags & POWERUP_FLAG_ONESHOT)
    {
        player->powers[powerup_id] = 0;
    }
}

int G_GetActivePowerupCount(player_t* player)
{
    int i;
    int count = 0;

    for (i = 0; i < NUMPOWERS; i++)
    {
        if (player->powers[i] > 0)
            count++;
    }

    return count;
}

#define COOP_POWERUP_SHARE_RADIUS (768 * FRACUNIT)

void G_PowerupShareWithNearbyPlayers(player_t* activator, int powerup_id)
{
    int i;
    player_t* other;
    mobj_t* activator_mo;
    fixed_t dist;

    if (!netgame || deathmatch)
        return;

    if (powerup_id < 0 || powerup_id >= NUMPOWERS)
        return;

    if (!(powerups[powerup_id].flags & POWERUP_FLAG_TIMED))
        return;

    if (!activator->mo)
        return;

    activator_mo = activator->mo;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        other = &players[i];

        if (other == activator)
            continue;

        if (!playeringame[i])
            continue;

        if (!other->mo)
            continue;

        if (other->playerstate != PST_LIVE)
            continue;

        dist = P_AproxDistance(activator_mo->x - other->mo->x,
                               activator_mo->y - other->mo->y);

        if (dist <= COOP_POWERUP_SHARE_RADIUS)
        {
            if (G_PowerupCanPickup(other, powerup_id))
            {
                int duration = powerups[powerup_id].default_duration;
                G_PowerupActivate(other, powerup_id, duration);
            }
        }
    }
}

int G_GetAndConsumeGuaranteedCrits(player_t* player)
{
    int crits = 0;

    if (player->powers[pw_dicefortune])
    {
        crits += 1;
        player->powers[pw_dicefortune] = 0;
    }

    if (player->powers[pw_luckyseven] > 0)
    {
        crits += player->powers[pw_luckyseven];
        player->powers[pw_luckyseven] = 0;
    }

    return crits;
}
