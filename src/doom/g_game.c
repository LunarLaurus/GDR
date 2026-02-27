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
// DESCRIPTION:  none
//



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "doomdef.h" 
#include "doomkeys.h"
#include "doomstat.h"

#include "deh_main.h"
#include "deh_misc.h"

#include "z_zone.h"
#include "f_finale.h"
#include "m_argv.h"
#include "m_controls.h"
#include "m_misc.h"
#include "m_menu.h"
#include "m_random.h"
#include "i_glob.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_timer.h"
#include "i_input.h"
#include "i_swap.h"
#include "i_video.h"

#include "p_setup.h"
#include "p_saveg.h"
#include "p_tick.h"

#include "d_main.h"

#include "wi_stuff.h"
#include "hu_stuff.h"
#include "st_stuff.h"
#include "am_map.h"
#include "statdump.h"

// Needs access to LFB.
#include "v_video.h"

#include "w_wad.h"

#include "p_local.h" 

#include "s_sound.h"

#include "g_rpg.h"
#include "g_survival.h"
#include "g_timeattack.h"
#include "g_achievement.h"
#include "g_leaderboard.h"
#include "g_stats.h"
#include "g_game.h"

// Data.
#include "dstrings.h"
#include "sounds.h"

// SKY handling - still the wrong place.
#include "r_data.h"
#include "r_sky.h


#define SAVEGAMESIZE	0x2c000

void	G_ReadDemoTiccmd (ticcmd_t* cmd); 
void	G_WriteDemoTiccmd (ticcmd_t* cmd); 
void	G_PlayerReborn (int player); 
 
void	G_DoReborn (int playernum); 
 
void	G_DoLoadLevel (void); 
void	G_DoNewGame (void); 
void	G_DoPlayDemo (void); 
void	G_DoCompleted (void); 
void	G_DoVictory (void); 
void	G_DoWorldDone (void); 
void	G_DoSaveGame (void); 
 
// Gamestate the last time G_Ticker was called.

gamestate_t     oldgamestate; 
 
gameaction_t    gameaction; 
gamestate_t     gamestate; 
skill_t         gameskill; 
boolean		respawnmonsters;
int             gameepisode; 
int             gamemap; 

// Goblin Dice Rollaz: Challenge mode flags
boolean         challenge_critonly;
boolean         challenge_nopowerups;
boolean         challenge_hardcore; 

// Goblin Dice Rollaz: Auto-save on level transition
boolean         auto_save_enabled = true; 

// If non-zero, exit the level after this number of minutes.

int             timelimit;

boolean         paused; 
boolean         sendpause;             	// send a pause event next tic 
boolean         sendsave;             	// send a save event next tic 
boolean         usergame;               // ok to save / end game 
 
boolean         timingdemo;             // if true, exit with report on completion 
boolean         nodrawers;              // for comparative timing purposes 
int             starttime;          	// for comparative timing purposes  	 
 
boolean         viewactive; 
 
int             deathmatch;           	// only if started as net death 
boolean         netgame;                // only true if packets are broadcast 
boolean         playeringame[MAXPLAYERS]; 
player_t        players[MAXPLAYERS]; 

boolean         turbodetected[MAXPLAYERS];
 
int             consoleplayer;          // player taking events and displaying 
int             displayplayer;          // view being displayed 
int             levelstarttic;          // gametic at level start 
int             totalkills, totalitems, totalsecret;    // for intermission 
 
char           *demoname;
boolean         demorecording; 
boolean         demoplayback;
boolean		netdemo; 
byte*		demo_p;
byte*		demoend; 
byte*		demo_buffer_start;
byte*		demo_buffer;        // for demo playback from file
size_t              demo_buffer_size;  // size of demo_buffer
boolean         singledemo;            	// quit after playing a demo from cmdline
 
boolean         precache = true;        // if true, load all graphics at start 

boolean         testcontrols = false;    // Invoked by setup to test controls
int             testcontrols_mousespeed;
 

 
wbstartstruct_t wminfo;               	// parms for world map / intermission 
 
byte		consistancy[MAXPLAYERS][BACKUPTICS]; 
 
#define MOVESPEED_SLOW      0x19
#define MOVESPEED_FAST      0x32
#define SIDESPEED_SLOW      0x18
#define SIDESPEED_FAST      0x28
#define TURBOTHRESHOLD      0x32
#define MAXPLMOVE           (MOVESPEED_FAST)

fixed_t         forwardmove[2] = {MOVESPEED_SLOW, MOVESPEED_FAST};
fixed_t         sidemove[2] = {SIDESPEED_SLOW, SIDESPEED_FAST};
fixed_t         angleturn[3] = {640, 1280, 320};    // + slow turn 

static int *weapon_keys[] = {
    &key_weapon1,
    &key_weapon2,
    &key_weapon3,
    &key_weapon4,
    &key_weapon5,
    &key_weapon6,
    &key_weapon7,
    &key_weapon8,
    &key_weapon9
};

// Set to -1 or +1 to switch to the previous or next weapon.

static int next_weapon = 0;

// Used for prev/next weapon keys.

static const struct
{
    weapontype_t weapon;
    weapontype_t weapon_num;
} weapon_order_table[] = {
    { wp_fist,            wp_fist },
    { wp_chainsaw,        wp_fist },
    { wp_pistol,          wp_pistol },
    { wp_shotgun,         wp_shotgun },
    { wp_supershotgun,    wp_shotgun },
    { wp_chaingun,        wp_chaingun },
    { wp_missile,         wp_missile },
    { wp_plasma,          wp_plasma },
    { wp_bfg,             wp_bfg }
};

#define SLOWTURNTICS	6 
 
#define NUMKEYS		256 
#define MAX_JOY_BUTTONS 20

static boolean  gamekeydown[NUMKEYS]; 
static int      turnheld;		// for accelerative turning 
 
static boolean  mousearray[MAX_MOUSE_BUTTONS + 1];
static boolean *mousebuttons = &mousearray[1];  // allow [-1]

// mouse values are used once 
int             mousex;
int             mousey;         

static int      dclicktime;
static boolean  dclickstate;
static int      dclicks; 
static int      dclicktime2;
static boolean  dclickstate2;
static int      dclicks2;

// joystick values are repeated 
static int      joyxmove;
static int      joyymove;
static int      joystrafemove;
static boolean  joyarray[MAX_JOY_BUTTONS + 1]; 
static boolean *joybuttons = &joyarray[1];		// allow [-1] 
 
static int      savegameslot; 
static char     savedescription[32]; 
static char     savecomment[SAVESTRINGSIZE];
 
#define	BODYQUESIZE	32

mobj_t*		bodyque[BODYQUESIZE]; 
int		bodyqueslot; 
 
int             vanilla_savegame_limit = 1;
 
int G_CmdChecksum (ticcmd_t* cmd) 
{ 
    size_t		i;
    int		sum = 0; 
	 
    for (i=0 ; i< sizeof(*cmd)/4 - 1 ; i++) 
	sum += ((int *)cmd)[i]; 
		 
    return sum; 
} 

static boolean WeaponSelectable(weapontype_t weapon)
{
    // Can't select the super shotgun in Doom 1.

    if (weapon == wp_supershotgun && logical_gamemission == doom)
    {
        return false;
    }

    // These weapons aren't available in shareware.

    if ((weapon == wp_plasma || weapon == wp_bfg)
     && gamemission == doom && gamemode == shareware)
    {
        return false;
    }

    // Can't select a weapon if we don't own it.

    if (!players[consoleplayer].weaponowned[weapon])
    {
        return false;
    }

    // Can't select the fist if we have the chainsaw, unless
    // we also have the berserk pack.

    if (weapon == wp_fist
     && players[consoleplayer].weaponowned[wp_chainsaw]
     && !players[consoleplayer].powers[pw_strength])
    {
        return false;
    }

    return true;
}

static int G_NextWeapon(int direction)
{
    weapontype_t weapon;
    int start_i, i;

    // Find index in the table.

    if (players[consoleplayer].pendingweapon == wp_nochange)
    {
        weapon = players[consoleplayer].readyweapon;
    }
    else
    {
        weapon = players[consoleplayer].pendingweapon;
    }

    for (i=0; i<arrlen(weapon_order_table); ++i)
    {
        if (weapon_order_table[i].weapon == weapon)
        {
            break;
        }
    }

    // The current weapon must be present in weapon_order_table
    // otherwise something has gone terribly wrong
    if (i >= arrlen(weapon_order_table)) {
        I_Error("Internal error: weapon %d not present in weapon_order_table", weapon);
    }

    // Switch weapon. Don't loop forever.
    start_i = i;
    do
    {
        i += direction;
        i = (i + arrlen(weapon_order_table)) % arrlen(weapon_order_table);
    } while (i != start_i && !WeaponSelectable(weapon_order_table[i].weapon));

    return weapon_order_table[i].weapon_num;
}

//
// G_BuildTiccmd
// Builds a ticcmd from all of the available inputs
// or reads it from the demo buffer. 
// If recording a demo, write it out 
// 
void G_BuildTiccmd (ticcmd_t* cmd, int maketic) 
{ 
    int		i; 
    boolean	strafe;
    boolean	bstrafe; 
    int		speed;
    int		tspeed; 
    int		forward;
    int		side;

    memset(cmd, 0, sizeof(ticcmd_t));

    cmd->consistancy = 
	consistancy[consoleplayer][maketic%BACKUPTICS]; 
 
    strafe = gamekeydown[key_strafe] || mousebuttons[mousebstrafe] 
	|| joybuttons[joybstrafe]; 

    // fraggle: support the old "joyb_speed = 31" hack which
    // allowed an autorun effect

    speed = key_speed >= NUMKEYS
         || joybspeed >= MAX_JOY_BUTTONS
         || gamekeydown[key_speed] 
         || joybuttons[joybspeed]
         || mousebuttons[mousebspeed];
 
    forward = side = 0;
    
    // use two stage accelerative turning
    // on the keyboard and joystick
    if (joyxmove < 0
	|| joyxmove > 0  
	|| gamekeydown[key_right]
	|| gamekeydown[key_left]
	|| mousebuttons[mousebturnright]
	|| mousebuttons[mousebturnleft])
	turnheld += ticdup; 
    else 
	turnheld = 0; 

    if (turnheld < SLOWTURNTICS) 
	tspeed = 2;             // slow turn 
    else 
	tspeed = speed;
    
    // let movement keys cancel each other out
    if (strafe) 
    { 
	if (gamekeydown[key_right] || mousebuttons[mousebturnright])
	{
	    // fprintf(stderr, "strafe right\n");
	    side += sidemove[speed]; 
	}
	if (gamekeydown[key_left] || mousebuttons[mousebturnleft])
	{
	    //	fprintf(stderr, "strafe left\n");
	    side -= sidemove[speed]; 
	}
        if (use_analog && joyxmove)
        {
            joyxmove = joyxmove * joystick_move_sensitivity / 10;
            joyxmove = (joyxmove > FRACUNIT) ? FRACUNIT : joyxmove;
            joyxmove = (joyxmove < -FRACUNIT) ? -FRACUNIT : joyxmove;
            side += FixedMul(sidemove[speed], joyxmove);
        }
        else if (joystick_move_sensitivity)
        {
            if (joyxmove > 0)
                side += sidemove[speed];
            if (joyxmove < 0)
                side -= sidemove[speed];
        }
    } 
    else 
    { 
	if (gamekeydown[key_right] || mousebuttons[mousebturnright])
	    cmd->angleturn -= angleturn[tspeed]; 
	if (gamekeydown[key_left] || mousebuttons[mousebturnleft])
	    cmd->angleturn += angleturn[tspeed]; 
        if (use_analog && joyxmove)
        {
            // Cubic response curve allows for finer control when stick
            // deflection is small.
            joyxmove = FixedMul(FixedMul(joyxmove, joyxmove), joyxmove);
            joyxmove = joyxmove * joystick_turn_sensitivity / 10;
            cmd->angleturn -= FixedMul(angleturn[1], joyxmove);
        }
        else if (joystick_turn_sensitivity)
        {
            if (joyxmove > 0)
                cmd->angleturn -= angleturn[tspeed];
            if (joyxmove < 0)
                cmd->angleturn += angleturn[tspeed];
        }
    } 
 
    if (gamekeydown[key_up]) 
    {
	// fprintf(stderr, "up\n");
	forward += forwardmove[speed]; 
    }
    if (gamekeydown[key_down]) 
    {
	// fprintf(stderr, "down\n");
	forward -= forwardmove[speed]; 
    }

    if (use_analog && joyymove)
    {
        joyymove = joyymove * joystick_move_sensitivity / 10;
        joyymove = (joyymove > FRACUNIT) ? FRACUNIT : joyymove;
        joyymove = (joyymove < -FRACUNIT) ? -FRACUNIT : joyymove;
        forward -= FixedMul(forwardmove[speed], joyymove);
    }
    else if (joystick_move_sensitivity)
    {
        if (joyymove < 0)
            forward += forwardmove[speed];
        if (joyymove > 0)
            forward -= forwardmove[speed];
    }

    if (gamekeydown[key_strafeleft]
     || joybuttons[joybstrafeleft]
     || mousebuttons[mousebstrafeleft])
    {
        side -= sidemove[speed];
    }

    if (gamekeydown[key_straferight]
     || joybuttons[joybstraferight]
     || mousebuttons[mousebstraferight])
    {
        side += sidemove[speed]; 
    }

    if (use_analog && joystrafemove)
    {
        joystrafemove = joystrafemove * joystick_move_sensitivity / 10;
        joystrafemove = (joystrafemove > FRACUNIT) ? FRACUNIT : joystrafemove;
        joystrafemove = (joystrafemove < -FRACUNIT) ? -FRACUNIT : joystrafemove;
        side += FixedMul(sidemove[speed], joystrafemove);
    }
    else if (joystick_move_sensitivity)
    {
        if (joystrafemove < 0)
            side -= sidemove[speed];
        if (joystrafemove > 0)
            side += sidemove[speed];
    }

    // buttons
    cmd->chatchar = HU_dequeueChatChar(); 
 
    if (gamekeydown[key_fire] || mousebuttons[mousebfire] 
	|| joybuttons[joybfire]) 
	cmd->buttons |= BT_ATTACK; 

    // Goblin Dice Rollaz: Alternate fire button
    if (mousebuttons[mousebaltfire])
	cmd->buttons |= BT_ALTATTACK;
  
    if (gamekeydown[key_use]
     || joybuttons[joybuse]
     || mousebuttons[mousebuse])
    { 
	cmd->buttons |= BT_USE;
	// clear double clicks if hit use button 
	dclicks = 0;                   
    } 

    // If the previous or next weapon button is pressed, the
    // next_weapon variable is set to change weapons when
    // we generate a ticcmd.  Choose a new weapon.

    if (gamestate == GS_LEVEL && next_weapon != 0)
    {
        i = G_NextWeapon(next_weapon);
        cmd->buttons |= BT_CHANGE;
        cmd->buttons |= i << BT_WEAPONSHIFT;
    }
    else
    {
        // Check weapon keys.

        for (i=0; i<arrlen(weapon_keys); ++i)
        {
            int key = *weapon_keys[i];

            if (gamekeydown[key])
            {
                cmd->buttons |= BT_CHANGE;
                cmd->buttons |= i<<BT_WEAPONSHIFT;
                break;
            }
        }
    }

    next_weapon = 0;

    // mouse
    if (mousebuttons[mousebforward]) 
    {
	forward += forwardmove[speed];
    }
    if (mousebuttons[mousebbackward])
    {
        forward -= forwardmove[speed];
    }

    if (dclick_use)
    {
        // forward double click
        if (mousebuttons[mousebforward] != dclickstate && dclicktime > 1 ) 
        { 
            dclickstate = mousebuttons[mousebforward]; 
            if (dclickstate) 
                dclicks++; 
            if (dclicks == 2) 
            { 
                cmd->buttons |= BT_USE; 
                dclicks = 0; 
            } 
            else 
                dclicktime = 0; 
        } 
        else 
        { 
            dclicktime += ticdup; 
            if (dclicktime > 20) 
            { 
                dclicks = 0; 
                dclickstate = 0; 
            } 
        }
        
        // strafe double click
        bstrafe =
            mousebuttons[mousebstrafe] 
            || joybuttons[joybstrafe]; 
        if (bstrafe != dclickstate2 && dclicktime2 > 1 ) 
        { 
            dclickstate2 = bstrafe; 
            if (dclickstate2) 
                dclicks2++; 
            if (dclicks2 == 2) 
            { 
                cmd->buttons |= BT_USE; 
                dclicks2 = 0; 
            } 
            else 
                dclicktime2 = 0; 
        } 
        else 
        { 
            dclicktime2 += ticdup; 
            if (dclicktime2 > 20) 
            { 
                dclicks2 = 0; 
                dclickstate2 = 0; 
            } 
        } 
    }

    forward += mousey; 

    if (strafe) 
	side += mousex*2; 
    else 
	cmd->angleturn -= mousex*0x8; 

    if (mousex == 0)
    {
        // No movement in the previous frame

        testcontrols_mousespeed = 0;
    }
    
    mousex = mousey = 0; 
	 
    if (forward > MAXPLMOVE) 
	forward = MAXPLMOVE; 
    else if (forward < -MAXPLMOVE) 
	forward = -MAXPLMOVE; 
    if (side > MAXPLMOVE) 
	side = MAXPLMOVE; 
    else if (side < -MAXPLMOVE) 
	side = -MAXPLMOVE; 
 
    cmd->forwardmove += forward; 
    cmd->sidemove += side;
    
    // special buttons
    if (sendpause) 
    { 
	sendpause = false; 
	cmd->buttons = BT_SPECIAL | BTS_PAUSE; 
    } 
 
    if (sendsave) 
    { 
	sendsave = false; 
	cmd->buttons = BT_SPECIAL | BTS_SAVEGAME | (savegameslot<<BTS_SAVESHIFT); 
    } 

    // low-res turning

    if (lowres_turn)
    {
        static signed short carry = 0;
        signed short desired_angleturn;

        desired_angleturn = cmd->angleturn + carry;

        // round angleturn to the nearest 256 unit boundary
        // for recording demos with single byte values for turn

        cmd->angleturn = (desired_angleturn + 128) & 0xff00;

        // Carry forward the error from the reduced resolution to the
        // next tic, so that successive small movements can accumulate.

        carry = desired_angleturn - cmd->angleturn;
    }
} 
 

//
// G_DoLoadLevel 
//
void G_DoLoadLevel (void) 
{ 
    int             i; 

    // Set the sky map.
    // First thing, we have a dummy sky texture name,
    //  a flat. The data is in the WAD only because
    //  we look for an actual index, instead of simply
    //  setting one.

    skyflatnum = R_FlatNumForName(DEH_String(SKYFLATNAME));

    // The "Sky never changes in Doom II" bug was fixed in
    // the id Anthology version of doom2.exe for Final Doom.
    if ((gamemode == commercial)
     && (gameversion == exe_final2 || gameversion == exe_chex))
    {
        const char *skytexturename;

        if (gamemap < 12)
        {
            skytexturename = "SKY1";
        }
        else if (gamemap < 21)
        {
            skytexturename = "SKY2";
        }
        else
        {
            skytexturename = "SKY3";
        }

        skytexturename = DEH_String(skytexturename);

        skytexture = R_TextureNumForName(skytexturename);
        R_PrecacheSkyColumns();
    }

    levelstarttic = gametic;        // for time calculation
    
    if (wipegamestate == GS_LEVEL) 
	wipegamestate = -1;             // force a wipe 

    gamestate = GS_LEVEL; 

    for (i=0 ; i<MAXPLAYERS ; i++) 
    { 
	turbodetected[i] = false;
	if (playeringame[i] && players[i].playerstate == PST_DEAD) 
	    players[i].playerstate = PST_REBORN; 
	memset (players[i].frags,0,sizeof(players[i].frags)); 
    } 
		 
    P_SetupLevel (gameepisode, gamemap, 0, gameskill);    
    displayplayer = consoleplayer;		// view the guy you are playing    
    gameaction = ga_nothing; 
    Z_CheckHeap ();
    
    // clear cmd building stuff

    memset (gamekeydown, 0, sizeof(gamekeydown));
    joyxmove = joyymove = joystrafemove = 0;
    mousex = mousey = 0;
    sendpause = sendsave = paused = false;
    memset(mousearray, 0, sizeof(mousearray));
    memset(joyarray, 0, sizeof(joyarray));

    if (testcontrols)
    {
        players[consoleplayer].message = "Press escape to quit.";
    }
} 

static void SetJoyButtons(unsigned int buttons_mask)
{
    int i;

    for (i=0; i<MAX_JOY_BUTTONS; ++i)
    {
        int button_on = (buttons_mask & (1 << i)) != 0;

        // Detect button press:

        if (!joybuttons[i] && button_on)
        {
            // Weapon cycling:

            if (i == joybprevweapon)
            {
                next_weapon = -1;
            }
            else if (i == joybnextweapon)
            {
                next_weapon = 1;
            }
        }

        joybuttons[i] = button_on;
    }
}

static void SetMouseButtons(unsigned int buttons_mask)
{
    int i;

    for (i=0; i<MAX_MOUSE_BUTTONS; ++i)
    {
        unsigned int button_on = (buttons_mask & (1 << i)) != 0;

        // Detect button press:

        if (!mousebuttons[i] && button_on)
        {
            if (i == mousebprevweapon)
            {
                next_weapon = -1;
            }
            else if (i == mousebnextweapon)
            {
                next_weapon = 1;
            }
        }

	mousebuttons[i] = button_on;
    }
}

//
// G_Responder  
// Get info needed to make ticcmd_ts for the players.
// 
boolean G_Responder (event_t* ev) 
{ 
    // allow spy mode changes even during the demo
    if (gamestate == GS_LEVEL && ev->type == ev_keydown 
     && ev->data1 == key_spy && (singledemo || !deathmatch) )
    {
	// spy mode 
	do 
	{ 
	    displayplayer++; 
	    if (displayplayer == MAXPLAYERS) 
		displayplayer = 0; 
	} while (!playeringame[displayplayer] && displayplayer != consoleplayer); 
	return true; 
    }
    
    // any other key pops up menu if in demos
    if (gameaction == ga_nothing && !singledemo && 
	(demoplayback || gamestate == GS_DEMOSCREEN) 
	) 
    { 
	if (ev->type == ev_keydown ||  
	    (ev->type == ev_mouse && ev->data1) || 
	    (ev->type == ev_joystick && ev->data1) ) 
	{ 
	    M_StartControlPanel (); 
	    joywait = I_GetTime() + 5;
	    return true; 
	} 
	return false; 
    } 

    if (gamestate == GS_LEVEL) 
    { 
	if (ev->type == ev_keydown && ev->data1 == KEY_F5)
	{
	    showfps = !showfps;
	    I_DisplayFPSDots(devparm || showfps);
	    return true;
	}
	if (ev->type == ev_keydown && ev->data1 == KEY_F8)
	{
	    extern void DBG_SetVisible(boolean);
	    static boolean dbg_visible = false;
	    dbg_visible = !dbg_visible;
	    DBG_SetVisible(dbg_visible);
	    players[consoleplayer].message = dbg_visible ? "Damage Debug ON" : "Damage Debug OFF";
	    return true;
	}
	if (ev->type == ev_keydown && ev->data1 == KEY_F6)
	{
	    r_showspritestats = !r_showspritestats;
	    if (r_showspritestats)
	    {
		DEH_printf("Sprite stats enabled. Press F6 to disable.\n");
	    }
	    else
	    {
		DEH_printf("Sprite stats disabled.\n");
		DEH_printf("=== Sprite Render Stats ===\n");
		DEH_printf("Vis sprites: %d\n", r_vissprite_count);
		DEH_printf("Sprites drawn: %d\n", r_sprite_count);
		DEH_printf("Drawseg checks: %d\n", r_sprite_drawseg_checks);
		DEH_printf("Est. pixels: %d\n", r_sprite_pixels_drawn);
	    }
	    return true;
	}
	if (ev->type == ev_keydown && ev->data1 == KEY_F7)
	{
	    thinker_profiling_enabled = !thinker_profiling_enabled;
	    if (!thinker_profiling_enabled && thinker_frame_count > 0)
	    {
		int avg_time = thinker_total_time_ms / thinker_frame_count;
		int avg_count = thinker_total_count / thinker_frame_count;
		DEH_printf("=== Thinker Profile Summary ===\n");
		DEH_printf("Frames: %d\n", thinker_frame_count);
		DEH_printf("Total thinkers executed: %d\n", thinker_total_count);
		DEH_printf("Total time: %d ms\n", thinker_total_time_ms);
		DEH_printf("Avg thinkers/frame: %d\n", avg_count);
		DEH_printf("Avg time/frame: %d ms\n", avg_time);
		P_ResetThinkerStats();
	    }
	    else if (thinker_profiling_enabled)
	    {
		P_ResetThinkerStats();
		DEH_printf("Thinker profiling enabled. Press F7 to disable and see stats.\n");
	    }
	    else
	    {
		DEH_printf("Thinker profiling disabled.\n");
	    }
	    return true;
	}
	if (ev->type == ev_keydown && ev->data1 == KEY_F6)
	{
	    if (rpg_mode && players[consoleplayer].stat_points > 0)
	    {
		M_LevelUp(0);
		return true;
	    }
	    else if (rpg_mode)
	    {
		players[consoleplayer].message = "No stat points available";
		return true;
	    }
	    else
	    {
		players[consoleplayer].message = "RPG Mode required for stats";
		return true;
	    }
	}
	// Goblin Dice Rollaz: F8 - Run RNG validation test
	if (ev->type == ev_keydown && ev->data1 == KEY_F8)
	{
	    extern void P_RunRNGValidationTest(void);
	    extern int rng_validation_enabled;
	    if (rng_validation_enabled || devparm)
	    {
		P_RunRNGValidationTest();
	    }
	    else
	    {
		players[consoleplayer].message = "RNG validation: Set rng_validation_enabled=1 or use -devparm";
	    }
	    return true;
	}
	// Goblin Dice Rollaz: F9 - Toggle damage logging
	if (ev->type == ev_keydown && ev->data1 == KEY_F9)
	{
	    extern int damage_log_enabled;
	    extern void DMG_ClearDamageLog(void);
	    damage_log_enabled = !damage_log_enabled;
	    if (damage_log_enabled)
	    {
		DMG_ClearDamageLog();
		DEH_printf("Damage logging enabled. Press F10 to view log.\n");
	    }
	    else
	    {
		DEH_printf("Damage logging disabled.\n");
	    }
	    return true;
	}
	// Goblin Dice Rollaz: F10 - Print damage log
	if (ev->type == ev_keydown && ev->data1 == KEY_F10)
	{
	    extern void DMG_PrintDamageLog(void);
	    DMG_PrintDamageLog();
	    return true;
	}
	// Goblin Dice Rollaz: F12 - Toggle render profiling
	if (ev->type == ev_keydown && ev->data1 == KEY_F12)
	{
	    extern int r_render_profile_enabled;
	    r_render_profile_enabled = !r_render_profile_enabled;
	    if (r_render_profile_enabled)
	    {
		R_ResetRenderProfile();
		DEH_printf("Render profiling enabled. Press F12 to disable and view stats.\n");
	    }
	    else
	    {
		R_PrintRenderProfile();
	    }
	    return true;
	}
	// Goblin Dice Rollaz: F8 - Print dice statistics
	if (ev->type == ev_keydown && ev->data1 == KEY_F8)
	{
	    extern void G_PrintDiceStats(void);
	    G_PrintDiceStats();
	    return true;
	}
	// Goblin Dice Rollaz: F9 - Print achievements
	if (ev->type == ev_keydown && ev->data1 == KEY_F9)
	{
	    extern void G_PrintAchievements(void);
	    G_PrintAchievements();
	    return true;
	}
	// Goblin Dice Rollaz: F12 - Print WAD info
	if (ev->type == ev_keydown && ev->data1 == KEY_F12)
	{
	    extern void W_PrintLoadedWADs(void);
	    extern void W_ValidateLoadedFiles(void);
	    W_PrintLoadedWADs();
	    W_ValidateLoadedFiles();
	    return true;
	}
	if (HU_Responder (ev))
	    return true;	// chat ate the event 
	if (ST_Responder (ev)) 
	    return true;	// status window ate it 
	if (AM_Responder (ev)) 
	    return true;	// automap ate it 
    } 
	 
    if (gamestate == GS_FINALE) 
    { 
	if (F_Responder (ev)) 
	    return true;	// finale ate the event 
    } 

    if (testcontrols && ev->type == ev_mouse)
    {
        // If we are invoked by setup to test the controls, save the 
        // mouse speed so that we can display it on-screen.
        // Perform a low pass filter on this so that the thermometer 
        // appears to move smoothly.

        testcontrols_mousespeed = abs(ev->data2);
    }

    // If the next/previous weapon keys are pressed, set the next_weapon
    // variable to change weapons when the next ticcmd is generated.

    if (ev->type == ev_keydown && ev->data1 == key_prevweapon)
    {
        next_weapon = -1;
    }
    else if (ev->type == ev_keydown && ev->data1 == key_nextweapon)
    {
        next_weapon = 1;
    }
    else if (ev->type == ev_keydown && ev->data1 == key_reload)
    {
        G_ReloadWeapon();
    }

    switch (ev->type) 
    { 
      case ev_keydown: 
	if (ev->data1 == key_pause) 
	{ 
	    sendpause = true; 
	}
        else if (ev->data1 <NUMKEYS) 
        {
	    gamekeydown[ev->data1] = true; 
        }

	return true;    // eat key down events 
 
      case ev_keyup: 
	if (ev->data1 <NUMKEYS) 
	    gamekeydown[ev->data1] = false; 
	return false;   // always let key up events filter down 
		 
      case ev_mouse: 
        SetMouseButtons(ev->data1);
	mousex = (int)(ev->data2*(mouseSensitivity+5)/10*mouse_sensitivity_scale); 
	mousey = (int)(ev->data3*(mouseSensitivity+5)/10*mouse_sensitivity_scale);
	return true;    // eat events 
 
      case ev_joystick: 
        SetJoyButtons(ev->data1);
	joyxmove = ev->data2; 
	joyymove = ev->data3; 
        joystrafemove = ev->data4;
	return true;    // eat events 
 
      default: 
	break; 
    } 
 
    return false; 
} 
 
 
 
//
// G_Ticker
// Make ticcmd_ts for the players.
//
void G_Ticker (void) 
{ 
    int		i;
    int		buf; 
    ticcmd_t*	cmd;

    // Goblin Dice Rollaz: Check for save import trigger
    if (import_saves_trigger == 1)
    {
        G_DoImportSaveGames();
        import_saves_trigger = 0;
    }
    
    // do player reborns if needed
    for (i=0 ; i<MAXPLAYERS ; i++) 
	if (playeringame[i] && players[i].playerstate == PST_REBORN) 
	    G_DoReborn (i);
    
    // do things to change the game state
    while (gameaction != ga_nothing) 
    { 
	switch (gameaction) 
	{ 
	  case ga_loadlevel: 
	    G_DoLoadLevel (); 
	    break; 
	  case ga_newgame: 
	    G_DoNewGame (); 
	    break; 
	  case ga_loadgame: 
	    G_DoLoadGame (); 
	    break; 
	  case ga_savegame: 
	    G_DoSaveGame (); 
	    break; 
	  case ga_playdemo: 
	    G_DoPlayDemo (); 
	    break; 
	  case ga_completed: 
	    G_DoCompleted (); 
	    break; 
	  case ga_victory: 
	    F_StartFinale (); 
	    break; 
	  case ga_worlddone: 
	    G_DoWorldDone (); 
	    break; 
	  case ga_screenshot: 
	    V_ScreenShot("GOBLIN%02i.%s"); 
            players[consoleplayer].message = DEH_String("screenshot - dice roll overlay captured");
	    gameaction = ga_nothing; 
	    break;
	  case ga_nothing: 
	    break; 
	} 
    }
    
    // Goblin Dice Rollaz: Update achievement progress
    G_UpdateAchievements();
    
    // get commands, check consistancy,
    // and build new consistancy check
    buf = (gametic/ticdup)%BACKUPTICS; 
 
    for (i=0 ; i<MAXPLAYERS ; i++)
    {
	if (playeringame[i]) 
	{ 
	    cmd = &players[i].cmd; 

	    memcpy(cmd, &netcmds[i], sizeof(ticcmd_t));

	    if (demoplayback) 
		G_ReadDemoTiccmd (cmd); 
	    if (demorecording) 
		G_WriteDemoTiccmd (cmd);
	    
	    // check for turbo cheats

            // check ~ 4 seconds whether to display the turbo message. 
            // store if the turbo threshold was exceeded in any tics
            // over the past 4 seconds.  offset the checking period
            // for each player so messages are not displayed at the
            // same time.

            if (cmd->forwardmove > TURBOTHRESHOLD)
            {
                turbodetected[i] = true;
            }

            if ((gametic & 31) == 0 
             && ((gametic >> 5) % MAXPLAYERS) == i
             && turbodetected[i])
            {
                static char turbomessage[80];
                M_snprintf(turbomessage, sizeof(turbomessage),
                           "%s is turbo!", player_names[i]);
                players[consoleplayer].message = turbomessage;
                turbodetected[i] = false;
            }

	    if (netgame && !netdemo && !(gametic%ticdup) ) 
	    { 
		if (gametic > BACKUPTICS 
		    && consistancy[i][buf] != cmd->consistancy) 
		{ 
		    I_Error ("consistency failure (%i should be %i)",
			     cmd->consistancy, consistancy[i][buf]); 
		} 
		if (players[i].mo) 
		    consistancy[i][buf] = players[i].mo->x; 
		else 
		    consistancy[i][buf] = rndindex; 
	    } 
	}
    }
    
    // check for special buttons
    for (i=0 ; i<MAXPLAYERS ; i++)
    {
	if (playeringame[i]) 
	{ 
	    if (players[i].cmd.buttons & BT_SPECIAL) 
	    { 
		switch (players[i].cmd.buttons & BT_SPECIALMASK) 
		{ 
		  case BTS_PAUSE: 
		    paused ^= 1; 
		    if (paused) 
			S_PauseSound (); 
		    else 
			S_ResumeSound (); 
		    break; 
					 
		  case BTS_SAVEGAME: 
		    if (!savedescription[0]) 
                    {
                        M_StringCopy(savedescription, "NET GAME",
                                     sizeof(savedescription));
                    }

		    savegameslot =  
			(players[i].cmd.buttons & BTS_SAVEMASK)>>BTS_SAVESHIFT; 
		    gameaction = ga_savegame; 
		    break; 
		} 
	    } 
	}
    }

    // Have we just finished displaying an intermission screen?

    if (oldgamestate == GS_INTERMISSION && gamestate != GS_INTERMISSION)
    {
        WI_End();
    }

    oldgamestate = gamestate;
    
    // do main actions
    switch (gamestate) 
    { 
      case GS_LEVEL: 
    P_Ticker (); 
    ST_Ticker (); 
    AM_Ticker (); 
    HU_Ticker ();             
    G_SurvivalTicker();
    G_TimeAttackTicker();
	break; 
	 
      case GS_INTERMISSION: 
	WI_Ticker (); 
	break; 
			 
      case GS_FINALE: 
	F_Ticker (); 
	break; 
 
      case GS_DEMOSCREEN: 
	D_PageTicker (); 
	break;
    }        
} 
 
 
//
// PLAYER STRUCTURE FUNCTIONS
// also see P_SpawnPlayer in P_Things
//

//
// G_InitPlayer 
// Called at the start.
// Called by the game initialization functions.
//
void G_InitPlayer (int player) 
{
    // clear everything else to defaults
    G_PlayerReborn (player); 
}
 
 

//
// G_PlayerFinishLevel
// Can when a player completes a level.
//
void G_PlayerFinishLevel (int player) 
{ 
    player_t*	p; 
	 
    p = &players[player]; 
	 
    memset (p->powers, 0, sizeof (p->powers)); 
    memset (p->cards, 0, sizeof (p->cards)); 
    p->mo->flags &= ~MF_SHADOW;		// cancel invisibility 
    p->extralight = 0;			// cancel gun flashes 
    p->fixedcolormap = 0;		// cancel ir gogles 
    p->damagecount = 0;			// no palette changes 
    p->bonuscount = 0; 
} 
 

//
// G_PlayerReborn
// Called after a player dies 
// almost everything is cleared and initialized 
//
void G_PlayerReborn (int player) 
{ 
    player_t*	p; 
    int		i; 
    int		frags[MAXPLAYERS]; 
    int		killcount;
    int		itemcount;
    int		secretcount; 
	 
    memcpy (frags,players[player].frags,sizeof(frags)); 
    killcount = players[player].killcount; 
    itemcount = players[player].itemcount; 
    secretcount = players[player].secretcount; 
	 
    p = &players[player]; 
    memset (p, 0, sizeof(*p)); 
 
    memcpy (players[player].frags, frags, sizeof(players[player].frags)); 
    players[player].killcount = killcount; 
    players[player].itemcount = itemcount; 
    players[player].secretcount = secretcount; 
 
    p->usedown = p->attackdown = p->altattackdown = true;	// don't do anything immediately 
    p->playerstate = PST_LIVE;       
    p->health = deh_initial_health;     // Use dehacked value
    p->readyweapon = p->pendingweapon = wp_pistol; 
    p->weaponowned[wp_fist] = true; 
    p->weaponowned[wp_pistol] = true; 
    p->ammo[am_clip] = deh_initial_bullets; 
	 
    for (i=0 ; i<NUMAMMO ; i++) 
	p->maxammo[i] = maxammo[i]; 
		 
}

//
// G_CheckSpot  
// Returns false if the player cannot be respawned
// at the given mapthing_t spot  
// because something is occupying it 
//
void P_SpawnPlayer (mapthing_t* mthing); 
 
boolean
G_CheckSpot
( int		playernum,
  mapthing_t*	mthing ) 
{ 
    fixed_t		x;
    fixed_t		y; 
    subsector_t*	ss; 
    mobj_t*		mo; 
    int			i;
	
    if (!players[playernum].mo)
    {
	// first spawn of level, before corpses
	for (i=0 ; i<playernum ; i++)
	    if (players[i].mo->x == mthing->x << FRACBITS
		&& players[i].mo->y == mthing->y << FRACBITS)
		return false;	
	return true;
    }
		
    x = mthing->x << FRACBITS; 
    y = mthing->y << FRACBITS; 
	 
    if (!P_CheckPosition (players[playernum].mo, x, y) ) 
	return false; 
 
    // flush an old corpse if needed 
    if (bodyqueslot >= BODYQUESIZE) 
	P_RemoveMobj (bodyque[bodyqueslot%BODYQUESIZE]); 
    bodyque[bodyqueslot%BODYQUESIZE] = players[playernum].mo; 
    bodyqueslot++; 

    // spawn a teleport fog
    ss = R_PointInSubsector (x,y);


    // The code in the released source looks like this:
    //
    //    an = ( ANG45 * (((unsigned int) mthing->angle)/45) )
    //         >> ANGLETOFINESHIFT;
    //    mo = P_SpawnMobj (x+20*finecosine[an], y+20*finesine[an]
    //                     , ss->sector->floorheight
    //                     , MT_TFOG);
    //
    // But 'an' can be a signed value in the DOS version. This means that
    // we get a negative index and the lookups into finecosine/finesine
    // end up dereferencing values in finetangent[].
    // A player spawning on a deathmatch start facing directly west spawns
    // "silently" with no spawn fog. Emulate this.
    //
    // This code is imported from PrBoom+.

    {
        fixed_t xa, ya;
        signed int an;

        // This calculation overflows in Vanilla Doom, but here we deliberately
        // avoid integer overflow as it is undefined behavior, so the value of
        // 'an' will always be positive.
        an = (ANG45 >> ANGLETOFINESHIFT) * ((signed int) mthing->angle / 45);

        switch (an)
        {
            case 4096:  // -4096:
                xa = finetangent[2048];    // finecosine[-4096]
                ya = finetangent[0];       // finesine[-4096]
                break;
            case 5120:  // -3072:
                xa = finetangent[3072];    // finecosine[-3072]
                ya = finetangent[1024];    // finesine[-3072]
                break;
            case 6144:  // -2048:
                xa = finesine[0];          // finecosine[-2048]
                ya = finetangent[2048];    // finesine[-2048]
                break;
            case 7168:  // -1024:
                xa = finesine[1024];       // finecosine[-1024]
                ya = finetangent[3072];    // finesine[-1024]
                break;
            case 0:
            case 1024:
            case 2048:
            case 3072:
                xa = finecosine[an];
                ya = finesine[an];
                break;
            default:
                I_Error("G_CheckSpot: unexpected angle %d\n", an);
                xa = ya = 0;
                break;
        }
        mo = P_SpawnMobj(x + 20 * xa, y + 20 * ya,
                         ss->sector->floorheight, MT_TFOG);
    }

    if (players[consoleplayer].viewz != 1) 
	S_StartSound (mo, sfx_telept);	// don't start sound on first frame 
 
    return true; 
} 


//
// G_DeathMatchSpawnPlayer 
// Spawns a player at one of the random death match spots 
// called at level load and each death 
//
void G_DeathMatchSpawnPlayer (int playernum) 
{ 
    int             i,j; 
    int				selections; 
	 
    selections = deathmatch_p - deathmatchstarts; 
    if (selections < 4) 
	I_Error ("Only %i deathmatch spots, 4 required", selections); 
 
    for (j=0 ; j<20 ; j++) 
    { 
	i = P_Random() % selections; 
	if (G_CheckSpot (playernum, &deathmatchstarts[i]) ) 
	{ 
	    deathmatchstarts[i].type = playernum+1; 
	    P_SpawnPlayer (&deathmatchstarts[i]); 
	    return; 
	} 
    } 
 
    // no good spot, so the player will probably get stuck 
    P_SpawnPlayer (&playerstarts[playernum]); 
} 

//
// G_DoReborn 
// 
void G_DoReborn (int playernum) 
{ 
    int                             i; 
	 
    if (!netgame)
    {
	// reload the level from scratch
	gameaction = ga_loadlevel;  
    }
    else 
    {
	// respawn at the start

	// first dissasociate the corpse 
	players[playernum].mo->player = NULL;   
		 
	// spawn at random spot if in death match 
	if (deathmatch) 
	{ 
	    G_DeathMatchSpawnPlayer (playernum); 
	    return; 
	} 
		 
	if (G_CheckSpot (playernum, &playerstarts[playernum]) ) 
	{ 
	    P_SpawnPlayer (&playerstarts[playernum]); 
	    return; 
	}
	
	// try to spawn at one of the other players spots 
	for (i=0 ; i<MAXPLAYERS ; i++)
	{
	    if (G_CheckSpot (playernum, &playerstarts[i]) ) 
	    { 
		playerstarts[i].type = playernum+1;	// fake as other player 
		P_SpawnPlayer (&playerstarts[i]); 
		playerstarts[i].type = i+1;		// restore 
		return; 
	    }	    
	    // he's going to be inside something.  Too bad.
	}
	P_SpawnPlayer (&playerstarts[playernum]); 
    } 
} 
 
 
void G_ScreenShot (void) 
{ 
    gameaction = ga_screenshot; 
} 
 


// DOOM Par Times
static const int pars[4][10] =
{ 
    {0}, 
    {0,30,75,120,90,165,180,180,30,165}, 
    {0,90,90,90,120,90,360,240,30,170}, 
    {0,90,45,90,150,90,90,165,30,135} 
}; 

// DOOM II Par Times
static const int cpars[32] =
{
    30,90,120,120,90,150,120,120,270,90,	//  1-10
    210,150,150,150,210,150,420,150,210,150,	// 11-20
    240,150,180,150,150,300,330,420,300,180,	// 21-30
    120,30					// 31-32
};

// Chex Quest Par Times
static const int chexpars[6] =
{ 
    0,120,360,480,200,360
}; 
 

//
// G_DoCompleted 
//
boolean		secretexit; 
 
void G_ExitLevel (void) 
{ 
    char autoSaveDescription[32];
    char autoSaveComment[SAVESTRINGSIZE];
    
    secretexit = false; 

    // Goblin Dice Rollaz: Track cumulative session stats
    G_TrackLevelComplete();
    G_AccumulateSessionStats();
    G_ResetDiceStats();

    // Goblin Dice Rollaz: Track time attack level completion
    if (G_IsTimeAttackMode())
    {
        G_RecordLevelCompleteTime(leveltime);
        G_TimeAttackCompleteLevel();
    }

    // Goblin Dice Rollaz: Track challenge mode completion
    if (IN_CHALLENGE_MODE() && gamemap >= MAX_TIMEATTACK_MAPS)
    {
        int score = players[consoleplayer].killcount * 10 + players[consoleplayer].itemcount;
        int wave = gamemap;

        if (challenge_critonly)
        {
            G_AddChallengeEntry(LEADERBOARD_CHALLENGE_CRITONLY, "Player", score, wave);
        }
        if (challenge_nopowerups)
        {
            G_AddChallengeEntry(LEADERBOARD_CHALLENGE_NOPOWERUPS, "Player", score, wave);
        }
        if (challenge_hardcore)
        {
            G_AddChallengeEntry(LEADERBOARD_CHALLENGE_HARDCORE, "Player", score, wave);
        }
    }

    // Goblin Dice Rollaz: Auto-save on level transition
    if (auto_save_enabled && !netgame && !demoplayback && usergame && !paused && players[consoleplayer].playerstate == PST_LIVE)
    {
        M_snprintf(autoSaveDescription, sizeof(autoSaveDescription), "Auto-save E%dm%d", gameepisode, gamemap);
        M_snprintf(autoSaveComment, sizeof(autoSaveComment), "Level %d - Auto saved", gamemap);
        G_SaveGame(0, autoSaveDescription, autoSaveComment);
    }

    gameaction = ga_completed; 
}

// Here's for the german edition.
void G_SecretExitLevel (void) 
{ 
    char autoSaveDescription[32];
    char autoSaveComment[SAVESTRINGSIZE];
    
    // IF NO WOLF3D LEVELS, NO SECRET EXIT!
    if ( (gamemode == commercial)
      && (W_CheckNumForName("map31")<0))
	secretexit = false;
    else
	secretexit = true; 

    // Goblin Dice Rollaz: Auto-save on secret level transition
    if (auto_save_enabled && !netgame && !demoplayback && usergame && !paused && players[consoleplayer].playerstate == PST_LIVE)
    {
        M_snprintf(autoSaveDescription, sizeof(autoSaveDescription), "Auto-save E%dm%d", gameepisode, gamemap);
        M_snprintf(autoSaveComment, sizeof(autoSaveComment), "Secret Exit - Auto saved");
        G_SaveGame(0, autoSaveDescription, autoSaveComment);
    }

    gameaction = ga_completed; 
}
 
void G_DoCompleted (void) 
{ 
    int             i; 
	 
    gameaction = ga_nothing; 
 
    for (i=0 ; i<MAXPLAYERS ; i++) 
	if (playeringame[i]) 
	    G_PlayerFinishLevel (i);        // take away cards and stuff 
	 
    if (automapactive) 
	AM_Stop (); 
	
    if (gamemode != commercial)
    {
        // Chex Quest ends after 5 levels, rather than 8.

        if (gameversion == exe_chex)
        {
            if (gamemap == 5)
            {
                gameaction = ga_victory;
                return;
            }
        }
        else
        {
            switch(gamemap)
            {
              case 8:
                gameaction = ga_victory;
                return;
              case 9: 
                for (i=0 ; i<MAXPLAYERS ; i++) 
                    players[i].didsecret = true; 
                break;
            }
        }
    }

//#if 0  Hmmm - why?
    if ( (gamemap == 8)
	 && (gamemode != commercial) ) 
    {
	// victory 
	gameaction = ga_victory; 
	return; 
    } 
	 
    if ( (gamemap == 9)
	 && (gamemode != commercial) ) 
    {
	// exit secret level 
	for (i=0 ; i<MAXPLAYERS ; i++) 
	    players[i].didsecret = true; 
    } 
//#endif
    
	 
    wminfo.didsecret = players[consoleplayer].didsecret; 
    wminfo.epsd = gameepisode -1; 
    wminfo.last = gamemap -1;
    
    // wminfo.next is 0 biased, unlike gamemap
    if ( gamemode == commercial)
    {
	if (secretexit)
	    switch(gamemap)
	    {
	      case 15: wminfo.next = 30; break;
	      case 31: wminfo.next = 31; break;
	    }
	else
	    switch(gamemap)
	    {
	      case 31:
	      case 32: wminfo.next = 15; break;
	      default: wminfo.next = gamemap;
	    }
    }
    else
    {
	if (secretexit) 
	    wminfo.next = 8; 	// go to secret level 
	else if (gamemap == 9) 
	{
	    // returning from secret level 
	    switch (gameepisode) 
	    { 
	      case 1: 
		wminfo.next = 3; 
		break; 
	      case 2: 
		wminfo.next = 5; 
		break; 
	      case 3: 
		wminfo.next = 6; 
		break; 
	      case 4:
		wminfo.next = 2;
		break;
	    }                
	} 
	else 
	    wminfo.next = gamemap;          // go to next level 
    }
		 
    wminfo.maxkills = totalkills; 
    wminfo.maxitems = totalitems; 
    wminfo.maxsecret = totalsecret; 
    wminfo.maxfrags = 0; 

    // Set par time. Exceptions are added for purposes of
    // statcheck regression testing.
    if (gamemode == commercial)
    {
        // map33 reads its par time from beyond the cpars[] array
        if (gamemap == 33)
        {
            int cpars32;

            memcpy(&cpars32, DEH_String(GAMMALVL0), sizeof(int));
            cpars32 = LONG(cpars32);

            wminfo.partime = TICRATE*cpars32;
        }
        else
        {
            wminfo.partime = TICRATE*cpars[gamemap-1];
        }
    }
    // Doom episode 4 doesn't have a par time, so this
    // overflows into the cpars array.
    else if (gameepisode < 4)
    {
        if (gameversion == exe_chex && gameepisode == 1 && gamemap < 6)
        {
            wminfo.partime = TICRATE*chexpars[gamemap];
        }
        else
        {
            wminfo.partime = TICRATE*pars[gameepisode][gamemap];
        }
    }
    else
    {
        wminfo.partime = TICRATE*cpars[gamemap];
    }

    wminfo.pnum = consoleplayer; 
 
    for (i=0 ; i<MAXPLAYERS ; i++) 
    { 
	wminfo.plyr[i].in = playeringame[i]; 
	wminfo.plyr[i].skills = players[i].killcount; 
	wminfo.plyr[i].sitems = players[i].itemcount; 
	wminfo.plyr[i].ssecret = players[i].secretcount; 
	wminfo.plyr[i].stime = leveltime; 
	memcpy (wminfo.plyr[i].frags, players[i].frags 
		, sizeof(wminfo.plyr[i].frags)); 
    } 
 
    gamestate = GS_INTERMISSION; 
    viewactive = false; 
    automapactive = false; 

    StatCopy(&wminfo);
 
    WI_Start (&wminfo); 
} 


//
// G_WorldDone 
//
void G_WorldDone (void) 
{ 
    gameaction = ga_worlddone; 

    if (secretexit) 
	players[consoleplayer].didsecret = true; 

    if ( gamemode == commercial )
    {
	switch (gamemap)
	{
	  case 15:
	  case 31:
	    if (!secretexit)
		break;
	  case 6:
	  case 11:
	  case 20:
	  case 30:
	    F_StartFinale ();
	    break;
	}
    }
} 
 
void G_DoWorldDone (void) 
{        
    gamestate = GS_LEVEL; 
    gamemap = wminfo.next+1; 
    G_DoLoadLevel (); 
    gameaction = ga_nothing; 
    viewactive = true; 
} 
 


//
// G_InitFromSavegame
// Can be called by the startup code or the menu task. 
//

char	savename[256];

void G_LoadGame (char* name) 
{ 
    M_StringCopy(savename, name, sizeof(savename));
    gameaction = ga_loadgame; 
} 

void G_DoLoadGame (void) 
{ 
    int savedleveltime;
	 
    gameaction = ga_nothing; 
	 
    save_stream = M_fopen(savename, "rb");

    if (save_stream == NULL)
    {
        I_Error("Could not load savegame %s", savename);
    }

    savegame_error = false;

    if (!P_ReadSaveGameHeader())
    {
        fclose(save_stream);
        return;
    }

    savedleveltime = leveltime;
    
    // load a base level 
    G_InitNew (gameskill, gameepisode, gamemap); 
 
    leveltime = savedleveltime;

    // dearchive all the modifications
    P_UnArchivePlayers (); 
    P_UnArchiveWorld (); 
    P_UnArchiveThinkers (); 
    P_UnArchiveSpecials (); 
 
    if (!P_ReadSaveGameEOF())
	I_Error ("Bad savegame");

    fclose(save_stream);
    
    if (setsizeneeded)
	R_ExecuteSetViewSize ();
    
    // draw the pattern into the back screen
    R_FillBackScreen ();   
} 
 

//
// G_SaveGame
// Called by the menu task.
// Description is a 24 byte text string 
//
void
G_SaveGame
( int	slot,
  char*	description,
  char*	comment )
{
    savegameslot = slot;
    M_StringCopy(savedescription, description, sizeof(savedescription));
    M_StringCopy(savecomment, comment, sizeof(savecomment));
    sendsave = true;
}

void G_DoSaveGame (void) 
{ 
    char *savegame_file;
    char *temp_savegame_file;
    char *recovery_savegame_file;

    recovery_savegame_file = NULL;
    temp_savegame_file = P_TempSaveGameFile();
    savegame_file = P_SaveGameFile(savegameslot);

    // Open the savegame file for writing.  We write to a temporary file
    // and then rename it at the end if it was successfully written.
    // This prevents an existing savegame from being overwritten by
    // a corrupted one, or if a savegame buffer overrun occurs.
    save_stream = M_fopen(temp_savegame_file, "wb");

    if (save_stream == NULL)
    {
        // Failed to save the game, so we're going to have to abort. But
        // to be nice, save to somewhere else before we call I_Error().
        recovery_savegame_file = M_TempFile("recovery.dsg");
        save_stream = M_fopen(recovery_savegame_file, "wb");
        if (save_stream == NULL)
        {
            I_Error("Failed to open either '%s' or '%s' to write savegame.",
                    temp_savegame_file, recovery_savegame_file);
        }
    }

    savegame_error = false;

    P_WriteSaveGameHeader(savedescription, savecomment);

    P_ArchivePlayers ();
    P_ArchiveWorld ();
    P_ArchiveThinkers ();
    P_ArchiveSpecials ();

    P_WriteSaveGameEOF();

    // Enforce the same savegame size limit as in Vanilla Doom,
    // except if the vanilla_savegame_limit setting is turned off.

    if (vanilla_savegame_limit && ftell(save_stream) > SAVEGAMESIZE)
    {
        I_Error("Savegame buffer overrun");
    }

    // Finish up, close the savegame file.

    fclose(save_stream);

    if (recovery_savegame_file != NULL)
    {
        // We failed to save to the normal location, but we wrote a
        // recovery file to the temp directory. Now we can bomb out
        // with an error.
        I_Error("Failed to open savegame file '%s' for writing.\n"
                "But your game has been saved to '%s' for recovery.",
                temp_savegame_file, recovery_savegame_file);
    }

    // Now rename the temporary savegame file to the actual savegame
    // file, overwriting the old savegame if there was one there.

    M_remove(savegame_file);
    M_rename(temp_savegame_file, savegame_file);

    gameaction = ga_nothing;
    M_StringCopy(savedescription, "", sizeof(savedescription));
    M_StringCopy(savecomment, "", sizeof(savecomment));

    players[consoleplayer].message = DEH_String(GGSAVED);

    // draw the pattern into the back screen
    R_FillBackScreen ();
}
 

//
// G_InitNew
// Can be called by the startup code or the menu task,
// consoleplayer, displayplayer, playeringame[] should be set. 
//
skill_t	d_skill; 
int     d_episode; 
int     d_map; 
 
void
G_DeferedInitNew
( skill_t	skill,
  int		episode,
  int		map) 
{ 
    d_skill = skill; 
    d_episode = episode; 
    d_map = map; 
    gameaction = ga_newgame; 
} 


void G_DoNewGame (void) 
{
    demoplayback = false; 
    netdemo = false;
    netgame = false;
    deathmatch = false;
    playeringame[1] = playeringame[2] = playeringame[3] = 0;
    respawnparm = false;
    fastparm = false;
    nomonsters = false;
    consoleplayer = 0;
    G_InitNew (d_skill, d_episode, d_map); 
    gameaction = ga_nothing; 
} 


void
G_InitNew
( skill_t	skill,
  int		episode,
  int		map )
{
    const char *skytexturename;
    int             i;

    // Goblin Dice Rollaz: Initialize RPG stats for new game
    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            G_InitPlayerRPG(&players[i]);
        }
    }

    // Goblin Dice Rollaz: Initialize session stats
    G_StartSession();
    G_ResetDiceStats();

    if (paused)
    {
	paused = false;
	S_ResumeSound ();
    }

    /*
    // Note: This commented-out block of code was added at some point
    // between the DOS version(s) and the Doom source release. It isn't
    // found in disassemblies of the DOS version and causes IDCLEV and
    // the -warp command line parameter to behave differently.
    // This is left here for posterity.

    // This was quite messy with SPECIAL and commented parts.
    // Supposedly hacks to make the latest edition work.
    // It might not work properly.
    if (episode < 1)
      episode = 1;

    if ( gamemode == retail )
    {
      if (episode > 4)
	episode = 4;
    }
    else if ( gamemode == shareware )
    {
      if (episode > 1)
	   episode = 1;	// only start episode 1 on shareware
    }
    else
    {
      if (episode > 3)
	episode = 3;
    }
    */

    if (skill > sk_challenge_hardcore)
	skill = sk_challenge_hardcore;

    if (episode == 0)
    {
        episode = 4;
    }
    else
    {
        if (episode < 1)
        {
            episode = 1;
        }
        if (episode > 3)
        {
            episode = 3;
        }
    }

    if (episode > 1 && gamemode == shareware)
    {
        episode = 1;
    }

    if (map < 1)
	map = 1;

    if ( (map > 9)
	 && ( gamemode != commercial) )
      map = 9;

    M_ClearRandom ();

    if (skill == sk_nightmare || skill >= sk_challenge_critonly || respawnparm )
	respawnmonsters = true;
    else
	respawnmonsters = false;

    if (fastparm || ((skill == sk_nightmare || skill >= sk_challenge_critonly) && gameskill != sk_nightmare && gameskill < sk_challenge_critonly) )
    {
	for (i=S_SARG_RUN1 ; i<=S_SARG_PAIN2 ; i++)
	    states[i].tics >>= 1;
	mobjinfo[MT_BRUISERSHOT].speed = 20*FRACUNIT;
	mobjinfo[MT_HEADSHOT].speed = 20*FRACUNIT;
	mobjinfo[MT_TROOPSHOT].speed = 20*FRACUNIT;
    }
    else if (skill != sk_nightmare && skill < sk_challenge_critonly && (gameskill == sk_nightmare || gameskill >= sk_challenge_critonly))
    {
	for (i=S_SARG_RUN1 ; i<=S_SARG_PAIN2 ; i++)
	    states[i].tics <<= 1;
	mobjinfo[MT_BRUISERSHOT].speed = 15*FRACUNIT;
	mobjinfo[MT_HEADSHOT].speed = 10*FRACUNIT;
	mobjinfo[MT_TROOPSHOT].speed = 10*FRACUNIT;
    }

    // force players to be initialized upon first level load
    for (i=0 ; i<MAXPLAYERS ; i++)
	players[i].playerstate = PST_REBORN;

    usergame = true;                // will be set false if a demo
    paused = false;
    demoplayback = false;
    automapactive = false;
    viewactive = true;
    gameepisode = episode;
    gamemap = map;
    gameskill = skill;

    // Goblin Dice Rollaz: Initialize challenge mode flags
    challenge_critonly = (skill == sk_challenge_critonly || skill == sk_challenge_hardcore);
    challenge_nopowerups = (skill == sk_challenge_nopowerups || skill == sk_challenge_hardcore);
    challenge_hardcore = (skill == sk_challenge_hardcore);

    // Set the sky to use.
    //
    // Note: This IS broken, but it is how Vanilla Doom behaves.
    // See http://doomwiki.org/wiki/Sky_never_changes_in_Doom_II.
    //
    // Because we set the sky here at the start of a game, not at the
    // start of a level, the sky texture never changes unless we
    // restore from a saved game.  This was fixed before the Doom
    // source release, but this IS the way Vanilla DOS Doom behaves.

    if (gamemode == commercial)
    {
        skytexturename = DEH_String("SKY3");
        skytexture = R_TextureNumForName(skytexturename);
        if (gamemap < 21)
        {
            skytexturename = DEH_String(gamemap < 12 ? "SKY1" : "SKY2");
            skytexture = R_TextureNumForName(skytexturename);
        }
    }
    else
    {
        switch (gameepisode)
        {
          default:
          case 1:
            skytexturename = "SKY1";
            break;
          case 2:
            skytexturename = "SKY2";
            break;
          case 3:
            skytexturename = "SKY3";
            break;
          case 4:        // Special Edition sky
            skytexturename = "SKY4";
            break;
        }
        skytexturename = DEH_String(skytexturename);
        skytexture = R_TextureNumForName(skytexturename);
    }

    R_PrecacheSkyColumns();

    G_DoLoadLevel ();
}


//
// DEMO RECORDING 
// 
#define DEMOMARKER		0x80


void G_ReadDemoTiccmd (ticcmd_t* cmd) 
{ 
    if (*demo_p == DEMOMARKER) 
    {
	// end of demo data stream 
	G_CheckDemoStatus (); 
	return; 
    } 
    cmd->forwardmove = ((signed char)*demo_p++); 
    cmd->sidemove = ((signed char)*demo_p++); 

    cmd->angleturn = ((unsigned char) *demo_p++)<<8;

    cmd->buttons = (unsigned char)*demo_p++; 
} 

// Increase the size of the demo buffer to allow unlimited demos

void G_WriteDemoTiccmd (ticcmd_t* cmd)
{ 
    byte *demo_start;

    if (gamekeydown[key_demo_quit])           // press q to end demo recording 
	G_CheckDemoStatus (); 

    demo_start = demo_p;

    *demo_p++ = cmd->forwardmove; 
    *demo_p++ = cmd->sidemove; 
    *demo_p++ = cmd->angleturn >> 8;

    *demo_p++ = cmd->buttons; 

    // reset demo pointer back
    demo_p = demo_start;
    	
    G_ReadDemoTiccmd (cmd);         // make SURE it is exactly the same
} 
 
 
 
//
// G_RecordDemo
//
void G_RecordDemo (const char *name)
{
    size_t demoname_size;
    int i;
    int maxsize;

    usergame = false;
    demoname_size = strlen(name) + 5;
    demoname = Z_Malloc(demoname_size, PU_STATIC, NULL);
    M_snprintf(demoname, demoname_size, "%s.lmp", name);
    maxsize = 0x20000;

    //!
    // @arg <size>
    // @category demo
    // @vanilla
    //
    // Specify the demo buffer size (KiB)
    //

    i = M_CheckParmWithArgs("-maxdemo", 1);
    if (i)
	maxsize = atoi(myargv[i+1])*1024;
    demo_buffer_start = Z_Malloc(maxsize, PU_STATIC, NULL);
    demo_p = demo_buffer_start;
    demoend = demo_p + maxsize;
  	
    demorecording = true;
} 

// Get the demo version code appropriate for the version set in gameversion.
#define G_VanillaVersionCode() GOBLIN_VERSION

void G_BeginRecording (void) 
{ 
    int             i; 

    //!
    // @category demo
    //
    // Record a demo.
    //

    if (gameversion > exe_doom_1_2)
    {
        *demo_p++ = G_VanillaVersionCode();
    }

    *demo_p++ = gameskill; 
    *demo_p++ = gameepisode; 
    *demo_p++ = gamemap; 
    if (gameversion > exe_doom_1_2)
    {
        *demo_p++ = deathmatch; 
        *demo_p++ = respawnparm;
        *demo_p++ = fastparm;
        *demo_p++ = nomonsters;
        *demo_p++ = consoleplayer;
    }
	 
    for (i=0 ; i<MAXPLAYERS ; i++) 
	*demo_p++ = playeringame[i]; 		 
} 
 

//
// G_PlayDemo 
//

static const char *defdemoname;
  
void G_DeferedPlayDemo(const char *name)
{ 
    defdemoname = name; 
    gameaction = ga_playdemo; 
} 


void G_DoPlayDemo (void)
{
    skill_t skill;
    int i, episode, map;
    int demoversion;
    boolean olddemo = false;
    FILE *fp;
    char demoname[16];
    size_t size;

    gameaction = ga_nothing;

    M_snprintf(demoname, sizeof(demoname), "%s.lmp", defdemoname);
    fp = fopen(demoname, "rb");
    if (!fp)
    {
        I_Error("Could not open demo file: %s", demoname);
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    demo_buffer = malloc(size);
    if (!demo_buffer)
    {
        fclose(fp);
        I_Error("Could not allocate demo buffer");
    }

    if (fread(demo_buffer, 1, size, fp) != size)
    {
        fclose(fp);
        free(demo_buffer);
        I_Error("Failed to read demo file");
    }
    fclose(fp);

    demo_buffer_size = size;
    demo_p = demo_buffer;

    demoversion = *demo_p++;

    if (demoversion >= 0 && demoversion <= 4)
    {
        olddemo = true;
        demo_p--;
    }

    skill = *demo_p++;
    episode = *demo_p++; 
    map = *demo_p++; 
    if (!olddemo)
    {
        deathmatch = *demo_p++;
        respawnparm = *demo_p++;
        fastparm = *demo_p++;
        nomonsters = *demo_p++;
        consoleplayer = *demo_p++;
    }
    else
    {
        deathmatch = 0;
        respawnparm = 0;
        fastparm = 0;
        nomonsters = 0;
        consoleplayer = 0;
    }
    
        
    for (i=0 ; i<MAXPLAYERS ; i++) 
	playeringame[i] = *demo_p++; 

    if (playeringame[1] || M_CheckParm("-solo-net") > 0
                        || M_CheckParm("-netdemo") > 0)
    {
	netgame = true;
	netdemo = true;
    }

    // don't spend a lot of time in loadlevel 
    precache = false;
    G_InitNew (skill, episode, map); 
    precache = true; 
    starttime = I_GetTime (); 

    usergame = false; 
    demoplayback = true; 
} 

//
// G_TimeDemo 
//
void G_TimeDemo (char* name) 
{
    //!
    // @category video
    // @vanilla
    //
    // Disable rendering the screen entirely.
    //

    nodrawers = M_CheckParm ("-nodraw");

    timingdemo = true; 
    singletics = true; 

    defdemoname = name; 
    gameaction = ga_playdemo; 
} 
 
 
/* 
=================== 
= 
= G_CheckDemoStatus 
= 
= Called after a death or level completion to allow demos to be cleaned up 
= Returns true if a new demo loop action will take place 
=================== 
*/ 
 
boolean G_CheckDemoStatus (void) 
{ 
    int             endtime; 
	 
    if (timingdemo) 
    { 
        float fps;
        int realtics;

	endtime = I_GetTime (); 
        realtics = endtime - starttime;
        fps = ((float) gametic * TICRATE) / realtics;

        // Prevent recursive calls
        timingdemo = false;
        demoplayback = false;

	I_Error ("timed %i gametics in %i realtics (%f fps)",
                 gametic, realtics, fps);
    } 
	 
    if (demoplayback) 
    { 
        if (demo_buffer)
        {
            free(demo_buffer);
            demo_buffer = NULL;
        }
	demoplayback = false;
	netdemo = false;
	netgame = false;
	deathmatch = false;
	playeringame[1] = playeringame[2] = playeringame[3] = 0;
	respawnparm = false;
	fastparm = false;
	nomonsters = false;
	consoleplayer = 0;
        
        if (singledemo) 
            I_Quit (); 
        else 
            D_AdvanceDemo (); 

	return true; 
    } 
 
    if (demorecording) 
    { 
	*demo_p++ = DEMOMARKER; 
	M_WriteFile (demoname, demo_buffer_start, demo_p - demo_buffer_start); 
	Z_Free (demo_buffer_start); 
	demorecording = false; 
	I_Error ("Demo %s recorded",demoname); 
    }
    return false; 
}

void G_ReloadWeapon(void)
{
    player_t* player;
    ammotype_t ammo;
    int clipAmount;

    if (demoplayback || !players[consoleplayer].mo)
        return;

    player = &players[consoleplayer];

    if (player->health <= 0)
        return;

    ammo = weaponinfo[player->readyweapon].ammo;

    if (ammo == am_noammo)
        return;

    if (player->ammo[ammo] >= player->maxammo[ammo])
        return;

    clipAmount = clipammo[ammo];

    if (clipAmount > 0)
    {
        P_GiveAmmo(player, ammo, clipAmount);
    }
}

static char *import_save_path = NULL;
static int import_saves_trigger = 0;

void G_SetImportSavePath(const char *path)
{
    if (import_save_path != NULL)
    {
        free(import_save_path);
    }
    if (path != NULL && path[0] != '\0')
    {
        import_save_path = M_StringDuplicate(path);
    }
    else
    {
        import_save_path = NULL;
    }
}

const char *G_GetImportSavePath(void)
{
    return import_save_path;
}

void G_SetImportSavesTrigger(int value)
{
    import_saves_trigger = value;
    if (import_saves_trigger == 1)
    {
        G_DoImportSaveGames();
        import_saves_trigger = 0;
    }
}

int G_GetImportSavesTrigger(void)
{
    return import_saves_trigger;
}

void G_DoImportSaveGames(void)
{
    glob_t *glob;
    const char *filename;
    int imported_count = 0;
    int slot;
    char src_path[512];
    char dest_path[512];
    byte *file_buffer;
    int file_size;

    if (import_save_path == NULL || *import_save_path == '\0')
    {
        DEH_printf("Import path not set. Use -import_saves <path> or set import_save_path.\n");
        DEH_printf("Set import_save_path to the directory containing savegames,\n");
        DEH_printf("then set import_saves_trigger to 1 to import.\n");
        return;
    }

    if (!M_FileExists(import_save_path))
    {
        DEH_printf("Import directory does not exist: %s\n", import_save_path);
        return;
    }

    DEH_printf("Scanning for savegames in: %s\n", import_save_path);

    glob = I_StartGlob(import_save_path, "doomsav*.dsg", GLOB_FLAG_NOCASE);
    if (glob == NULL)
    {
        DEH_printf("Failed to scan directory.\n");
        return;
    }

    while ((filename = I_NextGlob(glob)) != NULL)
    {
        M_snprintf(src_path, sizeof(src_path), "%s%s%s", 
                   import_save_path, DIR_SEPARATOR_S, filename);

        if (!M_FileExists(src_path))
            continue;

        file_size = M_ReadFile(src_path, &file_buffer);
        if (file_size <= 0)
        {
            DEH_printf("Failed to read: %s\n", filename);
            continue;
        }

        if (sscanf(filename, "doomsav%d.dsg", &slot) != 1)
        {
            DEH_printf("Could not parse slot from: %s\n", filename);
            free(file_buffer);
            continue;
        }

        M_snprintf(dest_path, sizeof(dest_path), "%sdoomsav%d.dsg",
                   savegamedir, slot);

        if (M_WriteFile(dest_path, file_buffer, file_size))
        {
            DEH_printf("Imported savegame %d from: %s\n", slot, filename);
            imported_count++;
        }
        else
        {
            DEH_printf("Failed to write savegame %d\n", slot);
        }

        free(file_buffer);
    }

    I_EndGlob(glob);

    if (imported_count > 0)
    {
        DEH_printf("Successfully imported %d savegame(s).\n", imported_count);
        DEH_printf("Load the game and check the Load Game menu.\n");
    }
    else
    {
        DEH_printf("No savegames found to import.\n");
    }
}
