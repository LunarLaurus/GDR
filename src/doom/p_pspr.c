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
#include "g_rpg.h"
#include "g_balance.h"
#include "g_stats.h"
#include "r_main.h"

// State.
#include "doomstat.h"

// Data.
#include "sounds.h"

#include "p_pspr.h"

#define LOWERSPEED		FRACUNIT*6
#define RAISESPEED		FRACUNIT*6

#define WEAPONBOTTOM	128*FRACUNIT
#define WEAPONTOP		32*FRACUNIT

// Goblin Dice Rollaz: Weapon recoil values per weapon
// Defines how much the weapon kicks back when fired
static const fixed_t weapon_recoil_values[NUMWEAPONS] = {
    0,              // wp_fist
    FRACUNIT/4,    // wp_pistol
    FRACUNIT/2,    // wp_shotgun
    FRACUNIT/3,    // wp_chaingun
    FRACUNIT/2,    // wp_missile
    0,              // wp_chainsaw
    FRACUNIT/4,    // wp_plasma
    FRACUNIT,      // wp_bfg
    FRACUNIT/4,    // wp_supershotgun
    FRACUNIT/2,    // wp_d6blaster - Goblin Dice Rollaz
    FRACUNIT/2,    // wp_d20cannon - Goblin Dice Rollaz
    FRACUNIT,      // wp_d12 - Goblin Dice Rollaz
    FRACUNIT/4,    // wp_percentile - Goblin Dice Rollaz
    FRACUNIT/3,    // wp_d4 - Goblin Dice Rollaz
    FRACUNIT/3,    // wp_d8 - Goblin Dice Rollaz
    FRACUNIT/2,    // wp_d10 - Goblin Dice Rollaz
    FRACUNIT/2,    // wp_twind6 - Goblin Dice Rollaz
    FRACUNIT/4,    // wp_arcaned20 - Goblin Dice Rollaz
    FRACUNIT,      // wp_cursed - Goblin Dice Rollaz
    FRACUNIT/5,    // wp_d2 - Goblin Dice Rollaz (low recoil)
    FRACUNIT/5,    // wp_d3 - Goblin Dice Rollaz (low recoil, piercing)
    FRACUNIT/4,    // wp_d7 - Goblin Dice Rollaz (ore fragment splash)
    FRACUNIT*3/4,  // wp_d14 - Goblin Dice Rollaz (dual dice, resonance bonus)
    FRACUNIT*3/5,  // wp_d24 - Goblin Dice Rollaz (slows enemies, freezes on crit)
    FRACUNIT*3/4,  // wp_d30 - Goblin Dice Rollaz (bonus vs armored dwarves)
    FRACUNIT,      // wp_d48 - Goblin Dice Rollaz (molten effect on crit, fire dmg)
    FRACUNIT*3/4,  // wp_d50 - Goblin Dice Rollaz (3x vs dwarf enemies)
    FRACUNIT,      // wp_d60 - Goblin Dice Rollaz (elemental chaos, random element)
    FRACUNIT,      // wp_d100plus1 - Goblin Dice Rollaz (no fumbles, 4x on crit)
    FRACUNIT*3/4,  // wp_quake - Goblin Dice Rollaz (shockwave, stun)
    FRACUNIT/3,    // wp_crystalshatter - Goblin Dice Rollaz (shard spray, shield break)
};



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

    // Goblin Dice Rollaz: Apply weapon recoil based on weapon type
    player->weapon_recoil = weapon_recoil_values[player->readyweapon];
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
        int highestRollInBurst = 0;
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
            int misfire = 0;
            int diceRoll = 0;
            // Calculate damage for each projectile
            damage = P_CalculateDiceDamage(wp_d4, guaranteedCrit, &critRoll, &misfire, &diceRoll, player);
            
            // Track highest roll in burst for screen shake
            if (diceRoll > highestRollInBurst)
                highestRollInBurst = diceRoll;
            
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
        
        // Screen shake for burst crits or high rolls (>= 75% of max die = 3 on d4)
        if (critRoll > 0 || highestRollInBurst >= 3)
        {
            int shakeIntensity = (critRoll > 0) ? (FRACUNIT * 3) : (FRACUNIT * 2);
            R_TriggerScreenShake(shakeIntensity, 4);
        }
        break;
    }
    case wp_d12:
    {
        // Goblin Dice Rollaz: d12 Charged Smash
        // Long windup with bonus crit chance based on charge time
        int damage;
        int critRoll;
        int diceRoll;
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
        int misfire = 0;
        damage = P_CalculateDiceDamage(wp_d12, guaranteedCrit, &critRoll, &misfire, &diceRoll, player);
        
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
            int ang = pushAngle >> ANGLETOFINESHIFT;
            player->mo->target->momx += FixedMul(chargeBonus * 20, finecosine[ang]);
            player->mo->target->momy += FixedMul(chargeBonus * 20, finesine[ang]);
        }
        
        // Screen shake for charged attack crits or high rolls (>= 75% = 9 on d12)
        if (critRoll > 0 || diceRoll >= 9)
        {
            int shakeIntensity = (critRoll > 0 || chargeBonus >= 35) ? (FRACUNIT * 4) : (FRACUNIT * 2);
            R_TriggerScreenShake(shakeIntensity, 6);
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
    
    // Goblin Dice Rollaz: Apply weapon recoil offset
    // Decay the recoil each frame (simple linear decay)
    fixed_t current_recoil = player->weapon_recoil;
    if (current_recoil > 0)
    {
        player->weapon_recoil -= FRACUNIT / 8;
        if (player->weapon_recoil < 0)
            player->weapon_recoil = 0;
    }
    
    // bob the weapon based on movement speed
    angle = (128*leveltime)&FINEMASK;
    psp->sx = FRACUNIT + FixedMul (player->bob, finecosine[angle]);
    angle &= FINEANGLES/2-1;
    // Goblin Dice Rollaz: Add recoil offset to weapon Y position
    psp->sy = WEAPONTOP + FixedMul (player->bob, finesine[angle]) + current_recoil;
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
        R_TriggerScreenShake(FRACUNIT * 1, 2);
    }
}


//
// A_FireD100Plus1 - Goblin Dice Rollaz d100+1 Fumble Finder weapon
// No fumbles - rolling 1 still deals minimum damage
// 4x damage multiplier on crit (rolling 100)
// 
void
A_FireD100Plus1
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d100plus1, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d100plus1];

    missile = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_d100plus1);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 8, 16);
        player->message = "FUMBLE PROOF!";
    }
    else if (diceRoll >= 80)
    {
        R_TriggerScreenShake(FRACUNIT * 5, 10);
        player->message = "EXTREME LUCK!";
    }
    else if (diceRoll >= 50)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
        player->message = "GREAT ROLL!";
    }
    else if (diceRoll >= 25)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 2);
    }
}


//
// A_FireQuake - Goblin Dice Rollaz d3 Quake Boulder weapon
// Creates a shockwave on impact that damages nearby enemies
// Critical hits apply a stun effect
// 
void
A_FireQuake
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_quake, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_quake];

    missile = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_quake);
    if (missile)
    {
        missile->damage = damage;
        if (critRoll > 0)
        {
            missile->flags |= MF_STUN;  // Mark missile for stun effect on impact
        }
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 6, 12);
        player->message = "STUNNING QUAKE!";
    }
    else if (diceRoll >= 2)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        player->message = "SHOCKWAVE!";
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
}


//
// A_FireCrystalShatter - Goblin Dice Rollaz Crystal d8 Shatter weapon
// Shards spray out to hit multiple enemies, shield break bonus vs armored
//
void
A_FireCrystalShatter
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d8);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_crystalshatter, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_crystalshatter];

    missile = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_crystalshatter);
    if (missile)
    {
        missile->damage = damage;
        missile->flags |= MF_CRYSTALDICE;  // Mark missile as crystal for shard spawn
        if (critRoll > 0)
        {
            missile->flags |= MF_CRITDICE;  // Enhanced shards on crit
        }
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        player->message = "CRYSTAL SHATTER!";
    }
    else if (diceRoll >= 6)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
        player->message = "SHIELD BREAK!";
    }
    else if (diceRoll >= 4)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
        player->message = "CRYSTAL HIT!";
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 2);
    }
}




void
A_FireChainReaction
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d12);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_chainreaction, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_chainreaction];

    missile = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_chainreaction);
    if (missile)
    {
        missile->damage = damage;
        missile->flags |= MF_STICKYDICE;  // Mark missile as sticky for chain behavior
        if (critRoll > 0)
        {
            missile->flags |= MF_CRITDICE;  // Mark for enhanced chain on crit
        }
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 5, 10);
        player->message = "CHAIN REACTION!";
    }
    else if (diceRoll >= 8)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
        player->message = "PIERCING HIT!";
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
}





void
A_CheckReload
( player_t*	player,
  pspdef_t*	psp )
{
    P_CheckAmmo (player);
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
// Goblin Dice Rollaz: exploding dice global toggle
//
extern int goblin_exploding_dice;
extern int advantage_mode;
extern int luck;
extern int crit_scaling_default;
extern int crit_scaling_param;
extern int net_sync_debug;
extern int min_damage_cap;

// Goblin Dice Rollaz: RNG sync validation
void P_ValidateRNGState(const char *checkpoint);

//
// P_RollDice - Goblin Dice Rollaz: Centralized dice roll function
// Uses deterministic Doom RNG for netplay compatibility
// Supports advantage (roll twice, take best) and disadvantage (roll twice, take worst)
// Supports luck stat affecting roll distribution
//
int
P_RollDice (int sides)
{
    int roll1, roll2;

    P_ValidateRNGState("P_RollDice_start");
    
    if (sides <= 0)
        return 0;
    
    roll1 = (P_Random() % sides) + 1;
    
    // Apply luck modifier - positive luck shifts roll higher, negative shifts lower
    if (luck != 0)
    {
        int luck_modifier = luck;
        int min_roll = 1;
        int max_roll = sides;
        
        // Apply luck and clamp to valid range
        roll1 += luck_modifier;
        if (roll1 < min_roll) roll1 = min_roll;
        if (roll1 > max_roll) roll1 = max_roll;
    }
    
    // Apply advantage/disadvantage system
    if (advantage_mode > 0)
    {
        // Advantage: roll twice, take best
        roll2 = (P_Random() % sides) + 1;
        if (luck != 0)
        {
            roll2 += luck;
            if (roll2 < 1) roll2 = 1;
            if (roll2 > sides) roll2 = sides;
        }
        P_ValidateRNGState("P_RollDice_end_adv");
        return roll1 > roll2 ? roll1 : roll2;
    }
    else if (advantage_mode < 0)
    {
        // Disadvantage: roll twice, take worst
        roll2 = (P_Random() % sides) + 1;
        if (luck != 0)
        {
            roll2 += luck;
            if (roll2 < 1) roll2 = 1;
            if (roll2 > sides) roll2 = sides;
        }
        P_ValidateRNGState("P_RollDice_end_disadv");
        return roll1 < roll2 ? roll1 : roll2;
    }
    
    P_ValidateRNGState("P_RollDice_end");
    return roll1;
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
// Supports Misfire mechanic for high-risk weapons
// Applies weapon mastery bonuses if player is provided
// outDiceRoll: returns the actual dice roll value for effects like screen shake
//
int
P_CalculateDiceDamage (int weapon, int guaranteedCrit, int *outCritRoll, int *outMisfire, int *outDiceRoll, player_t* player)
{
    int diceRoll;
    int damage = 0;
    int misfired = 0;
    dice_weapon_info_t *dwi;
    int masteryBonus = 0;
    int masteryCritBonus = 0;
    
    if (outCritRoll)
        *outCritRoll = 0;
    if (outMisfire)
        *outMisfire = 0;
    if (outDiceRoll)
        *outDiceRoll = 0;
    
    if (weapon < 0 || weapon >= NUMWEAPONS)
        return 1;
    
    dwi = &dice_weapon_info[weapon];
    
    // Goblin Dice Rollaz: Challenge mode - always crit
    if (CHALLENGE_CRIT_ONLY())
        guaranteedCrit = 1;
    
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
    
    // Store dice roll for effects like screen shake
    if (outDiceRoll)
        *outDiceRoll = diceRoll;
    
    if (diceRoll == dwi->crit_roll)
    {
        int baseCritDamage;
        crit_scaling_type_t scalingType;
        
        if (outCritRoll)
            *outCritRoll = diceRoll;
        
        // Use weapon's scaling type if set, otherwise use global default
        scalingType = (dwi->crit_scaling_type > 0 && dwi->crit_scaling_type < CRIT_SCALING_MAX) 
                      ? dwi->crit_scaling_type 
                      : (crit_scaling_type_t)crit_scaling_default;
        
        // Calculate base crit damage (before scaling)
        baseCritDamage = dwi->damage_table[6] * dwi->crit_multiplier;
        
        // Apply scaling curve
        damage = P_ApplyCritScaling(baseCritDamage, diceRoll, dwi->die_type, 
                                    scalingType, dwi->crit_scaling_param > 0 ? dwi->crit_scaling_param : crit_scaling_param);
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
    
    // Goblin Dice Rollaz: Exploding Max Roll mechanic
    // For gamble_shot weapons OR when goblin_exploding_dice is set globally,
    // max roll triggers additional rolls (exploding dice)
    if ((dwi->gamble_shot || goblin_exploding_dice) && diceRoll == dwi->die_type)
    {
        int explodeRolls = 0;
        int additionalDamage = 0;
        
        // Roll again for each explosion (up to 3 extra rolls)
        while (diceRoll == dwi->die_type && explodeRolls < 3)
        {
            explodeRolls++;
            diceRoll = P_RollDice(dwi->die_type);
            
            // Add damage from explosion
            if (diceRoll == dwi->crit_roll)
            {
                // Apply scaling for exploding crits (half damage of regular crit)
                int baseExplodeCrit = dwi->damage_table[6] * dwi->crit_multiplier / 2;
                crit_scaling_type_t scalingType = (dwi->crit_scaling_type > 0 && dwi->crit_scaling_type < CRIT_SCALING_MAX) 
                                  ? dwi->crit_scaling_type 
                                  : (crit_scaling_type_t)crit_scaling_default;
                additionalDamage += P_ApplyCritScaling(baseExplodeCrit, diceRoll, dwi->die_type, 
                                    scalingType, dwi->crit_scaling_param > 0 ? dwi->crit_scaling_param : crit_scaling_param);
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
    
    // Goblin Dice Rollaz: Misfire mechanic for high-risk weapons
    // Check if roll triggers misfire - only if not guaranteed crit (don't punish success)
    if (!guaranteedCrit && dwi->misfire_roll > 0 && diceRoll <= dwi->misfire_roll)
    {
        misfired = 1;
        // Apply misfire penalty - reduce damage based on weapon's penalty setting
        if (dwi->misfire_penalty > 0 && dwi->misfire_penalty < 100)
        {
            damage = (damage * dwi->misfire_penalty) / 100;
        }
        else if (dwi->misfire_penalty == -1)
        {
            // Self-damage mode: damage stays the same but flag for player self-harm
            // Return -1 to indicate self-damage should be applied
            if (outMisfire)
                *outMisfire = -1;
        }
        else
        {
            // Complete misfire - no damage
            damage = 0;
        }
    }
    
    if (outMisfire)
        *outMisfire = misfired;
    
    // Goblin Dice Rollaz: Apply weapon mastery bonuses
    if (player && rpg_mode)
    {
        masteryBonus = G_GetWeaponMasteryDamageBonus(player, weapon);
        damage += masteryBonus;
    }
    
    // Goblin Dice Rollaz: Apply minimum damage cap
    // Ensures no weapon attack deals less than the configured minimum
    if (min_damage_cap > 0 && damage > 0 && damage < min_damage_cap)
    {
        damage = min_damage_cap;
    }
    
    // Goblin Dice Rollaz: Track dice roll statistics
    // Track whether this was a crit or misfire for stats
    {
        boolean was_crit = (outCritRoll && *outCritRoll > 0) || guaranteedCrit;
        boolean was_misfire = (outMisfire && *outMisfire != 0);
        G_TrackDiceRoll(weapon, dwi->die_type, 
                        outDiceRoll ? *outDiceRoll : diceRoll, 
                        damage, was_crit, was_misfire);
    }
    
    return damage;
}


//
// P_ApplyCritScaling - Goblin Dice Rollaz: Apply configurable crit scaling curve
// Calculates crit damage based on the selected scaling type and parameters
//
int
P_ApplyCritScaling (int baseDamage, int critRoll, int dieType, crit_scaling_type_t scalingType, int scalingParam)
{
    int multiplier = 1;
    
    switch (scalingType)
    {
        case CRIT_SCALING_LINEAR:
        default:
            // Simple multiplier (original behavior)
            multiplier = scalingParam > 0 ? scalingParam : 2;
            break;
            
        case CRIT_SCALING_EXPONENTIAL:
        {
            // Multiplier increases based on how high the crit roll was
            // Higher roll = exponentially higher multiplier
            int rollBonus = critRoll;
            // Base 2x, then add exponential bonus
            multiplier = 2 + (rollBonus * rollBonus) / (dieType > 0 ? dieType : 1);
            if (scalingParam > 0)
                multiplier = (multiplier * scalingParam) / 100;
            break;
        }
        
        case CRIT_SCALING_BONUS_FLAT:
        {
            // Flat bonus added to base damage instead of multiplier
            int bonus = scalingParam > 0 ? scalingParam : critRoll;
            multiplier = 1;
            baseDamage += bonus;
            break;
        }
        
        case CRIT_SCALING_BONUS_PERCENT:
        {
            // Percentage bonus based on the crit roll value
            int percentBonus = (critRoll * 100) / (dieType > 0 ? dieType : 1);
            if (scalingParam > 0)
                percentBonus = (percentBonus * scalingParam) / 100;
            multiplier = 1 + (percentBonus / 100);
            break;
        }
        
        case CRIT_SCALING_CRIT_CHANCE:
        {
            // Crit chance affects the multiplier - higher base chance = lower multiplier
            // Implemented as: multiplier = baseMultiplier * CRIT_CHANCE_SCALE / (critChance + CRIT_CHANCE_SCALE)
            int critChance = scalingParam > 0 ? scalingParam : 10;
            multiplier = DEFAULT_CRIT_MULTIPLIER * CRIT_CHANCE_SCALE / (critChance + CRIT_CHANCE_SCALE);
            if (multiplier < 1) multiplier = 1;
            break;
        }
    }
    
    return baseDamage * multiplier;
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
    P_SpawnPlayerMissile (player->mo, MT_ROCKET, player->readyweapon);
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
    P_SpawnPlayerMissile (player->mo, MT_BFG, player->readyweapon);
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

    P_SpawnPlayerMissile (player->mo, MT_PLASMA, player->readyweapon);
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
    int diceRoll = 0;
    
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

    damage = P_CalculateDiceDamage(wp_d6blaster, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    // Goblin Dice Rollaz: Apply weapon recoil
    player->weapon_recoil = weapon_recoil_values[wp_d6blaster];

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, !player->refire, damage);
    
    // Goblin Dice Rollaz: Screen shake on critical hits or high rolls (>= 5 on d6)
    if (critRoll > 0 || diceRoll >= 5)
    {
        int shakeIntensity = (critRoll > 0) ? (FRACUNIT * 3) : (FRACUNIT * 2);
        R_TriggerScreenShake(shakeIntensity, 4);
    }
}


//
// A_FireTwinD6 - Goblin Dice Rollaz twin d6 scatter weapon
// Fires two d6 dice in quick succession for close-range burst
//
void
A_FireTwinD6
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage1;
    int damage2;
    int guaranteedCrit = 0;
    int critRoll1 = 0;
    int critRoll2 = 0;
    int diceRoll1 = 0;
    int diceRoll2 = 0;
    int highestRoll = 0;
    
    S_StartSound (player->mo, sfx_dice_d6);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 2);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate);

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage1 = P_CalculateDiceDamage(wp_twind6, guaranteedCrit, &critRoll1, NULL, &diceRoll1, player);
    damage2 = P_CalculateDiceDamage(wp_twind6, guaranteedCrit, &critRoll2, NULL, &diceRoll2, player);

    // Goblin Dice Rollaz: Apply weapon recoil
    player->weapon_recoil = weapon_recoil_values[wp_twind6];

    // Track highest roll for screen shake
    highestRoll = (diceRoll1 > diceRoll2) ? diceRoll1 : diceRoll2;
    if (diceRoll1 == 0) highestRoll = diceRoll2;
    if (diceRoll2 == 0) highestRoll = diceRoll1;

    P_BulletSlope (player->mo);
    P_GunShotWithDamage (player->mo, true, damage1);
    P_GunShotWithDamage (player->mo, false, damage2);
    
    // Screen shake on crits or high rolls (>= 5 on d6)
    if (critRoll1 > 0 || critRoll2 > 0 || highestRoll >= 5)
    {
        int shakeIntensity = ((critRoll1 > 0 || critRoll2 > 0) ? FRACUNIT * 3 : FRACUNIT * 2);
        R_TriggerScreenShake(shakeIntensity, 4);
    }
}


//
// A_FireArcaneD20 - Goblin Dice Rollaz arcane d20 beam weapon
// Fires a continuous beam projectile that rolls d20 for damage each hit
//
void
A_FireArcaneD20
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_arcaned20, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    // Goblin Dice Rollaz: Apply weapon recoil
    player->weapon_recoil = weapon_recoil_values[wp_arcaned20];

    missile = P_SpawnPlayerMissile (player->mo, MT_ARCANED20BEAM, wp_arcaned20);
    if (missile)
    {
        missile->damage = damage;
    }
    
    // Screen shake on crits or high rolls (>= 15 on d20)
    if (critRoll > 0 || diceRoll >= 15)
    {
        int shakeIntensity = (critRoll > 0) ? (FRACUNIT * 3) : (FRACUNIT * 2);
        R_TriggerScreenShake(shakeIntensity, 4);
    }
}


//
// A_FireCursed - Goblin Dice Rollaz cursed die weapon
// High damage weapon with self-risk mechanic - player takes damage on low rolls
//
void
A_FireCursed
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    int misfire = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d6);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_cursed, guaranteedCrit, &critRoll, &misfire, &diceRoll, player);

    // Goblin Dice Rollaz: Apply weapon recoil
    player->weapon_recoil = weapon_recoil_values[wp_cursed];

    missile = P_SpawnPlayerMissile (player->mo, MT_CURSEDDIE, wp_cursed);
    if (missile)
    {
        missile->damage = damage;
    }

    // Goblin Dice Rollaz: Self-damage mechanic for cursed die
    // If misfire is -1, player takes damage equal to the damage they would have dealt
    if (misfire == -1)
    {
        player->health -= damage;
        player->message = "CURSED!";
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        if (player->health <= 0)
        {
            P_KillMobj(player->mo, player->mo, player->mo, true);
        }
    }
    // Screen shake on crits or high rolls (>= 5 on the base die type)
    else if (critRoll > 0 || diceRoll >= 5)
    {
        int shakeIntensity = (critRoll > 0) ? (FRACUNIT * 4) : (FRACUNIT * 2);
        R_TriggerScreenShake(shakeIntensity, 6);
    }
}


//
// A_FireD2 - Goblin Dice Rollaz d2 Flip of Fate weapon
// Binary damage: 50% chance for 1 damage, 50% chance for 2 damage
// Crit: double the result (2 or 4 damage)
// 
void
A_FireD2
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d6);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d2, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d2];

    missile = P_SpawnPlayerMissile (player->mo, MT_D2PROJECTILE, wp_d2);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
    }
    else if (diceRoll >= 2)
    {
        R_TriggerScreenShake(FRACUNIT * 1, 4);
    }
}


//
// A_FireD3 - Goblin Dice Rollaz d3 Skewered Luck weapon
// Guaranteed 1-3 damage, piercing projectile
// Crit: double the result (2-6 damage)
// 
void
A_FireD3
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d6);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d3, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d3];

    missile = P_SpawnPlayerMissile (player->mo, MT_D3PROJECTILE, wp_d3);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 5);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 3);
    }
}


//
// A_FireD7 - Goblin Dice Rollaz d7 Seven Veils weapon
// Ore fragment splash: on impact, fires secondary fragment projectiles
// Crit: double damage plus larger splash
// 
void
A_FireD7
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d6);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d7, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d7];

    missile = P_SpawnPlayerMissile (player->mo, MT_D7PROJECTILE, wp_d7);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
    }
    else if (diceRoll >= 5)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 3);
    }
}


//
// A_FireD14 - Goblin Dice Rollaz d14 Fused Doubler weapon
// Fires two d14 dice - resonance (same roll on both) causes explosion
// Dual dice rolled, combined damage with bonus for matching numbers
// 
void
A_FireD14
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage1;
    int damage2;
    int guaranteedCrit = 0;
    int critRoll1 = 0;
    int critRoll2 = 0;
    int diceRoll1 = 0;
    int diceRoll2 = 0;
    mobj_t* missile;
    int resonanceBonus = 0;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage1 = P_CalculateDiceDamage(wp_d14, guaranteedCrit, &critRoll1, NULL, &diceRoll1, player);
    damage2 = P_CalculateDiceDamage(wp_d14, guaranteedCrit, &critRoll2, NULL, &diceRoll2, player);

    if (diceRoll1 == diceRoll2 && diceRoll1 > 0)
    {
        resonanceBonus = diceRoll1 * 2;
        player->message = "RESONANCE!";
    }

    damage1 += resonanceBonus;

    player->weapon_recoil = weapon_recoil_values[wp_d14];

    missile = P_SpawnPlayerMissile (player->mo, MT_D14PROJECTILE, wp_d14);
    if (missile)
    {
        missile->damage = damage1;
    }

    if (resonanceBonus > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
    }
    else if (critRoll1 > 0 || critRoll2 > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
    }
    else if (diceRoll1 >= 10 || diceRoll2 >= 10)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 3);
    }
}


//
// A_FireD24 - Goblin Dice Rollaz d24 Hourglass Cannon weapon
// Slows enemies on hit, freezes on critical hit
// 
void
A_FireD24
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d24, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d24];

    missile = P_SpawnPlayerMissile (player->mo, MT_D24PROJECTILE, wp_d24);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
        player->message = "FROZEN!";
    }
    else if (diceRoll >= 18)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 5);
        player->message = "SLOWED!";
    }
    else if (diceRoll >= 12)
    {
        R_TriggerScreenShake(FRACUNIT * 1, 3);
    }
}


//
// A_FireD30 - Goblin Dice Rollaz d30 Gear Grinder weapon
// Bonus damage vs armored dwarves
// 
void
A_FireD30
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* th;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d30, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d30];

    th = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_d30);
    if (th)
    {
        th->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        player->message = "ARMOR CRUSH!";
    }
}


//
// A_FireD48 - Goblin Dice Rollaz d48 Forge Hammer weapon
// Molten effect on critical hit, fire damage type
// 
void
A_FireD48
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d48, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d48];

    missile = P_SpawnPlayerMissile (player->mo, MT_D48PROJECTILE, wp_d48);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 5, 10);
        player->message = "MOLTEN!";
    }
    else if (diceRoll >= 36)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        player->message = "FORGE BURN!";
    }
    else if (diceRoll >= 24)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
    }
    else if (diceRoll >= 16)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 2);
    }
}


//
// A_FireD50 - Goblin Dice Rollaz d50 Dwarf's Bane weapon
// 3x damage vs dwarf enemies
// 
void
A_FireD50
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d50, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d50];

    missile = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_d50);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 6, 12);
        player->message = "DWARF SLAYER!";
    }
    else if (diceRoll >= 42)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        player->message = "CRUSHING BLOW!";
    }
    else if (diceRoll >= 28)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
        player->message = "HEAVY HIT!";
    }
    else if (diceRoll >= 18)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 2);
    }
}


//
// A_FireD60 - Goblin Dice Rollaz d60 Elemental Chaos weapon
// Random elemental damage on each hit: fire/ice/lightning/poison
// 
void
A_FireD60
( player_t*	player,
  pspdef_t*	psp ) 
{
    int damage;
    int guaranteedCrit = 0;
    int critRoll = 0;
    int diceRoll = 0;
    mobj_t* missile;
    
    S_StartSound (player->mo, sfx_dice_d20);

    P_SetMobjState (player->mo, S_PLAY_ATK2);
    DecreaseAmmo(player, weaponinfo[player->readyweapon].ammo, 1);

    P_SetPsprite (player,
		  ps_flash,
		  weaponinfo[player->readyweapon].flashstate+(P_Random ()&1));

    if (player->powers[pw_dicefortune])
    {
        guaranteedCrit = 1;
        player->powers[pw_dicefortune] = 0;
        player->message = "CRITICAL!";
    }

    damage = P_CalculateDiceDamage(wp_d60, guaranteedCrit, &critRoll, NULL, &diceRoll, player);

    player->weapon_recoil = weapon_recoil_values[wp_d60];

    missile = P_SpawnPlayerMissile (player->mo, MT_DICE_GLOW, wp_d60);
    if (missile)
    {
        missile->damage = damage;
    }

    if (critRoll > 0)
    {
        R_TriggerScreenShake(FRACUNIT * 6, 12);
        player->message = "ELEMENTAL SURGE!";
    }
    else if (diceRoll >= 42)
    {
        R_TriggerScreenShake(FRACUNIT * 4, 8);
        player->message = "CHAOS STRIKE!";
    }
    else if (diceRoll >= 28)
    {
        R_TriggerScreenShake(FRACUNIT * 3, 6);
        player->message = "ELEMENTAL HIT!";
    }
    else if (diceRoll >= 18)
    {
        R_TriggerScreenShake(FRACUNIT * 2, 4);
    }
    else
    {
        R_TriggerScreenShake(FRACUNIT * 1, 2);
    }
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


