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
//  Internally used data structures for virtually everything,
//   lots of other stuff.
//

#ifndef __DOOMDEF__
#define __DOOMDEF__

#include <stdio.h>
#include <string.h>

#include "doomtype.h"
#include "i_timer.h"
#include "d_mode.h"

//
// Global parameters/defines.
//
// DOOM version
#define DOOM_VERSION 109


// If rangecheck is undefined,
// most parameter validation debugging code will not be compiled
#define RANGECHECK

// The maximum number of players, multiplayer/networking.
#define MAXPLAYERS 4

// The current state of the game: whether we are
// playing, gazing at the intermission screen,
// the game final animation, or a demo. 
typedef enum
{
    GS_LEVEL,
    GS_INTERMISSION,
    GS_FINALE,
    GS_DEMOSCREEN,
} gamestate_t;

typedef enum
{
    ga_nothing,
    ga_loadlevel,
    ga_newgame,
    ga_loadgame,
    ga_savegame,
    ga_playdemo,
    ga_completed,
    ga_victory,
    ga_worlddone,
    ga_screenshot
} gameaction_t;

//
// Difficulty/skill settings/filters.
//

// Skill flags.
#define	MTF_EASY		1
#define	MTF_NORMAL		2
#define	MTF_HARD		4

// Deaf monsters/do not react to sound.
#define	MTF_AMBUSH		8


//
// Key cards.
//
typedef enum
{
    it_bluecard,
    it_yellowcard,
    it_redcard,
    it_blueskull,
    it_yellowskull,
    it_redskull,
    
    NUMCARDS
    
} card_t;



// The defined weapons,
//  including a marker indicating
//  user has not changed weapon.
typedef enum
{
    wp_fist,
    wp_d6blaster,
    wp_d20cannon,
    wp_d12,       // d12 heavy impact weapon
    wp_shotgun,
    wp_chaingun,
    wp_missile,
    wp_plasma,
    wp_bfg,
    wp_chainsaw,
    wp_supershotgun,
    wp_percentile,    // Percentile dice (rolls 1-100)
    wp_d4,            // d4 throwing knives
    wp_d8,            // d8 balanced mid-tier weapon
    wp_d10,           // d10 ricochet weapon (projectiles bounce once)
    wp_twind6,        // twin d6 scatter weapon (close-range burst)
    wp_arcaned20,     // arcane d20 beam (continuous roll tick damage)
    wp_cursed,        // cursed die (high damage, self-risk mechanic)
    wp_d2,            // d2 Flip of Fate (binary damage, 50% 1dmg/50% 2dmg)
    wp_d3,            // d3 Skewered Luck (guaranteed 1-3 damage, piercing)
    wp_d7,            // d7 Seven Veils (ore fragment splash, 14% crit)

    NUMWEAPONS,
    
    // No pending weapon change.
    wp_nochange

} weapontype_t;


// Ammunition types defined.
typedef enum
{
    am_clip,	// Pistol / chaingun ammo.
    am_shell,	// Shotgun / double barreled shotgun.
    am_cell,	// Plasma rifle, BFG.
    am_misl,	// Missile launcher.
    am_lightdice,    // Light dice: d4, d6, d8
    am_heavydice,    // Heavy dice: d10, d12, d20, percentile
    am_arcanedice,   // Arcane dice: magic/dice spells
    NUMAMMO,
    am_noammo	// Unlimited for chainsaw / fist.	
	
} ammotype_t;


// Additional colormap indices for special effects
#define INVERSECOLORMAP        32
#define CRITCOLORMAP           20
#define DOUBLEDAMAGECOLORMAP   21
#define NUMCOLORMAPS           33


// Power up artifacts.
typedef enum
{
    pw_invulnerability,
    pw_strength,
    pw_invisibility,
    pw_ironfeet,
    pw_allmap,
    pw_infrared,
    pw_critboost,
    pw_doubledamage,
    pw_dicefortune,
    NUMPOWERS
    
} powertype_t;



//
// Power up durations,
//  how many seconds till expiration,
//  assuming TICRATE is 35 ticks/second.
//
typedef enum
{
    INVULNTICS	= (30*TICRATE),
    INVISTICS	= (60*TICRATE),
    INFRATICS	= (120*TICRATE),
    IRONTICS	= (60*TICRATE),
    CRITBOOSTTICS = (30*TICRATE),
    DOUBLEDAMAGETICS = (30*TICRATE)
    
} powerduration_t;

typedef enum
{
    st_none,
    st_burning,
    st_frozen,
    st_stunned,
    st_dicecurse,
    st_enraged,
    NUMSTATUSEFFECTS
    
} statuseffect_t;

#define STATUSEFFECT_FLAG_TIMED      0x0001
#define STATUSEFFECT_FLAG_DAMAGE    0x0002
#define STATUSEFFECT_FLAG_MOVESLOW  0x0004
#define STATUSEFFECT_FLAG_ATTACKDISABLE 0x0008
#define STATUSEFFECT_FLAG_DAMAGEBOOST  0x0010

typedef struct
{
    int         effect_id;
    const char* name;
    int         default_duration;
    int         flags;
    int         damage_per_second;
    int         move_speed_multiplier;
    int         damage_multiplier;
    int         color;
} status_info_t;

#endif          // __DOOMDEF__
