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
//
//


#ifndef __D_PLAYER__
#define __D_PLAYER__


// The player data structure depends on a number
// of other structs: items (internal inventory),
// animation states (closely tied to the sprites
// used to represent them, unfortunately).
#include "d_items.h"
#include "p_pspr.h"

// In addition, the player is just a special
// case of the generic moving object/actor.
#include "p_mobj.h"

// Finally, for odd reasons, the player input
// is buffered within the player data struct,
// as commands per game tick.
#include "d_ticcmd.h"

#include "net_defs.h"




//
// Player states.
//
typedef enum
{
    // Playing or camping.
    PST_LIVE,
    // Dead on the ground, view follows killer.
    PST_DEAD,
    // Ready to restart/respawn???
    PST_REBORN		

} playerstate_t;


//
// Player internal flags, for cheats and debug.
//
typedef enum
{
    // No clipping, walk through barriers.
    CF_NOCLIP		= 1,
    // No damage, no health loss.
    CF_GODMODE		= 2,
    // Not really a cheat, just a debug aid.
    CF_NOMOMENTUM	= 4,
    // Infinite ammo, never run out.
    CF_INFINITE_AMMO	= 8

} cheat_t;


//
// Extended player object info: player_t
//
typedef struct player_s
{
    mobj_t*		mo;
    playerstate_t	playerstate;
    ticcmd_t		cmd;

    // Determine POV,
    //  including viewpoint bobbing during movement.
    // Focal origin above r.z
    fixed_t		viewz;
    // Base height above floor for viewz.
    fixed_t		viewheight;
    // Bob/squat speed.
    fixed_t         	deltaviewheight;
    // bounded/scaled total momentum.
    fixed_t         	bob;	

    // This is only used between levels,
    // mo->health is used during levels.
    int			health;	
    int			armorpoints;
    // Armor type is 0-2.
    int			armortype;	

    // Power ups. invinc and invis are tic counters.
    int			powers[NUMPOWERS];
    boolean		cards[NUMCARDS];
    boolean		backpack;
    
    // Frags, kills of other players.
    int			frags[MAXPLAYERS];
    weapontype_t	readyweapon;
    
    // Is wp_nochange if not changing.
    weapontype_t	pendingweapon;

    int                 weaponowned[NUMWEAPONS];
    int			ammo[NUMAMMO];
    int			maxammo[NUMAMMO];

    // True if button down last tic.
    int			attackdown;
    // Goblin Dice Rollaz: True if alternate attack button down last tic
    int			altattackdown;
    int			usedown;

    // Bit flags, for cheats and debug.
    // See cheat_t, above.
    int			cheats;		

    // Refired shots are less accurate.
    int			refire;		

     // For intermission stats.
    int			killcount;
    int			itemcount;
    int			secretcount;

    // Hint messages.
    const char		*message;
    
    // For screen flashing (red or bright).
    int			damagecount;
    int			bonuscount;

    // Who did damage (NULL for floors/ceilings).
    mobj_t*		attacker;
    
    // So gun flashes light up areas.
    int			extralight;

    // Current PLAYPAL, ???
    //  can be set to REDCOLORMAP for pain, etc.
    int			fixedcolormap;

    // Player skin colorshift,
    //  0-3 for which color to draw player.
    int			colormap;	

    // Overlay view sprites (gun, etc).
    pspdef_t		psprites[NUMPSPRITES];

    // True if secret level has been done.
    boolean		didsecret;	

    // Goblin Dice Rollaz: Weapon charging for charged attacks
    int			weaponcharge;

    // Goblin Dice Rollaz: Boss target for health bar overlay
    struct mobj_s	*bosstarget;

    // Goblin Dice Rollaz: Crit combo system - consecutive crit streak
    int			crit_combo;
    // Time remaining (in tics) before combo resets
    int			crit_combo_timer;

    // Goblin Dice Rollaz: RPG Progression System
    int			experience;
    int			level;
    int			stat_points;
    int			strength_bonus;
    int			dexterity_bonus;
    int			vitality_bonus;
    int			luck_bonus;

    // Goblin Dice Rollaz: Weapon Mastery System
    // Number of kills per weapon type
    int			weapon_kills[NUMWEAPONS];
    // Mastery level per weapon (0-10)
    int			weapon_mastery[NUMWEAPONS];
    // XP toward next mastery level per weapon
    int			weapon_mastery_xp[NUMWEAPONS];

    // Goblin Dice Rollaz: Weapon recoil offset
    // Current weapon kickback from firing (decays over time)
    fixed_t		weapon_recoil;

    // Goblin Dice Rollaz: Siege weapon mounted state
    // Pointer to mounted siege weapon (NULL when not mounted)
    struct mobj_s	*mounted_siege_weapon;
    // Angle offset when mounted (for rotating siege weapon)
    angle_t		siege_weapon_angle;

    // Goblin Dice Rollaz: Dash Token powerup state
    // Cooldown between dashes (in tics)
    int			dash_cooldown;
    // Invulnerability frames during dash (in tics)
    int			dash_iframes;

    // Goblin Dice Rollaz: Dodge roll ability state
    // Cooldown between rolls (in tics)
    int			roll_cooldown;
    // Invulnerability frames during roll (in tics)
    int			roll_iframes;
    // Direction of roll (negative = left, positive = right, 0 = not rolling)
    int			roll_direction;

    // Goblin Dice Rollaz: Madness Die powerup damage modifier (100 = normal)
    int			madness_damage_mod;

    // Goblin Dice Rollaz: Mantle/climbing state
    // Tics remaining in mantle animation
    int			mantle_tics;
    // Height being mantled to
    fixed_t		mantle_height;

    // Goblin Dice Rollaz: Crouch/slide state
    // True if player is currently crouching
    boolean		crouching;
    // True if player was crouching last frame (for transition detection)
    boolean		was_crouching;
    // Tics remaining in slide animation (when releasing crouch key while moving)
    int			slide_tics;
    // Height offset from crouching (smooth transition)
    fixed_t		crouch_height_offset;

    // Goblin Dice Rollaz: Wall jump state
    // Cooldown between wall jumps (in tics)
    int			walljump_cooldown;
    // Direction of wall (-1 = left wall, 1 = right wall, 0 = not touching wall)
    int			wall_contact;
    // Tics remaining in wall slide (slower fall when hugging wall)
    int			wallslide_tics;

    // Goblin Dice Rollaz: Ledge grab state
    // True if player is currently hanging from a ledge
    boolean		ledge_hanging;
    // Height of the ledge being grabbed
    fixed_t		ledge_height;
    // X position of ledge grab point
    fixed_t		ledge_x;
    // Y position of ledge grab point
    fixed_t		ledge_y;
    // Tics remaining in climb animation
    int			ledge_climb_tics;

    // Goblin Dice Rollaz: Swimming/flooded area state
    // True if player is currently swimming (in flooded sector)
    boolean		swimming;
    // Height of water in flooded sector
    fixed_t		water_level;
    // Z position of water surface (ceiling of flooded sector)
    fixed_t		water_z;

    // Goblin Dice Rollaz: Ladder climbing state
    // True if player is currently climbing a ladder
    boolean		on_ladder;
    // Z position of ladder top
    fixed_t		ladder_top;
    // Z position of ladder bottom
    fixed_t		ladder_bottom;
    // Current ladder being climbed (for position tracking)
    fixed_t		ladder_x;
    fixed_t		ladder_y;

    // Goblin Dice Rollaz: Momentum-based movement system
    // Forward/backward velocity component
    fixed_t		momentum_forward;
    // Left/right velocity component
    fixed_t		momentum_side;
    // Acceleration rate (higher = faster buildup)
    fixed_t		momentum_accel;
    // Friction rate (higher = faster slowdown)
    fixed_t		momentum_friction;

} player_t;


//
// INTERMISSION
// Structure passed e.g. to WI_Start(wb)
//
typedef struct
{
    boolean	in;	// whether the player is in game
    
    // Player stats, kills, collected items etc.
    int		skills;
    int		sitems;
    int		ssecret;
    int		stime; 
    int		frags[4];
    int		score;	// current score on entry, modified on return
  
} wbplayerstruct_t;

typedef struct
{
    int		epsd;	// episode # (0-2)

    // if true, splash the secret level
    boolean	didsecret;
    
    // previous and next levels, origin 0
    int		last;
    int		next;	
    
    int		maxkills;
    int		maxitems;
    int		maxsecret;
    int		maxfrags;

    // the par time
    int		partime;
    
    // index of this player in game
    int		pnum;	

    wbplayerstruct_t	plyr[MAXPLAYERS];

} wbstartstruct_t;


#endif
