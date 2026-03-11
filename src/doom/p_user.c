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
//	Player related stuff.
//	Bobbing POV/weapon, movement.
//	Pending weapon.
//




#include "doomdef.h"
#include "d_event.h"
#include "p_local.h"
#include "p_siege_weapon.h"
#include "doomstat.h"
#include "s_sound.h"
#include "sounds.h"
#include "g_status.h"
#include "g_powerup.h"
#include "m_controls.h"



// Index of the special effects (INVUL inverse) map.
// Use colormap 20 for crit boost (darker = more intense)
#define INVERSECOLORMAP		32
#define CRITCOLORMAP		20
#define DOUBLEDAMAGECOLORMAP	21

// Goblin Dice Rollaz: Dash Token powerup constants
#define DASH_SPEED		(32*FRACUNIT)
#define DASH_IFRAME_DURATION	(10)
#define DASH_COOLDOWN		(15*TICRATE)

// Goblin Dice Rollaz: Dodge roll ability constants
#define ROLL_SPEED		(28*FRACUNIT)
#define ROLL_IFRAME_DURATION	(8)
#define ROLL_COOLDOWN		(30*TICRATE)

// Goblin Dice Rollaz: Mantle/climbing constants
#define MANTLE_HEIGHT		(56*FRACUNIT)
#define MANTLE_SPEED		(4*FRACUNIT)
#define MANTLE_CHECK_DIST	(32*FRACUNIT)

// Goblin Dice Rollaz: Ledge grab/climb constants
#define LEDGE_GRAB_HEIGHT	(80*FRACUNIT)   // Max height to grab ledge
#define LEDGE_REACH_DIST	(40*FRACUNIT)   // How far forward to check for ledge
#define LEDGE_HANG_OFFSET	(8*FRACUNIT)    // Offset from ledge when hanging
#define LEDGE_CLIMB_SPEED	(4*FRACUNIT)    // Speed of climb animation
#define LEDGE_CLIMB_HEIGHT	(8*FRACUNIT)    // Height per climb tic

// Goblin Dice Rollaz: Crouch/slide constants
#define CROUCH_HEIGHT		(24*FRACUNIT)
#define CROUCH_SPEED_MULT	(60)  // 60% of normal speed
#define CROUCH_VIEW_OFFSET	(17*FRACUNIT)  // VIEWHEIGHT - CROUCH_HEIGHT
#define SLIDE_SPEED_MULT	(80)  // 80% of normal speed while sliding
#define SLIDE_DURATION		(15)  // tics to slide after releasing crouch while moving

// Goblin Dice Rollaz: Wall jump constants
#define WALLJUMP_SPEED		(24*FRACUNIT)
#define WALLJUMP_COOLDOWN	(10*TICRATE)
#define WALLJUMP_CHECK_DIST	(32*FRACUNIT)
#define WALLSLIDE_SPEED		(4*FRACUNIT)  // Slower fall when sliding down wall


//
// Movement.
//

// 16 pixels of bob
#define MAXBOB	0x100000	

boolean		onground;


//
// P_Thrust
// Moves the given origin along a given angle.
//
void
P_Thrust
( player_t*	player,
  angle_t	angle,
  fixed_t	move ) 
{
    angle >>= ANGLETOFINESHIFT;
    
    player->mo->momx += FixedMul(move,finecosine[angle]); 
    player->mo->momy += FixedMul(move,finesine[angle]);
}




//
// P_CalcHeight
// Calculate the walking / running height adjustment
//
void P_CalcHeight (player_t* player) 
{
    int		angle;
    fixed_t	bob;
    fixed_t	target_viewheight;
    
    // Regular movement bobbing
    // (needs to be calculated for gun swing
    // even if not on ground)
    // OPTIMIZE: tablify angle
    // Note: a LUT allows for effects
    //  like a ramp with low health.
    player->bob =
	FixedMul (player->mo->momx, player->mo->momx)
	+ FixedMul (player->mo->momy,player->mo->momy);
    
    player->bob >>= 2;

    if (player->bob>MAXBOB)
	player->bob = MAXBOB;

    // Goblin Dice Rollaz: Determine target view height based on crouching state
    if (player->crouching || player->slide_tics > 0)
    {
        target_viewheight = CROUCH_HEIGHT;
    }
    else
    {
        target_viewheight = VIEWHEIGHT;
    }

    if ((player->cheats & CF_NOMOMENTUM) || !onground)
    {
	player->viewz = player->mo->z + target_viewheight;

	if (player->viewz > player->mo->ceilingz-4*FRACUNIT)
	    player->viewz = player->mo->ceilingz-4*FRACUNIT;

	player->viewz = player->mo->z + player->viewheight;
	return;
    }
		
    angle = (FINEANGLES/20*leveltime)&FINEMASK;
    bob = FixedMul ( player->bob/2, finesine[angle]);

    
    // move viewheight
    if (player->playerstate == PST_LIVE)
    {
	player->viewheight += player->deltaviewheight;

	// Goblin Dice Rollaz: Handle smooth transition to/from crouch
	if (player->crouching || player->slide_tics > 0)
	{
	    // Target is crouching height
	    if (player->viewheight > CROUCH_HEIGHT)
	    {
		player->deltaviewheight = -FRACUNIT/2;
	    }
	    else if (player->viewheight < CROUCH_HEIGHT)
	    {
		player->viewheight = CROUCH_HEIGHT;
		player->deltaviewheight = 0;
	    }
	}
	else
	{
	    // Target is standing height
	    if (player->viewheight > VIEWHEIGHT)
	    {
		player->viewheight = VIEWHEIGHT;
		player->deltaviewheight = 0;
	    }
	}

	if (player->viewheight > VIEWHEIGHT)
	{
	    player->viewheight = VIEWHEIGHT;
	    player->deltaviewheight = 0;
	}

	if (player->viewheight < CROUCH_HEIGHT)
	{
	    player->viewheight = CROUCH_HEIGHT;
	    if (player->deltaviewheight <= 0)
		player->deltaviewheight = 1;
	}
	
	if (player->deltaviewheight)	
	{
	    player->deltaviewheight += FRACUNIT/4;
	    if (!player->deltaviewheight)
		player->deltaviewheight = 1;
	}
    }
    player->viewz = player->mo->z + player->viewheight + bob;

    if (player->viewz > player->mo->ceilingz-4*FRACUNIT)
	player->viewz = player->mo->ceilingz-4*FRACUNIT;
}



//
// P_MovePlayer
//
void P_MovePlayer (player_t* player)
{
    ticcmd_t*	cmd;
    fixed_t	forward_scale = 2048;
    fixed_t	side_scale = 2048;
    
    cmd = &player->cmd;
    
    // Goblin Dice Rollaz: Apply freeze/slow effect to player
    if (player->mo->freeze_tics > 0)
    {
        player->mo->freeze_tics--;
    }

    // Goblin Dice Rollaz: Apply status effect movement slowdown
    // Use g_status framework for frozen effect with configurable speed reduction
    if (G_StatusEffectIsActive(player->mo, st_frozen))
    {
        G_StatusEffectTick(player->mo);
        forward_scale = (forward_scale * G_StatusEffectGetMoveSpeedMultiplier(player->mo)) / 100;
        side_scale = (side_scale * G_StatusEffectGetMoveSpeedMultiplier(player->mo)) / 100;
    }
    else if (player->mo->freeze_tics > 0)
    {
        // Legacy freeze_tics: reduce movement speed by 50%
        forward_scale = 1024;
        side_scale = 1024;
    }

    // Goblin Dice Rollaz: Handle Dash Token powerup
    if (player->dash_cooldown > 0)
    {
        player->dash_cooldown--;
    }

    if (player->dash_iframes > 0)
    {
        player->dash_iframes--;
        if (player->dash_iframes > 0)
        {
            player->mo->flags |= MF_INVULNERABLE;
            player->fixedcolormap = INVERSECOLORMAP;
        }
        else
        {
            player->mo->flags &= ~MF_INVULNERABLE;
            player->fixedcolormap = 0;
        }
    }

    // Goblin Dice Rollaz: Trigger dash when moving forward with Dash Token
    if (G_PowerupIsActive(player, pw_dashtoken) && player->dash_cooldown == 0 && cmd->forwardmove > 0 && onground)
    {
        P_Thrust(player, player->mo->angle, DASH_SPEED);
        player->dash_iframes = DASH_IFRAME_DURATION;
        player->dash_cooldown = DASH_COOLDOWN;
        player->mo->flags |= MF_INVULNERABLE;
        player->fixedcolormap = INVERSECOLORMAP;
    }

    // Goblin Dice Rollaz: Handle dodge roll cooldown
    if (player->roll_cooldown > 0)
    {
        player->roll_cooldown--;
    }

    // Goblin Dice Rollaz: Handle dodge roll invulnerability frames
    if (player->roll_iframes > 0)
    {
        player->roll_iframes--;
        if (player->roll_iframes > 0)
        {
            player->mo->flags |= MF_INVULNERABLE;
            player->fixedcolormap = INVERSECOLORMAP;
        }
        else
        {
            player->mo->flags &= ~MF_INVULNERABLE;
            player->fixedcolormap = 0;
            player->roll_direction = 0;
        }
    }

    // Goblin Dice Rollaz: Trigger dodge roll when dodge key is pressed
    if (gamekeydown[key_dodge] && player->roll_cooldown == 0 && onground && player->roll_iframes == 0)
    {
        // Determine roll direction based on strafing or default to forward
        int roll_dir = 0;
        if (cmd->sidemove < 0)
        {
            // Strafe left - roll left
            roll_dir = -1;
        }
        else if (cmd->sidemove > 0)
        {
            // Strafe right - roll right
            roll_dir = 1;
        }
        else
        {
            // No strafe - roll forward
            roll_dir = 0;
        }

        // Apply roll thrust
        if (roll_dir == 0)
        {
            P_Thrust(player, player->mo->angle, ROLL_SPEED);
        }
        else if (roll_dir < 0)
        {
            P_Thrust(player, player->mo->angle - ANG90, ROLL_SPEED);
        }
        else
        {
            P_Thrust(player, player->mo->angle + ANG90, ROLL_SPEED);
        }

        player->roll_iframes = ROLL_IFRAME_DURATION;
        player->roll_cooldown = ROLL_COOLDOWN;
        player->roll_direction = roll_dir;
        player->mo->flags |= MF_INVULNERABLE;
        player->fixedcolormap = INVERSECOLORMAP;
    }

    // Goblin Dice Rollaz: Handle crouch/slide mechanics
    // Track previous crouching state
    player->was_crouching = player->crouching;

    // Check for crouch key press
    if (gamekeydown[key_crouch] && onground && player->roll_iframes == 0 && player->dash_iframes == 0)
    {
        // Start crouching
        if (!player->crouching)
        {
            player->crouching = true;
        }
    }
    else if (!gamekeydown[key_crouch] && player->crouching)
    {
        // Player released crouch key - check for slide
        player->crouching = false;

        // If moving when releasing crouch, apply slide
        if ((cmd->forwardmove != 0 || cmd->sidemove != 0) && onground)
        {
            player->slide_tics = SLIDE_DURATION;
        }
    }

    // Apply slide if active
    if (player->slide_tics > 0)
    {
        player->slide_tics--;
        // Apply slide momentum
        if (cmd->forwardmove != 0)
        {
            P_Thrust(player, player->mo->angle, (cmd->forwardmove * SLIDE_SPEED_MULT) / 100);
        }
    }

    // Apply movement speed reduction when crouching
    if (player->crouching || player->slide_tics > 0)
    {
        forward_scale = (forward_scale * CROUCH_SPEED_MULT) / 100;
        side_scale = (side_scale * CROUCH_SPEED_MULT) / 100;
    }
	
    player->mo->angle += (cmd->angleturn<<FRACBITS);

    // Do not let the player control movement
    //  if not onground.
    onground = (player->mo->z <= player->mo->floorz);
	
    if (cmd->forwardmove && onground)
	P_Thrust (player, player->mo->angle, cmd->forwardmove*forward_scale);
    
    if (cmd->sidemove && onground)
	P_Thrust (player, player->mo->angle-ANG90, cmd->sidemove*side_scale);

    if ( (cmd->forwardmove || cmd->sidemove) 
	 && player->mo->state == &states[S_PLAY] )
    {
	P_SetMobjState (player->mo, S_PLAY_RUN1);
    }

    // Goblin Dice Rollaz: Handle mantling/climbing small obstacles
    if (player->mantle_tics > 0)
    {
        player->mantle_tics--;
        if (player->mantle_tics > 0 && player->mantle_height > player->mo->z)
        {
            player->mo->z += MANTLE_SPEED;
            if (player->mo->z > player->mantle_height)
            {
                player->mo->z = player->mantle_height;
            }
            player->mo->floorz = player->mantle_height;
        }
        else
        {
            player->mantle_tics = 0;
            player->mantle_height = 0;
        }
    }
    else if (onground && cmd->forwardmove > 0)
    {
        // Check if there's a mantle-able surface ahead
        fixed_t checkx = player->mo->x + FixedMul(MANTLE_CHECK_DIST, finecosine[player->mo->angle >> ANGLETOFINESHIFT]);
        fixed_t checky = player->mo->y + FixedMul(MANTLE_CHECK_DIST, finesine[player->mo->angle >> ANGLETOFINESHIFT]);

        P_CheckPosition(player->mo, checkx, checky);

        fixed_t step_height = tmfloorz - player->mo->floorz;
        if (step_height > 24*FRACUNIT && step_height <= MANTLE_HEIGHT)
        {
            // Check if there's enough ceiling clearance to mantle
            if (tmceilingz - tmfloorz >= player->mo->height + 16*FRACUNIT)
            {
                player->mantle_tics = (step_height / MANTLE_SPEED) + 1;
                player->mantle_height = tmfloorz;
                // Keep some forward momentum during mantle
                player->mo->momx = FixedMul(4*FRACUNIT, finecosine[player->mo->angle >> ANGLETOFINESHIFT]);
                player->mo->momy = FixedMul(4*FRACUNIT, finesine[player->mo->angle >> ANGLETOFINESHIFT]);
            }
        }
    }

    // Goblin Dice Rollaz: Handle wall jump cooldown
    if (player->walljump_cooldown > 0)
    {
        player->walljump_cooldown--;
    }

    // Goblin Dice Rollaz: Handle wall slide
    if (player->wallslide_tics > 0)
    {
        player->wallslide_tics--;
        // Slow down descent when hugging wall
        if (player->mo->momz < -WALLSLIDE_SPEED)
        {
            player->mo->momz = -WALLSLIDE_SPEED;
        }
    }

    // Goblin Dice Rollaz: Wall jump detection and execution
    // Only when in air, not already wall sliding or jumping, and not in other special states
    if (!onground && player->roll_iframes == 0 && player->dash_iframes == 0 && 
        player->mantle_tics == 0 && player->walljump_cooldown == 0)
    {
        // Check for wall on right side
        fixed_t checkx_right = player->mo->x + FixedMul(WALLJUMP_CHECK_DIST, finecosine[(player->mo->angle + ANG90) >> ANGLETOFINESHIFT]);
        fixed_t checky_right = player->mo->y + FixedMul(WALLJUMP_CHECK_DIST, finesine[(player->mo->angle + ANG90) >> ANGLETOFINESHIFT]);
        
        // Check for wall on left side
        fixed_t checkx_left = player->mo->x + FixedMul(WALLJUMP_CHECK_DIST, finecosine[(player->mo->angle - ANG90) >> ANGLETOFINESHIFT]);
        fixed_t checky_left = player->mo->y + FixedMul(WALLJUMP_CHECK_DIST, finesine[(player->mo->angle - ANG90) >> ANGLETOFINESHIFT]);

        // Check right wall
        if (!P_CheckPosition(player->mo, checkx_right, checky_right))
        {
            // Wall on right - initiate wall slide if falling
            if (player->mo->momz < 0)
            {
                player->wall_contact = 1;
                player->wallslide_tics = 5;  // Keep slide active briefly
            }
            
            // Apply wall jump if jump key is pressed
            if (gamekeydown[key_jump])
            {
                // Jump away from wall (to the left)
                angle_t jump_angle = player->mo->angle - ANG90;
                P_Thrust(player, jump_angle, WALLJUMP_SPEED);
                player->mo->momz = (14*FRACUNIT);  // Upward boost
                player->walljump_cooldown = WALLJUMP_COOLDOWN;
                player->wall_contact = 0;
                player->wallslide_tics = 0;
            }
        }
        // Check left wall
        else if (!P_CheckPosition(player->mo, checkx_left, checky_left))
        {
            // Wall on left - initiate wall slide if falling
            if (player->mo->momz < 0)
            {
                player->wall_contact = -1;
                player->wallslide_tics = 5;
            }
            
            // Apply wall jump if jump key is pressed
            if (gamekeydown[key_jump])
            {
                // Jump away from wall (to the right)
                angle_t jump_angle = player->mo->angle + ANG90;
                P_Thrust(player, jump_angle, WALLJUMP_SPEED);
                player->mo->momz = (14*FRACUNIT);  // Upward boost
                player->walljump_cooldown = WALLJUMP_COOLDOWN;
                player->wall_contact = 0;
                player->wallslide_tics = 0;
            }
        }
        else
        {
            player->wall_contact = 0;
        }
    }

    // Goblin Dice Rollaz: Ledge grab/climb ability
    // Handle climbing animation when hanging from ledge
    if (player->ledge_hanging && player->ledge_climb_tics > 0)
    {
        player->ledge_climb_tics--;
        // Move player up during climb
        player->mo->z += LEDGE_CLIMB_HEIGHT;
        player->mo->floorz = player->mo->z;
        
        // Keep player at ledge position
        player->mo->x = player->ledge_x;
        player->mo->y = player->ledge_y;
        
        // Check if climb is complete
        if (player->mo->z >= player->ledge_height)
        {
            player->ledge_hanging = false;
            player->ledge_climb_tics = 0;
            player->mo->z = player->ledge_height;
            player->mo->floorz = player->ledge_height;
        }
    }
    // Detect ledge grab opportunity when in air and moving forward
    else if (!onground && cmd->forwardmove > 0 && player->roll_iframes == 0 && 
             player->dash_iframes == 0 && player->mantle_tics == 0 &&
             player->mo->momz < 0 && !player->ledge_hanging)
    {
        // Check for ledge in front of player at a higher elevation
        fixed_t checkx = player->mo->x + FixedMul(LEDGE_REACH_DIST, finecosine[player->mo->angle >> ANGLETOFINESHIFT]);
        fixed_t checky = player->mo->y + FixedMul(LEDGE_REACH_DIST, finesine[player->mo->angle >> ANGLETOFINESHIFT]);

        // First check the ground at the target position
        if (P_CheckPosition(player->mo, checkx, checky))
        {
            // Get the floor height at target position
            fixed_t target_floor = tmfloorz;
            fixed_t height_diff = target_floor - player->mo->z;

            // Check if this is a grabbable ledge (higher than mantle height but within reach)
            if (height_diff > MANTLE_HEIGHT && height_diff <= LEDGE_GRAB_HEIGHT)
            {
                // Verify ceiling clearance at the ledge
                if (tmceilingz - target_floor >= player->mo->height + 16*FRACUNIT)
                {
                    // Grab the ledge!
                    player->ledge_hanging = true;
                    player->ledge_height = target_floor;
                    player->ledge_x = checkx;
                    player->ledge_y = checky;
                    
                    // Stop vertical momentum and position player at ledge
                    player->mo->momz = 0;
                    player->mo->z = target_floor - player->mo->height + LEDGE_HANG_OFFSET;
                    player->mo->floorz = target_floor;
                    
                    // Reset other movement states
                    player->wall_contact = 0;
                    player->wallslide_tics = 0;
                }
            }
        }
    }
    // Handle jump to climb up from ledge
    else if (player->ledge_hanging && gamekeydown[key_jump])
    {
        // Start climbing animation
        fixed_t climb_distance = player->ledge_height - player->mo->z;
        player->ledge_climb_tics = climb_distance / LEDGE_CLIMB_HEIGHT;
        if (player->ledge_climb_tics < 1)
            player->ledge_climb_tics = 1;
    }
    // Handle dropping from ledge (release forward key)
    else if (player->ledge_hanging && cmd->forwardmove <= 0)
    {
        // Drop down from ledge
        player->ledge_hanging = false;
        player->ledge_climb_tics = 0;
    }
}



//
// P_DeathThink
// Fall on your face when dying.
// Decrease POV height to floor height.
//
#define ANG5   	(ANG90/18)

void P_DeathThink (player_t* player)
{
    angle_t		angle;
    angle_t		delta;

    P_MovePsprites (player);
	
    // fall to the ground
    if (player->viewheight > 6*FRACUNIT)
	player->viewheight -= FRACUNIT;

    if (player->viewheight < 6*FRACUNIT)
	player->viewheight = 6*FRACUNIT;

    player->deltaviewheight = 0;
    onground = (player->mo->z <= player->mo->floorz);
    P_CalcHeight (player);
	
    if (player->attacker && player->attacker != player->mo)
    {
	angle = R_PointToAngle2 (player->mo->x,
				 player->mo->y,
				 player->attacker->x,
				 player->attacker->y);
	
	delta = angle - player->mo->angle;
	
	if (delta < ANG5 || delta > (unsigned)-ANG5)
	{
	    // Looking at killer,
	    //  so fade damage flash down.
	    player->mo->angle = angle;

	    if (player->damagecount)
		player->damagecount--;
	}
	else if (delta < ANG180)
	    player->mo->angle += ANG5;
	else
	    player->mo->angle -= ANG5;
    }
    else if (player->damagecount)
	player->damagecount--;
	

    if (player->cmd.buttons & BT_USE)
    {
        // Goblin Dice Rollaz: Reset crit combo on death/respawn
        player->crit_combo = 0;
        player->crit_combo_timer = 0;
        player->playerstate = PST_REBORN;
    }
}



//
// P_PlayerThink
//
void P_PlayerThink (player_t* player)
{
    ticcmd_t*		cmd;
    weapontype_t	newweapon;
	
    // fixme: do this in the cheat code
    if (player->cheats & CF_NOCLIP)
	player->mo->flags |= MF_NOCLIP;
    else
	player->mo->flags &= ~MF_NOCLIP;
    
    // chain saw run forward
    cmd = &player->cmd;
    if (player->mo->flags & MF_JUSTATTACKED)
    {
	cmd->angleturn = 0;
	cmd->forwardmove = 0xc800/512;
	cmd->sidemove = 0;
	player->mo->flags &= ~MF_JUSTATTACKED;
    }
			
	
    if (player->playerstate == PST_DEAD)
    {
	P_DeathThink (player);
	return;
    }
    
    // Move around.
    // Reactiontime is used to prevent movement
    //  for a bit after a teleport.
    if (player->mo->reactiontime)
	player->mo->reactiontime--;
    else
	P_MovePlayer (player);
    
    P_CalcHeight (player);

    if (player->mo->subsector->sector->special)
	P_PlayerInSpecialSector (player);
    
    // Check for weapon change.

    // A special event has no other buttons.
    if (cmd->buttons & BT_SPECIAL)
	cmd->buttons = 0;			
		
    if (cmd->buttons & BT_CHANGE)
    {
	// The actual changing of the weapon is done
	//  when the weapon psprite can do it
	//  (read: not in the middle of an attack).
	newweapon = (cmd->buttons&BT_WEAPONMASK)>>BT_WEAPONSHIFT;
	
	if (newweapon == wp_fist
	    && player->weaponowned[wp_chainsaw]
	    && !(player->readyweapon == wp_chainsaw
		 && player->powers[pw_strength]))
	{
	    newweapon = wp_chainsaw;
	}
	
	if ( (gamemode == commercial)
	    && newweapon == wp_shotgun 
	    && player->weaponowned[wp_supershotgun]
	    && player->readyweapon != wp_supershotgun)
	{
	    newweapon = wp_supershotgun;
	}
	

	if (player->weaponowned[newweapon]
	    && newweapon != player->readyweapon)
	{
	    // Do not go to plasma or BFG in shareware,
	    //  even if cheated.
	    if ((newweapon != wp_plasma
		 && newweapon != wp_bfg)
		|| (gamemode != shareware) )
	    {
		player->pendingweapon = newweapon;
	    }
	}
    }
    
    // check for use
    if (cmd->buttons & BT_USE)
    {
	if (!player->usedown)
	{
	    if (player->mounted_siege_weapon)
	    {
		P_DismountSiegeWeapon(player);
	    }
	    else if (P_CanPlayerUseSiegeWeapon(player))
	    {
		P_TryMountSiegeWeapon(player);
	    }
	    else
	    {
		P_UseLines (player);
	    }
	    player->usedown = true;
	}
    }
    else
	player->usedown = false;

    if (player->mounted_siege_weapon && (cmd->buttons & BT_ATTACK))
    {
	if (!player->attackdown)
	{
	    P_FireSiegeWeapon(player);
	    player->attackdown = true;
	}
    }
    else if (!(cmd->buttons & BT_ATTACK))
    {
	player->attackdown = false;
    }
    
    // cycle psprites
    P_MovePsprites (player);
    
    // Counters, time dependend power ups.

    // Strength counts up to diminish fade.
    if (player->powers[pw_strength])
	player->powers[pw_strength]++;	
		
    if (player->powers[pw_invulnerability])
	player->powers[pw_invulnerability]--;

    if (player->powers[pw_invisibility])
	if (! --player->powers[pw_invisibility] )
	    player->mo->flags &= ~MF_SHADOW;
			
    if (player->powers[pw_infrared])
	player->powers[pw_infrared]--;
		
    if (player->powers[pw_ironfeet])
	player->powers[pw_ironfeet]--;

    if (player->powers[pw_critboost])
    {
        if (player->powers[pw_critboost] == 1)
        {
            S_StartSound(&player->mo->sphere, sfx_wpnup);
        }
        player->powers[pw_critboost]--;
    }

    if (player->powers[pw_doubledamage])
    {
        if (player->powers[pw_doubledamage] == 1)
        {
            S_StartSound(&player->mo->sphere, sfx_wpnup);
        }
        player->powers[pw_doubledamage]--;
    }

    if (player->powers[pw_trapsense])
    {
        player->powers[pw_trapsense]--;
    }

    if (player->powers[pw_glasscannon])
    {
        if (player->powers[pw_glasscannon] == 1)
        {
            // Powerup expiring - health will be restored in G_PowerupDeactivate
        }
        player->powers[pw_glasscannon]--;
    }

    // Goblin Dice Rollaz: Berserker Regen - regenerate health when below 50%
    if (player->powers[pw_berserkerregen])
    {
        player->powers[pw_berserkerregen]--;
        // Regenerate 1 HP every second (every 35 ticks) when below 50% health
        if (player->powers[pw_berserkerregen] % TICRATE == 0)
        {
            int maxhp = MAXHEALTH;
            int threshold = maxhp / 2;
            if (player->health < threshold && player->health > 0)
            {
                player->health++;
                player->mo->health = player->health;
            }
        }
    }

    // Goblin Dice Rollaz: Cursed D4 - 3x damage, no armor protection
    if (player->powers[pw_cursedd4])
    {
        player->powers[pw_cursedd4]--;
    }

    // Goblin Dice Rollaz: Blood Pact - infinite ammo, health drain
    if (player->powers[pw_bloodpact])
    {
        player->powers[pw_bloodpact]--;
        // Drain 1 HP every second (every 35 ticks)
        if (player->powers[pw_bloodpact] > 0 && player->powers[pw_bloodpact] % TICRATE == 0)
        {
            if (player->health > 1)
            {
                player->health--;
                player->mo->health = player->health;
            }
            else
            {
                // Player dies from blood pact drain
                player->powers[pw_bloodpact] = 0;
            }
        }
    }
		
    if (player->damagecount)
	player->damagecount--;
		
    if (player->bonuscount)
	player->bonuscount--;

    
    // Handling colormaps.
    if (player->powers[pw_invulnerability])
    {
	if (player->powers[pw_invulnerability] > 4*32
	    || (player->powers[pw_invulnerability]&8) )
	    player->fixedcolormap = INVERSECOLORMAP;
	else
	    player->fixedcolormap = 0;
    }
    else if (player->powers[pw_critboost])
    {
	player->fixedcolormap = CRITCOLORMAP;
    }
    else if (player->powers[pw_doubledamage])
    {
	player->fixedcolormap = DOUBLEDAMAGECOLORMAP;
    }
    else if (player->powers[pw_glasscannon])
    {
	player->fixedcolormap = CRITCOLORMAP;
    }
    else if (player->powers[pw_infrared])
    {
	if (player->powers[pw_infrared] > 4*32
	    || (player->powers[pw_infrared]&8) )
	{
	    // almost full bright
	    player->fixedcolormap = 1;
	}
	else
	    player->fixedcolormap = 0;
    }
    else
	player->fixedcolormap = 0;
}


