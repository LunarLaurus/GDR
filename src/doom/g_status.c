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
//     Status effect framework implementation for Goblin Dice Rollaz.
//     Provides duration-based status effects that can be applied
//     to players and monsters (burning, frozen, stunned, etc).
//

#include "doomdef.h"
#include "dstrings.h"
#include "p_local.h"
#include "info.h"
#include "doomstat.h"
#include "g_status.h"

#define BURNING_DAMAGE    2
#define BURNING_TICS      (10 * TICRATE)
#define FROZEN_SPEED_MUL  30
#define FROZEN_TICS       (5 * TICRATE)
#define STUNNED_TICS      (3 * TICRATE)
#define DICE_CURSE_TICS   (15 * TICRATE)

status_info_t status_effects[NUMSTATUSEFFECTS] = {
    {
        st_none,
        "None",
        0,
        0,
        0,
        100,
        0
    },
    {
        st_burning,
        "Burning",
        BURNING_TICS,
        STATUSEFFECT_FLAG_TIMED | STATUSEFFECT_FLAG_DAMAGE,
        BURNING_DAMAGE,
        100,
        0xFF4400
    },
    {
        st_frozen,
        "Frozen",
        FROZEN_TICS,
        STATUSEFFECT_FLAG_TIMED | STATUSEFFECT_FLAG_MOVESLOW,
        0,
        FROZEN_SPEED_MUL,
        0x4488FF
    },
    {
        st_stunned,
        "Stunned",
        STUNNED_TICS,
        STATUSEFFECT_FLAG_TIMED | STATUSEFFECT_FLAG_ATTACKDISABLE,
        0,
        100,
        0xFFFF00
    },
    {
        st_dicecurse,
        "Dice Curse",
        DICE_CURSE_TICS,
        STATUSEFFECT_FLAG_TIMED,
        0,
        100,
        0xFF00FF
    }
};

boolean G_StatusEffectIsActive(mobj_t* target, int effect_id)
{
    if (!target || effect_id <= st_none || effect_id >= NUMSTATUSEFFECTS)
        return false;
    return target->status_effects[effect_id] > 0;
}

int G_StatusEffectTimeRemaining(mobj_t* target, int effect_id)
{
    if (!target || effect_id <= st_none || effect_id >= NUMSTATUSEFFECTS)
        return 0;
    return target->status_effects[effect_id];
}

void G_StatusEffectApply(mobj_t* target, int effect_id, int duration)
{
    status_info_t* se;

    if (!target || effect_id <= st_none || effect_id >= NUMSTATUSEFFECTS)
        return;

    se = &status_effects[effect_id];

    if (duration <= 0)
        duration = se->default_duration;

    if (target->status_effects[effect_id] > 0)
    {
        target->status_effects[effect_id] += duration;
    }
    else
    {
        target->status_effects[effect_id] = duration;
    }
}

void G_StatusEffectRemove(mobj_t* target, int effect_id)
{
    if (!target || effect_id <= st_none || effect_id >= NUMSTATUSEFFECTS)
        return;
    target->status_effects[effect_id] = 0;
}

void G_StatusEffectRemoveAll(mobj_t* target)
{
    int i;
    if (!target)
        return;
    for (i = 1; i < NUMSTATUSEFFECTS; i++)
    {
        target->status_effects[i] = 0;
    }
}

int G_StatusEffectGetMoveSpeedMultiplier(mobj_t* target)
{
    int multiplier = 100;
    int i;

    if (!target)
        return 100;

    for (i = 1; i < NUMSTATUSEFFECTS; i++)
    {
        if (target->status_effects[i] > 0)
        {
            status_info_t* se = &status_effects[i];
            if (se->flags & STATUSEFFECT_FLAG_MOVESLOW)
            {
                if (se->move_speed_multiplier < multiplier)
                    multiplier = se->move_speed_multiplier;
            }
        }
    }

    return multiplier;
}

boolean G_StatusEffectCanAttack(mobj_t* target)
{
    int i;

    if (!target)
        return true;

    for (i = 1; i < NUMSTATUSEFFECTS; i++)
    {
        if (target->status_effects[i] > 0)
        {
            status_info_t* se = &status_effects[i];
            if (se->flags & STATUSEFFECT_FLAG_ATTACKDISABLE)
                return false;
        }
    }

    return true;
}

void G_StatusEffectTick(mobj_t* target)
{
    int i;

    if (!target)
        return;

    for (i = 1; i < NUMSTATUSEFFECTS; i++)
    {
        if (target->status_effects[i] > 0)
        {
            status_info_t* se = &status_effects[i];
            target->status_effects[i]--;

            if (target->status_effects[i] == 0)
            {
                if (i == st_burning)
                {
                    if (target == &players[consoleplayer].mo)
                    {
                        players[consoleplayer].message = "The flames subside.";
                    }
                }
            }

            if ((se->flags & STATUSEFFECT_FLAG_DAMAGE) && (leveltime % TICRATE == 0))
            {
                P_DamageMobj(target, NULL, NULL, se->damage_per_second);
                if (target == &players[consoleplayer].mo && target->health > 0)
                {
                    static char burnmsg[64];
                    snprintf(burnmsg, sizeof(burnmsg), "BURNING! -%d", se->damage_per_second);
                    players[consoleplayer].message = burnmsg;
                }
            }
        }
    }
}

boolean G_StatusEffectDamageTarget(mobj_t* target, int damage)
{
    int i;
    boolean has_damage_effect = false;

    if (!target)
        return false;

    for (i = 1; i < NUMSTATUSEFFECTS; i++)
    {
        if (target->status_effects[i] > 0)
        {
            status_info_t* se = &status_effects[i];
            if (se->flags & STATUSEFFECT_FLAG_DAMAGE)
            {
                has_damage_effect = true;
                P_DamageMobj(target, NULL, NULL, se->damage_per_second / 35 + 1);
            }
        }
    }

    return has_damage_effect;
}
