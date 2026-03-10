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
//  Faction AI system implementation.
//  Handles faction assignment, tactics, pack behavior, and morale.
//

#include "p_ai_faction.h"
#include "info.h"
#include "g_game.h"
#include "doomstat.h"
#include "s_sound.h"

#define PACK_DETECT_RADIUS (512*FRACUNIT)
#define PACK_AGGRESSION_BONUS_PER_ALLY 10
#define PACK_MAX_BONUS 40
#define PACK_RETREAT_THRESHOLD_BASE 128
#define PACK_RETREAT_REDUCTION_PER_ALLY 16

#define MORALE_FLAG_LEADER     0x01
#define MORALE_FLAG_FOLLOWER  0x02
#define MORALE_FLAG_BROKEN     0x04
#define MORALE_FLAG_RETREATING 0x08

faction_t P_GetFaction(mobjtype_t type)
{
    if (type == MT_GOBLIN_SHAMAN ||
        type == MT_GOBLIN_SCOUT ||
        type == MT_GOBLIN_SNEAK ||
        type == MT_GOBLIN_ALCHEMIST ||
        type == MT_GOBLIN_TOTEMIST)
        return FACTION_GOBLIN;

    if (type == MT_DWARF ||
        type == MT_DWARF_BERSERKER ||
        type == MT_DWARF_ENGINEER ||
        type == MT_DWARF_TURRET ||
        type == MT_DWARF_DEFENDER ||
        type == MT_DWARF_MARKSMAN ||
        type == MT_DWARF_MINER ||
        type == MT_DWARF_CAPTAIN ||
        type == MT_DWARF_BOMBARDIER ||
        type == MT_DWARF_ARMORED ||
        type == MT_DWARF_FLAMETHROWER ||
        type == MT_DWARF_THUNDERER ||
        type == MT_DWARF_IRONCLAD ||
        type == MT_DWARF_STONECUTTER ||
        type == MT_DWARF_THUNDERMAGE ||
        type == MT_DWARF_WARLORD ||
        type == MT_DWARF_RUNESMITH ||
        type == MT_DWARF_RUNEBEARER ||
        type == MT_DWARF_HIGHPRIEST ||
        type == MT_DWARF_COMMANDER ||
        type == MT_DWARF_TINKERER ||
        type == MT_DWARF_DRILLTANK ||
        type == MT_DWARF_BARRELELITE ||
        type == MT_DWARF_SHADOWBLADE ||
        type == MT_DWARF_GEOLOGIST ||
        type == MT_DWARF_ALCHEMIST ||
        type == MT_DWARF_TREASUREHUNTER ||
        type == MT_DWARF_JUGGERNAUT ||
        type == MT_DWARF_SAPPER ||
        type == MT_ROCK_GOLEM)
        return FACTION_DWARF;

    return FACTION_NONE;
}

tactic_t P_GetTactic(mobjtype_t type)
{
    faction_t fac = P_GetFaction(type);

    if (fac == FACTION_GOBLIN)
        return TACTIC_FLANK;

    if (fac == FACTION_DWARF)
    {
        if (P_CanBeLeader(type))
            return TACTIC_SHIELD_WALL;
        return TACTIC_HOLD;
    }

    return TACTIC_DEFAULT;
}

int P_GetNearbyAllyCount(mobj_t* actor)
{
    mobj_t* mo;
    sector_t* sec;
    int count = 0;
    faction_t my_faction;

    if (!actor)
        return 0;

    my_faction = P_GetFaction(actor->type);
    if (my_faction == FACTION_NONE)
        return 0;

    sec = actor->subsector->sector;

    for (mo = sec->thinglist; mo; mo = mo->snext)
    {
        if (mo == actor)
            continue;

        if (mo->health <= 0)
            continue;

        if (P_GetFaction(mo->type) != my_faction)
            continue;

        if (P_AproxDistance(mo->x - actor->x, mo->y - actor->y) <= PACK_DETECT_RADIUS)
            count++;
    }

    return count;
}

int P_GetPackAggression(mobj_t* actor)
{
    int base_aggression;
    int ally_bonus;
    int ally_count;

    if (!actor || !actor->info)
        return 50;

    base_aggression = actor->info->aggression;
    if (base_aggression <= 0)
        base_aggression = 50;

    ally_count = P_GetNearbyAllyCount(actor);
    ally_bonus = ally_count * PACK_AGGRESSION_BONUS_PER_ALLY;

    if (ally_bonus > PACK_MAX_BONUS)
        ally_bonus = PACK_MAX_BONUS;

    return base_aggression + ally_bonus;
}

fixed_t P_GetPackRetreatThreshold(mobj_t* actor)
{
    int ally_count;
    int reduction;

    ally_count = P_GetNearbyAllyCount(actor);
    reduction = ally_count * PACK_RETREAT_REDUCTION_PER_ALLY;

    if (reduction > PACK_RETREAT_THRESHOLD_BASE - 32)
        reduction = PACK_RETREAT_THRESHOLD_BASE - 32;

    return (PACK_RETREAT_THRESHOLD_BASE - reduction) * FRACUNIT;
}

boolean P_CanBeLeader(mobjtype_t type)
{
    if (type == MT_DWARF_CAPTAIN ||
        type == MT_DWARF_BOMBARDIER ||
        type == MT_DWARF_WARLORD ||
        type == MT_DWARF_COMMANDER ||
        type == MT_GOBLIN_SHAMAN ||
        type == MT_GOBLIN_TOTEMIST)
        return true;
    return false;
}

boolean P_CanFollow(mobjtype_t type)
{
    if (type == MT_DWARF ||
        type == MT_DWARF_BERSERKER ||
        type == MT_DWARF_ENGINEER ||
        type == MT_DWARF_DEFENDER ||
        type == MT_DWARF_MARKSMAN ||
        type == MT_DWARF_MINER ||
        type == MT_DWARF_ARMORED ||
        type == MT_GOBLIN_SCOUT ||
        type == MT_GOBLIN_SNEAK ||
        type == MT_GOBLIN_ALCHEMIST)
        return true;
    return false;
}

void P_InitMorale(mobj_t* actor)
{
    if (!P_CanFollow(actor->type))
    {
        actor->morale = 100;
        actor->morale_flags = 0;
        actor->leader = NULL;
        return;
    }

    actor->morale = 100;
    actor->morale_flags = MORALE_FLAG_FOLLOWER;
    actor->leader = NULL;
}

void P_UpdateMorale(mobj_t* actor)
{
    if (!(actor->morale_flags & MORALE_FLAG_FOLLOWER))
        return;

    if (actor->morale_flags & MORALE_FLAG_BROKEN)
        return;

    if (actor->leader != NULL)
    {
        if (actor->leader->health <= 0)
        {
            actor->morale = 0;
            actor->morale_flags |= MORALE_FLAG_BROKEN;
            actor->leader = NULL;
        }
        return;
    }

    mobj_t* mo;
    sector_t* sec = actor->subsector->sector;
    thinker_t* th;

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 != (actionf_p1)P_MobjThinker)
            continue;

        mo = (mobj_t*)th;

        if (mo == actor)
            continue;

        if (mo->health <= 0)
            continue;

        if (!P_CanBeLeader(mo->type))
            continue;

        if (P_GetFaction(mo->type) != P_GetFaction(actor->type))
            continue;

        if (P_AproxDistance(mo->x - actor->x, mo->y - actor->y) < 512*FRACUNIT)
        {
            actor->leader = mo;
            actor->morale = 100;
            return;
        }
    }

    if (actor->reactiontime > 0)
    {
        actor->reactiontime--;
    }
    else
    {
        int morale_drop = 5;
        int ally_count = P_GetNearbyAllyCount(actor);
        morale_drop -= ally_count;
        if (morale_drop < 1)
            morale_drop = 1;

        actor->morale -= morale_drop;
        actor->reactiontime = 35 * 5;

        if (actor->morale <= 0)
        {
            actor->morale = 0;
            actor->morale_flags |= MORALE_FLAG_BROKEN;
        }
    }
}

boolean P_MoraleBroken(mobj_t* actor)
{
    return (actor->morale_flags & MORALE_FLAG_BROKEN) != 0;
}

void P_InitLeader(mobj_t* actor)
{
    if (!P_CanBeLeader(actor->type))
    {
        actor->morale = 100;
        actor->morale_flags = 0;
        return;
    }

    actor->morale = 100;
    actor->morale_flags = MORALE_FLAG_LEADER;
}

#define CRIT_AURA_RADIUS (256*FRACUNIT)
#define CRIT_AURA_BONUS 15

int P_GetCritAuraBonus(mobj_t* actor)
{
    thinker_t* th;
    mobj_t* mo;
    sector_t* sec;
    int bonus = 0;

    if (!actor)
        return 0;

    sec = actor->subsector->sector;

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 != (actionf_p1)P_MobjThinker)
            continue;

        mo = (mobj_t*)th;

        if (mo->type != MT_CRIT_AURA)
            continue;

        if (mo->health <= 0)
            continue;

        if (P_AproxDistance(mo->x - actor->x, mo->y - actor->y) <= CRIT_AURA_RADIUS)
        {
            bonus += CRIT_AURA_BONUS;
        }
    }

    return bonus;
}

#define FORMATION_FORM_DISTANCE (64*FRACUNIT)
#define FORMATION_SPACING (48*FRACUNIT)
#define SHIELD_WALL_SPACING (56*FRACUNIT)

static formation_t dwarf_formations[1024];
static mobj_t* formation_leaders[1024];

formation_t P_GetFormation(mobj_t* actor)
{
    if (!actor || actor->type >= 1024)
        return FORMATION_NONE;
    return dwarf_formations[actor->type];
}

void P_SetFormation(mobj_t* actor, formation_t form)
{
    if (!actor || actor->type >= 1024)
        return;
    dwarf_formations[actor->type] = form;
    if (form != FORMATION_NONE && P_CanBeLeader(actor->type))
        formation_leaders[actor->type] = actor;
}

fixed_t P_GetFormationSlot(mobj_t* actor, int slot)
{
    angle_t base_angle;
    fixed_t dist, x, y;

    if (!actor)
        return 0;

    base_angle = actor->angle;

    switch(P_GetFormation(actor))
    {
        case FORMATION_SHIELD_WALL:
            dist = 0;
            x = actor->x + SHIELD_WALL_SPACING * (slot - 2) * cos(base_angle);
            y = actor->y + SHIELD_WALL_SPACING * (slot - 2) * sin(base_angle);
            break;

        case FORMATION_WEDGE:
            dist = slot * FORMATION_SPACING;
            x = actor->x + dist * cos(base_angle);
            y = actor->y + dist * sin(base_angle);
            if (slot > 0)
            {
                x += (slot % 2 == 0 ? -1 : 1) * (slot/2 + 1) * FORMATION_SPACING * cos(base_angle + ANG90);
                y += (slot % 2 == 0 ? -1 : 1) * (slot/2 + 1) * FORMATION_SPACING * sin(base_angle + ANG90);
            }
            break;

        case FORMATION_LINE:
            x = actor->x + SHIELD_WALL_SPACING * (slot - 2) * cos(base_angle + ANG90);
            y = actor->y + SHIELD_WALL_SPACING * (slot - 2) * sin(base_angle + ANG90);
            break;

        case FORMATION_CIRCLE:
            dist = FORMATION_SPACING * 2;
            base_angle += (ANG360 / MAX_FORMATION_MEMBERS) * slot;
            x = actor->x + dist * cos(base_angle);
            y = actor->y + dist * sin(base_angle);
            break;

        default:
            return 0;
    }

    return x | (y << 16);
}

static int P_GetFormationMemberCount(mobj_t* leader)
{
    thinker_t* th;
    mobj_t* mo;
    int count = 0;
    faction_t fac;

    if (!leader || !P_CanBeLeader(leader->type))
        return 0;

    fac = P_GetFaction(leader->type);

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 != (actionf_p1)P_MobjThinker)
            continue;

        mo = (mobj_t*)th;

        if (mo == leader)
            continue;

        if (mo->health <= 0)
            continue;

        if (P_GetFaction(mo->type) != fac)
            continue;

        if (mo->leader != leader)
            continue;

        count++;
    }

    return count;
}

void P_UpdateFormation(mobj_t* actor)
{
    thinker_t* th;
    mobj_t* mo;
    mobj_t* best_leader = NULL;
    int best_count = 0;
    int member_count;
    faction_t fac;

    if (!actor)
        return;

    fac = P_GetFaction(actor->type);
    if (fac != FACTION_DWARF)
        return;

    if (actor->morale_flags & MORALE_FLAG_BROKEN)
        return;

    if (P_CanBeLeader(actor->type))
    {
        member_count = P_GetFormationMemberCount(actor);
        if (member_count >= 3)
        {
            P_SetFormation(actor, FORMATION_SHIELD_WALL);
        }
        else if (member_count >= 2)
        {
            P_SetFormation(actor, FORMATION_WEDGE);
        }
        else
        {
            P_SetFormation(actor, FORMATION_NONE);
        }
        return;
    }

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 != (actionf_p1)P_MobjThinker)
            continue;

        mo = (mobj_t*)th;

        if (!P_CanBeLeader(mo->type))
            continue;

        if (P_GetFaction(mo->type) != fac)
            continue;

        if (mo->health <= 0)
            continue;

        member_count = P_GetFormationMemberCount(mo);
        if (member_count > best_count)
        {
            best_count = member_count;
            best_leader = mo;
        }
    }

    if (best_leader && best_count >= 2)
    {
        actor->leader = best_leader;

        if (best_count >= 3)
            P_SetFormation(actor, FORMATION_SHIELD_WALL);
        else
            P_SetFormation(actor, FORMATION_WEDGE);
    }
}

boolean P_IsInShieldWall(mobj_t* actor)
{
    return P_GetFormation(actor) == FORMATION_SHIELD_WALL;
}

mobj_t* P_GetFormationLeader(mobj_t* actor)
{
    if (!actor)
        return NULL;

    if (P_CanBeLeader(actor->type) && actor->morale_flags & MORALE_FLAG_LEADER)
        return actor;

    return actor->leader;
}

#define CALL_FOR_HELP_RADIUS (768*FRACUNIT)
#define ALERT_SOUND_CHANCE 50

void P_AlertNearbyAllies(mobj_t* actor, mobj_t* source)
{
    thinker_t* th;
    mobj_t* mo;
    faction_t fac;
    fixed_t dist;

    if (!actor)
        return;

    fac = P_GetFaction(actor->type);
    if (fac == FACTION_NONE)
        return;

    if (!source)
        return;

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 != (actionf_p1)P_MobjThinker)
            continue;

        mo = (mobj_t*)th;

        if (mo == actor)
            continue;

        if (mo->health <= 0)
            continue;

        if (P_GetFaction(mo->type) != fac)
            continue;

        if (mo->flags & MF_NOTDMATCH)
            continue;

        dist = P_AproxDistance(mo->x - actor->x, mo->y - actor->y);
        if (dist > CALL_FOR_HELP_RADIUS)
            continue;

        if (mo->target && mo->target != source && mo->target != actor)
            continue;

        if (!mo->target)
        {
            mo->target = source;
            mo->threshold = BASETHRESHOLD;
            if (mo->state == &states[mo->info->spawnstate]
                && mo->info->seestate != S_NULL)
            {
                P_SetMobjState(mo, mo->info->seestate);
            }

            if (fac == FACTION_DWARF && (P_Random() % 100) < ALERT_SOUND_CHANCE)
            {
                S_StartSound(mo, sfx_dwarfsee);
            }
        }
    }
}
