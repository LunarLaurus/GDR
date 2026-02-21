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
//	Weapon sprite animation, weapon objects.
//	Action functions for weapons.
//


#include "doomdef.h"
#include "d_event.h"

#include "deh_misc.h"

#include "m_random.h"
#include "p_local.h"
#include "d_items.h"
#include "s_sound.h"

// State.
#include "doomstat.h"

// Data.
#include "sounds.h"

#include "p_pspr.h"

#define LOWERSPEED		FRACUNIT*6
#define RAISESPEED		FRACUNIT*6

#define WEAPONBOTTOM	128*FRACUNIT
#define WEAPONTOP		32*FRACUNIT



//
// P_SetPsprite
//
void
P_SetPsprite
( player_t*	player,
  int		position,
  statenum_t	stnum ) 
{
    pspdef_t*	psp;
    state_t*	state;
	
    psp = &player->psprites[position];
	
    do
    {
	if (!stnum)
	{
	    // object removed itself
	    psp->state = NULL;
	    break;	
	}
	
	state = &states[stnum];
	psp->state = state;
	psp->tics = state->tics;	// could be 0

	if (state->misc1)
	{
	    // coordinate set
	    psp->sx = state->misc1 << FRACBITS;
	    psp->sy = state->misc2 << FRACBITS;
	}
	
	// Call action routine.
	// Modified handling.
	if (state->action.acp2)
	{
	    state->action.acp2(player, psp);
	    if (!psp->state)
		break;
	}
	
	stnum = psp->state->nextstate;
	
    } while (!psp->tics);
    // an initial state of 0 could cycle through
}



//
// P_CalcSwing
//	
fixed_t		swingx;
fixed_t		swingy;

void P_CalcSwing (player_t*	player)
{
    fixed_t	swing;
    int		angle;
	
    // OPTIMIZE: tablify this.
    // A LUT would allow for different modes,
    //  and add flexibility.

    swing = player->bob;

    angle = (FINEANGLES/70*leveltime)&FINEMASK;
    swingx = FixedMul ( swing, finesine[angle]);

    angle = (FINEANGLES/70*leveltime+FINEANGLES/2)&FINEMASK;
    swingy = -FixedMul ( swingx, finesine[angle]);
}



//
// P_BringUpWeapon
// Starts bringing the pending weapon up
// from the bottom of the screen.
// Uses player
//
void P_BringUpWeapon (player_t* player)
{
    statenum_t	newstate;
	
    if (player->pendingweapon == wp_nochange)
	player->pendingweapon = player->readyweapon;
		
    if (player->pendingweapon == wp_chainsaw)
	S_StartSound (player->mo, sfx_sawup);
		
    newstate = weaponinfo[player->pendingweapon].upstate;

    player->pendingweapon = wp_nochange;
    player->psprites[ps_weapon].sy = WEAPONBOTTOM;

    P_SetPsprite (player, ps_weapon, newstate);
}

//
// P_CheckAmmo
// Returns true if there is enough ammo to shoot.
// If not, selects the next weapon to use.
//
boolean P_CheckAmmo (player_t* player)
{
    ammotype_t		ammo;
    int			count;

    ammo = weaponinfo[player->readyweapon].ammo;

    // Minimal amount for one shot varies.
    if (player->readyweapon == wp_bfg)
	count = deh_bfg_cells_per_shot;
    else if (player->readyweapon == wp_supershotgun)
	count = 2;	// Double barrel.
    else
	count = 1;	// Regular.

    // Some do not need ammunition anyway.
    // Return if current ammunition sufficient.
    if (ammo == am_noammo || player->ammo[ammo] >= count)
	return true;
		
    // Out of ammo, pick a weapon to change to.
    // Preferences are set here.
    do
    {
	if (player->weaponowned[wp_plasma]
	    && player->ammo[am_cell]
	    && (gamemode != shareware) )
	{
	    player->pendingweapon = wp_plasma;
	}
	else if (player->weaponowned[wp_supershotgun] 
		 && player->ammo[am_shell]>2
		 && (gamemode == commercial) )
	{
	    player->pendingweapon = wp_supershotgun;
	}
	else if (player->weaponowned[wp_chaingun]
		 && player->ammo[am_clip])
	{
	    player->pendingweapon = wp_chaingun;
	}
	else if (player->weaponowned[wp_shotgun]
		 && player->ammo[am_shell])
	{
	    player->pendingweapon = wp_shotgun;
	}
	else if (player->ammo[am_clip])
	{
	    player->pendingweapon = wp_pistol;
	}
	else if (player->weaponowned[wp_chainsaw])
	{
	    player->pendingweapon = wp_chainsaw;
	}
	else if (player->weaponowned[wp_missile]
		 && player->ammo[am_misl])
	{
	    player->pendingweapon = wp_missile;
	}
	else if (player->weaponowned[wp_bfg]
		 && player->ammo[am_cell]>40
		 && (gamemode != shareware) )
	{
	    player->pendingweapon = wp_bfg;
	}
	else
	{
	    // If everything fails.
	    player->pendingweapon = wp_fist;
	}
	
    } while (player->pendingweapon == wp_nochange);

    // Now set appropriate weapon overlay.
    P_SetPsprite (player,
		  ps_weapon,
		  weaponinfo[player->readyweapon].downstate);

    return false;	
}


//
// P_FireWeapon.
//
void P_FireWeapon (player_t* player)
{
    statenum_t	newstate;
	
    if (!P_CheckAmmo (player))
	return;
	
    P_SetMobjState (player->mo, S_PLAY_ATK1);
    newstate = weaponinfo[player->readyweapon].atkstate;
    P_SetPsprite (player, ps_weapon, newstate);
    P_NoiseAlert (player->mo, player->mo);
}



//
// P_FireAltWeapon - Goblin Dice Rollaz: Alternate fire mode
// Handles special attack modes for dice weapons
//
void P_FireAltWeapon (player_t* player)
{
    if (!P_CheckAmmo (player))
	return;

    P_SetMobjState (player->mo, S_PLAY_ATK1);
    P_NoiseAlert (player->mo, player->mo);

    // Switch based on weapon type for alternate fire modes
    switch (player->readyweapon)
    {
    case wp_d4:
    {
        // Goblin Dice Rollaz: d4 Rapid Burst Throw
        // Fire 3 d4 projectiles in quick succession with slight spread
        int i;
        int damage;
        int critRoll;
        int guaranteedCrit = 0;
        
        // d4 Rapid Burst Throw - quick whoosh sound
        S_StartSound (player->mo, sfx_dice_d4);
        
        // Check for guaranteed crit from Dice Fortune powerup
        if (player->powers[pw_dicefortune])
        {
            guaranteedCrit = 1;
            player->powers[pw_dicefortune] = 0;
            player->message = "CRITICAL BURST!";
        }
        
        // Fire 3 projectiles in rapid succession
        for (i = 0; i < 3; i++)
        {
            // Calculate damage for each projectile
            damage = P_CalculateDiceDamage(wp_d4, guaranteedCrit, &critRoll);
            
            // Add slight spread for burst inaccuracy
            if (i == 0)
            {
                // First shot - accurate
                P_BulletSlope(player->mo);
                P_GunShotWithDamage(player->mo, true, damage);
            }
            else if (i == 1)
            {
                // Second shot - slightly off
                P_BulletSlope(player->mo);
                P_GunShotWithDamage(player->mo, false, damage);
            }
            else
            {
                // Third shot - more spread
                P_BulletSlope(player->mo);
                P_GunShotWithDamage(player->mo, false, damage);
            }
        }
        
        // Consume 3 ammo for the burst
        DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 3);
        
        // Show flash
        P_SetPsprite(player, ps_flash, weaponinfo[player->readyweapon].flashstate);
        break;
    }
    case wp_d12:
    {
        // Goblin Dice Rollaz: d12 Charged Smash
        // Long windup with bonus crit chance based on charge time
        int damage;
        int critRoll;
        int guaranteedCrit = 0;
        int chargeBonus = player->weaponcharge;
        
        // Consume ammo
        DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);
        
        // Check for guaranteed crit from Dice Fortune powerup
        if (player->powers[pw_dicefortune])
        {
            guaranteedCrit = 1;
            player->powers[pw_dicefortune] = 0;
            player->message = "CRITICAL SMASH!";
        }
        
        // Calculate base damage
        damage = P_CalculateDiceDamage(wp_d12, guaranteedCrit, &critRoll);
        
        // Apply charge bonus: up to +100% damage at max charge
        // weaponcharge ranges from 0-35 (about 1 second at 35fps)
        if (chargeBonus > 0)
        {
            int bonusPct = (chargeBonus * 100) / 35;  // 0-100%
            damage += (damage * bonusPct) / 100;
            
            // Bonus crit chance: up to +25% at max charge
            // Base crit is 8%, max charge adds 25% = up to 33% crit chance
            if (chargeBonus >= 35 && critRoll == 12)
            {
                damage *= 2;  // Apply crit multiplier
                player->message = "MAX CHARGE CRIT!";
            }
        }
        
        // Reset charge
        player->weaponcharge = 0;
        
        // Play sound based on charge level - use d12 heavy impact sounds
        if (chargeBonus >= 35)
            S_StartSound(player->mo, sfx_dice_d12);  // Full charged - thunderous throw
        else if (chargeBonus >= 20)
            S_StartSound(player->mo, sfx_dice_d12);  // Partial charge - solid hit
        else
            S_StartSound(player->mo, sfx_dice_d12);  // Light charge - light toss
        
        // Show appropriate flash
        if (chargeBonus >= 20)
            P_SetPsprite(player, ps_flash, weaponinfo[player->readyweapon].flashstate);
        
        // Fire the attack
        P_BulletSlope(player->mo);
        P_GunShotWithDamage(player->mo, true, damage);
        
        // Apply knockback for charged attack
        if (chargeBonus >= 20 && player->mo->target)
        {
            angle_t pushAngle = R_PointToAngle2(player->mo->x, player->mo->y,
                                                player->mo->target->x, player->mo->target->y);
            player->mo->target->momx += FixedMul(chargeBonus * 20, cos(pushAngle));
            player->mo->target->momy += FixedMul(chargeBonus * 20, sin(pushAngle));
        }
        break;
    }
    case wp_percentile:
	break;
    default:
	break;
    }
}



//
// P_DropWeapon
// Player died, so put the weapon away.
//
void P_DropWeapon (player_t* player)
{
    P_SetPsprite (player,
		  ps_weapon,
		  weaponinfo[player->readyweapon].downstate);
}



//
// A_WeaponReady
// The player can fire the weapon
// or change to another weapon at this time.
// Follows after getting weapon up,
// or after previous attack/fire sequence.
//
void
A_WeaponReady
( player_t*	player,
  pspdef_t*	psp )
{	
    statenum_t	newstate;
    int		angle;
    
    // get out of attack state
    if (player->mo->state == &states[S_PLAY_ATK1]
	|| player->mo->state == &states[S_PLAY_ATK2] )
    {
	P_SetMobjState (player->mo, S_PLAY);
    }
    
    if (player->readyweapon == wp_chainsaw
	&& psp->state == &states[S_SAW])
    {
	S_StartSound (player->mo, sfx_sawidl);
    }
    
    // check for change
    //  if player is dead, put the weapon away
    if (player->pendingweapon != wp_nochange || !player->health)
    {
	// change weapon
	//  (pending weapon should allready be validated)
	newstate = weaponinfo[player->readyweapon].downstate;
	P_SetPsprite (player, ps_weapon, newstate);
	return;	
    }
    
    // check for fire
    //  the missile launcher and bfg do not auto fire
    if (player->cmd.buttons & BT_ATTACK)
    {
	if ( !player->attackdown
	     || (player->readyweapon != wp_missile
		 && player->readyweapon != wp_bfg) )
	{
	    player->attackdown = true;
	    P_FireWeapon (player);		
	    return;
	}
    }
    else
	player->attackdown = false;

    // Goblin Dice Rollaz: Check for alternate fire
    if (player->cmd.buttons & BT_ALTATTACK)
    {
	if (!player->altattackdown)
	{
	    // Button just pressed - start charging for d12
	    if (player->readyweapon == wp_d12)
	    {
		player->altattackdown = true;
		player->weaponcharge = 0;
		// Show charging state - don't fire yet
		player->message = "Charging...";
		return;
	    }
	    player->altattackdown = true;
	    P_FireAltWeapon (player);
	    return;
	}
	else
	{
	    // Button is being held - increment charge for d12
	    if (player->readyweapon == wp_d12 && player->weaponcharge < 35)
	    {
		player->weaponcharge++;
	    }
	}
    }
    else
    {
	// Button released - fire charged attack
	if (player->altattackdown && player->readyweapon == wp_d12 && player->weaponcharge > 0)
	{
	    player->altattackdown = false;
	    P_FireAltWeapon (player);
	    return;
	}
	player->altattackdown = false;
    }
    
    // bob the weapon based on movement speed
    angle = (128*leveltime)&FINEMASK;
    psp->sx = FRACUNIT + FixedMul (player->bob, finecosine[angle]);
    angle &= FINEANGLES/2-1;
    psp->sy = WEAPONTOP + FixedMul (player->bob, finesine[angle]);
}



//
// A_ReFire
// The player can re-fire the weapon
// without lowering it entirely.
//
void A_ReFire
( player_t*	player,
  pspdef_t*	psp )
{
    
    // check for fire
    //  (if a weaponchange is pending, let it go through instead)
    if ( (player->cmd.buttons & BT_ATTACK) 
	 && player->pendingweapon == wp_nochange
	 && player->health)
    {
	player->refire++;
	P_FireWeapon (player);
    }
    else
    {
	player->refire = 0;
	P_CheckAmmo (player);
    }
}


void
A_CheckReload
( player_t*	player,
  pspdef_t*	psp )
{
    P_CheckAmmo (player);
#if 0
    if (player->ammo[am_shell]<2)
	P_SetPsprite (player, ps_weapon, S_DSNR1);
#endif
}



//
// A_Lower
// Lowers current weapon,
//  and changes weapon at bottom.
//
void
A_Lower
( player_t*	player,
  pspdef_t*	psp )
{	
    psp->sy += LOWERSPEED;

    // Is already down.
    if (psp->sy < WEAPONBOTTOM )
	return;

    // Player is dead.
    if (player->playerstate == PST_DEAD)
    {
	psp->sy = WEAPONBOTTOM;

	// don't bring weapon back up
	return;		
    }
    
    // The old weapon has been lowered off the screen,
    // so change the weapon and start raising it
    if (!player->health)
    {
	// Player is dead, so keep the weapon off screen.
	P_SetPsprite (player,  ps_weapon, S_NULL);
	return;	
    }
	
    player->readyweapon = player->pendingweapon; 

    P_BringUpWeapon (player);
}


//
// A_Raise
//
void
A_Raise
( player_t*	player,
  pspdef_t*	psp )
{
    statenum_t	newstate;
	
    psp->sy -= RAISESPEED;

    if (psp->sy > WEAPONTOP )
	return;
    
    psp->sy = WEAPONTOP;
    
    // The weapon has been raised all the way,
    //  so change to the ready state.
    newstate = weaponinfo[player->readyweapon].readystate;

    P_SetPsprite (player, ps_weapon, newstate);
}



//
// A_GunFlash
//
void
A_GunFlash
( player_t*	player,
  pspdef_t*	psp ) 
{
    P_SetMobjState (player->mo, S_PLAY_ATK2);
    P_SetPsprite (player,ps_flash,weaponinfo[player->readyweapon].flashstate);
}



//
// WEAPON ATTACKS
//


//
// A_Punch
//
void
A_Punch
( player_t*	player,
  pspdef_t*	psp ) 
{
    angle_t	angle;
    int		damage;
    int		slope;
	
    damage = (P_Random ()%10+1)<<1;

    if (player->powers[pw_strength])	
	damage *= 10;

    angle = player->mo->angle;
    angle += P_SubRandom() << 18;
    slope = P_AimLineAttack (player->mo, angle, MELEERANGE);
    P_LineAttack (player->mo, angle, MELEERANGE, slope, damage);

    // turn to face target
    if (linetarget)
    {
	S_StartSound (player->mo, sfx_punch);
	player->mo->angle = R_PointToAngle2 (player->mo->x,
					     player->mo->y,
					     linetarget->x,
					     linetarget->y);
    }
}


//
// A_Saw
//
void
A_Saw
( player_t*	player,
  pspdef_t*	psp ) 
{
    angle_t	angle;
    int		damage;
    int		slope;

    damage = 2*(P_Random ()%10+1);
    angle = player->mo->angle;
    angle += P_SubRandom() << 18;
    
    // use meleerange + 1 se the puff doesn't skip the flash
    slope = P_AimLineAttack (player->mo, angle, MELEERANGE+1);
    P_LineAttack (player->mo, angle, MELEERANGE+1, slope, damage);

    if (!linetarget)
    {
	S_StartSound (player->mo, sfx_sawful);
	return;
    }
    S_StartSound (player->mo, sfx_sawhit);
	
    // turn to face target
    angle = R_PointToAngle2 (player->mo->x, player->mo->y,
			     linetarget->x, linetarget->y);
    if (angle - player->mo->angle > ANG180)
    {
	if ((signed int) (angle - player->mo->angle) < -ANG90/20)
	    player->mo->angle = angle + ANG90/21;
	else
	    player->mo->angle -= ANG90/20;
    }
    else
    {
	if (angle - player->mo->angle > ANG90/20)
	    player->mo->angle = angle - ANG90/21;
	else
	    player->mo->angle += ANG90/20;
    }
    player->mo->flags |= MF_JUSTATTACKED;
}

// Doom does not check the bounds of the ammo array.  As a result,
// it is possible to use an ammo type > 4 that overflows into the
// maxammo array and affects that instead.  Through dehacked, for
// example, it is possible to make a weapon that decreases the max
// number of ammo for another weapon.  Emulate this.

static void DecreaseAmmo(player_t *player, int ammonum, int amount)
{
    if (player->cheats & CF_INFINITE_AMMO)
    {
        return;
    }

    if (ammonum < NUMAMMO)
    {
        player->ammo[ammonum] -= amount;
    }
    else
    {
        player->maxammo[ammonum - NUMAMMO] -= amount;
    }
}


//
// P_RollDice - Goblin Dice Rollaz: Centralized dice roll function
// Uses deterministic Doom RNG for netplay compatibility
//
int
P_RollDice (int sides)
{
    if (sides <= 0)
        return 0;
    return (P_Random() % sides) + 1;
}


//
// P_WeaponCanCrit - Check if weapon can score critical hits
//
int
P_WeaponCanCrit (int weapon)
{
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return 0;
    return dice_weapon_info[weapon].die_type > 0 ? 1 : 0;
}


//
// P_CalculateDiceDamage - Goblin Dice Rollaz: Centralized damage calculation
// Handles guaranteed crit, dice rolling, and damage mapping
// Supports Gamble Shot mode for wider variance
//
int
P_CalculateDiceDamage (int weapon, int guaranteedCrit, int *outCritRoll)
{
    int diceRoll;
    int damage = 0;
    dice_weapon_info_t *dwi;
    
    if (outCritRoll)
        *outCritRoll = 0;
    
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return 1;
    
    dwi = &dice_weapon_info[weapon];
    
    if (dwi->die_type == 0)
        return 1;
    
    if (guaranteedCrit)
    {
        diceRoll = dwi->die_type;
    }
    else
    {
        diceRoll = P_RollDice(dwi->die_type);
    }
    
    if (diceRoll == dwi->crit_roll)
    {
        if (outCritRoll)
            *outCritRoll = diceRoll;
        damage = dwi->damage_table[6] * dwi->crit_multiplier;
    }
    else
    {
        int bucket;
        if (dwi->die_type <= 6)
        {
            bucket = diceRoll - 1;
        }
        else if (dwi->die_type <= 12)
        {
            bucket = ((diceRoll - 1) * 6) / dwi->die_type;
        }
        else if (dwi->die_type <= 20)
        {
            bucket = ((diceRoll - 1) * 6) / dwi->die_type;
        }
        else
        {
            bucket = ((diceRoll - 1) * 6) / dwi->die_type;
        }
        if (bucket > 5) bucket = 5;
        if (bucket < 0) bucket = 0;
        
        damage = dwi->damage_table[bucket];
        if (damage == 0)
            damage = dwi->min_damage;
    }
    
    // Goblin Dice Rollaz: Gamble Shot - exploding roll mechanic
    // For gamble_shot weapons, rolls >= 95 trigger additional rolls
    if (dwi->gamble_shot && diceRoll >= 95 && diceRoll < dwi->crit_roll)
    {
        int explodeRolls = 0;
        int additionalDamage = 0;
        
        // Roll again for each point above 95 (up to 3 extra rolls)
        while (diceRoll >= 95 && explodeRolls < 3)
        {
            explodeRolls++;
            diceRoll = P_RollDice(dwi->die_type);
            
            // Add damage from explosion
            if (diceRoll == dwi->crit_roll)
            {
                additionalDamage += dwi->damage_table[6] * dwi->crit_multiplier / 2;
            }
            else
            {
                int bucket = ((diceRoll - 1) * 6) / dwi->die_type;
                if (bucket > 5) bucket = 5;
                if (bucket < 0) bucket = 0;
                additionalDamage += dwi->damage_table[bucket];
            }
        }
        
        damage += additionalDamage;
    }
    
    return damage;
}


//
// A_FireMissile
//
void
A_FireMissile
( player_t*	player,
  pspdef_t*	psp ) 
{
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);
    P_SpawnPlayerMissile (player->mo, MT_ROCKET);
}


//
// A_FireBFG
//
void
A_FireBFG
( player_t*	player,
  pspdef_t*	psp ) 
{
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 
                 deh_bfg_cells_per_shot);
    P_SpawnPlayerMissile (player->mo, MT_BFG);
}



//
// A_FirePlasma
//
void
A_FirePlasma
( player_t*	player,
  pspdef_t*	psp ) 
{
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1) );

    P_SpawnPlayerMissile (player->mo, MT_PLASMA);
}



//
// P_BulletSlope
// Sets a slope so a near miss is at aproximately
// the height of the intended target
//
fixed_t		bulletslope;


void P_BulletSlope (mobj_t*	mo)
{
    angle_t	an;
    
    // see which target is to be aimed at
    an = mo->angle;
    bulletslope = P_AimLineAttack (mo, an, 16*64*FRACUNIT);

    if (!linetarget)
    {
	an += 1<<26;
	bulletslope = P_AimLineAttack (mo, an, 16*64*FRACUNIT);
	if (!linetarget)
	{
	    an -= 2<<26;
	    bulletslope = P_AimLineAttack (mo, an, 16*64*FRACUNIT);
	}
    }
}


//
// P_GunShot
//
void
P_GunShot
( mobj_t*	mo,
  boolean	accurate )
{
    angle_t	angle;
    int		damage;
	
    damage = 5*(P_Random ()%3+1);
    angle = mo->angle;

    if (!accurate)
	angle += P_SubRandom() << 18;

    P_LineAttack (mo, angle, MISSILERANGE, bulletslope, damage);
}

//
// P_GunShotWithDamage - Goblin Dice Rollaz
// Variant that accepts custom damage (for dice weapons)
//
void
P_GunShotWithDamage
( mobj_t*	mo,
  boolean	accurate,
  int		damage )
{
    angle_t	angle;
	
    angle = mo->angle;

    if (!accurate)
	angle += P_SubRandom() << 18;

    P_LineAttack (mo, angle, MISSILERANGE, bulletslope, damage);
}


//
// A_FirePistol
//
void
A_FirePistol
( player_t*	player,
  pspdef_t*	psp ) 
{
    S_StartSound (player->mo, sfx_pistol);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    P_BulletSlope (player->mo);
    P_GunShot (player->mo, !player->refire);
}


//
// A_FireD6Blast - Goblin Dice Rollaz d6 blaster
// Uses shared dice-roll backend
//
void
A_FireD6Blast
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    
    S_StartSound (player->mo, sfx_dice_d6);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d6blaster, guaranteedCrit, &critRoll);

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, !player->refire, damage);
}


//
// A_FireShotgun
//
void
A_FireShotgun
( player_t*	player,
  pspdef_t*	psp ) 
{
    int		i;
	
    S_StartSound (player->mo, sfx_shotgn);
    P_SetMobjState (player->mo, S_PLAY_ATK2);

    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    P_BulletSlope (player->mo);
	
    for (i=0 ; i<7 ; i++)
	P_GunShot (player->mo, false);
}



//
// A_FireShotgun2
//
void
A_FireShotgun2
( player_t*	player,
  pspdef_t*	psp ) 
{
    int		i;
    angle_t	angle;
    int		damage;
		
	
    S_StartSound (player->mo, sfx_dshtgn);
    P_SetMobjState (player->mo, S_PLAY_ATK2);

    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 2);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    P_BulletSlope (player->mo);
	
    for (i=0 ; i<20 ; i++)
    {
	damage = 5*(P_Random ()%3+1);
	angle = player->mo->angle;
	angle += P_SubRandom() << ANGLETOFINESHIFT;
	P_LineAttack (player->mo,
		      angle,
		      MISSILERANGE,
		      bulletslope + (P_SubRandom() << 5), damage);
    }
}


//
// A_FireCGun
//
void
A_FireCGun
( player_t*	player,
  pspdef_t*	psp ) 
{
    S_StartSound (player->mo, sfx_chgun);

    if (!player->ammo[weaponinfo[player->readyweapon].ammo])
	return;
		
    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate
		  + psp->state
		  - &states[S_CHAIN1] );

    P_BulletSlope (player->mo);
	
    P_GunShot (player->mo, !player->refire);
}


//
// A_FireD20Cannon - Goblin Dice Rollaz d20 cannon
// Uses shared dice-roll backend
//
void
A_FireD20Cannon
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d20cannon, guaranteedCrit, &critRoll);

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, !player->refire, damage);
}


//
// A_FireD12 - Goblin Dice Rollaz d12 heavy impact weapon
// Uses shared dice-roll backend
//
void
A_FireD12
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    
    S_StartSound (player->mo, sfx_dice_d12);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d12, guaranteedCrit, &critRoll);

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, !player->refire, damage);
}


//
// A_FirePercentile - Goblin Dice Rollaz percentile dice weapon
// Uses shared dice-roll backend
//
void
A_FirePercentile
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    
    S_StartSound (player->mo, sfx_dice_percent);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_percentile, guaranteedCrit, &critRoll);

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, !player->refire, damage);
}


//
// A_FireD4 - Goblin Dice Rollaz d4 throwing knives
// Uses shared dice-roll backend
//
void
A_FireD4
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    
    S_StartSound (player->mo, sfx_dice_d4);

    P_SetMobjState (player->mo, S_PLAY_ATK1);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d4, guaranteedCrit, &critRoll);

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, !player->refire, damage);
}



//
// ?
//
void A_Light0 (player_t *player, pspdef_t *psp)
{
    player->extralight = 0;
}

void A_Light1 (player_t *player, pspdef_t *psp)
{
    player->extralight = 1;
}

void A_Light2 (player_t *player, pspdef_t *psp)
{
    player->extralight = 2;
}


//
// A_BFGSpray
// Spawn a BFG explosion on every monster in view
//
void A_BFGSpray (mobj_t* mo) 
{
    int			i;
    int			j;
    int			damage;
    angle_t		an;
	
    // offset angles from its attack angle
    for (i=0 ; i<40 ; i++)
    {
	an = mo->angle - ANG90/2 + ANG90/40*i;

	// mo->target is the originator (player)
	//  of the missile
	P_AimLineAttack (mo->target, an, 16*64*FRACUNIT);

	if (!linetarget)
	    continue;

	P_SpawnMobj (linetarget->x,
		     linetarget->y,
		     linetarget->z + (linetarget->height>>2),
		     MT_EXTRABFG);
	
	damage = 0;
	for (j=0;j<15;j++)
	    damage += (P_Random()&7) + 1;

	P_DamageMobj (linetarget, mo->target,mo->target, damage);
    }
}


//
// A_BFGsound
//
void
A_BFGsound
( player_t*	player,
  pspdef_t*	psp )
{
    S_StartSound (player->mo, sfx_bfg);
}



//
// P_SetupPsprites
// Called at start of level for each player.
//
void P_SetupPsprites (player_t* player) 
{
    int	i;
	
    // remove all psprites
    for (i=0 ; i<NUMPSPRITES ; i++)
	player->psprites[i].state = NULL;
		
    // spawn the gun
    player->pendingweapon = player->readyweapon;
    P_BringUpWeapon (player);
}




//
// P_MovePsprites
// Called every tic by player thinking routine.
//
void P_MovePsprites (player_t* player) 
{
    int		i;
    pspdef_t*	psp;
	
    psp = &player->psprites[0];
    for (i=0 ; i<NUMPSPRITES ; i++, psp++)
    {
	// a null state means not active
	if (psp->state)
	{
	    // drop tic count and possibly change state

	    // a -1 tic count never changes
	    if (psp->tics != -1)	
	    {
		psp->tics--;
		if (!psp->tics)
		    P_SetPsprite (player, i, psp->state->nextstate);
	    }				
	}
    }
    
    player->psprites[ps_flash].sx = player->psprites[ps_weapon].sx;
    player->psprites[ps_flash].sy = player->psprites[ps_weapon].sy;
}


