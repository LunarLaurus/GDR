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
//	Enemy thinking, AI.
//	Action Pointer Functions
//	that are associated with states/frames. 
//

#include <stdio.h>
#include <stdlib.h>

#include "m_random.h"
#include "i_system.h"

#include "doomdef.h"
#include "p_local.h"
#include "info.h"      // Goblin Dice Rollaz: for state enums
#include "s_sound.h"
#include "g_game.h"
#include "doomstat.h"
#include "r_state.h"
#include "sounds.h"
#include "g_status.h"

// Goblin Dice Rollaz: Arena lock system
extern int arena_locked;
extern int arena_lock_tag;
extern int EV_DoArenaLock(int tag, boolean close);




typedef enum
{
    DI_EAST,
    DI_NORTHEAST,
    DI_NORTH,
    DI_NORTHWEST,
    DI_WEST,
    DI_SOUTHWEST,
    DI_SOUTH,
    DI_SOUTHEAST,
    DI_NODIR,
    NUMDIRS
    
} dirtype_t;


//
// P_NewChaseDir related LUT.
//
dirtype_t opposite[] =
{
  DI_WEST, DI_SOUTHWEST, DI_SOUTH, DI_SOUTHEAST,
  DI_EAST, DI_NORTHEAST, DI_NORTH, DI_NORTHWEST, DI_NODIR
};

dirtype_t diags[] =
{
    DI_NORTHWEST, DI_NORTHEAST, DI_SOUTHWEST, DI_SOUTHEAST
};


// Goblin Dice Rollaz: Get difficulty-scaled AI parameter
// Returns a modifier based on current skill level
// Higher difficulty = faster enemy decision making
int P_GetDifficultyAIModifier(void)
{
    switch(gameskill)
    {
        case sk_baby:
            return 2;    // Enemies are slower to react
        case sk_easy:
            return 1;   // Slightly slower
        case sk_medium:
            return 0;   // Normal
        case sk_hard:
            return -1;  // Slightly faster
        case sk_nightmare:
            return -2;  // Fastest reactions
        default:
            return 0;
    }
}

// A_Flee - Enemy flees when morale is broken
// Runs away from the player instead of attacking
void A_Flee(mobj_t* actor)
{
    angle_t ang;
    fixed_t dist;

    if (!actor->target)
        return;

    // Get angle AWAY from player
    ang = R_PointToAngle2(actor->y, actor->x,
                          actor->target->y, actor->target->x);

    // Add some randomness to the retreat direction
    if (P_Random() < 128)
        ang += ANG45;
    else
        ang -= ANG45;

    // Set movement direction based on retreat angle
    if (ang >= 0 && ang < ANG45 * 2)
        actor->movedir = DI_EAST;
    else if (ang >= ANG45 * 2 && ang < ANG45 * 4)
        actor->movedir = DI_NORTH;
    else if (ang >= ANG45 * 4 && ang < ANG45 * 6)
        actor->movedir = DI_WEST;
    else
        actor->movedir = DI_SOUTH;

    // Move faster when fleeing
    actor->movecount = 10;

    // Try to move in retreat direction
    if (P_Move(actor))
    {
        // Successfully moved - play flee sound occasionally
        if (actor->info->activesound && P_Random() < 30)
        {
            S_StartSound(actor, actor->info->activesound);
        }
        return;
    }

    // Can't move - try random direction
    P_NewChaseDir(actor);
}

// P_IsRangedEnemy - Check if enemy type is a ranged unit
// Returns true for dwarves that prefer to keep distance
boolean P_IsRangedEnemy(mobj_t* actor)
{
    return actor->type == MT_DWARF_MARKSMAN ||
           actor->type == MT_DWARF_MINER ||
           actor->type == MT_DWARF_ENGINEER ||
           actor->type == MT_DWARF_BOMBARDIER;
}

// P_RetreatFromTarget - Ranged enemies retreat when player gets too close
// Maintains optimal combat distance for ranged attacks
void P_RetreatFromTarget(mobj_t* actor)
{
    angle_t ang;
    fixed_t dist;

    if (!actor->target)
        return;

    // Get angle AWAY from player
    ang = R_PointToAngle2(actor->y, actor->x,
                          actor->target->y, actor->target->x);

    // Add slight randomness to make retreat less predictable
    if (P_Random() < 100)
        ang += (P_Random() - 128) >> 5;
    else if (P_Random() < 50)
        ang += ANG30;
    else
        ang -= ANG30;

    // Set movement direction based on retreat angle
    if (ang >= 0 && ang < ANG45 * 2)
        actor->movedir = DI_EAST;
    else if (ang >= ANG45 * 2 && ang < ANG45 * 4)
        actor->movedir = DI_NORTH;
    else if (ang >= ANG45 * 4 && ang < ANG45 * 6)
        actor->movedir = DI_WEST;
    else
        actor->movedir = DI_SOUTH;

    // Set movement timer for retreat
    actor->movecount = 5 + (P_Random() >> 4);

    // Try to move in retreat direction
    if (P_Move(actor))
        return;

    // Can't move - try a different direction
    P_NewChaseDir(actor);
}




void A_Fall (mobj_t *actor);
void A_Flee (mobj_t *actor);


//
// ENEMY THINKING
// Enemies are allways spawned
// with targetplayer = -1, threshold = 0
// Most monsters are spawned unaware of all players,
// but some can be made preaware
//


//
// Called by P_NoiseAlert.
// Recursively traverse adjacent sectors,
// sound blocking lines cut off traversal.
//

mobj_t*		soundtarget;

void
P_RecursiveSound
( sector_t*	sec,
  int		soundblocks )
{
    int		i;
    line_t*	check;
    sector_t*	other;
	
    // wake up all monsters in this sector
    if (sec->validcount == validcount
	&& sec->soundtraversed <= soundblocks+1)
    {
	return;		// already flooded
    }
    
    sec->validcount = validcount;
    sec->soundtraversed = soundblocks+1;
    sec->soundtarget = soundtarget;
	
    for (i=0 ;i<sec->linecount ; i++)
    {
	check = sec->lines[i];
	if (! (check->flags & ML_TWOSIDED) )
	    continue;
	
	P_LineOpening (check);

	if (openrange <= 0)
	    continue;	// closed door
	
	if ( sides[ check->sidenum[0] ].sector == sec)
	    other = sides[ check->sidenum[1] ] .sector;
	else
	    other = sides[ check->sidenum[0] ].sector;
	
	if (check->flags & ML_SOUNDBLOCK)
	{
	    if (!soundblocks)
		P_RecursiveSound (other, 1);
	}
	else
	    P_RecursiveSound (other, soundblocks);
    }
}



//
// P_NoiseAlert
// If a monster yells at a player,
// it will alert other monsters to the player.
//
void
P_NoiseAlert
( mobj_t*	target,
  mobj_t*	emmiter )
{
    soundtarget = target;
    validcount++;
    P_RecursiveSound (emmiter->subsector->sector, 0);
}




//
// P_CheckMeleeRange
//
boolean P_CheckMeleeRange (mobj_t*	actor)
{
    mobj_t*	pl;
    fixed_t	dist;
    fixed_t range;
	
    if (!actor->target)
	return false;
		
    pl = actor->target;
    dist = P_AproxDistance (pl->x-actor->x, pl->y-actor->y);

    if (gameversion < exe_doom_1_5)
        range = MELEERANGE;
    else
        range = MELEERANGE-20*FRACUNIT+pl->info->radius;

    if (dist >= range)
        return false;

	
    if (! P_CheckSight (actor, actor->target) )
	return false;
							
    return true;		
}

//
// P_CheckMissileRange
//
boolean P_CheckMissileRange (mobj_t* actor)
{
    fixed_t	dist;
	
    if (! P_CheckSight (actor, actor->target) )
	return false;
	
    if ( actor->flags & MF_JUSTHIT )
    {
	// the target just hit the enemy,
	// so fight back!
	actor->flags &= ~MF_JUSTHIT;
	return true;
    }
	
    if (actor->reactiontime)
	return false;	// do not attack yet
		
    // OPTIMIZE: get this from a global checksight
    dist = P_AproxDistance ( actor->x-actor->target->x,
			     actor->y-actor->target->y) - 64*FRACUNIT;
    
    if (!actor->info->meleestate)
	dist -= 128*FRACUNIT;	// no melee attack, so fire more

    dist >>= FRACBITS;

    if (actor->type == MT_VILE)
    {
	if (dist > 14*64)	
	    return false;	// too far away
    }
	

    if (actor->type == MT_UNDEAD)
    {
	if (dist < 196)	
	    return false;	// close for fist attack
	dist >>= 1;
    }
	

    if (actor->type == MT_CYBORG
	|| actor->type == MT_SPIDER
	|| actor->type == MT_SKULL)
    {
	dist >>= 1;
    }
    
    if (dist > 200)
	dist = 200;
		
    if (actor->type == MT_CYBORG && dist > 160)
	dist = 160;
		
    if (P_Random () < dist)
	return false;
		
    return true;
}


//
// P_Move
// Move in the current direction,
// returns false if the move is blocked.
//
fixed_t	xspeed[8] = {FRACUNIT,47000,0,-47000,-FRACUNIT,-47000,0,47000};
fixed_t yspeed[8] = {0,47000,FRACUNIT,47000,0,-47000,-FRACUNIT,-47000};

boolean P_Move (mobj_t*	actor)
{
    fixed_t	tryx;
    fixed_t	tryy;
    
    line_t*	ld;
    
    // warning: 'catch', 'throw', and 'try'
    // are all C++ reserved words
    boolean	try_ok;
    boolean	good;
    
    // Goblin Dice Rollaz: Check for freeze effect
    if (actor->freeze_tics > 0)
    {
        // Frozen enemies don't move - they can only turn
        return false;
    }
		
    if (actor->movedir == DI_NODIR)
	return false;
		
    if ((unsigned)actor->movedir >= 8)
	I_Error ("Weird actor->movedir!");

    // Goblin Dice Rollaz: Apply frozen status effect speed reduction
    // Use g_status framework for configurable slow effect
    if (G_StatusEffectIsActive(actor, st_frozen))
    {
        G_StatusEffectTick(actor);
        int speed_mult = G_StatusEffectGetMoveSpeedMultiplier(actor);
        tryx = actor->x + FixedMul(actor->info->speed, (speed_mult * xspeed[actor->movedir]) / 100);
        tryy = actor->y + FixedMul(actor->info->speed, (speed_mult * yspeed[actor->movedir]) / 100);
    }
    else
    {
        tryx = actor->x + actor->info->speed*xspeed[actor->movedir];
        tryy = actor->y + actor->info->speed*yspeed[actor->movedir];
    }

    try_ok = P_TryMove (actor, tryx, tryy);

    if (!try_ok)
    {
	// open any specials
	if (actor->flags & MF_FLOAT && floatok)
	{
	    // must adjust height
	    if (actor->z < tmfloorz)
		actor->z += FLOATSPEED;
	    else
		actor->z -= FLOATSPEED;

	    actor->flags |= MF_INFLOAT;
	    return true;
	}
		
	if (!numspechit)
	    return false;
			
	actor->movedir = DI_NODIR;
	good = false;
	while (numspechit--)
	{
	    ld = spechit[numspechit];
	    // if the special is not a door
	    // that can be opened,
	    // return false
	    if (P_UseSpecialLine (actor, ld,0))
		good = true;
	}
	return good;
    }
    else
    {
	actor->flags &= ~MF_INFLOAT;
    }
	
	
    if (! (actor->flags & MF_FLOAT) )	
	actor->z = actor->floorz;
    return true; 
}


//
// TryWalk
// Attempts to move actor on
// in its current (ob->moveangle) direction.
// If blocked by either a wall or an actor
// returns FALSE
// If move is either clear or blocked only by a door,
// returns TRUE and sets...
// If a door is in the way,
// an OpenDoor call is made to start it opening.
//
boolean P_TryWalk (mobj_t* actor)
{	
    if (!P_Move (actor))
    {
	return false;
    }

    actor->movecount = P_Random()&15;
    return true;
}




void P_NewChaseDir (mobj_t*	actor)
{
    fixed_t	deltax;
    fixed_t	deltay;
    
    dirtype_t	d[3];
    
    int		tdir;
    dirtype_t	olddir;
    
    dirtype_t	turnaround;
    tactic_t	tactic;

    if (!actor->target)
	I_Error ("P_NewChaseDir: called with no target");

    // Goblin Dice Rollaz: Get faction tactic
    tactic = P_GetTactic(actor->type);
		
    olddir = actor->movedir;
    turnaround=opposite[olddir];

    deltax = actor->target->x - actor->x;
    deltay = actor->target->y - actor->y;

    // Goblin Dice Rollaz: Flanking logic for goblins
    // Instead of going directly at player, try to go to the side
    if (tactic == TACTIC_FLANK)
    {
        // Calculate perpendicular directions (flanking paths)
        // If player is to the east, try north or south first (side paths)
        if (deltax > 10*FRACUNIT && abs(deltay) < abs(deltax))
        {
            // Player is east, try flanking from north/south
            if (deltay > 10*FRACUNIT)
            {
                d[1] = DI_NORTH;
                d[2] = DI_SOUTH;
            }
            else if (deltay < -10*FRACUNIT)
            {
                d[1] = DI_SOUTH;
                d[2] = DI_NORTH;
            }
            else
            {
                // Equal - pick random side
                d[1] = (P_Random() & 1) ? DI_NORTH : DI_SOUTH;
                d[2] = (P_Random() & 1) ? DI_SOUTH : DI_NORTH;
            }
            // Still include forward as third option
            if (deltay > 10*FRACUNIT)
                d[0] = DI_NORTHEAST;
            else if (deltay < -10*FRACUNIT)
                d[0] = DI_SOUTHEAST;
            else
                d[0] = DI_EAST;
        }
        else if (deltax < -10*FRACUNIT && abs(deltay) < abs(deltax))
        {
            // Player is west, try flanking from north/south
            if (deltay > 10*FRACUNIT)
            {
                d[1] = DI_NORTH;
                d[2] = DI_SOUTH;
            }
            else if (deltay < -10*FRACUNIT)
            {
                d[1] = DI_SOUTH;
                d[2] = DI_NORTH;
            }
            else
            {
                d[1] = (P_Random() & 1) ? DI_NORTH : DI_SOUTH;
                d[2] = (P_Random() & 1) ? DI_SOUTH : DI_NORTH;
            }
            if (deltay > 10*FRACUNIT)
                d[0] = DI_NORTHWEST;
            else if (deltay < -10*FRACUNIT)
                d[0] = DI_SOUTHWEST;
            else
                d[0] = DI_WEST;
        }
        else
        {
            // Direct the directions normally for diagonal/near cases
            if (deltax>10*FRACUNIT)
                d[1]= DI_EAST;
            else if (deltax<-10*FRACUNIT)
                d[1]= DI_WEST;
            else
                d[1]=DI_NODIR;

            if (deltay<-10*FRACUNIT)
                d[2]= DI_SOUTH;
            else if (deltay>10*FRACUNIT)
                d[2]= DI_NORTH;
            else
                d[2]=DI_NODIR;
        }
    }
    else
    {
        // Default movement direction calculation
        if (deltax>10*FRACUNIT)
    	    d[1]= DI_EAST;
        else if (deltax<-10*FRACUNIT)
    	    d[1]= DI_WEST;
        else
    	    d[1]=DI_NODIR;

        if (deltay<-10*FRACUNIT)
    	    d[2]= DI_SOUTH;
        else if (deltay>10*FRACUNIT)
    	    d[2]= DI_NORTH;
        else
    	    d[2]=DI_NODIR;
    }

    // try direct route
    if (d[1] != DI_NODIR
	&& d[2] != DI_NODIR)
    {
	actor->movedir = diags[((deltay<0)<<1)+(deltax>0)];
	if (actor->movedir != (int) turnaround && P_TryWalk(actor))
	    return;
    }

    // Goblin Dice Rollaz: Dwarf hold formation logic
    // Dwarves are more conservative - only advance if target is far
    if (tactic == TACTIC_HOLD)
    {
        fixed_t dist_to_target = P_AproxDistance(deltax, deltay);
        // Only advance if target is more than 16*64 units away (about 2x normal engagement range)
        if (dist_to_target < 1024*FRACUNIT && olddir != DI_NODIR)
        {
            // Hold position - try to keep current direction or stand ground
            actor->movedir = olddir;
            if (P_TryWalk(actor))
                return;
            // If can't move forward, try to strafe but prefer not moving toward target
            for (tdir = DI_NORTH; tdir <= DI_SOUTHEAST; tdir++)
            {
                if (tdir != (int)turnaround && tdir != (int)olddir)
                {
                    actor->movedir = tdir;
                    if (P_TryWalk(actor))
                        return;
                }
            }
            // If all else fails, just stay put (don't advance)
            actor->movedir = DI_NODIR;
            return;
        }
    }

    // try other directions
    if (P_Random() > 200
	||  abs(deltay)>abs(deltax))
    {
	tdir=d[1];
	d[1]=d[2];
	d[2]=tdir;
    }

    if (d[1]==turnaround)
	d[1]=DI_NODIR;
    if (d[2]==turnaround)
	d[2]=DI_NODIR;
	
    if (d[1]!=DI_NODIR)
    {
	actor->movedir = d[1];
	if (P_TryWalk(actor))
	{
	    // either moved forward or attacked
	    return;
	}
    }

    if (d[2]!=DI_NODIR)
    {
	actor->movedir =d[2];

	if (P_TryWalk(actor))
	    return;
    }

    // there is no direct path to the player,
    // so pick another direction.
    if (olddir!=DI_NODIR)
    {
	actor->movedir =olddir;

	if (P_TryWalk(actor))
	    return;
    }

    // randomly determine direction of search
    if (P_Random()&1) 	
    {
	for ( tdir=DI_EAST;
	      tdir<=DI_SOUTHEAST;
	      tdir++ )
	{
	    if (tdir != (int) turnaround)
	    {
		actor->movedir =tdir;
		
		if ( P_TryWalk(actor) )
		    return;
	    }
	}
    }
    else
    {
	for ( tdir=DI_SOUTHEAST;
	      tdir != (DI_EAST-1);
	      tdir-- )
	{
	    if (tdir != (int) turnaround)
	    {
		actor->movedir = tdir;
		
		if ( P_TryWalk(actor) )
		    return;
	    }
	}
    }

    if (turnaround !=  DI_NODIR)
    {
	actor->movedir =turnaround;
	if ( P_TryWalk(actor) )
	    return;
    }

    actor->movedir = DI_NODIR;	// can not move
}



//
// P_LookForPlayers
// If allaround is false, only look 180 degrees in front.
// Returns true if a player is targeted.
//
boolean
P_LookForPlayers
( mobj_t*	actor,
  boolean	allaround )
{
    int		c;
    int		stop;
    player_t*	player;
    angle_t	an;
    fixed_t	dist;

    c = 0;
    stop = (actor->lastlook-1)&3;
	
    for ( ; ; actor->lastlook = (actor->lastlook+1)&3 )
    {
	if (!playeringame[actor->lastlook])
	    continue;
			
	if (c++ == 2
	    || actor->lastlook == stop)
	{
	    // done looking
	    return false;	
	}
	
	player = &players[actor->lastlook];

	if (player->health <= 0)
	    continue;		// dead

	if (!P_CheckSight (actor, player->mo))
	    continue;		// out of sight
			
	if (!allaround)
	{
	    an = R_PointToAngle2 (actor->x,
				  actor->y, 
				  player->mo->x,
				  player->mo->y)
		- actor->angle;
	    
	    if (an > ANG90 && an < ANG270)
	    {
		dist = P_AproxDistance (player->mo->x - actor->x,
					player->mo->y - actor->y);
		// if real close, react anyway
		if (dist > MELEERANGE)
		    continue;	// behind back
	    }
	}
		
	actor->target = player->mo;
	return true;
    }

    return false;
}


//
// A_KeenDie
// DOOM II special, map 32.
// Uses special tag 666.
//
void A_KeenDie (mobj_t* mo)
{
    thinker_t*	th;
    mobj_t*	mo2;
    line_t	junk;

    A_Fall (mo);
    
    // scan the remaining thinkers
    // to see if all Keens are dead
    for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    {
	if (th->function.acp1 != (actionf_p1)P_MobjThinker)
	    continue;

	mo2 = (mobj_t *)th;
	if (mo2 != mo
	    && mo2->type == mo->type
	    && mo2->health > 0)
	{
	    // other Keen not dead
	    return;		
	}
    }

    junk.tag = 666;
    EV_DoDoor(&junk, vld_open);
}


//
// ACTION ROUTINES
//

//
// A_Look
// Stay in state until a player is sighted.
//
void A_Look (mobj_t* actor)
{
    mobj_t*	targ;
	
    actor->threshold = 0;	// any shot will wake up
    targ = actor->subsector->sector->soundtarget;

    if (targ
	&& (targ->flags & MF_SHOOTABLE) )
    {
	actor->target = targ;

	if ( actor->flags & MF_AMBUSH )
	{
	    if (P_CheckSight (actor, actor->target))
		goto seeyou;
	}
	else
	    goto seeyou;
    }
	
	
    if (!P_LookForPlayers (actor, false) )
	return;
		
    // go into chase state
  seeyou:
    if (actor->info->seesound)
    {
	int		sound;
		
	switch (actor->info->seesound)
	{
	  case sfx_posit1:
	  case sfx_posit2:
	  case sfx_posit3:
	    sound = sfx_posit1+P_Random()%3;
	    break;

	  case sfx_bgsit1:
	  case sfx_bgsit2:
	    sound = sfx_bgsit1+P_Random()%2;
	    break;

	  default:
	    sound = actor->info->seesound;
	    break;
	}

	if (actor->type==MT_SPIDER
	    || actor->type == MT_CYBORG)
	{
	    // full volume
	    S_StartSound (NULL, sound);
	}
	else
	    S_StartSound (actor, sound);
    }

    // Goblin Dice Rollaz: Trigger boss music when boss first sees player
    // Only trigger if not already playing boss music and not in melee range
    if (actor->type == MT_GOBLIN_KING || actor->type == MT_DWARVEN_WAR_MACHINE)
    {
        // Lock arena when boss first sees player
        if (!arena_locked && arena_lock_tag > 0)
        {
            arena_locked = 1;
            EV_DoArenaLock(arena_lock_tag, true);
        }
        
        if (!P_CheckMeleeRange(actor))
        {
            if (actor->type == MT_GOBLIN_KING)
                S_ChangeMusic(mus_goblin_boss, true);
            else if (actor->type == MT_DWARVEN_WAR_MACHINE)
                S_ChangeMusic(mus_dwarf_boss, true);
        }
    }

    P_SetMobjState (actor, actor->info->seestate);
}


//
// A_Chase
// Actor has a melee attack,
// so it tries to close as fast as possible
//
void A_Chase (mobj_t*	actor)
{
    int		delta;

    if (actor->reactiontime)
	actor->reactiontime--;
    
    // Goblin Dice Rollaz: Handle morale system
    P_UpdateMorale(actor);
    
    // If morale is broken, flee instead of chase!
    if (P_MoraleBroken(actor))
    {
        // Run away from player
        A_Flee(actor);
        return;
    }
    
    // Goblin Dice Rollaz: Handle freeze effect
    if (actor->freeze_tics > 0)
    {
        actor->freeze_tics--;
        // Still let them turn but don't move as fast
        if (actor->freeze_tics <= 0)
            actor->movecount = 0;  // Reset movement when unfrozen
    }

    // Goblin Dice Rollaz: Stunned enemies cannot move
    if (!G_StatusEffectCanAttack(actor))
    {
        // Enemies can only turn in place when stunned
        // Skip movement and attack logic
        if (actor->target)
        {
            A_FaceTarget(actor);
        }
        return;
    }

    // Goblin Dice Rollaz: Tactical retreat for ranged enemies
    // Ranged units retreat when player gets too close but not in melee range
    if (P_IsRangedEnemy(actor) && actor->target)
    {
        fixed_t dist = P_AproxDistance(actor->x - actor->target->x,
                                       actor->y - actor->target->y);

        // Goblin Dice Rollaz: Pack behavior affects retreat threshold
        // More allies = more confident = stay closer to player before retreating
        fixed_t retreat_threshold = P_GetPackRetreatThreshold(actor);

        // Retreat if player is within missile range but outside melee range
        // This gives ranged enemies time to fire and reposition
        if (dist < MISSILERANGE && dist > MELEERANGE)
        {
            // Face the player to fire while retreating
            A_FaceTarget(actor);

            // Only retreat occasionally to avoid being too kitable
            // But pack members retreat less often (more aggressive in groups)
            int retreat_chance = 80;
            int ally_count = P_GetNearbyAllyCount(actor);
            retreat_chance -= ally_count * 5;  // Reduce retreat chance by 5% per ally

            if (P_Random() < retreat_chance)
            {
                P_RetreatFromTarget(actor);
                return;
            }
        }
    }

    // Goblin Dice Rollaz: Pack bloodlust - fight harder when allies are in danger
    // If pack member is being attacked, become more aggressive
    if (actor->target && P_IsAllyInDanger(actor))
    {
        // Override retreat - rush to aid ally!
        // Reduce reaction time to attack faster
        if (actor->reactiontime > 2)
            actor->reactiontime = 2;
    }


    // modify target threshold
    if  (actor->threshold)
    {
        if (gameversion > exe_doom_1_2 && 
            (!actor->target || actor->target->health <= 0))
	{
	    actor->threshold = 0;
	}
	else
	    actor->threshold--;
    }
    
    // turn towards movement direction if not there yet
    if (actor->movedir < 8)
    {
        actor->angle &= (7u << 29);
	delta = actor->angle - (actor->movedir << 29);
	
	if (delta > 0)
	    actor->angle -= ANG90/2;
	else if (delta < 0)
	    actor->angle += ANG90/2;
    }

    if (!actor->target
	|| !(actor->target->flags&MF_SHOOTABLE))
    {
	// look for a new target
	if (P_LookForPlayers(actor,true))
	    return; 	// got a new target
	
	P_SetMobjState (actor, actor->info->spawnstate);
	return;
    }
    
    // do not attack twice in a row
    if (actor->flags & MF_JUSTATTACKED)
    {
	actor->flags &= ~MF_JUSTATTACKED;
	if (gameskill != sk_nightmare && !fastparm)
	    P_NewChaseDir (actor);
	return;
    }
    
    // check for melee attack
    if (actor->info->meleestate
	&& P_CheckMeleeRange (actor))
    {
	if (actor->info->attacksound)
	    S_StartSound (actor, actor->info->attacksound);

	P_SetMobjState (actor, actor->info->meleestate);
	return;
    }
    
    // check for missile attack
    if (actor->info->missilestate)
    {
	if (gameskill < sk_nightmare
	    && !fastparm && actor->movecount)
	{
	    goto nomissile;
	}
	
	if (!P_CheckMissileRange (actor))
	    goto nomissile;
	
	P_SetMobjState (actor, actor->info->missilestate);
	actor->flags |= MF_JUSTATTACKED;
	return;
    }

    // ?
  nomissile:
    // possibly choose another target
    if (netgame
	&& !actor->threshold
	&& !P_CheckSight (actor, actor->target) )
    {
	if (P_LookForPlayers(actor,true))
	    return;	// got a new target
    }
    
    // Goblin Dice Rollaz: Difficulty-scaled AI movement
    // Higher difficulty = enemies change direction more frequently
    {
        int ai_mod = P_GetDifficultyAIModifier();
        if (actor->movecount + ai_mod < 0)
            actor->movecount = 0;
        else
            actor->movecount += ai_mod;
    }
    
    // chase towards player
    if (--actor->movecount<0
	|| !P_Move (actor))
    {
	P_NewChaseDir (actor);
    }
    
    // make active sound
    if (actor->info->activesound
	&& P_Random () < 3)
    {
	S_StartSound (actor, actor->info->activesound);
    }
}


//
// A_FaceTarget
//
void A_FaceTarget (mobj_t* actor)
{	
    if (!actor->target)
	return;
    
    actor->flags &= ~MF_AMBUSH;
	
    actor->angle = R_PointToAngle2 (actor->x,
				    actor->y,
				    actor->target->x,
				    actor->target->y);
    
    if (actor->target->flags & MF_SHADOW)
	actor->angle += P_SubRandom() << 21;
}


//
// A_PosAttack
//
void A_PosAttack (mobj_t* actor)
{
    int		angle;
    int		damage;
    int		slope;
	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);
    angle = actor->angle;
    slope = P_AimLineAttack (actor, angle, MISSILERANGE);

    S_StartSound (actor, sfx_pistol);
    angle += P_SubRandom() << 20;
    damage = ((P_Random()%5)+1)*3;
    P_LineAttack (actor, angle, MISSILERANGE, slope, damage);
}

void A_SPosAttack (mobj_t* actor)
{
    int		i;
    int		angle;
    int		bangle;
    int		damage;
    int		slope;
    mobj_t* mo;
    
    if (!actor->target)
	return;

    if (actor->type == MT_DWARF_MARKSMAN)
    {
        A_FaceTarget (actor);
        S_StartSound (actor, sfx_shotgn);
        mo = P_SpawnMissile (actor, actor->target, MT_MARKSMAN_BOLT);
        if (mo)
        {
            mo->special1 = 0;
        }
        return;
    }

    if (actor->type == MT_DWARF_MINER)
    {
        A_FaceTarget (actor);
        S_StartSound (actor, sfx_tink);
        mo = P_SpawnMissile (actor, actor->target, MT_MINER_PICKAXE);
        if (mo)
        {
            mo->special1 = 0;
        }
        return;
    }

    if (actor->type == MT_DWARF_ENGINEER)
    {
        mobj_t* mo;
        
        actor->special1++;
        
        if (actor->special1 >= 5 && P_Random() < 64)
        {
            A_EngineerDeployTurret(actor);
            actor->special1 = 0;
            return;
        }
        
        A_FaceTarget (actor);
        S_StartSound (actor, sfx_rlaunc);
        mo = P_SpawnMissile (actor, actor->target, MT_DWARF_BOMB);
        if (mo)
        {
            mo->special1 = 0;
        }
        return;
    }

    if (actor->type == MT_DWARF_BOMBARDIER)
    {
        A_FaceTarget (actor);
        S_StartSound (actor, sfx_rlaunc);
        mo = P_SpawnMissile (actor, actor->target, MT_DWARF_BOMB);
        if (mo)
        {
            mo->special1 = 0;
            mo->damage = 50;
        }
        return;
    }

    if (actor->type == MT_GOBLIN_ALCHEMIST)
    {
        A_FaceTarget (actor);
        S_StartSound (actor, sfx_pistol);
        mo = P_SpawnMissile (actor, actor->target, MT_ALCHEMIST_POTION);
        if (mo)
        {
            mo->special1 = 0;
        }
        return;
    }

    S_StartSound (actor, sfx_shotgn);
    A_FaceTarget (actor);
    bangle = actor->angle;
    slope = P_AimLineAttack (actor, bangle, MISSILERANGE);

    for (i=0 ; i<3 ; i++)
    {
	angle = bangle + (P_SubRandom() << 20);
	damage = ((P_Random()%5)+1)*3;
	P_LineAttack (actor, angle, MISSILERANGE, slope, damage);
    }
}

void A_CPosAttack (mobj_t* actor)
{
    int		angle;
    int		bangle;
    int		damage;
    int		slope;
	
    if (!actor->target)
	return;

    S_StartSound (actor, sfx_shotgn);
    A_FaceTarget (actor);
    bangle = actor->angle;
    slope = P_AimLineAttack (actor, bangle, MISSILERANGE);

    angle = bangle + (P_SubRandom() << 20);
    damage = ((P_Random()%5)+1)*3;
    P_LineAttack (actor, angle, MISSILERANGE, slope, damage);
}

void A_TotemistDeploy(mobj_t* actor)
{
    mobj_t* mo;
    int numTotems;
    
    if (!actor->target)
        return;
    
    A_FaceTarget(actor);
    
    numTotems = P_Random() % 2;
    
    if (numTotems == 0)
    {
        mo = P_SpawnMobj(actor->x + 24*FRACUNIT, actor->y, actor->z - 20*FRACUNIT, MT_BUFF_TOTEM);
        if (mo)
        {
            mo->angle = actor->angle;
            mo->flags |= MF_TELESTICK;
            S_StartSound(actor, sfx_itmbk);
        }
    }
    else
    {
        mo = P_SpawnMobj(actor->x - 24*FRACUNIT, actor->y, actor->z - 20*FRACUNIT, MT_DEBUFF_TOTEM);
        if (mo)
        {
            mo->angle = actor->angle;
            mo->flags |= MF_TELESTICK;
            S_StartSound(actor, sfx_itmbk);
        }
    }
    
    if (P_Random() < 128)
    {
        mo = P_SpawnMobj(actor->x, actor->y + 24*FRACUNIT, actor->z - 20*FRACUNIT, MT_BUFF_TOTEM);
        if (mo)
        {
            mo->angle = actor->angle;
            mo->flags |= MF_TELESTICK;
        }
    }
}

void A_EngineerDeployTurret(mobj_t* actor)
{
    mobj_t* mo;
    fixed_t spawnX, spawnY;
    
    if (!actor->target)
        return;
    
    A_FaceTarget(actor);
    
    spawnX = actor->x + 48 * FRACUNIT;
    spawnY = actor->y;
    
    mo = P_SpawnMobj(spawnX, spawnY, ONFLOORZ, MT_DWARF_TURRET);
    if (mo)
    {
        mo->angle = actor->angle;
        mo->flags |= MF_TELESTICK;
        mo->special1 = 0;
        mo->special2 = 0;
        S_StartSound(actor, sfx_itmbk);
    }
}

void A_TurretAttack(mobj_t* actor)
{
    mobj_t* mo;
    
    if (!actor->target)
        return;
    
    if (!P_CheckSight(actor, actor->target))
        return;
    
    A_FaceTarget(actor);
    S_StartSound(actor, sfx_rlaunc);
    
    mo = P_SpawnMissile(actor, actor->target, MT_DWARF_BOMB);
    if (mo)
    {
        mo->special1 = 0;
        mo->damage = 30;
    }
}

void A_TurretThink(mobj_t* actor)
{
    player_t* player;
    
    if (!actor->target)
    {
        for (player = players; player < &players[MAXPLAYERS]; player++)
        {
            if (player->mo && player->mo->health > 0 && P_CheckSight(actor, player->mo))
            {
                actor->target = player->mo;
                break;
            }
        }
    }
    
    if (actor->target && actor->target->health <= 0)
    {
        actor->target = NULL;
    }
    
    actor->special2++;
    if (actor->special2 >= TICRATE)
    {
        actor->special2 = 0;
        if (actor->special1 < 3)
        {
            A_TurretAttack(actor);
        }
    }
}

void A_CPosRefire (mobj_t* actor)
{	
    // keep firing unless target got out of sight
    A_FaceTarget (actor);

    if (P_Random () < 40)
	return;

    if (!actor->target
	|| actor->target->health <= 0
	|| !P_CheckSight (actor, actor->target) )
    {
	P_SetMobjState (actor, actor->info->seestate);
    }
}


void A_SpidRefire (mobj_t* actor)
{	
    // keep firing unless target got out of sight
    A_FaceTarget (actor);

    if (P_Random () < 10)
	return;

    if (!actor->target
	|| actor->target->health <= 0
	|| !P_CheckSight (actor, actor->target) )
    {
	P_SetMobjState (actor, actor->info->seestate);
    }
}

void A_BspiAttack (mobj_t *actor)
{	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);

    // launch a missile
    P_SpawnMissile (actor, actor->target, MT_ARACHPLAZ);
}


//
// A_TroopAttack
//
void A_TroopAttack (mobj_t* actor)
{
    int		damage;
	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);
    if (P_CheckMeleeRange (actor))
    {
	S_StartSound (actor, sfx_claw);
	damage = (P_Random()%8+1)*3;
	P_DamageMobj (actor->target, actor, actor, damage);
	return;
    }

    if (actor->type == MT_GOBLIN_TOTEMIST)
    {
        mobj_t* mo;
        
        if (P_Random() < 128)
        {
            mo = P_SpawnMobj(actor->x + 24*FRACUNIT, actor->y, actor->z - 20*FRACUNIT, MT_BUFF_TOTEM);
            if (mo)
            {
                mo->flags |= MF_TELESTICK;
                S_StartSound(actor, sfx_itmbk);
            }
        }
        else
        {
            mo = P_SpawnMobj(actor->x - 24*FRACUNIT, actor->y, actor->z - 20*FRACUNIT, MT_DEBUFF_TOTEM);
            if (mo)
            {
                mo->flags |= MF_TELESTICK;
                S_StartSound(actor, sfx_itmbk);
            }
        }
        return;
    }

    
    // launch a missile
    P_SpawnMissile (actor, actor->target, MT_TROOPSHOT);
}


void A_SargAttack (mobj_t* actor)
{
    int		damage;

    if (!actor->target)
	return;
		
    A_FaceTarget (actor);

    if (gameversion >= exe_doom_1_5)
    {
        if (!P_CheckMeleeRange (actor))
            return;
    }

    damage = ((P_Random()%10)+1)*4;

    if (gameversion <= exe_doom_1_2)
        P_LineAttack(actor, actor->angle, MELEERANGE, 0, damage);
    else
        P_DamageMobj (actor->target, actor, actor, damage);
}

void A_HeadAttack (mobj_t* actor)
{
    int		damage;
	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);
    if (P_CheckMeleeRange (actor))
    {
	damage = (P_Random()%6+1)*10;
	P_DamageMobj (actor->target, actor, actor, damage);
	return;
    }
    
    // launch a missile
    P_SpawnMissile (actor, actor->target, MT_HEADSHOT);
}

void A_MarksmanAttack (mobj_t* actor)
{
    mobj_t* mo;
    
    if (!actor->target)
        return;
        
    A_FaceTarget (actor);
    S_StartSound (actor, sfx_shotgn);
    mo = P_SpawnMissile (actor, actor->target, MT_MARKSMAN_BOLT);
    if (mo)
    {
        mo->special1 = 0;
    }
}

void A_CaptainBuff (mobj_t* actor)
{
    thinker_t* th;
    mobj_t* mo;
    fixed_t dist;
    fixed_t buff_radius = 256*FRACUNIT;

    if (!actor->target)
        return;

    A_FaceTarget (actor);
    S_StartSound (actor, sfx_posact);

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 != (actionf_p1)P_MobjThinker)
            continue;

        mo = (mobj_t*)th;

        if (!(mo->flags & MF_SHOOTABLE))
            continue;

        if (mo->flags & MF_CORPSE)
            continue;

        if (!mo->target)
            continue;

        dist = P_AproxDistance(mo->x - actor->x, mo->y - actor->y);

        if (dist > buff_radius)
            continue;

        if (mo->type == MT_DWARF ||
            mo->type == MT_DWARF_BERSERKER ||
            mo->type == MT_DWARF_ENGINEER ||
            mo->type == MT_DWARF_DEFENDER ||
            mo->type == MT_DWARF_MARKSMAN ||
            mo->type == MT_DWARF_MINER)
        {
            if (mo->health > 0)
            {
                mo->special2 = 150;
                if (mo->info->speed < 12*FRACUNIT)
                {
                    mo->speed += 2*FRACUNIT;
                }
            }
        }
    }
}

void A_CyberAttack (mobj_t* actor)
{	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);
    P_SpawnMissile (actor, actor->target, MT_ROCKET);
}


//
// A_GoblinKingAttack - Goblin Dice Rollaz
// Goblin King throws dice projectiles at the player
//
void A_GoblinKingAttack (mobj_t* actor)
{	
    int damage;
	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);

    // Phase 2 (enraged) has faster attacks
    if (actor->state == &states[S_GKNG_P2_ATK2] || 
        actor->state == &states[S_GKNG_P2_ATK4])
    {
        // Enraged: throw multiple dice (d6 + d8 combo)
        P_SpawnMissile (actor, actor->target, MT_D6PROJECTILE);
        damage = (P_Random()%6+1) + (P_Random()%8+1);
        P_DamageMobj (actor->target, actor, actor, damage);
    }
    else
    {
        // Phase 1: single heavy die throw (d12)
        P_SpawnMissile (actor, actor->target, MT_D12PROJECTILE);
        damage = (P_Random()%12+1) * 3; // Higher damage
        P_DamageMobj (actor->target, actor, actor, damage);
    }
}


//
// A_DWMAttack - Goblin Dice Rollaz
// Dwarven War Machine fires heavy cannon/dice blasts
//
void A_DWMAttack (mobj_t* actor)
{	
    int damage;
    
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);

    // Phase 2 (turbo) has rapid fire
    if (actor->state == &states[S_DWM_P2_ATK2] || 
        actor->state == &states[S_DWM_P2_ATK4])
    {
        // Turbo mode: rapid fire percentile dice
        P_SpawnMissile (actor, actor->target, MT_PERCENTILEPROJECTILE);
        damage = (P_Random()%100+1);
        P_DamageMobj (actor->target, actor, actor, damage);
    }
    else
    {
        // Phase 1: heavy d20 cannon shot
        P_SpawnMissile (actor, actor->target, MT_D20PROJECTILE);
        damage = (P_Random()%20+1) * 5; // Very high damage
        P_DamageMobj (actor->target, actor, actor, damage);
    }
}


void A_BruisAttack (mobj_t* actor)
{
    int		damage;
	
    if (!actor->target)
	return;
		
    if (P_CheckMeleeRange (actor))
    {
	S_StartSound (actor, sfx_claw);
	damage = (P_Random()%8+1)*10;
	P_DamageMobj (actor->target, actor, actor, damage);
	return;
    }
    
    // launch a missile
    P_SpawnMissile (actor, actor->target, MT_BRUISERSHOT);
}


//
// A_SkelMissile
//
void A_SkelMissile (mobj_t* actor)
{	
    mobj_t*	mo;
	
    if (!actor->target)
	return;
		
    A_FaceTarget (actor);
    actor->z += 16*FRACUNIT;	// so missile spawns higher
    mo = P_SpawnMissile (actor, actor->target, MT_TRACER);
    actor->z -= 16*FRACUNIT;	// back to normal

    mo->x += mo->momx;
    mo->y += mo->momy;
    mo->tracer = actor->target;
}

int	TRACEANGLE = 0xc000000;

void A_Tracer (mobj_t* actor)
{
    angle_t	exact;
    fixed_t	dist;
    fixed_t	slope;
    mobj_t*	dest;
    mobj_t*	th;
		
    if (gametic & 3)
	return;
    
    // spawn a puff of smoke behind the rocket		
    P_SpawnPuff (actor->x, actor->y, actor->z);
	
    th = P_SpawnMobj (actor->x-actor->momx,
		      actor->y-actor->momy,
		      actor->z, MT_SMOKE);
    
    th->momz = FRACUNIT;
    th->tics -= P_Random()&3;
    if (th->tics < 1)
	th->tics = 1;
    
    // adjust direction
    dest = actor->tracer;
	
    if (!dest || dest->health <= 0)
	return;
    
    // change angle	
    exact = R_PointToAngle2 (actor->x,
			     actor->y,
			     dest->x,
			     dest->y);

    if (exact != actor->angle)
    {
	if (exact - actor->angle > 0x80000000)
	{
	    actor->angle -= TRACEANGLE;
	    if (exact - actor->angle < 0x80000000)
		actor->angle = exact;
	}
	else
	{
	    actor->angle += TRACEANGLE;
	    if (exact - actor->angle > 0x80000000)
		actor->angle = exact;
	}
    }
    
    // Goblin Dice Rollaz: Apply frozen status effect speed reduction
    exact = actor->angle>>ANGLETOFINESHIFT;
    if (G_StatusEffectIsActive(actor, st_frozen))
    {
        int speed_mult = G_StatusEffectGetMoveSpeedMultiplier(actor);
        actor->momx = FixedMul (FixedMul(actor->info->speed, speed_mult / 100), finecosine[exact]);
        actor->momy = FixedMul (FixedMul(actor->info->speed, speed_mult / 100), finesine[exact]);
    }
    else
    {
        actor->momx = FixedMul (actor->info->speed, finecosine[exact]);
        actor->momy = FixedMul (actor->info->speed, finesine[exact]);
    }
    
    // change slope
    dist = P_AproxDistance (dest->x - actor->x,
			    dest->y - actor->y);
    
    dist = dist / actor->info->speed;

    if (dist < 1)
	dist = 1;
    slope = (dest->z+40*FRACUNIT - actor->z) / dist;

    if (slope < actor->momz)
	actor->momz -= FRACUNIT/8;
    else
	actor->momz += FRACUNIT/8;
}


void A_SkelWhoosh (mobj_t*	actor)
{
    if (!actor->target)
	return;
    A_FaceTarget (actor);
    S_StartSound (actor,sfx_skeswg);
}

void A_SkelFist (mobj_t*	actor)
{
    int		damage;

    if (!actor->target)
	return;
		
    A_FaceTarget (actor);
	
    if (P_CheckMeleeRange (actor))
    {
	damage = ((P_Random()%10)+1)*6;
	S_StartSound (actor, sfx_skepch);
	P_DamageMobj (actor->target, actor, actor, damage);
    }
}



//
// PIT_VileCheck
// Detect a corpse that could be raised.
//
mobj_t*		corpsehit;
mobj_t*		vileobj;
fixed_t		viletryx;
fixed_t		viletryy;

boolean PIT_VileCheck (mobj_t*	thing)
{
    int		maxdist;
    boolean	check;
	
    if (!(thing->flags & MF_CORPSE) )
	return true;	// not a monster
    
    if (thing->tics != -1)
	return true;	// not lying still yet
    
    if (thing->info->raisestate == S_NULL)
	return true;	// monster doesn't have a raise state
    
    maxdist = thing->info->radius + mobjinfo[MT_VILE].radius;
	
    if ( abs(thing->x - viletryx) > maxdist
	 || abs(thing->y - viletryy) > maxdist )
	return true;		// not actually touching
		
    corpsehit = thing;
    corpsehit->momx = corpsehit->momy = 0;
    corpsehit->height <<= 2;
    check = P_CheckPosition (corpsehit, corpsehit->x, corpsehit->y);
    corpsehit->height >>= 2;

    if (!check)
	return true;		// doesn't fit here
		
    return false;		// got one, so stop checking
}



//
// A_VileChase
// Check for ressurecting a body
//
void A_VileChase (mobj_t* actor)
{
    int			xl;
    int			xh;
    int			yl;
    int			yh;
    
    int			bx;
    int			by;

    mobjinfo_t*		info;
    mobj_t*		temp;
	
    if (actor->movedir != DI_NODIR)
    {
	// check for corpses to raise
	viletryx =
	    actor->x + actor->info->speed*xspeed[actor->movedir];
	viletryy =
	    actor->y + actor->info->speed*yspeed[actor->movedir];

	xl = (viletryx - bmaporgx - MAXRADIUS*2)>>MAPBLOCKSHIFT;
	xh = (viletryx - bmaporgx + MAXRADIUS*2)>>MAPBLOCKSHIFT;
	yl = (viletryy - bmaporgy - MAXRADIUS*2)>>MAPBLOCKSHIFT;
	yh = (viletryy - bmaporgy + MAXRADIUS*2)>>MAPBLOCKSHIFT;
	
	vileobj = actor;
	for (bx=xl ; bx<=xh ; bx++)
	{
	    for (by=yl ; by<=yh ; by++)
	    {
		// Call PIT_VileCheck to check
		// whether object is a corpse
		// that canbe raised.
		if (!P_BlockThingsIterator(bx,by,PIT_VileCheck))
		{
		    // got one!
		    temp = actor->target;
		    actor->target = corpsehit;
		    A_FaceTarget (actor);
		    actor->target = temp;
					
		    P_SetMobjState (actor, S_VILE_HEAL1);
		    S_StartSound (corpsehit, sfx_slop);
		    info = corpsehit->info;
		    
		    P_SetMobjState (corpsehit,info->raisestate);
		    corpsehit->height <<= 2;
		    corpsehit->flags = info->flags;
		    corpsehit->health = info->spawnhealth;
		    corpsehit->target = NULL;

		    return;
		}
	    }
	}
    }

    // Return to normal attack.
    A_Chase (actor);
}


//
// A_VileStart
//
void A_VileStart (mobj_t* actor)
{
    S_StartSound (actor, sfx_vilatk);
}


//
// A_Fire
// Keep fire in front of player unless out of sight
//
void A_Fire (mobj_t* actor);

void A_StartFire (mobj_t* actor)
{
    S_StartSound(actor,sfx_flamst);
    A_Fire(actor);
}

void A_FireCrackle (mobj_t* actor)
{
    S_StartSound(actor,sfx_flame);
    A_Fire(actor);
}

void A_Fire (mobj_t* actor)
{
    mobj_t*	dest;
    mobj_t*     target;
    unsigned	an;
		
    dest = actor->tracer;
    if (!dest)
	return;

    target = P_SubstNullMobj(actor->target);
		
    // don't move it if the vile lost sight
    if (!P_CheckSight (target, dest) )
	return;

    an = dest->angle >> ANGLETOFINESHIFT;

    P_UnsetThingPosition (actor);
    actor->x = dest->x + FixedMul (24*FRACUNIT, finecosine[an]);
    actor->y = dest->y + FixedMul (24*FRACUNIT, finesine[an]);
    actor->z = dest->z;
    P_SetThingPosition (actor);
}



//
// A_VileTarget
// Spawn the hellfire
//
void A_VileTarget (mobj_t*	actor)
{
    mobj_t*	fog;
	
    if (!actor->target)
	return;

    A_FaceTarget (actor);

    fog = P_SpawnMobj (actor->target->x,
		       actor->target->x,
		       actor->target->z, MT_FIRE);
    
    actor->tracer = fog;
    fog->target = actor;
    fog->tracer = actor->target;
    A_Fire (fog);
}




//
// A_VileAttack
//
void A_VileAttack (mobj_t* actor)
{	
    mobj_t*	fire;
    int		an;
	
    if (!actor->target)
	return;
    
    A_FaceTarget (actor);

    if (!P_CheckSight (actor, actor->target) )
	return;

    S_StartSound (actor, sfx_barexp);
    P_DamageMobj (actor->target, actor, actor, 20);
    actor->target->momz = 1000*FRACUNIT/actor->target->info->mass;
	
    an = actor->angle >> ANGLETOFINESHIFT;

    fire = actor->tracer;

    if (!fire)
	return;
		
    // move the fire between the vile and the player
    fire->x = actor->target->x - FixedMul (24*FRACUNIT, finecosine[an]);
    fire->y = actor->target->y - FixedMul (24*FRACUNIT, finesine[an]);	
    P_RadiusAttack (fire, actor, 70 );
}




//
// Mancubus attack,
// firing three missiles (bruisers)
// in three different directions?
// Doesn't look like it. 
//
#define	FATSPREAD	(ANG90/8)

void A_FatRaise (mobj_t *actor)
{
    A_FaceTarget (actor);
    S_StartSound (actor, sfx_manatk);
}


void A_FatAttack1 (mobj_t* actor)
{
    mobj_t*	mo;
    mobj_t*     target;
    int		an;

    A_FaceTarget (actor);

    // Change direction  to ...
    actor->angle += FATSPREAD;
    target = P_SubstNullMobj(actor->target);
    P_SpawnMissile (actor, target, MT_FATSHOT);

    mo = P_SpawnMissile (actor, target, MT_FATSHOT);
    mo->angle += FATSPREAD;
    an = mo->angle >> ANGLETOFINESHIFT;
    mo->momx = FixedMul (mo->info->speed, finecosine[an]);
    mo->momy = FixedMul (mo->info->speed, finesine[an]);
}

void A_FatAttack2 (mobj_t* actor)
{
    mobj_t*	mo;
    mobj_t*     target;
    int		an;

    A_FaceTarget (actor);
    // Now here choose opposite deviation.
    actor->angle -= FATSPREAD;
    target = P_SubstNullMobj(actor->target);
    P_SpawnMissile (actor, target, MT_FATSHOT);

    mo = P_SpawnMissile (actor, target, MT_FATSHOT);
    mo->angle -= FATSPREAD*2;
    an = mo->angle >> ANGLETOFINESHIFT;
    mo->momx = FixedMul (mo->info->speed, finecosine[an]);
    mo->momy = FixedMul (mo->info->speed, finesine[an]);
}

void A_FatAttack3 (mobj_t*	actor)
{
    mobj_t*	mo;
    mobj_t*     target;
    int		an;

    A_FaceTarget (actor);

    target = P_SubstNullMobj(actor->target);
    
    mo = P_SpawnMissile (actor, target, MT_FATSHOT);
    mo->angle -= FATSPREAD/2;
    an = mo->angle >> ANGLETOFINESHIFT;
    mo->momx = FixedMul (mo->info->speed, finecosine[an]);
    mo->momy = FixedMul (mo->info->speed, finesine[an]);

    mo = P_SpawnMissile (actor, target, MT_FATSHOT);
    mo->angle += FATSPREAD/2;
    an = mo->angle >> ANGLETOFINESHIFT;
    mo->momx = FixedMul (mo->info->speed, finecosine[an]);
    mo->momy = FixedMul (mo->info->speed, finesine[an]);
}


//
// SkullAttack
// Fly at the player like a missile.
//
#define	SKULLSPEED		(20*FRACUNIT)

void A_SkullAttack (mobj_t* actor)
{
    mobj_t*		dest;
    angle_t		an;
    int			dist;

    if (!actor->target)
	return;
		
    dest = actor->target;	
    actor->flags |= MF_SKULLFLY;

    S_StartSound (actor, actor->info->attacksound);
    A_FaceTarget (actor);
    an = actor->angle >> ANGLETOFINESHIFT;
    actor->momx = FixedMul (SKULLSPEED, finecosine[an]);
    actor->momy = FixedMul (SKULLSPEED, finesine[an]);
    dist = P_AproxDistance (dest->x - actor->x, dest->y - actor->y);
    dist = dist / SKULLSPEED;
    
    if (dist < 1)
	dist = 1;
    actor->momz = (dest->z+(dest->height>>1) - actor->z) / dist;
}


//
// A_PainShootSkull
// Spawn a lost soul and launch it at the target
//
void
A_PainShootSkull
( mobj_t*	actor,
  angle_t	angle )
{
    fixed_t	x;
    fixed_t	y;
    fixed_t	z;
    
    mobj_t*	newmobj;
    angle_t	an;
    int		prestep;
    int		count;
    thinker_t*	currentthinker;

    // count total number of skull currently on the level
    count = 0;

    currentthinker = thinkercap.next;
    while (currentthinker != &thinkercap)
    {
	if (   (currentthinker->function.acp1 == (actionf_p1)P_MobjThinker)
	    && ((mobj_t *)currentthinker)->type == MT_SKULL)
	    count++;
	currentthinker = currentthinker->next;
    }

    // if there are allready 20 skulls on the level,
    // don't spit another one
    if (count > 20)
	return;


    // okay, there's playe for another one
    an = angle >> ANGLETOFINESHIFT;
    
    prestep =
	4*FRACUNIT
	+ 3*(actor->info->radius + mobjinfo[MT_SKULL].radius)/2;
    
    x = actor->x + FixedMul (prestep, finecosine[an]);
    y = actor->y + FixedMul (prestep, finesine[an]);
    z = actor->z + 8*FRACUNIT;
		
    newmobj = P_SpawnMobj (x , y, z, MT_SKULL);

    // Check for movements.
    if (!P_TryMove (newmobj, newmobj->x, newmobj->y))
    {
	// kill it immediately
	P_DamageMobj (newmobj,actor,actor,10000);	
	return;
    }
		
    newmobj->target = actor->target;
    A_SkullAttack (newmobj);
}


//
// A_PainAttack
// Spawn a lost soul and launch it at the target
// 
void A_PainAttack (mobj_t* actor)
{
    if (!actor->target)
	return;

    A_FaceTarget (actor);
    A_PainShootSkull (actor, actor->angle);
}


void A_PainDie (mobj_t* actor)
{
    A_Fall (actor);
    A_PainShootSkull (actor, actor->angle+ANG90);
    A_PainShootSkull (actor, actor->angle+ANG180);
    A_PainShootSkull (actor, actor->angle+ANG270);
}

#define SHAMAN_COOLDOWN_TICS 140
#define SHAMAN_HEAL_AMOUNT 15
#define SHAMAN_FREEZE_DURATION 180

typedef struct {
    int cooldown_tics;
} shaman_cooldown_t;

static void P_SetShamanCooldown(mobj_t* actor, int tics)
{
    if (!actor->player)
    {
        actor->reactiontime = tics;
    }
}

static int P_GetShamanCooldown(mobj_t* actor)
{
    if (!actor->player)
    {
        return actor->reactiontime;
    }
    return 0;
}

void A_ShamanSpell (mobj_t* actor)
{
    int spell_roll;

    if (!actor->target)
	return;

    if (P_GetShamanCooldown(actor) > 0)
    {
        P_SetShamanCooldown(actor, P_GetShamanCooldown(actor) - 1);
        return;
    }

    A_FaceTarget (actor);

    spell_roll = P_Random ();

    if (spell_roll < 70)
    {
        P_SpawnMissile (actor, actor->target, MT_SHAMAN_FIREBOLT);
    }
    else if (spell_roll < 120)
    {
        mobj_t* target;
        int best_dist = 0;
        mobj_t* best_ally = NULL;
        
        target = actor->subsector->sector->thinglist;
        while (target)
        {
            if (target != actor && 
                target->type >= MT_DWARF_DEFENDER && 
                target->type <= MT_GOBLIN_ALCHEMIST &&
                target->health < target->info->spawnhealth &&
                target->flags & MF_SOLID)
            {
                int dist = P_AproximalDistance (actor->x - target->x, actor->y - target->y);
                if (!best_ally || dist < best_dist)
                {
                    best_dist = dist;
                    best_ally = target;
                }
            }
            target = target->next;
        }

        if (best_ally && best_dist < 512*FRACUNIT)
        {
            P_SetMobjState (actor, S_SHAMAN_CAST1);
            if (best_ally->health + SHAMAN_HEAL_AMOUNT < best_ally->info->spawnhealth)
            {
                P_DamageMobj (best_ally, actor, actor, -SHAMAN_HEAL_AMOUNT);
                S_StartSound (best_ally, sfx_itemup);
            }
            P_SetShamanCooldown(actor, SHAMAN_COOLDOWN_TICS);
            return;
        }
        else
        {
            P_SpawnMissile (actor, actor->target, MT_SHAMAN_FIREBOLT);
        }
    }
    else if (spell_roll < 160)
    {
        mobj_t* target;
        int buff_count = 0;
        
        target = actor->subsector->sector->thinglist;
        while (target)
        {
            if (target != actor && 
                target->type >= MT_DWARF_DEFENDER && 
                target->type <= MT_GOBLIN_ALCHEMIST &&
                target->health > 0 &&
                (target->flags & MF_SOLID) &&
                !(target->flags & MF_CORPSE))
            {
                int dist = P_AproximalDistance (actor->x - target->x, actor->y - target->y);
                if (dist < 512*FRACUNIT)
                {
                    if (!G_StatusEffectIsActive(target, st_enraged))
                    {
                        G_StatusEffectApply(target, st_enraged, enraged_tics);
                        buff_count++;
                    }
                }
            }
            target = target->next;
        }

        if (buff_count > 0)
        {
            P_SetMobjState (actor, S_SHAMAN_CAST1);
            S_StartSound (actor, sfx_posit1);
        }
        P_SetShamanCooldown(actor, SHAMAN_COOLDOWN_TICS);
        return;
    }
    else if (spell_roll < 200)
    {
        P_SpawnMissile (actor, actor->target, MT_SHAMAN_FREEZE);
    }
    else
    {
        P_SetMobjState (actor, S_SHAMAN_TELEPORT1);
    }

    P_SetShamanCooldown(actor, SHAMAN_COOLDOWN_TICS);
}

void A_ShamanFirebolt (mobj_t* actor)
{
    if (!actor->target)
	return;

    P_SpawnMissile (actor, actor->target, MT_SHAMAN_FIREBOLT);
}

void A_ShamanHeal (mobj_t* actor)
{
    mobj_t* target;
    int best_dist = 0;
    mobj_t* best_ally = NULL;
    
    target = actor->subsector->sector->thinglist;
    while (target)
    {
        if (target != actor && 
            target->type >= MT_DWARF_DEFENDER && 
            target->type <= MT_GOBLIN_ALCHEMIST &&
            target->health < target->info->spawnhealth &&
            (target->flags & MF_SOLID) && !(target->flags & MF_COUNTKILL))
        {
            int dist = P_AproximalDistance (actor->x - target->x, actor->y - target->y);
            if (!best_ally || dist < best_dist)
            {
                best_dist = dist;
                best_ally = target;
            }
        }
        target = target->next;
    }

    if (best_ally && best_dist < 512*FRACUNIT)
    {
        if (best_ally->health + SHAMAN_HEAL_AMOUNT < best_ally->info->spawnhealth)
        {
            P_DamageMobj (best_ally, actor, actor, -SHAMAN_HEAL_AMOUNT);
            S_StartSound (best_ally, sfx_itemup);
        }
    }
}

void A_ShamanFreeze (mobj_t* actor)
{
    if (!actor->target)
	return;

    P_SpawnMissile (actor, actor->target, MT_SHAMAN_FREEZE);
}

void A_ShamanTeleport (mobj_t* actor)
{
    angle_t an;
    fixed_t dist;
    fixed_t x;
    fixed_t y;
    fixed_t z;
    int i;
    int valid_position = 0;

    if (!actor->target)
        return;

    for (i = 0; i < 16; i++)
    {
        an = P_Random () << 24;
        dist = (P_Random () & 7) + 1;
        dist <<= FRACBITS;
        dist = dist * 128 + 128*FRACUNIT;

        x = actor->x + FixedMul (finecosine[an>>ANGLETOFINESHIFT], dist);
        y = actor->y + FixedMul (finesine[an>>ANGLETOFINESHIFT], dist);
        z = actor->z;

        if (P_CheckPosition (actor, x, y))
        {
            valid_position = 1;
            break;
        }
    }

    if (valid_position)
    {
        P_UnsetThingPosition (actor);
        actor->x = x;
        actor->y = y;
        actor->z = z;
        P_SetThingPosition (actor);
        S_StartSound (actor, sfx_telept);
    }
}

void A_ShamanChaos (mobj_t* actor)
{
    int chaos_roll;

    if (!actor->target)
        return;

    chaos_roll = P_Random () % 6;

    switch (chaos_roll)
    {
        case 0:
        case 1:
            P_SpawnMissile (actor, actor->target, MT_SHAMAN_FIREBOLT);
            break;
        case 2:
            P_SpawnMissile (actor, actor->target, MT_SHAMAN_FREEZE);
            break;
        case 3:
        {
            mobj_t* target;
            int best_dist = 0;
            mobj_t* best_ally = NULL;
            
            target = actor->subsector->sector->thinglist;
            while (target)
            {
                if (target != actor && 
                    target->type >= MT_DWARF_DEFENDER && 
                    target->type <= MT_GOBLIN_ALCHEMIST &&
                    target->health < target->info->spawnhealth &&
                    target->flags & MF_SOLID)
                {
                    int dist = P_AproximalDistance (actor->x - target->x, actor->y - target->y);
                    if (!best_ally || dist < best_dist)
                    {
                        best_dist = dist;
                        best_ally = target;
                    }
                }
                target = target->next;
            }

            if (best_ally && best_dist < 512*FRACUNIT)
            {
                if (best_ally->health + SHAMAN_HEAL_AMOUNT < best_ally->info->spawnhealth)
                {
                    P_DamageMobj (best_ally, actor, actor, -SHAMAN_HEAL_AMOUNT);
                    S_StartSound (best_ally, sfx_itemup);
                }
            }
            break;
        }
        case 4:
        {
            angle_t an = P_Random () << 24;
            int j;
            for (j = 0; j < 3; j++)
            {
                angle_t offset = (P_Random () - 128) << 23;
                fixed_t shoot_angle = actor->angle + offset;
                P_SpawnMissileAngle (actor, MT_SHAMAN_FIREBOLT, shoot_angle, 0);
            }
            break;
        }
        case 5:
            if (actor->health < actor->info->spawnhealth)
            {
                P_DamageMobj (actor, actor, actor, -SHAMAN_HEAL_AMOUNT);
                S_StartSound (actor, sfx_itemup);
            }
            break;
    }
}









void A_Scream (mobj_t* actor)
{
    int		sound;
	
    switch (actor->info->deathsound)
    {
      case 0:
	return;
		
      case sfx_podth1:
      case sfx_podth2:
      case sfx_podth3:
	sound = sfx_podth1 + P_Random ()%3;
	break;
		
      case sfx_bgdth1:
      case sfx_bgdth2:
	sound = sfx_bgdth1 + P_Random ()%2;
	break;
	
      default:
	sound = actor->info->deathsound;
	break;
    }

    // Check for bosses.
    if (actor->type==MT_SPIDER
	|| actor->type == MT_CYBORG)
    {
	// full volume
	S_StartSound (NULL, sound);
    }
    else
	S_StartSound (actor, sound);
}


void A_XScream (mobj_t* actor)
{
    S_StartSound (actor, sfx_slop);	
}

void A_Pain (mobj_t* actor)
{
    if (actor->info->painsound)
	S_StartSound (actor, actor->info->painsound);	
}



void A_Fall (mobj_t *actor)
{
    // actor is on ground, it can be walked over
    actor->flags &= ~MF_SOLID;

    // So change this if corpse objects
    // are meant to be obstacles.
}


//
// A_Explode
//
void A_Explode (mobj_t* thingy)
{
    P_RadiusAttack(thingy, thingy->target, 128);
}

// Check whether the death of the specified monster type is allowed
// to trigger the end of episode special action.
//
// This behavior changed in v1.9, the most notable effect of which
// was to break uac_dead.wad

static boolean CheckBossEnd(mobjtype_t motype)
{
    if (gameversion < exe_ultimate)
    {
        if (gamemap != 8)
        {
            return false;
        }

        // Baron death on later episodes is nothing special.

        if (motype == MT_BRUISER && gameepisode != 1)
        {
            return false;
        }

        return true;
    }
    else
    {
        // New logic that appeared in Ultimate Doom.
        // Looks like the logic was overhauled while adding in the
        // episode 4 support.  Now bosses only trigger on their
        // specific episode.

	switch(gameepisode)
	{
            case 1:
                return gamemap == 8 && motype == MT_BRUISER;

            case 2:
                return gamemap == 8 && motype == MT_CYBORG;

            case 3:
                return gamemap == 8 && motype == MT_SPIDER;

	    case 4:
                return (gamemap == 6 && motype == MT_CYBORG)
                    || (gamemap == 8 && motype == MT_SPIDER);

            default:
                return gamemap == 8;
	}
    }
}

//
// A_BossDeath
// Possibly trigger special effects
// if on first boss level
//
void A_BossDeath (mobj_t* mo)
{
    thinker_t*	th;
    mobj_t*	mo2;
    line_t	junk;
    int		i;
		
    if ( gamemode == commercial)
    {
	if (gamemap != 7)
	    return;
		
	if ((mo->type != MT_FATSO)
	    && (mo->type != MT_BABY))
	    return;
    }
    else
    {
        if (!CheckBossEnd(mo->type))
        {
            return;
        }
    }

    // make sure there is a player alive for victory
    for (i=0 ; i<MAXPLAYERS ; i++)
	if (playeringame[i] && players[i].health > 0)
	    break;
    
    if (i==MAXPLAYERS)
	return;	// no one left alive, so do not end game
    
    // scan the remaining thinkers to see
    // if all bosses are dead
    for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    {
	if (th->function.acp1 != (actionf_p1)P_MobjThinker)
	    continue;
	
	mo2 = (mobj_t *)th;
	if (mo2 != mo
	    && mo2->type == mo->type
	    && mo2->health > 0)
	{
	    // other boss not dead
	    return;
	}
    }
	
    // victory!
    if ( gamemode == commercial)
    {
	if (gamemap == 7)
	{
	    if (mo->type == MT_FATSO)
	    {
		junk.tag = 666;
		EV_DoFloor(&junk,lowerFloorToLowest);
		return;
	    }
	    
	    if (mo->type == MT_BABY)
	    {
		junk.tag = 667;
		EV_DoFloor(&junk,raiseToTexture);
		return;
	    }
	}
    }
    else
    {
	switch(gameepisode)
	{
	  case 1:
	    junk.tag = 666;
	    EV_DoFloor (&junk, lowerFloorToLowest);
	    return;
	    break;
	    
	  case 4:
	    switch(gamemap)
	    {
	      case 6:
		junk.tag = 666;
		EV_DoDoor (&junk, vld_blazeOpen);
		return;
		break;
		
	      case 8:
		junk.tag = 666;
		EV_DoFloor (&junk, lowerFloorToLowest);
		return;
		break;
	    }
	}
    }
	
    G_ExitLevel ();
}


void A_Hoof (mobj_t* mo)
{
    S_StartSound (mo, sfx_hoof);
    A_Chase (mo);
}

void A_Metal (mobj_t* mo)
{
    S_StartSound (mo, sfx_metal);
    A_Chase (mo);
}

void A_BabyMetal (mobj_t* mo)
{
    S_StartSound (mo, sfx_bspwlk);
    A_Chase (mo);
}

void
A_OpenShotgun2
( player_t*	player,
  pspdef_t*	psp )
{
    S_StartSound (player->mo, sfx_dbopn);
}

void
A_LoadShotgun2
( player_t*	player,
  pspdef_t*	psp )
{
    S_StartSound (player->mo, sfx_dbload);
}

void
A_ReFire
( player_t*	player,
  pspdef_t*	psp );

void
A_CloseShotgun2
( player_t*	player,
  pspdef_t*	psp )
{
    S_StartSound (player->mo, sfx_dbcls);
    A_ReFire(player,psp);
}



mobj_t*		braintargets[32];
int		numbraintargets;
int		braintargeton = 0;

void A_BrainAwake (mobj_t* mo)
{
    thinker_t*	thinker;
    mobj_t*	m;
	
    // find all the target spots
    numbraintargets = 0;
    braintargeton = 0;

    for (thinker = thinkercap.next ;
	 thinker != &thinkercap ;
	 thinker = thinker->next)
    {
	if (thinker->function.acp1 != (actionf_p1)P_MobjThinker)
	    continue;	// not a mobj

	m = (mobj_t *)thinker;

	if (m->type == MT_BOSSTARGET )
	{
	    braintargets[numbraintargets] = m;
	    numbraintargets++;
	}
    }
	
    S_StartSound (NULL,sfx_bossit);
}


void A_BrainPain (mobj_t*	mo)
{
    S_StartSound (NULL,sfx_bospn);
}


void A_BrainScream (mobj_t*	mo)
{
    int		x;
    int		y;
    int		z;
    mobj_t*	th;
	
    for (x=mo->x - 196*FRACUNIT ; x< mo->x + 320*FRACUNIT ; x+= FRACUNIT*8)
    {
	y = mo->y - 320*FRACUNIT;
	z = 128 + P_Random()*2*FRACUNIT;
	th = P_SpawnMobj (x,y,z, MT_ROCKET);
	th->momz = P_Random()*512;

	P_SetMobjState (th, S_BRAINEXPLODE1);

	th->tics -= P_Random()&7;
	if (th->tics < 1)
	    th->tics = 1;
    }
	
    S_StartSound (NULL,sfx_bosdth);
}



void A_BrainExplode (mobj_t* mo)
{
    int		x;
    int		y;
    int		z;
    mobj_t*	th;
	
    x = mo->x +  P_SubRandom() * 2048;
    y = mo->y;
    z = 128 + P_Random()*2*FRACUNIT;
    th = P_SpawnMobj (x,y,z, MT_ROCKET);
    th->momz = P_Random()*512;

    P_SetMobjState (th, S_BRAINEXPLODE1);

    th->tics -= P_Random()&7;
    if (th->tics < 1)
	th->tics = 1;
}


void A_BrainDie (mobj_t*	mo)
{
    G_ExitLevel ();
}

void A_BrainSpit (mobj_t*	mo)
{
    mobj_t*	targ;
    mobj_t*	newmobj;
    
    static int	easy = 0;
	
    easy ^= 1;
    if (gameskill <= sk_easy && (!easy))
	return;
		
    // shoot a cube at current target
    targ = braintargets[braintargeton];
    if (numbraintargets == 0)
    {
        I_Error("A_BrainSpit: numbraintargets was 0 (vanilla crashes here)");
    }
    braintargeton = (braintargeton+1)%numbraintargets;

    // spawn brain missile
    newmobj = P_SpawnMissile (mo, targ, MT_SPAWNSHOT);
    newmobj->target = targ;
    newmobj->reactiontime =
	((targ->y - mo->y)/newmobj->momy) / newmobj->state->tics;

    S_StartSound(NULL, sfx_bospit);
}



void A_SpawnFly (mobj_t* mo);

// travelling cube sound
void A_SpawnSound (mobj_t* mo)	
{
    S_StartSound (mo,sfx_boscub);
    A_SpawnFly(mo);
}

void A_SpawnFly (mobj_t* mo)
{
    mobj_t*	newmobj;
    mobj_t*	fog;
    mobj_t*	targ;
    int		r;
    mobjtype_t	type;
	
    if (--mo->reactiontime)
	return;	// still flying
	
    targ = P_SubstNullMobj(mo->target);

    // First spawn teleport fog.
    fog = P_SpawnMobj (targ->x, targ->y, targ->z, MT_SPAWNFIRE);
    S_StartSound (fog, sfx_telept);

    // Randomly select monster to spawn.
    r = P_Random ();

    // Probability distribution (kind of :),
    // decreasing likelihood.
    if ( r<50 )
	type = MT_TROOP;
    else if (r<90)
	type = MT_SERGEANT;
    else if (r<120)
	type = MT_SHADOWS;
    else if (r<130)
	type = MT_PAIN;
    else if (r<160)
	type = MT_HEAD;
    else if (r<162)
	type = MT_VILE;
    else if (r<172)
	type = MT_UNDEAD;
    else if (r<192)
	type = MT_BABY;
    else if (r<222)
	type = MT_FATSO;
    else if (r<246)
	type = MT_KNIGHT;
    else
	type = MT_BRUISER;		

    newmobj	= P_SpawnMobj (targ->x, targ->y, targ->z, type);
    if (P_LookForPlayers (newmobj, true) )
	P_SetMobjState (newmobj, newmobj->info->seestate);
	
    // telefrag anything in this spot
    P_TeleportMove (newmobj, newmobj->x, newmobj->y);

    // remove self (i.e., cube).
    P_RemoveMobj (mo);
}



void A_PlayerScream (mobj_t* mo)
{
    // Default death sound.
    int		sound = sfx_pldeth;
	
    if ( (gamemode == commercial)
	&& 	(mo->health < -50))
    {
	// IF THE PLAYER DIES
	// LESS THAN -50% WITHOUT GIBBING
	sound = sfx_pdiehi;
    }
    
    S_StartSound (mo, sound);
}
