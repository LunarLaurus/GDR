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
//	Handling interactions (i.e., collisions).
//

// Goblin Dice Rollaz: Freeze duration constant
#define SHAMAN_FREEZE_DURATION 180



// Data.
#include "doomdef.h"
#include "dstrings.h"
#include "sounds.h"

#include "deh_main.h"
#include "deh_misc.h"
#include "doomstat.h"

#include "m_random.h"
#include "i_system.h"

#include "am_map.h"

#include "p_local.h"
#include "p_ai_faction.h"
#include "r_state.h"
#include "s_sound.h"
#include "p_inter.h"
#include "p_particles.h"
#include "dmg_ovl.h"
#include "dbg_ovl.h"
#include "dice_predict.h"
#include "d_items.h"
#include "g_status.h"
#include "g_rpg.h"
#include "g_survival.h"
#include "g_powerup.h"
#include "g_achievement.h"
#include "g_balance.h"

// Goblin Dice Rollaz: Arena lock system
extern int arena_locked;
extern int arena_lock_tag;
extern int EV_DoArenaLock(int tag, boolean close);

// Goblin Dice Rollaz: Auto-switch weapon on pickup
extern int auto_switch_weapon;


#define BONUSADD	6

// Goblin Dice Rollaz: Default crit system configuration
int	crit_chance_default = DEFAULT_CRIT_CHANCE;
int	crit_multiplier_default = DEFAULT_CRIT_MULTIPLIER;
int	crit_combo_timeout = DEFAULT_CRIT_COMBO_TIMEOUT;
int	crit_combo_bonus = DEFAULT_CRIT_COMBO_BONUS;
int	crit_combo_max = DEFAULT_CRIT_COMBO_MAX;
int	dice_arena_damage_mult = 150;

// Goblin Dice Rollaz: Combo multiplier system
int	goblin_crit_boost_bonus = CRIT_POWERUP_BONUS;
int	goblin_exploding_dice = 0;

// Goblin Dice Rollaz: Minimum damage cap
// Ensures no attack deals less than this amount of damage
int	min_damage_cap = DEFAULT_MIN_DAMAGE_CAP;

// Goblin Dice Rollaz: Net sync debug logging
// When enabled, logs RNG state and desync warnings to console
int net_sync_debug = 0;
int net_desync_count = 0;
int rng_validation_enabled = 0;

// Goblin Dice Rollaz: Server-authoritative damage validation
// When enabled, server recalculates damage to validate client predictions
int sv_authoritative_damage = 0;
int sv_validate_frequency = 4;  // Validate every Nth attack
int sv_damage_validate_count = 0;

// Goblin Dice Rollaz: Damage log system
#define DAMAGE_LOG_MAX 64

typedef struct {
    int tick;
    int damage;
    boolean is_crit;
    int crit_roll;
    const char *target_name;
    const char *weapon_name;
} damage_log_entry_t;

static damage_log_entry_t damage_log[DAMAGE_LOG_MAX];
static int damage_log_count = 0;
static int damage_log_enabled = 0;

void DMG_AddDamageLog(int damage, boolean is_crit, int crit_roll, const char *target_name, const char *weapon_name)
{
    if (!damage_log_enabled || damage_log_count >= DAMAGE_LOG_MAX)
        return;
    
    damage_log[damage_log_count].tick = gametic;
    damage_log[damage_log_count].damage = damage;
    damage_log[damage_log_count].is_crit = is_crit;
    damage_log[damage_log_count].crit_roll = crit_roll;
    damage_log[damage_log_count].target_name = target_name;
    damage_log[damage_log_count].weapon_name = weapon_name;
    damage_log_count++;
}

void DMG_ClearDamageLog(void)
{
    damage_log_count = 0;
}

void DMG_PrintDamageLog(void)
{
    int i;
    
    if (!damage_log_enabled)
    {
        DEH_printf("Damage logging is disabled. Press F9 to enable.\n");
        return;
    }
    
    if (damage_log_count == 0)
    {
        DEH_printf("No damage logged yet.\n");
        return;
    }
    
    DEH_printf("=== Damage Log (%d entries) ===\n", damage_log_count);
    DEH_printf("%-6s %-6s %-5s %-8s %-s\n", "Tick", "Damage", "Crit", "Roll", "Target (Weapon)");
    DEH_printf("------------------------------------------------\n");
    
    for (i = 0; i < damage_log_count; i++)
    {
        DEH_printf("%-6d %-6d %-5s %-8d %s (%s)\n",
            damage_log[i].tick,
            damage_log[i].damage,
            damage_log[i].is_crit ? "YES" : "no",
            damage_log[i].crit_roll,
            damage_log[i].target_name ? damage_log[i].target_name : "unknown",
            damage_log[i].weapon_name ? damage_log[i].weapon_name : "none");
    }
    DEH_printf("=== End of Damage Log ===\n");
}

// Goblin Dice Rollaz: Broadcast critical hit message to all netgame players
// Ensures all players see the same critical hit notifications
void P_BroadcastCritMessage(int player_num, const char *message, boolean is_crit, int damage)
{
    int i;
    
    if (!netgame)
        return;
    
    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            players[i].message = message;
        }
    }
    
    if (net_sync_debug)
    {
        DEH_printf("[NETSYNC] Crit broadcast: Player %d - %s (damage: %d)\n", 
               player_num, is_crit ? "CRITICAL" : "hit", damage);
    }
}

// Goblin Dice Rollaz: Broadcast arena PvP kill message
void P_BroadcastArenaKill(int killer, int victim)
{
    int i;
    static char killmsg[64];
    
    if (!netgame)
        return;

    snprintf(killmsg, sizeof(killmsg), "Player %d eliminated Player %d!", killer + 1, victim + 1);
    
    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            players[i].message = killmsg;
        }
    }
}

// Goblin Dice Rollaz: Log desync warning for debugging
// Called when RNG state mismatch is detected
void P_LogDesync(const char *context, int expected, int actual)
{
    net_desync_count++;
    
    if (net_sync_debug)
    {
        DEH_printf("[DESYNC] %s: expected RNG=%d, got=%d (total desyncs: %d)\n",
               context, expected, actual, net_desync_count);
    }
}

// Goblin Dice Rollaz: Validate RNG state at key points
// Call this from critical code paths to verify sync
void P_ValidateRNGState(const char *checkpoint)
{
    if (net_sync_debug && netgame)
    {
        extern int P_GetRNGState(void);
        extern int M_GetRNGState(void);
        DEH_printf("[NETSYNC] %s: prnd=%d, rnd=%d, gametic=%d\n",
               checkpoint, P_GetRNGState(), M_GetRNGState(), gametic);
    }
}

// Goblin Dice Rollaz: Run deterministic RNG validation test
// Outputs a sequence of RNG values that should be identical across platforms
// Used to verify that the RNG is deterministic across different systems
void P_RunRNGValidationTest(void)
{
    int i;
    int save_prndindex, save_rndindex;
    extern int P_Random(void);
    extern int M_Random(void);
    extern int P_GetRNGState(void);
    extern int M_GetRNGState(void);
    
    DEH_printf("=== RNG Validation Test ===\n");
    DEH_printf("Initial state: P_Random index=%d, M_Random index=%d\n",
           P_GetRNGState(), M_GetRNGState());
    
    // Save current RNG state
    save_prndindex = P_GetRNGState();
    save_rndindex = M_GetRNGState();
    
    // Run 20 P_Random calls and output results
    DEH_printf("P_Random sequence (20 calls): ");
    for (i = 0; i < 20; i++)
    {
        DEH_printf("%d ", P_Random());
    }
    DEH_printf("\n");
    
    // Run 20 M_Random calls and output results  
    DEH_printf("M_Random sequence (20 calls): ");
    for (i = 0; i < 20; i++)
    {
        DEH_printf("%d ", M_Random());
    }
    DEH_printf("\n");
    
    // Run dice roll simulation
    DEH_printf("Dice roll simulation (d6, d20, d100): ");
    DEH_printf("%d %d %d\n", 
           (P_Random() % 6) + 1,
           (P_Random() % 20) + 1,
           (P_Random() % 100) + 1);
    
    DEH_printf("Final state: P_Random index=%d, M_Random index=%d\n",
           P_GetRNGState(), M_GetRNGState());
    DEH_printf("=== End RNG Test ===\n");
}

// Goblin Dice Rollaz: Crit scaling curve settings
// crit_scaling_default: 0=linear, 1=exponential, 2=bonus_flat, 3=bonus_percent, 4=crit_chance
// crit_scaling_param: parameter for the scaling curve (multiplier, bonus, etc.)
int	crit_scaling_default = DEFAULT_CRIT_SCALING_TYPE;
int	crit_scaling_param = DEFAULT_CRIT_SCALING_PARAM;

// Goblin Dice Rollaz: Advantage/Disadvantage system
// 0 = normal, 1 = advantage (roll twice, take best), -1 = disadvantage (roll twice, take worst)
int	advantage_mode = 0;

// Goblin Dice Rollaz: Luck stat affecting roll distribution
// Positive values shift rolls higher, negative values shift rolls lower
// Range: -5 to +5 (clamped to prevent invalid roll results)
int	luck = 0;

int	difficulty_hp_scale[5] = {75, 90, 100, 120, 150};




// a weapon is found with two clip loads,
// a big item has five clip loads
int	maxammo[NUMAMMO] = {200, 50, 300, 50, 100, 50, 50};
int	clipammo[NUMAMMO] = {10, 4, 20, 1, 6, 3, 2};


//
// GET STUFF
//

//
// P_GiveAmmo
// Num is the number of clip loads,
// not the individual count (0= 1/2 clip).
// Returns false if the ammo can't be picked up at all
//

boolean
P_GiveAmmo
( player_t*	player,
  ammotype_t	ammo,
  int		num )
{
    int		oldammo;
	
    if (ammo == am_noammo)
	return false;
		
    if (ammo >= NUMAMMO)
	I_Error ("P_GiveAmmo: bad type %i", ammo);
		
    if ( player->ammo[ammo] == player->maxammo[ammo]  )
	return false;
		
    if (num)
	num *= clipammo[ammo];
    else
	num = clipammo[ammo]/2;
    
    if (gameskill == sk_baby
	|| gameskill == sk_nightmare)
    {
	// give double ammo in trainer mode,
	// you'll need in nightmare
	num <<= 1;
    }
    
		
    oldammo = player->ammo[ammo];
    player->ammo[ammo] += num;

    if (player->ammo[ammo] > player->maxammo[ammo])
	player->ammo[ammo] = player->maxammo[ammo];

    // If non zero ammo, 
    // don't change up weapons,
    // player was lower on purpose.
    if (oldammo)
	return true;	

    // We were down to zero,
    // so select a new weapon.
    // Preferences are not user selectable.
    switch (ammo)
    {
      case am_clip:
	if (player->readyweapon == wp_fist)
	{
	    if (player->weaponowned[wp_chaingun])
		player->pendingweapon = wp_chaingun;
	    else
		player->pendingweapon = wp_pistol;
	}
	break;
	
      case am_shell:
	if (player->readyweapon == wp_fist
	    || player->readyweapon == wp_pistol)
	{
	    if (player->weaponowned[wp_shotgun])
		player->pendingweapon = wp_shotgun;
	}
	break;
	
      case am_cell:
	if (player->readyweapon == wp_fist
	    || player->readyweapon == wp_pistol)
	{
	    if (player->weaponowned[wp_plasma])
		player->pendingweapon = wp_plasma;
	}
	break;
	
      case am_misl:
	if (player->readyweapon == wp_fist)
	{
	    if (player->weaponowned[wp_missile])
		player->pendingweapon = wp_missile;
	}
	break;

      // Goblin Dice Rollaz: Auto-switch to dice weapons when picking up ammo
      case am_lightdice:
	if (player->readyweapon == wp_fist
	    || player->readyweapon == wp_pistol)
	{
	    if (player->weaponowned[wp_d6blaster])
		player->pendingweapon = wp_d6blaster;
	    else if (player->weaponowned[wp_d4])
		player->pendingweapon = wp_d4;
	}
	break;

      case am_heavydice:
	if (player->readyweapon == wp_fist
	    || player->readyweapon == wp_pistol)
	{
	    if (player->weaponowned[wp_d20cannon])
		player->pendingweapon = wp_d20cannon;
	    else if (player->weaponowned[wp_d12])
		player->pendingweapon = wp_d12;
	    else if (player->weaponowned[wp_percentile])
		player->pendingweapon = wp_percentile;
	    else if (player->weaponowned[wp_cursed])
		player->pendingweapon = wp_cursed;
	}
	break;

      case am_arcanedice:
	// Arcane dice weapons (future implementation)
	break;

      default:
	break;
    }
	
    return true;
}


//
// P_GiveWeapon
// The weapon name may have a MF_DROPPED flag ored in.
//
boolean
P_GiveWeapon
( player_t*	player,
  weapontype_t	weapon,
  boolean	dropped )
{
    boolean	gaveammo;
    boolean	gaveweapon;
	
    if (netgame
	&& (deathmatch!=2)
	 && !dropped )
    {
	// leave placed weapons forever on net games
	if (player->weaponowned[weapon])
	    return false;

	player->bonuscount += BONUSADD;
	player->weaponowned[weapon] = true;

	if (deathmatch)
	    P_GiveAmmo (player, weaponinfo[weapon].ammo, 5);
	else
	    P_GiveAmmo (player, weaponinfo[weapon].ammo, 2);
	player->pendingweapon = weapon;

	if (player == &players[consoleplayer])
	    S_StartSound (NULL, sfx_wpnup);
	return false;
    }
	
    if (weaponinfo[weapon].ammo != am_noammo)
    {
	// give one clip with a dropped weapon,
	// two clips with a found weapon
	if (dropped)
	    gaveammo = P_GiveAmmo (player, weaponinfo[weapon].ammo, 1);
	else
	    gaveammo = P_GiveAmmo (player, weaponinfo[weapon].ammo, 2);
    }
    else
	gaveammo = false;
	
    if (player->weaponowned[weapon])
	gaveweapon = false;
    else
    {
	gaveweapon = true;
	player->weaponowned[weapon] = true;
	player->pendingweapon = weapon;

	if (auto_switch_weapon && !dropped)
	{
	    player->readyweapon = weapon;
	    P_SetPsprite(player, ps_weapon, weaponinfo[weapon].readystate);
	}
    }
	
    return (gaveweapon || gaveammo);
}

 

//
// P_GiveBody
// Returns false if the body isn't needed at all
//
boolean
P_GiveBody
( player_t*	player,
  int		num )
{
    if (player->health >= MAXHEALTH)
	return false;
		
    player->health += num;
    if (player->health > MAXHEALTH)
	player->health = MAXHEALTH;
    player->mo->health = player->health;
	
    return true;
}



//
// P_GiveArmor
// Returns false if the armor is worse
// than the current armor.
//
boolean
P_GiveArmor
( player_t*	player,
  int		armortype )
{
    int		hits;
	
    hits = armortype*100;
    if (player->armorpoints >= hits)
	return false;	// don't pick up
		
    player->armortype = armortype;
    player->armorpoints = hits;
	
    return true;
}



//
// P_GiveCard
//
void
P_GiveCard
( player_t*	player,
  card_t	card )
{
    if (player->cards[card])
	return;
    
    player->bonuscount = BONUSADD;
    player->cards[card] = 1;
}


//
// P_GivePower
//
boolean
P_GivePower
( player_t*	player,
  int /*powertype_t*/	power )
{
    if (power == pw_invulnerability)
    {
	player->powers[power] = INVULNTICS;
	return true;
    }
    
    if (power == pw_invisibility)
    {
	player->powers[power] = INVISTICS;
	player->mo->flags |= MF_SHADOW;
	return true;
    }
    
    if (power == pw_infrared)
    {
	player->powers[power] = INFRATICS;
	return true;
    }
    
    if (power == pw_ironfeet)
    {
	player->powers[power] = IRONTICS;
	return true;
    }
    
    if (power == pw_strength)
    {
	P_GiveBody (player, 100);
	player->powers[power] = 1;
	return true;
    }

    if (power == pw_critboost)
    {
        if (player->powers[pw_doubledamage])
        {
            return false;
        }
        player->powers[power] = CRITBOOSTTICS;
        S_StartSound(&player->mo->sphere, sfx_getpow);
        G_PowerupShareWithNearbyPlayers(player, pw_critboost);
        return true;
    }

    if (power == pw_doubledamage)
    {
        if (player->powers[pw_critboost])
        {
            return false;
        }
        player->powers[power] = DOUBLEDAMAGETICS;
        S_StartSound(&player->mo->sphere, sfx_getpow);
        G_PowerupShareWithNearbyPlayers(player, pw_doubledamage);
        return true;
    }

    if (power == pw_dicefortune)
    {
        if (player->powers[pw_dicefortune])
        {
            return false;
        }
        player->powers[power] = 1;
        S_StartSound(&player->mo->sphere, sfx_getpow);
        return true;
    }

    if (power == pw_snakeeyes)
    {
        if (player->powers[pw_snakeeyes])
        {
            return false;
        }
        if (player->powers[pw_doubledamage])
        {
            return false;
        }
        player->powers[power] = SNAKEEYESTICS;
        S_StartSound(&player->mo->sphere, sfx_getpow);
        return true;
    }
	
    if (player->powers[power])
	return false;	// already got it
		
    player->powers[power] = 1;
    return true;
}



//
// P_TouchSpecialThing
//
void
P_TouchSpecialThing
( mobj_t*	special,
  mobj_t*	toucher )
{
    player_t*	player;
    int		i;
    fixed_t	delta;
    int		sound;
		
    delta = special->z - toucher->z;

    if (delta > toucher->height
	|| delta < -8*FRACUNIT)
    {
	// out of reach
	return;
    }
    
	
    sound = sfx_itemup;	
    player = toucher->player;

    // Dead thing touching.
    // Can happen with a sliding player corpse.
    if (toucher->health <= 0)
	return;

    // Goblin Dice Rollaz: Handle dice ammo pickups by type (before sprite check)
    // These use placeholder sprites, so we check type first
    switch (special->type)
    {
      case MT_AMMO_LIGHTDICE:
	if (special->flags & MF_DROPPED)
	{
	    if (!P_GiveAmmo (player, am_lightdice, 0))
		return;
	}
	else
	{
	    if (!P_GiveAmmo (player, am_lightdice, 5))
		return;
	}
	player->message = DEH_String(GOTLIGHTDICE);
	sound = sfx_itemup;
	goto pickup_success;

      case MT_AMMO_HEAVYDICE:
	if (special->flags & MF_DROPPED)
	{
	    if (!P_GiveAmmo (player, am_heavydice, 0))
		return;
	}
	else
	{
	    if (!P_GiveAmmo (player, am_heavydice, 3))
		return;
	}
	player->message = DEH_String(GOTHEAVYDICE);
	sound = sfx_itemup;
	goto pickup_success;

      case MT_AMMO_ARCANEDICE:
	if (special->flags & MF_DROPPED)
	{
	    if (!P_GiveAmmo (player, am_arcanedice, 0))
		return;
	}
	else
	{
	    if (!P_GiveAmmo (player, am_arcanedice, 10))
		return;
	}
	player->message = DEH_String(GOTARCANEDICE);
	sound = sfx_getpow;
	goto pickup_success;
    }

    // Identify by sprite.
    switch (special->sprite)
    {
	// armor
      case SPR_ARM1:
	if (!P_GiveArmor (player, deh_green_armor_class))
	    return;
	player->message = DEH_String(GOTARMOR);
	break;
		
      case SPR_ARM2:
	if (!P_GiveArmor (player, deh_blue_armor_class))
	    return;
	player->message = DEH_String(GOTMEGA);
	break;
	
	// bonus items
      case SPR_BON1:
	player->health++;		// can go over 100%
	if (player->health > deh_max_health)
	    player->health = deh_max_health;
	player->mo->health = player->health;
	player->message = DEH_String(GOTHTHBONUS);
	break;
	
      case SPR_BON2:
	player->armorpoints++;		// can go over 100%
	if (player->armorpoints > deh_max_armor && gameversion > exe_doom_1_2)
	    player->armorpoints = deh_max_armor;
        // deh_green_armor_class only applies to the green armor shirt;
        // for the armor helmets, armortype 1 is always used.
	if (!player->armortype)
	    player->armortype = 1;
	player->message = DEH_String(GOTARMBONUS);
	break;
	
      case SPR_SOUL:
	player->health += deh_soulsphere_health;
	if (player->health > deh_max_soulsphere)
	    player->health = deh_max_soulsphere;
	player->mo->health = player->health;
	player->message = DEH_String(GOTSUPER);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_MEGA:
	if (gamemode != commercial)
	    return;
	player->health = deh_megasphere_health;
	player->mo->health = player->health;
        // We always give armor type 2 for the megasphere; dehacked only 
        // affects the MegaArmor.
	P_GiveArmor (player, 2);
	player->message = DEH_String(GOTMSPHERE);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
	// cards
	// leave cards for everyone
      case SPR_BKEY:
	if (!player->cards[it_bluecard])
	    player->message = DEH_String(GOTBLUECARD);
	P_GiveCard (player, it_bluecard);
	if (!netgame)
	    break;
	return;
	
      case SPR_YKEY:
	if (!player->cards[it_yellowcard])
	    player->message = DEH_String(GOTYELWCARD);
	P_GiveCard (player, it_yellowcard);
	if (!netgame)
	    break;
	return;
	
      case SPR_RKEY:
	if (!player->cards[it_redcard])
	    player->message = DEH_String(GOTREDCARD);
	P_GiveCard (player, it_redcard);
	if (!netgame)
	    break;
	return;
	
      case SPR_BSKU:
	if (!player->cards[it_blueskull])
	    player->message = DEH_String(GOTBLUESKUL);
	P_GiveCard (player, it_blueskull);
	if (!netgame)
	    break;
	return;
	
      case SPR_YSKU:
	if (!player->cards[it_yellowskull])
	    player->message = DEH_String(GOTYELWSKUL);
	P_GiveCard (player, it_yellowskull);
	if (!netgame)
	    break;
	return;
	
      case SPR_RSKU:
	if (!player->cards[it_redskull])
	    player->message = DEH_String(GOTREDSKULL);
	P_GiveCard (player, it_redskull);
	if (!netgame)
	    break;
	return;
	
	// medikits, heals
      case SPR_STIM:
	if (!P_GiveBody (player, 10))
	    return;
	player->message = DEH_String(GOTSTIM);
	break;
	
      case SPR_MEDI:
	if (!P_GiveBody (player, 25))
	    return;

	if (player->health < 25)
	    player->message = DEH_String(GOTMEDINEED);
	else
	    player->message = DEH_String(GOTMEDIKIT);
	break;

	
	// power ups
      case SPR_PINV:
	if (!P_GivePower (player, pw_invulnerability))
	    return;
	player->message = DEH_String(GOTINVUL);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_PSTR:
	if (!P_GivePower (player, pw_strength))
	    return;
	player->message = DEH_String(GOTBERSERK);
	if (player->readyweapon != wp_fist)
	    player->pendingweapon = wp_fist;
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_PINS:
	if (!P_GivePower (player, pw_invisibility))
	    return;
	player->message = DEH_String(GOTINVIS);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_SUIT:
	if (!P_GivePower (player, pw_ironfeet))
	    return;
	player->message = DEH_String(GOTSUIT);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_PMAP:
	if (!P_GivePower (player, pw_allmap))
	    return;
	player->message = DEH_String(GOTMAP);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_PVIS:
	if (!P_GivePower (player, pw_infrared))
	    return;
	player->message = DEH_String(GOTVISOR);
	if (gameversion > exe_doom_1_2)
	    sound = sfx_getpow;
	break;
	
      case SPR_PCRT:
	if (!P_GivePower (player, pw_critboost))
	    return;
	player->message = DEH_String(GOTCRITS);
	sound = sfx_critup;
	break;
	
      case SPR_PDMD:
	if (!P_GivePower (player, pw_doubledamage))
	    return;
	player->message = DEH_String(GOTDOUBLEDAMAGE);
	sound = sfx_doubup;
	break;
	
      case SPR_PFTR:
	if (!P_GivePower (player, pw_dicefortune))
	    return;
	player->message = "DICE FORTUNE!";
	sound = sfx_critup;
	break;
	
	// ammo
      case SPR_CLIP:
	if (special->flags & MF_DROPPED)
	{
	    if (!P_GiveAmmo (player,am_clip,0))
		return;
	}
	else
	{
	    if (!P_GiveAmmo (player,am_clip,1))
		return;
	}
	player->message = DEH_String(GOTCLIP);
	break;
	
      case SPR_AMMO:
	if (!P_GiveAmmo (player, am_clip,5))
	    return;
	player->message = DEH_String(GOTCLIPBOX);
	break;
	
      case SPR_ROCK:
	if (!P_GiveAmmo (player, am_misl,1))
	    return;
	player->message = DEH_String(GOTROCKET);
	break;
	
      case SPR_BROK:
	if (!P_GiveAmmo (player, am_misl,5))
	    return;
	player->message = DEH_String(GOTROCKBOX);
	break;
	
      case SPR_CELL:
	if (!P_GiveAmmo (player, am_cell,1))
	    return;
	player->message = DEH_String(GOTCELL);
	break;
	
      case SPR_CELP:
	if (!P_GiveAmmo (player, am_cell,5))
	    return;
	player->message = DEH_String(GOTCELLBOX);
	break;
	
      case SPR_SHEL:
	if (!P_GiveAmmo (player, am_shell,1))
	    return;
	player->message = DEH_String(GOTSHELLS);
	break;
	
      case SPR_SBOX:
	if (!P_GiveAmmo (player, am_shell,5))
	    return;
	player->message = DEH_String(GOTSHELLBOX);
	break;
	
      case SPR_BPAK:
	if (!player->backpack)
	{
	    for (i=0 ; i<NUMAMMO ; i++)
		player->maxammo[i] *= 2;
	    player->backpack = true;
	}
	for (i=0 ; i<NUMAMMO ; i++)
	    P_GiveAmmo (player, i, 1);
	player->message = DEH_String(GOTBACKPACK);
	break;
	
	// weapons
      case SPR_BFUG:
	if (!P_GiveWeapon (player, wp_bfg, false) )
	    return;
	player->message = DEH_String(GOTBFG9000);
	sound = sfx_wpnup;	
	break;
	
      case SPR_MGUN:
        if (!P_GiveWeapon(player, wp_chaingun,
                          (special->flags & MF_DROPPED) != 0))
            return;
	player->message = DEH_String(GOTCHAINGUN);
	sound = sfx_wpnup;	
	break;
	
      case SPR_CSAW:
	if (!P_GiveWeapon (player, wp_chainsaw, false) )
	    return;
	player->message = DEH_String(GOTCHAINSAW);
	sound = sfx_wpnup;	
	break;
	
      case SPR_LAUN:
	if (!P_GiveWeapon (player, wp_missile, false) )
	    return;
	player->message = DEH_String(GOTLAUNCHER);
	sound = sfx_wpnup;	
	break;
	
      case SPR_PLAS:
	if (!P_GiveWeapon (player, wp_plasma, false) )
	    return;
	player->message = DEH_String(GOTPLASMA);
	sound = sfx_wpnup;	
	break;
	
      case SPR_SHOT:
        if (!P_GiveWeapon(player, wp_shotgun,
                          (special->flags & MF_DROPPED) != 0))
            return;
	player->message = DEH_String(GOTSHOTGUN);
	sound = sfx_wpnup;	
	break;
		
      case SPR_SGN2:
        if (!P_GiveWeapon(player, wp_supershotgun,
                          (special->flags & MF_DROPPED) != 0))
            return;
	player->message = DEH_String(GOTSHOTGUN2);
	sound = sfx_wpnup;	
	break;
		
      default:
	I_Error ("P_SpecialThing: Unknown gettable thing");
    }
	
pickup_success:
    if (special->flags & MF_COUNTITEM)
	player->itemcount++;
    P_RemoveMobj (special);
    player->bonuscount += BONUSADD;
    if (player == &players[consoleplayer])
	S_StartSound (NULL, sound);
}


//
// KillMobj
//
void
P_KillMobj
( mobj_t*	source,
  mobj_t*	target )
{
    mobjtype_t	item;
    mobj_t*	mo;
	
    target->flags &= ~(MF_SHOOTABLE|MF_FLOAT|MF_SKULLFLY);

    if (target->type != MT_SKULL)
	target->flags &= ~MF_NOGRAVITY;

    target->flags |= MF_CORPSE|MF_DROPOFF;
    target->height >>= 2;

    if (source && source->player)
    {
	// count for intermission
	if (target->flags & MF_COUNTKILL)
	    source->player->killcount++;	
		
	// Goblin Dice Rollaz: Track survival mode kills
	G_SurvivalEnemyKilled();

	// Goblin Dice Rollaz: Track achievement kills
	G_TrackAchievementProgress(ACH_TYPE_KILLS, global_dice_stats.kills);
	if (target->flags & MF_BOSS)
	{
	    G_TrackAchievementProgress(ACH_TYPE_BOSSES, 0);
	}

	if (target->player)
	{
	    source->player->frags[target->player-players]++;

	    // Goblin Dice Rollaz: Dice Arena PvP announcements
	    if (IN_DICE_ARENA())
	    {
		P_BroadcastArenaKill(source->player - players, target->player - players);
	    }
	}

	// Goblin Dice Rollaz: Give XP for kills in RPG mode
	G_AddPlayerXPForKill(target, source);

	// Goblin Dice Rollaz: Add weapon mastery XP for kills
	G_AddWeaponKill(source->player, source->player->readyweapon);
    }
    else if (!netgame && (target->flags & MF_COUNTKILL) )
    {
	// count all monster deaths,
	// even those caused by other monsters
	players[0].killcount++;
	// Goblin Dice Rollaz: Track survival mode kills
	G_SurvivalEnemyKilled();
    }
    
    if (target->player)
    {
  	// count environment kills against you
  	if (!source)	
  	    target->player->frags[target->player-players]++;
  			
	target->flags &= ~MF_SOLID;
	target->player->playerstate = PST_DEAD;
	P_DropWeapon (target->player);

	target->player->powers[pw_dicefortune] = 0;

	// Goblin Dice Rollaz: Track player death for achievements
	G_TrackDeath();

	if (target->player == &players[consoleplayer]
	    && automapactive)
	{
	    // don't die in auto map,
	    // switch view prior to dying
	    AM_Stop ();
	}
	
    }

    if (target->health < -target->info->spawnhealth 
	&& target->info->xdeathstate)
    {
	P_SetMobjState (target, target->info->xdeathstate);
    }
    else
	P_SetMobjState (target, target->info->deathstate);
    target->tics -= P_Random()&3;

    if (target->tics < 1)
	target->tics = 1;
		
    //	I_StartSound (&actor->r, actor->info->deathsound);

    // In Chex Quest, monsters don't drop items.

    if (gameversion == exe_chex)
    {
        return;
    }

    // Drop stuff.
    // This determines the kind of object spawned
    // during the death frame of a thing.
    switch (target->type)
    {
      case MT_WOLFSS:
      case MT_POSSESSED:
	item = MT_CLIP;
	break;
	
      case MT_SHOTGUY:
	item = MT_SHOTGUN;
	break;
	
      case MT_CHAINGUY:
	item = MT_CHAINGUN;
	break;

      case MT_DWARF_TREASUREHUNTER:
	mo = P_SpawnMobj (target->x,target->y,ONFLOORZ, MT_AMMO_LIGHTDICE);
	mo->flags |= MF_DROPPED;
	mo = P_SpawnMobj (target->x+FRACUNIT,target->y,ONFLOORZ, MT_AMMO_LIGHTDICE);
	mo->flags |= MF_DROPPED;
	mo = P_SpawnMobj (target->x,target->y+FRACUNIT,ONFLOORZ, MT_AMMO_HEAVYDICE);
	mo->flags |= MF_DROPPED;
	return;
	
      default:
	return;
    }

    mo = P_SpawnMobj (target->x,target->y,ONFLOORZ, item);
    mo->flags |= MF_DROPPED;	// special versions of items
}




//
// P_DamageMobj
// Damages both enemies and players
// "inflictor" is the thing that caused the damage
//  creature or missile, can be NULL (slime, etc)
// "source" is the thing to target after taking damage
//  creature or NULL
// Source and inflictor are the same for melee attacks.
// Source can be NULL for slime, barrel explosions
// and other environmental stuff.
//
void
P_DamageMobj
( mobj_t*	target,
  mobj_t*	inflictor,
  mobj_t*	source,
  int 		damage )
{
    unsigned	ang;
    int		saved;
    player_t*	player;
    fixed_t	thrust;
    int		temp;
    boolean was_critical = false;
    int crit_roll = 0;
    int screen_x, screen_y;

    P_ValidateRNGState("P_DamageMobj_start");
    
    if ( !(target->flags & MF_SHOOTABLE) )
	return;	// shouldn't happen...
		
    if (target->health <= 0)
	return;

    if ( target->flags & MF_SKULLFLY )
    {
	target->momx = target->momy = target->momz = 0;
    }
    
    if (source && source->player && damage > 0)
    {
        int effectiveCritChance = crit_chance_default;
        int effectiveCritMultiplier = crit_multiplier_default;
        boolean guaranteed_crit = false;

        if (source->player->powers[pw_dicefortune])
        {
            guaranteed_crit = true;
            source->player->powers[pw_dicefortune] = 0;
        }

        if (source->player->powers[pw_critboost])
        {
            effectiveCritChance += goblin_crit_boost_bonus;
        }

        if (source->player->readyweapon >= 0 && source->player->readyweapon < NUMWEAPONS)
        {
            dice_weapon_info_t *dwi = &dice_weapon_info[source->player->readyweapon];
            if (dwi->die_type > 0)
            {
                effectiveCritChance = dwi->crit_chance;
                effectiveCritMultiplier = dwi->crit_multiplier;
            }
        }

        if (rpg_mode)
        {
            effectiveCritChance += G_GetPlayerCritBonus(source->player);
            effectiveCritChance += G_GetPlayerLevelCritBonus(source->player);
        }

        if (guaranteed_crit)
        {
            damage *= effectiveCritMultiplier;
            was_critical = true;
            crit_roll = (P_Random() % 20) + 1;
            if (guaranteed_crit)
            {
                S_StartSound(source, sfx_fortcrit);
            }
            P_BroadcastCritMessage(source->player - players, "GUARANTEED CRITICAL!", true, damage);
        }
        else
        {
            int targetCritResistance = 0;
            if (target && target->info)
            {
                targetCritResistance = target->info->crit_resistance;
                targetCritResistance += P_GetCritAuraBonus(target);
            }

            int adjustedCritChance = effectiveCritChance - targetCritResistance;
            if (adjustedCritChance < 0) adjustedCritChance = 0;

            if ((P_Random() % 100) < adjustedCritChance)
            {
                damage *= effectiveCritMultiplier;
                was_critical = true;
                crit_roll = (P_Random() % 20) + 1;
                P_BroadcastCritMessage(source->player - players, "CRITICAL HIT!", true, damage);
            }
        }

        if (source->player->powers[pw_doubledamage])
        {
            damage *= 2;
            P_BroadcastCritMessage(source->player - players, "DOUBLE DAMAGE!", false, damage);
        }

        // Goblin Dice Rollaz: Apply Snake Eyes powerup - double damage on hit
        if (source->player->powers[pw_snakeeyes] && target && !(target->flags & MF_CORPSE))
        {
            damage *= 2;
            P_BroadcastCritMessage(source->player - players, "SNAKE EYES!", false, damage);
        }

        // Goblin Dice Rollaz: Apply Dice Curse damage variance
        if (source && G_StatusEffectIsActive(source, st_dicecurse))
        {
            int curseMultiplier = G_GetDiceCurseDamageMultiplier(source);
            damage = (damage * curseMultiplier) / 100;
            if (source == &players[consoleplayer].mo && curseMultiplier != 100)
            {
                static char curemsg[64];
                if (curseMultiplier < 100)
                    snprintf(curemsg, sizeof(curemsg), "CURSED! %d%% damage", curseMultiplier);
                else
                    snprintf(curemsg, sizeof(curemsg), "FORTUNATE! %d%% damage", curseMultiplier);
                players[consoleplayer].message = curemsg;
            }
        }

        // Goblin Dice Rollaz: Apply Enraged status effect damage boost
        if (source && G_StatusEffectIsActive(source, st_enraged))
        {
            int enragedMultiplier = G_GetStatusEffectDamageMultiplier(source);
            damage = (damage * enragedMultiplier) / 100;
            if (source == &players[consoleplayer].mo)
            {
                players[consoleplayer].message = "ENRAGED!";
            }
        }

        // Goblin Dice Rollaz: Apply stun on critical hits (20% chance)
        if (was_critical && target && !target->player && target->health > 0)
        {
            if ((P_Random() % 100) < 20)
            {
                G_StatusEffectApply(target, st_stunned, STUNNED_TICS);
            }
        }

        // Goblin Dice Rollaz: Crit combo system - track and apply combo multiplier
        if (source && source->player)
        {
            // Update combo timer (decays over time)
            if (source->player->crit_combo_timer > 0)
            {
                source->player->crit_combo_timer--;
            }

            if (was_critical)
            {
                // Increment combo on critical hit
                source->player->crit_combo++;
                // Reset combo timeout
                source->player->crit_combo_timer = crit_combo_timeout;

                // Apply combo bonus damage
                int comboLevel = source->player->crit_combo;
                if (comboLevel > 0)
                {
                    // Cap at max multiplier
                    int bonusMultiplier = 1 + (comboLevel * crit_combo_bonus / 100);
                    if (bonusMultiplier > crit_combo_max)
                    {
                        bonusMultiplier = crit_combo_max;
                    }

                    if (bonusMultiplier > 1)
                    {
                        damage *= bonusMultiplier;
                        // Show combo message
                        if (source == &players[consoleplayer].mo)
                        {
                            static char combomsg[64];
                            snprintf(combomsg, sizeof(combomsg), "COMBO x%d!", bonusMultiplier);
                            players[consoleplayer].message = combomsg;
                        }
                    }
                }
            }
            else
            {
                // Reset combo on non-crit hit
                source->player->crit_combo = 0;
                source->player->crit_combo_timer = 0;
            }
        }
    }

    // Goblin Dice Rollaz: Shield frontal damage reduction
    if (target && target->info && target->info->shield_reduction > 0)
    {
        int shieldReduction = target->info->shield_reduction;
        
        // Check if attacker is in front of target (facing shield)
        if (source)
        {
            angle_t targetAngle = R_PointToAngle2(target->x, target->y, source->x, source->y);
            angle_t targetFacing = target->angle;
            angle_t angleDiff = targetAngle - targetFacing;
            
            // If attacker is in front (within ~60 degrees either side)
            if (angleDiff < ANG60 || angleDiff > ANG300)
            {
                int reducedDamage = (damage * (100 - shieldReduction)) / 100;
                if (reducedDamage < damage)
                {
                    damage = reducedDamage;
                }
            }
        }
    }

    // Goblin Dice Rollaz: Rear weakness damage bonus
    if (target && target->info && target->info->rear_weakness > 0)
    {
        int rearWeakness = target->info->rear_weakness;
        
        // Check if attacker is behind target
        if (source)
        {
            angle_t targetAngle = R_PointToAngle2(target->x, target->y, source->x, source->y);
            angle_t targetFacing = target->angle;
            angle_t angleDiff = targetAngle - targetFacing;
            
            // If attacker is behind (within ~60 degrees of rear)
            if (angleDiff > ANG240 && angleDiff < ANG120)
            {
                int bonusDamage = (damage * rearWeakness) / 100;
                if (bonusDamage > 0)
                {
                    damage += bonusDamage;
                }
            }
        }
    }

    // Goblin Dice Rollaz: Armor damage type bonus vs armored dwarves
    if (source && source->player)
    {
        int weapon = source->player->readyweapon;
        if (weapon >= 0 && weapon < NUMWEAPONS)
        {
            damage_type_t wpnDtype = dice_weapon_info[weapon].damage_type;
            if (wpnDtype == DAMAGETYPE_ARMOR && target)
            {
                // Check if target is an armored dwarf
                if (target->type == MT_DWARF_ARMORED ||
                    target->type == MT_DWARF_DEFENDER ||
                    target->type == MT_DWARF_CAPTAIN ||
                    target->type == MT_DWARF_TURRET ||
                    target->type == MT_DWARF_BOMBARDIER ||
                    target->type == MT_DWARF_ALCHEMIST ||
                    target->type == MT_DWARF_IRONCLAD ||
                    target->type == MT_DWARF_COMMANDER ||
                    target->type == MT_DWARF_TREASUREHUNTER ||
                    target->type == MT_DWARF_JUGGERNAUT)
                {
                    // 50% bonus damage vs armored dwarves
                    damage = (damage * 150) / 100;
                }
            }
            // Goblin Dice Rollaz: d50 Dwarf's Bane - 3x vs ALL dwarf enemies
            if (weapon == wp_d50 && target)
            {
                if (target->type == MT_DWARF_BERSERKER ||
                    target->type == MT_DWARF_ENGINEER ||
                    target->type == MT_DWARF_BOMB ||
                    target->type == MT_DWARF_TURRET ||
                    target->type == MT_DWARF_DEFENDER ||
                    target->type == MT_DWARF_MARKSMAN ||
                    target->type == MT_DWARF_MINER ||
                    target->type == MT_DWARF_CAPTAIN ||
                    target->type == MT_DWARF_BOMBARDIER ||
                    target->type == MT_DWARF_ARMORED ||
                    target->type == MT_DWARF_ALCHEMIST ||
                    target->type == MT_DWARF_IRONCLAD ||
                    target->type == MT_DWARF_STONECUTTER ||
                    target->type == MT_DWARF_THUNDERMAGE ||
                    target->type == MT_DWARF_WARLORD ||
                    target->type == MT_DWARF_RUNESMITH ||
                    target->type == MT_DWARF_RUNEBEARER ||
                    target->type == MT_DWARF_HIGHPRIEST ||
                    target->type == MT_DWARF_COMMANDER ||
                    target->type == MT_DWARF_TINKERER ||
                    target->type == MT_DWARF_DRILLTANK ||
                    target->type == MT_DWARF_BARRELELITE ||
                    target->type == MT_DWARF_SHADOWBLADE ||
                    target->type == MT_DWARF_GEOLOGIST ||
                    target->type == MT_DWARF_TREASUREHUNTER ||
                    target->type == MT_DWARF_JUGGERNAUT ||
                    target->type == MT_DWARF_SAPPER)
                {
                    // 3x damage vs dwarf enemies
                    damage = damage * 3;
                }
            }
            }
        }
    }
	
    player = target->player;
    if (player && gameskill == sk_baby)
	damage >>= 1; 	// take half damage in trainer mode
		

    // Some close combat weapons should not
    // inflict thrust and push the victim out of reach,
    // thus kick away unless using the chainsaw.
    if (inflictor
	&& !(target->flags & MF_NOCLIP)
	&& (!source
	    || !source->player
	    || source->player->readyweapon != wp_chainsaw))
    {
	ang = R_PointToAngle2 ( inflictor->x,
				inflictor->y,
				target->x,
				target->y);
		
	thrust = damage*(FRACUNIT>>3)*100/target->info->mass;

	// make fall forwards sometimes
	if ( damage < 40
	     && damage > target->health
	     && target->z - inflictor->z > 64*FRACUNIT
	     && (P_Random ()&1) )
	{
	    ang += ANG180;
	    thrust *= 4;
	}
		
	ang >>= ANGLETOFINESHIFT;
	target->momx += FixedMul (thrust, finecosine[ang]);
	target->momy += FixedMul (thrust, finesine[ang]);
    }
    
    // player specific
    if (player)
    {
	// end of game hell hack
	if (target->subsector->sector->special == 11
	    && damage >= target->health)
	{
	    damage = target->health - 1;
	}
	

	// Below certain threshold,
	// ignore damage in GOD mode, or with INVUL power.
	if ( damage < 1000
	     && ( (player->cheats&CF_GODMODE)
		  || player->powers[pw_invulnerability] ) )
	{
	    return;
	}
	
	if (player->armortype)
	{
	    if (player->armortype == 1)
		saved = damage/3;
	    else
		saved = damage/2;
	    
	    if (player->armorpoints <= saved)
	    {
		// armor is used up
		saved = player->armorpoints;
		player->armortype = 0;
	    }
	    player->armorpoints -= saved;
	    damage -= saved;
	}
	player->health -= damage; 	// mirror mobj health here for Dave
	if (player->health < 0)
	    player->health = 0;
	
	player->attacker = source;
	player->damagecount += damage;	// add damage after armor / invuln

	if (player->damagecount > 100)
	    player->damagecount = 100;	// teleport stomp does 10k points...
	
	temp = damage < 100 ? damage : 100;

	if (player == &players[consoleplayer])
	    ;
    }

    // Goblin Dice Rollaz: Dice Arena PvP damage boost
    // Increase damage in PvP mode for more exciting combat
    if (IN_DICE_ARENA() && target->player && source && source->player)
    {
        damage = (damage * dice_arena_damage_mult) / 100;
    }

    // do the damage
    target->health -= damage;	
    
    // Goblin Dice Rollaz: Log damage to damage log
    if (source && source->player && damage > 0)
    {
        const char *target_name = NULL;
        const char *weapon_name = NULL;
        
        if (target && target->info && target->info->name)
            target_name = target->info->name;
        
        if (source->player->readyweapon >= 0 && source->player->readyweapon < NUMWEAPONS)
        {
            weapon_name = weaponinfo[source->player->readyweapon].name;
        }
        
        DMG_AddDamageLog(damage, was_critical, crit_roll, target_name, weapon_name);
        
        // Goblin Dice Rollaz: Debug overlay - record last damage info
        if (source && source->player)
        {
            int debugCritChance = 0;
            int debugCritMult = 1;
            int diceRoll = 0;
            
            dice_weapon_info_t *dwi = &dice_weapon_info[source->player->readyweapon];
            if (dwi->die_type > 0)
            {
                debugCritChance = dwi->crit_chance;
                debugCritMult = dwi->crit_multiplier;
            }
            
            DBG_SetLastDamage(damage, debugCritChance, debugCritMult, 
                             (P_Random() % 20) + 1, crit_roll, 
                             was_critical, guaranteed_crit, target ? target->type : 0);
            
            if (target && target->info && target->info->name)
            {
                DBG_SetLastHitTarget(target->info->name);
            }
        }
    }
    
    // Goblin Dice Rollaz: Boss phase transition
    // If boss health drops below phase threshold and hasn't transitioned yet
    if (target->health > 0 && target->info && target->info->phase2_health > 0)
    {
        // Check if boss should enter phase 2
        // We use a flag to track if already in phase 2
        // The flag is stored in a way compatible with the existing mobj structure
        // using the special1 field as phase tracker (0 = phase 1, 1 = phase 2)
        
        if (target->health <= target->info->phase2_health && target->special1 == 0)
        {
            // Boss enters phase 2!
            target->special1 = 1;  // Mark as phase 2
            
            // Goblin Dice Rollaz: Play phase transition music
            S_ChangeMusic(mus_boss_victory, true);
            
            // Trigger phase 2 state based on boss type
            if (target->type == MT_GOBLIN_KING)
            {
                P_SetMobjState(target, S_GKNG_P2_STND);
                // Play phase transition sound
                S_StartSound(target, sfx_bossit);
                // Show message to player
                if (source && source->player == &players[consoleplayer])
                {
                    players[consoleplayer].message = "GOBLIN KING ENRAGED!";
                }
            }
            else if (target->type == MT_DWARVEN_WAR_MACHINE)
            {
                P_SetMobjState(target, S_DWM_P2_STND);
                // Play phase transition sound
                S_StartSound(target, sfx_cybsit);
                // Show message to player
                if (source && source->player == &players[consoleplayer])
                {
                    players[consoleplayer].message = "Dwarven War Machine OVERDRIVE!";
                }
            }
        }
    }	
    
    // Goblin Dice Rollaz: Apply freeze effect from shaman freeze projectile
    // Use both legacy freeze_tics and new g_status framework for compatibility
    if (inflictor && inflictor->type == MT_SHAMAN_FREEZE && target->health > 0)
    {
        target->freeze_tics = SHAMAN_FREEZE_DURATION;
        // Also apply g_status frozen effect with configurable duration and speed reduction
        G_StatusEffectApply(target, st_frozen, FROZEN_TICS);
    }
    
    // Goblin Dice Rollaz: Apply slow/frozen effect from d24 Hourglass Cannon
    // On hit: slow effect; on crit: frozen effect
    if (inflictor && inflictor->type == MT_D24PROJECTILE && target->health > 0)
    {
        // Check if this was a critical hit (we can tell by the damage multiplier)
        // If damage >= 15, it was a crit (base damage 1 * 15 = 15 from damage_table[6])
        boolean wasCrit = (damage >= 15);
        
        if (wasCrit)
        {
            // Full freeze on crit
            target->freeze_tics = FROZEN_TICS;
            G_StatusEffectApply(target, st_frozen, FROZEN_TICS);
        }
        else
        {
            // Regular slow on hit (50% speed for half the frozen duration)
            G_StatusEffectApply(target, st_frozen, FROZEN_TICS / 2);
        }
    }
    
    if (damage > 0 && target->health > 0)
    {
        fixed_t tr_x = target->x - viewx;
        fixed_t tr_y = target->y - viewy;
        fixed_t gxt = FixedMul(tr_x, viewcos);
        fixed_t gyt = -FixedMul(tr_y, viewsin);
        fixed_t tz = gxt - gyt;
        
        if (tz > 0)
        {
            fixed_t xscale = FixedDiv(projection, tz);
            gxt = -FixedMul(tr_x, viewsin);
            gyt = FixedMul(tr_y, viewcos);
            fixed_t tx = -(gyt + gxt);
            
            int screen_x = (centerxfrac + FixedMul(tx, xscale)) >> FRACBITS;
            int screen_y = (centeryfrac + FixedMul(target->z - viewz, xscale)) >> FRACBITS;
            
            if (screen_x > 0 && screen_x < SCREENWIDTH && screen_y > 0 && screen_y < SCREENHEIGHT)
            {
                int weapon = -1;
                damage_type_t dtype = DAMAGETYPE_NORMAL;
                if (source && source->player)
                {
                    weapon = source->player->readyweapon;
                    if (weapon >= 0 && weapon < NUMWEAPONS)
                    {
                        dtype = dice_weapon_info[weapon].damage_type;
                        // Goblin Dice Rollaz: Elemental Chaos weapon - random element on each hit
                        if (dtype == DAMAGETYPE_ELEMENTAL)
                        {
                            int roll = P_Random() % 4;
                            if (roll == 0)
                                dtype = DAMAGETYPE_FIRE;
                            else if (roll == 1)
                                dtype = DAMAGETYPE_ICE;
                            else if (roll == 2)
                                dtype = DAMAGETYPE_LIGHTNING;
                            else
                                dtype = DAMAGETYPE_POISON;
                        }
                    }
                }
                PREDICT_RecordDamage(screen_x, screen_y, damage, was_critical, crit_roll, weapon);
                DMG_AddDamage(screen_x, screen_y, damage, was_critical, crit_roll, dtype);
                if (was_critical && target)
                {
                    P_SpawnCritParticles(target->x, target->y, target->z + (target->height / 2), damage, crit_roll);
                }
            }
        }
    }
    
    if (target->health <= 0)
    {
        // Goblin Dice Rollaz: Add kill confirmation popup
        if (source && source->player && !(target->flags & MF_MISSILE) && kill_confirm_enabled)
        {
            fixed_t tr_x = target->x - viewx;
            fixed_t tr_y = target->y - viewy;
            fixed_t gxt = FixedMul(tr_x, viewcos);
            fixed_t gyt = -FixedMul(tr_y, viewsin);
            fixed_t tz = gxt - gyt;

            if (tz > 0)
            {
                fixed_t xscale = FixedDiv(projection, tz);
                gxt = -FixedMul(tr_x, viewsin);
                gyt = FixedMul(tr_y, viewcos);
                fixed_t tx = -(gyt + gxt);

                int screen_x = (centerxfrac + FixedMul(tx, xscale)) >> FRACBITS;
                int screen_y = (centeryfrac + FixedMul(target->z - viewz + target->height, xscale)) >> FRACBITS;

                if (screen_x > 0 && screen_x < SCREENWIDTH && screen_y > 0 && screen_y < SCREENHEIGHT)
                {
                    DMG_AddKillConfirm(screen_x, screen_y, target);
                }
            }
        }

        // Goblin Dice Rollaz: Play boss victory music when boss dies
        if (target->type == MT_GOBLIN_KING || target->type == MT_DWARVEN_WAR_MACHINE)
        {
            S_ChangeMusic(mus_boss_victory, true);
            // Goblin Dice Rollaz: Unlock arena when boss dies
            if (arena_locked && arena_lock_tag > 0)
            {
                EV_DoArenaLock(arena_lock_tag, false);
                arena_locked = 0;
            }
        }
        
	P_KillMobj (source, target);
	return;
    }

    if ( (P_Random () < target->info->painchance)
	 && !(target->flags&MF_SKULLFLY) )
    {
	target->flags |= MF_JUSTHIT;	// fight back!
	
	P_SetMobjState (target, target->info->painstate);
    }
			
    target->reactiontime = 0;		// we're awake now...	

    if ( (!target->threshold || target->type == MT_VILE)
	 && source && (source != target || gameversion < exe_doom_1_5)
	 && source->type != MT_VILE)
    {
	// if not intent on another player,
	// chase after this one
	target->target = source;
	target->threshold = BASETHRESHOLD;
	if (target->state == &states[target->info->spawnstate]
	    && target->info->seestate != S_NULL)
	    P_SetMobjState (target, target->info->seestate);
    }

    // Goblin Dice Rollaz: Track boss target for health bar overlay
    if (source && source->player)
    {
        // Player is attacking - track the target if it's a boss
        if (target->flags & MF_BOSS)
        {
            source->player->bosstarget = target;
        }
    }
    if (target->player && source && (source->flags & MF_BOSS))
    {
        // Boss is attacking player - track the boss
        target->player->bosstarget = source;
    }
    // If player damages a boss, also track it
    if (target->player == NULL && source && source->player && (target->flags & MF_BOSS))
    {
        source->player->bosstarget = target;
    }
    // Clear boss target if boss is dead
    if (target->player && target->player->bosstarget && target->player->bosstarget->health <= 0)
    {
        target->player->bosstarget = NULL;
    }
    // Clear if source (attacker) dies
    if (source && source->player && source->player->bosstarget && source->player->bosstarget->health <= 0)
    {
        source->player->bosstarget = NULL;
    }
			
}

