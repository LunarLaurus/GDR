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
//	DOOM selection menu, options, episode etc.
//	Sliders and icons. Kinda widget stuff.
//


#include <stdlib.h>
#include <math.h>
#include <ctype.h>


#include "doomdef.h"
#include "doomkeys.h"
#include "dstrings.h"
#include "g_balance.h"

#include "d_main.h"
#include "deh_main.h"

#include "i_input.h"
#include "i_joystick.h"
#include "i_swap.h"
#include "i_system.h"
#include "i_timer.h"
#include "i_video.h"
#include "m_misc.h"
#include "m_random.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "r_local.h"


#include "hu_stuff.h"

#include "g_game.h"
#include "g_rpg.h"
#include "g_survival.h"
#include "g_timeattack.h"
#include "g_leaderboard.h"

#include "m_argv.h"
#include "m_controls.h"
#include "p_saveg.h"
#include "p_setup.h"

#include "s_sound.h"

#include "doomstat.h"

// Data.
#include "sounds.h"

#include "m_menu.h"

#include "st_stuff.h"

#include "m_menu_gameopts.h"
#include "m_menu_accessibility.h"


//
// defaulted values
//

#define DEFAULT_DICE_VOLUME 8

int			mouseSensitivity = 5;
float		mouse_sensitivity_scale = 1.0f;

// Show messages has default, 0 = off, 1 = on
int			showMessages = 1;

// Goblin Dice Rollaz: RPG Progression Mode toggle (0=off, 1=on)
int			rpg_mode = 0;

// Goblin Dice Rollaz: Game Settings
int			dice_sound_volume = DEFAULT_DICE_VOLUME;       // Dice roll sound volume (0-15)
int			crit_notification = 1;        // Show crit text notifications (0=off, 1=on)
float		screen_shake_intensity = DEFAULT_SHAKE_SCALE; // Screen shake multiplier (0.0-2.0)
float		damage_number_scale = DEFAULT_DAMAGE_SCALE;    // Damage number size multiplier (0.5-2.0)
int			particle_effects = 1;         // Enable particle effects (0=off, 1=on)
int			kill_confirm_enabled = 1;     // Show kill confirmation popups (0=off, 1=on)

// Goblin Dice Rollaz: Accessibility Settings
int			goblin_colorblind_mode = 0;          // Colorblind mode (0=off, 1=red-green, 2=blue-yellow)
float		hud_scale = 1.0f;              // HUD scale multiplier (0.5-2.0)
int			goblin_reduce_motion = 0;            // Reduce motion effects (0=off, 1=on)
int			screen_reader_enabled = 0;   // Screen reader mode (0=off, 1=on)

// Goblin Dice Rollaz: Region settings for content warnings
// 0=North America (ESRB), 1=Europe (PEGI), 2=Australia (ACB), 3=Japan (CERO)
int			player_region = 0;
int			content_warning_accepted = 0;  // Has user acknowledged warning

// Goblin Dice Rollaz: Main menu animated background
#define MENU_ANIM_FRAMES 8
#define MENU_ANIM_SPEED 4
#define MAX_MENU_PARTICLES 20

// Die type palette: index into Doom palette for distinctive per-die colours
// Safe dim palette indices that show up on black without blowing out
static const int dice_palette_color[] = {
    160, // d4  — orange-gold range
    112, // d6  — green range
    180, // d8  — amber
     44, // d10 — rust red
      8, // d12 — brown
     80, // d20 — muted blue-purple range
     32, // d100 — dark red
};

static const int dice_types[] = { 4, 6, 8, 10, 12, 20, 100 };
#define NUM_DICE_TYPES 7

typedef struct {
    float x;
    float y;      // float so sub-pixel speed actually moves the particle
    int   die_type;
    float speed_y;
    int   color;
    int   wobble_offset; // horizontal oscillation phase
} menu_dice_particle_t;

static int menu_anim_tic = 0;
static menu_dice_particle_t menu_particles[MAX_MENU_PARTICLES];
static boolean menu_anim_initialized = false;

void M_InitMenuAnimation(void);
void M_DrawMenuBackground(void);
  
// Goblin Dice Rollaz: detailLevel removed - always high detail
int			screenblocks = 9;

// GDR: Legacy menu variables preserved for M_DrawOptions / M_Episode compatibility
static int		epi = 0;          // last selected episode (0-based)
static int		detail = 0;       // detail level row index (always high detail)
static int		detailLevel = 0;  // 0 = high, 1 = low; always 0 in GDR
static int		messages = 1;     // messages row index in options menu
static int		mousesens = 3;    // mouse sensitivity row index in options menu
static int		scrnsize = 4;     // screen size row index in options menu

// temp for screenblocks (0-9)
int			screenSize;

// -1 = no quicksave slot picked!
int			quickSaveSlot;
int			quickSaveSlot2;

 // 1 = message to be printed
int			messageToPrint;
// ...and here is the message string!
const char		*messageString;

// message x & y
int			messx;
int			messy;
int			messageLastMenuActive;

// timed message = no input from user
boolean			messageNeedsInput;

void    (*messageRoutine)(int response);

char gammamsg[5][26] =
{
    GAMMALVL0,
    GAMMALVL1,
    GAMMALVL2,
    GAMMALVL3,
    GAMMALVL4
};

// we are going to be entering a savegame string
int			saveStringEnter;              
int             	saveSlot;	// which slot to save in
int			saveCharIndex;	// which char we're editing
static boolean          joypadSave = false; // was the save action initiated by joypad?
// old save description before edit
char			saveOldString[SAVESTRINGSIZE];  
// old save comment before edit
char			saveOldComment[SAVESTRINGSIZE];  
// are we editing the comment instead of the name?
static boolean          editingComment = false;

boolean			inhelpscreens;
boolean			menuactive;

#define SKULLXOFF		-32
// LINEHEIGHT is defined in m_menu.h

char			savegamestrings[10][SAVESTRINGSIZE];
char			savecomments[10][SAVESTRINGSIZE];

char	endstring[160];

static boolean opldev;

// menuitem_t and menu_t are defined in m_menu.h

short		itemOn;			// menu item skull is on
short		skullAnimCounter;	// skull animation counter
short		whichSkull;		// which skull to draw

// graphic name of skulls
// warning: initializer-string for array of chars is too long
const char *skullName[2] = {"M_SKULL1","M_SKULL2"};

// current menudef
menu_t*	currentMenu;                          

//
// PROTOTYPES
//
static void M_NewGame(int choice);
static void M_SurvivalMode(int choice);
static void M_TimeAttackMode(int choice);
static void M_Episode(int choice);
static void M_ChooseSkill(int choice);
static void M_LoadGame(int choice);
static void M_SaveGame(int choice);
static void M_Options(int choice);
static void M_EndGame(int choice);
static void M_ReadThis(int choice);
static void M_ReadThis2(int choice);
static void M_QuitDOOM(int choice);

static void M_ChangeMessages(int choice);
static void M_ChangeRPGMode(int choice);
static void M_ChangeSensitivity(int choice);
static void M_SfxVol(int choice);
static void M_MusicVol(int choice);
static void M_ChangeDetail(int choice);
static void M_SizeDisplay(int choice);
static void M_Sound(int choice);

// Goblin Dice Rollaz: Accessibility menu (defined in m_menu_accessibility.c)
// GDR STUB: forward declarations provided via m_menu_accessibility.h
static void M_ColorblindMode(int choice);
static void M_HUDScale(int choice);
static void M_ReduceMotion(int choice);
static void M_ScreenReaderMode(int choice);

// Goblin Dice Rollaz: Content Warning menu
static void M_ContentWarning(int choice);
static void M_ChangeRegion(int choice);
static void M_ShowContentWarning(int choice);
static void M_DrawContentWarning(void);
static void M_AcceptContentWarning(int choice);

static void M_FinishReadThis(int choice);
static void M_LoadSelect(int choice);
static void M_SaveSelect(int choice);
static void M_ReadSaveStrings(void);
static void M_QuickSave(void);
static void M_QuickLoad(void);
static void M_QuickSave2(void);
static void M_QuickLoad2(void);

static void M_DrawMainMenu(void);
static void M_DrawReadThis1(void);
static void M_DrawReadThis2(void);
static void M_DrawNewGame(void);
static void M_DrawEpisode(void);
static void M_DrawOptions(void);
static void M_DrawSound(void);
static void M_DrawLoad(void);
static void M_DrawSave(void);

static void M_DrawSaveLoadBorder(int x,int y);
// M_SetupNextMenu is declared in m_menu.h
// M_DrawThermo is declared in m_menu.h
void M_WriteText(int x, int y, const char *string);
static int  M_StringWidth(const char *string);
static int  M_StringHeight(const char *string);
static void M_StartMessage(const char *string, void *routine, boolean input);
static void M_ClearMenus (void);

// Goblin Dice Rollaz: Level Up menu
void M_LevelUp(int choice);  /* non-static: called from g_game.c */
static void M_DrawLevelUp(void);
static void M_LevelUpStrength(int choice);
static void M_LevelUpDexterity(int choice);
static void M_LevelUpVitality(int choice);
static void M_LevelUpLuck(int choice);
static void M_CloseLevelUp(int choice);
static void M_AllocateStat(int choice);

// Goblin Dice Rollaz: Leaderboard menu
static void M_Leaderboard(int choice);
static void M_DrawLeaderboard(void);
static void M_LeaderboardSurvival(int choice);
static void M_LeaderboardTimeAttack(int choice);
static void M_LeaderboardCritOnly(int choice);
static void M_LeaderboardNoPowerups(int choice);
static void M_LeaderboardHardcore(int choice);
static void M_ClearLeaderboards(int choice);
//
// EPISODE SELECT
//
enum
{
    ep1,
    ep2,
    ep3,
    ep4,
    ep_end
} episodes_e;

menuitem_t EpisodeMenu[]=
{
    {1,"M_EPI1", M_Episode,'k'},
    {1,"M_EPI2", M_Episode,'t'},
    {1,"M_EPI3", M_Episode,'i'},
    {1,"M_EPI4", M_Episode,'t'}
};

menu_t  EpiDef =
{
    ep_end,		// # of menu items
    &MainDef,		// previous menu
    EpisodeMenu,	// menuitem_t ->
    M_DrawEpisode,	// drawing routine ->
    48,63,              // x,y
    ep1			// lastOn
};

//
// NEW GAME
//
enum
{
    killthings,
    toorough,
    hurtme,
    violence,
    nightmare,
    newg_end
} newgame_e;

menuitem_t NewGameMenu[]=
{
    {1,"",	M_ChooseSkill, 'i'},
    {1,"",	M_ChooseSkill, 'h'},
    {1,"",	M_ChooseSkill, 'h'},
    {1,"",	M_ChooseSkill, 'u'},
    {1,"",	M_ChooseSkill, 'n'}
};

menu_t  NewDef =
{
    newg_end,		// # of menu items
    &EpiDef,		// previous menu
    NewGameMenu,	// menuitem_t ->
    M_DrawNewGame,	// drawing routine ->
    48,63,              // x,y
    hurtme		// lastOn
};



//
// DOOM MENU
//
enum
{
    newgame = 0,
    survivalmode,
    timeattackmode,
    leaderboard,
    options,
    gamesettings,
    accessibility,
    contentwarn,
    loadgame,
    readthis,
    quitdoom,
    main_end
} main_e;

menuitem_t MainMenu[]=
{
    {1,"M_NGAME",M_NewGame,'n'},
    {1,"M_SURVIV",M_SurvivalMode,'s'},
    {1,"M_TIMATT",M_TimeAttackMode,'t'},
    {1,"M_LEADER",M_Leaderboard,'l'},
    {1,"M_OPTION",M_Options,'o'},
    {1,"M_GAMESET",M_GameSettings,'g'},
    {1,"M_ACCESS",M_Accessibility,'a'},
    {1,"",M_ContentWarning,'c'},
    {1,"M_LOADG",M_LoadGame,'b'},
    {1,"M_SAVEG",M_SaveGame,'v'},
    // Another hickup with Special edition.
    {1,"M_RDTHIS",M_ReadThis,'r'},
    {1,"M_QUITG",M_QuitDOOM,'q'}
};

menu_t  MainDef =
{
    main_end,
    NULL,
    MainMenu,
    M_DrawMainMenu,
    97,64,
    0
};

//
// OPTIONS MENU
//
enum
{
    opt_endflags,
    opt_customize,
    opt_messages,
    opt_sound,
    opt_sensitivity,
    opt_quick,
    opt_always,
    opt_end
} options_e;

menuitem_t OptionsMenu[]=
{
    {1,"M_CUSTM", M_Options, 'c'},
    {1,"M_MSGS",  M_ChangeMessages, 's'},
    {1,"M_SNDvol", M_Sound, 's'},
    {1,"M_SENS",  M_ChangeSensitivity, 's'},
    {1,"M_QUICK", M_QuickSave2, 'q'},
    // Goblin Dice Rollaz: Removed detail level - always high detail
    {1,"M_ENDGC", M_EndGame, 'e'}
};

menu_t  OptionsDef =
{
    opt_end,
    &MainDef,
    OptionsMenu,
    M_DrawOptions,
    60,37,
    0
};

//
// Goblin Dice Rollaz: LEADERBOARD MENU
//
enum
{
    leaderboard_survival,
    leaderboard_timeattack,
    leaderboard_critonly,
    leaderboard_nopowerups,
    leaderboard_hardcore,
    leaderboard_clear,
    leaderboard_back,
    leaderboard_end
} leaderboard_e;

menuitem_t LeaderboardMenu[]=
{
    {1,"M_SURVIV", M_LeaderboardSurvival, 's'},
    {1,"M_TIMATT", M_LeaderboardTimeAttack, 't'},
    {1,"M_CRITON", M_LeaderboardCritOnly, 'c'},
    {1,"M_NOPWUP", M_LeaderboardNoPowerups, 'n'},
    {1,"M_HARDC", M_LeaderboardHardcore, 'h'},
    {1,"M_CLEAR", M_ClearLeaderboards, 'l'},
    {1,"M_BACK", M_CloseLeaderboard, 'b'}
};

menu_t  LeaderboardDef =
{
    leaderboard_end,
    &MainDef,
    LeaderboardMenu,
    M_DrawLeaderboard,
    60,37,
    0
};

//
// Goblin Dice Rollaz: CONTENT WARNING MENU
//
enum
{
    contentwarn_region,
    contentwarn_view,
    contentwarn_accept,
    contentwarn_end
} contentwarn_e;

menuitem_t ContentWarningMenu[]=
{
    {2,"M_REGIONS", M_ChangeRegion, 'r'},
    {1,"M_VIEW", M_ShowContentWarning, 'v'},
    {1,"M_ACKWRN", M_AcceptContentWarning, 'a'}
};

menu_t  ContentWarningDef =
{
    contentwarn_end,
    &MainDef,
    ContentWarningMenu,
    M_DrawContentWarning,
    60,37,
    0
};

//
// Read This! MENU 1 & 2
//
enum
{
    rdthsempty1,
    read1_end
} read_e;

menuitem_t ReadMenu1[] =
{
    {1,"",M_ReadThis2,0}
};

menu_t  ReadDef1 =
{
    read1_end,
    &MainDef,
    ReadMenu1,
    M_DrawReadThis1,
    280,185,
    0
};

enum
{
    rdthsempty2,
    read2_end
} read_e2;

menuitem_t ReadMenu2[]=
{
    {1,"",M_FinishReadThis,0}
};

menu_t  ReadDef2 =
{
    read2_end,
    &ReadDef1,
    ReadMenu2,
    M_DrawReadThis2,
    330,175,
    0
};

//
// SOUND VOLUME MENU
//
enum
{
    sfx_vol,
    sfx_empty1,
    music_vol,
    sfx_empty2,
    sound_end
} sound_e;

menuitem_t SoundMenu[]=
{
    {2,"M_SFXVOL",M_SfxVol,'s'},
    {-1,"",0,'\0'},
    {2,"M_MUSVOL",M_MusicVol,'m'},
    {-1,"",0,'\0'}
};

menu_t  SoundDef =
{
    sound_end,
    &OptionsDef,
    SoundMenu,
    M_DrawSound,
    80,64,
    0
};

//
// LEVEL UP STAT SELECTION MENU (Goblin Dice Rollaz)
//
enum
{
    levup_strength,
    levup_dexterity,
    levup_vitality,
    levup_luck,
    levup_close,
    levup_end
} levup_e;

menuitem_t LevelUpMenu[] =
{
    {1,"", M_LevelUpStrength,'s'},
    {1,"", M_LevelUpDexterity,'d'},
    {1,"", M_LevelUpVitality,'v'},
    {1,"", M_LevelUpLuck,'l'},
    {1,"", M_CloseLevelUp,'c'}
};

menu_t LevelUpDef =
{
    levup_end,
    NULL,
    LevelUpMenu,
    M_DrawLevelUp,
    180,110,
    0
};

//
// LOAD GAME MENU
//
enum
{
    load1,
    load2,
    load3,
    load4,
    load5,
    load6,
    load7,
    load8,
    load9,
    load10,
    load_end
} load_e;

menuitem_t LoadMenu[]=
{
    {1,"", M_LoadSelect,'1'},
    {1,"", M_LoadSelect,'2'},
    {1,"", M_LoadSelect,'3'},
    {1,"", M_LoadSelect,'4'},
    {1,"", M_LoadSelect,'5'},
    {1,"", M_LoadSelect,'6'},
    {1,"", M_LoadSelect,'7'},
    {1,"", M_LoadSelect,'8'},
    {1,"", M_LoadSelect,'9'},
    {1,"", M_LoadSelect,'0'}
};

menu_t  LoadDef =
{
    load_end,
    &MainDef,
    LoadMenu,
    M_DrawLoad,
    80,32,
    0
};

//
// SAVE GAME MENU
//
menuitem_t SaveMenu[]=
{
    {1,"", M_SaveSelect,'1'},
    {1,"", M_SaveSelect,'2'},
    {1,"", M_SaveSelect,'3'},
    {1,"", M_SaveSelect,'4'},
    {1,"", M_SaveSelect,'5'},
    {1,"", M_SaveSelect,'6'},
    {1,"", M_SaveSelect,'7'},
    {1,"", M_SaveSelect,'8'},
    {1,"", M_SaveSelect,'9'},
    {1,"", M_SaveSelect,'0'}
};

menu_t  SaveDef =
{
    load_end,
    &MainDef,
    SaveMenu,
    M_DrawSave,
    80,32,
    0
};


//
// M_ReadSaveStrings
//  read the strings from the savegame files
//
void M_ReadSaveStrings(void)
{
    FILE   *handle;
    int     i;
    char    name[256];

    for (i = 0;i < load_end;i++)
    {
        int retval;
        M_StringCopy(name, P_SaveGameFile(i), sizeof(name));

	handle = M_fopen(name, "rb");
        if (handle == NULL)
        {
            M_StringCopy(savegamestrings[i], EMPTYSTRING, SAVESTRINGSIZE);
            M_StringCopy(savecomments[i], EMPTYSTRING, SAVESTRINGSIZE);
            LoadMenu[i].status = 0;
            continue;
        }
        retval = fread(&savegamestrings[i], 1, SAVESTRINGSIZE, handle);
        retval += fread(&savecomments[i], 1, SAVESTRINGSIZE, handle);
	fclose(handle);
        LoadMenu[i].status = retval >= SAVESTRINGSIZE;
    }
}


//
// M_LoadGame & Cie.
//
void M_DrawLoad(void)
{
    int             i;
	
    V_DrawPatchDirect(72, 28, 
                      W_CacheLumpName(DEH_String("M_LOADG"), PU_CACHE));

    for (i = 0;i < load_end; i++)
    {
	M_DrawSaveLoadBorder(LoadDef.x,LoadDef.y+LINEHEIGHT*i);
	M_WriteText(LoadDef.x,LoadDef.y+LINEHEIGHT*i,savegamestrings[i]);
	if (savecomments[i][0])
	{
	    M_WriteText(LoadDef.x + 16, LoadDef.y + LINEHEIGHT*i + 10, savecomments[i]);
	}
    }
}



//
// Draw border for the savegame description
//
void M_DrawSaveLoadBorder(int x,int y)
{
    int             i;
	
    V_DrawPatchDirect(x - 8, y + 7,
                      W_CacheLumpName(DEH_String("M_LSLEFT"), PU_CACHE));
	
    for (i = 0;i < 24;i++)
    {
	V_DrawPatchDirect(x, y + 7,
                          W_CacheLumpName(DEH_String("M_LSCNTR"), PU_CACHE));
	x += 8;
    }

    V_DrawPatchDirect(x, y + 7, 
                      W_CacheLumpName(DEH_String("M_LSRGHT"), PU_CACHE));
}



//
// User wants to load this game
//
void M_LoadSelect(int choice)
{
    char    name[256];
	
    M_StringCopy(name, P_SaveGameFile(choice), sizeof(name));

    G_LoadGame (name);
    M_ClearMenus ();
}

//
// Selected from DOOM menu
//
void M_LoadGame (int choice)
{
    if (netgame)
    {
	M_StartMessage(DEH_String(LOADNET),NULL,false);
	return;
    }
	
    M_SetupNextMenu(&LoadDef);
    M_ReadSaveStrings();
}


//
//  M_SaveGame & Cie.
//
void M_DrawSave(void)
{
    int             i;
	
    V_DrawPatchDirect(72, 28, W_CacheLumpName(DEH_String("M_SAVEG"), PU_CACHE));
    for (i = 0;i < load_end; i++)
    {
	M_DrawSaveLoadBorder(LoadDef.x,LoadDef.y+LINEHEIGHT*i);
	M_WriteText(LoadDef.x,LoadDef.y+LINEHEIGHT*i,savegamestrings[i]);
	if (savecomments[i][0])
	{
	    M_WriteText(LoadDef.x + 16, LoadDef.y + LINEHEIGHT*i + 10, savecomments[i]);
	}
    }
	
    if (saveStringEnter)
    {
	if (editingComment)
	{
	    int i = M_StringWidth(savecomments[saveSlot]);
	    M_WriteText(LoadDef.x + 16 + i, LoadDef.y+LINEHEIGHT*saveSlot + 10, "_");
	}
	else
	{
	    i = M_StringWidth(savegamestrings[saveSlot]);
	    M_WriteText(LoadDef.x + i,LoadDef.y+LINEHEIGHT*saveSlot,"_");
	}
    }
}

//
// M_Responder calls this when user is finished
//
void M_DoSave(int slot)
{
    G_SaveGame (slot,savegamestrings[slot], savecomments[slot]);
    M_ClearMenus ();

    // PICK QUICKSAVE SLOT YET?
    if (quickSaveSlot == -2)
	quickSaveSlot = slot;
    if (quickSaveSlot2 == -2)
	quickSaveSlot2 = slot;
}

//
// Generate a default save slot name when the user saves to
// an empty slot via the joypad.
//
static void SetDefaultSaveName(int slot)
{
    // map from IWAD or PWAD?
    if (W_IsIWADLump(maplumpinfo) && strcmp(savegamedir, ""))
    {
        M_snprintf(savegamestrings[itemOn], SAVESTRINGSIZE,
                   "%s", maplumpinfo->name);
    }
    else
    {
        char *wadname = M_StringDuplicate(W_WadNameForLump(maplumpinfo));
        char *ext = strrchr(wadname, '.');

        if (ext != NULL)
        {
            *ext = '\0';
        }

        M_snprintf(savegamestrings[itemOn], SAVESTRINGSIZE,
                   "%s (%s)", maplumpinfo->name,
                   wadname);
        free(wadname);
    }
    M_ForceUppercase(savegamestrings[itemOn]);
    joypadSave = false;
}

//
// User wants to save. Start string input for M_Responder
//
void M_SaveSelect(int choice)
{
    int x, y;

    // we are going to be intercepting all chars
    saveStringEnter = 1;
    editingComment = false;

    // We need to turn on text input:
    x = LoadDef.x - 11;
    y = LoadDef.y + choice * LINEHEIGHT - 4;
    I_StartTextInput(x, y, x + 8 + 24 * 8 + 8, y + LINEHEIGHT - 2);

    saveSlot = choice;
    M_StringCopy(saveOldString,savegamestrings[choice], SAVESTRINGSIZE);
    M_StringCopy(saveOldComment,savecomments[choice], SAVESTRINGSIZE);
    if (!strcmp(savegamestrings[choice], EMPTYSTRING))
    {
        savegamestrings[choice][0] = 0;

        if (joypadSave)
        {
            SetDefaultSaveName(choice);
        }
    }
    if (!strcmp(savecomments[choice], EMPTYSTRING))
    {
        M_snprintf(savecomments[choice], SAVESTRINGSIZE,
                   "Level %d", gamemap);
    }
    saveCharIndex = strlen(savegamestrings[choice]);
}

//
// Selected from DOOM menu
//
void M_SaveGame (int choice)
{
    if (!usergame)
    {
	M_StartMessage(DEH_String(SAVEDEAD),NULL,false);
	return;
    }
	
    if (gamestate != GS_LEVEL)
	return;
	
    M_SetupNextMenu(&SaveDef);
    M_ReadSaveStrings();
}



//
//      M_QuickSave
//
static char tempstring[90];

void M_QuickSaveResponse(int key)
{
    if (key == key_menu_confirm)
    {
	M_DoSave(quickSaveSlot);
	S_StartSound(NULL,sfx_swtchx);
    }
}

void M_QuickSave(void)
{
    if (!usergame)
    {
	S_StartSound(NULL,sfx_oof);
	return;
    }

    if (gamestate != GS_LEVEL)
	return;
	
    if (quickSaveSlot < 0)
    {
	M_StartControlPanel();
	M_ReadSaveStrings();
	M_SetupNextMenu(&SaveDef);
	quickSaveSlot = -2;	// means to pick a slot now
	return;
    }
    DEH_snprintf(tempstring, sizeof(tempstring),
                 QSPROMPT, savegamestrings[quickSaveSlot]);
    M_StartMessage(tempstring, M_QuickSaveResponse, true);
}



//
// M_QuickLoad
//
void M_QuickLoadResponse(int key)
{
    if (key == key_menu_confirm)
    {
	M_LoadSelect(quickSaveSlot);
	S_StartSound(NULL,sfx_swtchx);
    }
}


void M_QuickLoad(void)
{
    if (netgame)
    {
	M_StartMessage(DEH_String(QLOADNET),NULL,false);
	return;
    }
	
    if (quickSaveSlot < 0)
    {
	M_StartMessage(DEH_String(QSAVESPOT),NULL,false);
	return;
    }
    DEH_snprintf(tempstring, sizeof(tempstring),
                 QLPROMPT, savegamestrings[quickSaveSlot]);
    M_StartMessage(tempstring, M_QuickLoadResponse, true);
}



//
// M_QuickSave2 - Alternate quicksave slot
//
void M_QuickSave2Response(int key)
{
    if (key == key_menu_confirm)
    {
	M_DoSave(quickSaveSlot2);
	S_StartSound(NULL,sfx_swtchx);
    }
}

void M_QuickSave2(void)
{
    if (!usergame)
    {
	S_StartSound(NULL,sfx_oof);
	return;
    }

    if (gamestate != GS_LEVEL)
	return;
	
    if (quickSaveSlot2 < 0)
    {
	M_StartControlPanel();
	M_ReadSaveStrings();
	M_SetupNextMenu(&SaveDef);
	quickSaveSlot2 = -2;
	return;
    }
    DEH_snprintf(tempstring, sizeof(tempstring),
                 QSPROMPT, savegamestrings[quickSaveSlot2]);
    M_StartMessage(tempstring, M_QuickSave2Response, true);
}



//
// M_QuickLoad2 - Alternate quickload slot
//
void M_QuickLoad2Response(int key)
{
    if (key == key_menu_confirm)
    {
	M_LoadSelect(quickSaveSlot2);
	S_StartSound(NULL,sfx_swtchx);
    }
}


void M_QuickLoad2(void)
{
    if (netgame)
    {
	M_StartMessage(DEH_String(QLOADNET),NULL,false);
	return;
    }
	
    if (quickSaveSlot2 < 0)
    {
	M_StartMessage(DEH_String(QSAVESPOT),NULL,false);
	return;
    }
    DEH_snprintf(tempstring, sizeof(tempstring),
                 QLPROMPT, savegamestrings[quickSaveSlot2]);
    M_StartMessage(tempstring, M_QuickLoad2Response, true);
}




//
// Read This Menus
// Had a "quick hack to fix romero bug"
//
void M_DrawReadThis1(void)
{
    inhelpscreens = true;

    V_DrawPatchDirect(0, 0, W_CacheLumpName(DEH_String("HELP2"), PU_CACHE));
}



//
// Read This Menus - optional second page.
//
void M_DrawReadThis2(void)
{
    inhelpscreens = true;

    // We only ever draw the second page if this is 
    // gameversion == exe_doom_1_9 and gamemode == registered

    V_DrawPatchDirect(0, 0, W_CacheLumpName(DEH_String("HELP1"), PU_CACHE));
}

void M_DrawReadThisCommercial(void)
{
    inhelpscreens = true;

    V_DrawPatchDirect(0, 0, W_CacheLumpName(DEH_String("HELP"), PU_CACHE));
}


//
// Change Sfx & Music volumes
//
void M_DrawSound(void)
{
    V_DrawPatchDirect (60, 38, W_CacheLumpName(DEH_String("M_SVOL"), PU_CACHE));

    M_DrawThermo(SoundDef.x,SoundDef.y+LINEHEIGHT*(sfx_vol+1),
		 16,sfxVolume);

    M_DrawThermo(SoundDef.x,SoundDef.y+LINEHEIGHT*(music_vol+1),
		 16,musicVolume);
}

void M_Sound(int choice)
{
    M_SetupNextMenu(&SoundDef);
}

void M_SfxVol(int choice)
{
    switch(choice)
    {
      case 0:
	if (sfxVolume)
	    sfxVolume--;
	break;
      case 1:
	if (sfxVolume < 15)
	    sfxVolume++;
	break;
    }
	
    S_SetSfxVolume(sfxVolume * 8);
}

void M_MusicVol(int choice)
{
    switch(choice)
    {
      case 0:
	if (musicVolume)
	    musicVolume--;
	break;
      case 1:
	if (musicVolume < 15)
	    musicVolume++;
	break;
    }
	
    S_SetMusicVolume(musicVolume * 8);
}

//
// M_InitMenuAnimation
// Initialize floating dice particles for main menu background
//
void M_InitMenuAnimation(void)
{
    int i;
    if (menu_anim_initialized)
        return;

    for (i = 0; i < MAX_MENU_PARTICLES; i++)
    {
        int dtype_idx = i % NUM_DICE_TYPES;
        menu_particles[i].x            = (float)((M_Random() % (SCREENWIDTH - 48)) + 16);
        menu_particles[i].y            = (float)(M_Random() % SCREENHEIGHT);
        menu_particles[i].die_type     = dice_types[dtype_idx];
        // Speed range 0.4–1.2 px/frame so all particles visibly move
        menu_particles[i].speed_y      = 0.4f + (float)(M_Random() % 80) / 100.0f;
        menu_particles[i].color        = dice_palette_color[dtype_idx];
        menu_particles[i].wobble_offset = M_Random() % 64;
    }
    menu_anim_initialized = true;
}

//
// M_DrawMenuBackground
// Draw animated background with floating dice for main menu
//
// Draw a single character from hu_font at (x,y), returns glyph width
static int M_DrawMenuChar(int x, int y, char c)
{
    int cidx;
    if (c >= 'a' && c <= 'z')
        cidx = (c - 'a' + 'A') - HU_FONTSTART;
    else
        cidx = (unsigned char)c - HU_FONTSTART;

    if (cidx >= 0 && cidx < HU_FONTSIZE && hu_font[cidx])
    {
        V_DrawPatchDirect(x, y, hu_font[cidx]);
        return SHORT(hu_font[cidx]->width);
    }
    return 6;
}

// Draw a dice pip pattern for faces 1-6 inside a box at (bx,by) size (bw,bh)
static void M_DrawDiePips(int bx, int by, int bw, int bh, int face, int col)
{
    // Pip positions as fractions of box (col_frac * bw / 4, row_frac * bh / 4)
    static const int pip_col[6][6] = {
        {2,-1,-1,-1,-1,-1}, // 1
        {1, 3,-1,-1,-1,-1}, // 2
        {1, 2, 3,-1,-1,-1}, // 3
        {1, 3, 1, 3,-1,-1}, // 4
        {1, 3, 2, 1, 3,-1}, // 5
        {1, 3, 1, 3, 1, 3}, // 6
    };
    static const int pip_row[6][6] = {
        {2,-1,-1,-1,-1,-1},
        {1, 3,-1,-1,-1,-1},
        {1, 2, 3,-1,-1,-1},
        {1, 3, 1, 3,-1,-1},
        {1, 3, 2, 1, 3,-1},
        {1, 3, 1, 3, 1, 3},
    };
    int n, p;
    if (face < 1 || face > 6) return;
    n = face;
    for (p = 0; p < n; p++)
    {
        int px = bx + (pip_col[face-1][p] * bw) / 4;
        int py = by + (pip_row[face-1][p] * bh) / 4;
        if (px >= 0 && py >= 0 && px < SCREENWIDTH && py < SCREENHEIGHT)
            V_DrawFilledBox(px - 1, py - 1, 3, 3, col);
    }
}

void M_DrawMenuBackground(void)
{
    int i;

    if (!menu_anim_initialized)
        M_InitMenuAnimation();

    menu_anim_tic++;

    // Dark background
    V_DrawFilledBox(0, 0, SCREENWIDTH, SCREENHEIGHT, 0);

    // Subtle horizontal scan-line bands for depth (every 4 rows, 1px tall)
    for (i = 0; i < SCREENHEIGHT - 1; i += 4)
        V_DrawFilledBox(0, i, SCREENWIDTH, 1, 2);

    // Update and draw each die particle
    for (i = 0; i < MAX_MENU_PARTICLES; i++)
    {
        int px, py, bw, bh;
        int wobble;
        char label[8];
        int lx, j;

        // Move upward — float y means every frame has real movement
        menu_particles[i].y -= menu_particles[i].speed_y;

        // Vertical wrap: reappear at bottom when off top
        if (menu_particles[i].y < -24.0f)
        {
            menu_particles[i].y      = (float)(SCREENHEIGHT + 10);
            menu_particles[i].x      = (float)((M_Random() % (SCREENWIDTH - 48)) + 16);
            menu_particles[i].speed_y = 0.4f + (float)(M_Random() % 80) / 100.0f;
        }

        py = (int)menu_particles[i].y;
        if (py < -20 || py >= SCREENHEIGHT)
            continue;

        // Gentle horizontal wobble
        wobble = (int)(2.0f * sinf(
            (float)(menu_anim_tic + menu_particles[i].wobble_offset) * 0.08f));
        px = (int)menu_particles[i].x + wobble;
        px = px < 2 ? 2 : (px > SCREENWIDTH - 24 ? SCREENWIDTH - 24 : px);

        // Die box size: d4/d6 need more room for pip patterns
        bw = (menu_particles[i].die_type <= 6) ? 16 : 14;
        bh = bw;

        // Draw die border box (two-tone: bright edge, dark fill)
        V_DrawFilledBox(px,     py,     bw,     bh,     menu_particles[i].color);
        V_DrawFilledBox(px + 1, py + 1, bw - 2, bh - 2, 0);

        // For d4/d6 draw pip pattern; for larger dice draw "dN" label
        if (menu_particles[i].die_type <= 6)
        {
            // Animated face: cycle through 1..die_type slowly
            int face = ((menu_anim_tic / 8 + i * 3) % menu_particles[i].die_type) + 1;
            M_DrawDiePips(px + 1, py + 1, bw - 2, bh - 2, face,
                          menu_particles[i].color);
        }
        else
        {
            // Draw "d20", "d12", "d100" etc. as compact text inside the box
            if (menu_particles[i].die_type == 100)
                M_snprintf(label, sizeof(label), "d%%");
            else
                M_snprintf(label, sizeof(label), "d%d", menu_particles[i].die_type);

            lx = px + 1;
            for (j = 0; label[j] && lx < px + bw - 1; j++)
            {
                int w = M_DrawMenuChar(lx, py + 2, label[j]);
                lx += w + 1;
            }
        }
    }
}




//
// M_DrawMainMenu
//
void M_DrawMainMenu(void)
{
    M_DrawMenuBackground();

    V_DrawFilledBox(40, 4, 240, 20, 0);
    V_DrawFilledBox(40, 4,  240, 1, 160);
    V_DrawFilledBox(40, 23, 240, 1, 160);
    V_DrawGDRStringScaled(44, 7, "GOBLIN DICE ROLLAZ", 224, 1);

    M_WriteText(MainDef.x + 120, MainDef.y + LINEHEIGHT * 7, "Content");
}




//
// M_NewGame
//
void M_DrawNewGame(void)
{
    static const char *skill_labels[] = {
        "1 - TOO YOUNG TO DIE",
        "2 - HEY NOT TOO ROUGH",
        "3 - HURT ME PLENTY",
        "4 - ULTRA-VIOLENCE",
        "6 - NIGHTMARE"
    };
    int i;
    V_DrawGDRStringScaled(NewDef.x, NewDef.y - 24, "NEW GAME", 224, 1);
    V_DrawGDRStringScaled(NewDef.x, NewDef.y - 12, "CHOOSE DIFFICULTY", 160, 1);
    for (i = 0; i < 5; i++)
        M_WriteText(NewDef.x, NewDef.y + i * LINEHEIGHT + 1, skill_labels[i]);
}

void M_NewGame(int choice)
{
    if (netgame && !demoplayback)
    {
	M_StartMessage(DEH_String(NEWGAME),NULL,false);
	return;
    }
	
    // Chex Quest disabled the episode select screen, as did Doom II.

    if (gamemode == commercial || gameversion == exe_chex)
	M_SetupNextMenu(&NewDef);
    else
	M_SetupNextMenu(&EpiDef);
}


void M_SurvivalMode(int choice)
{
    if (netgame && !demoplayback)
    {
	M_StartMessage(DEH_String(NEWGAME),NULL,false);
	return;
    }
    	
    G_InitSurvival();
    G_DeferedInitNew(sk_medium, 1, 1);
    M_ClearMenus();
}


void M_TimeAttackMode(int choice)
{
    if (netgame && !demoplayback)
    {
	M_StartMessage(DEH_String(NEWGAME),NULL,false);
	return;
    }
    	
    G_StartTimeAttack();
    G_DeferedInitNew(sk_medium, 1, 1);
    M_ClearMenus();
}


void M_DrawEpisode(void)
{
    V_DrawPatchDirect(54, 38, W_CacheLumpName(DEH_String("M_EPISOD"), PU_CACHE));
}

void M_VerifyNightmare(int key)
{
    if (key != key_menu_confirm)
	return;
		
    G_DeferedInitNew(nightmare,epi+1,1);
    M_ClearMenus ();
}

void M_ChooseSkill(int choice)
{
    if (choice == nightmare)
    {
	M_StartMessage(DEH_String(NIGHTMARE),M_VerifyNightmare,true);
	return;
    }
	
    G_DeferedInitNew(choice,epi+1,1);
    M_ClearMenus ();
}

void M_Episode(int choice)
{
    if ( (gamemode == shareware)
	 && choice)
    {
	M_StartMessage(DEH_String(SWSTRING),NULL,false);
	M_SetupNextMenu(&ReadDef1);
	return;
    }

    epi = choice;
    M_SetupNextMenu(&NewDef);
}



//
// M_Options
//
static const char *detailNames[2] = {"M_GDHIGH","M_GDLOW"};
static const char *msgNames[2] = {"M_MSGOFF","M_MSGON"};
static const char *rpgModeNames[2] = {"RPG Mode OFF", "RPG Mode ON"};

void M_DrawOptions(void)
{
    V_DrawPatchDirect(108, 15, W_CacheLumpName(DEH_String("M_OPTTTL"),
                                               PU_CACHE));
	
    V_DrawPatchDirect(OptionsDef.x + 175, OptionsDef.y + LINEHEIGHT * detail,
		      W_CacheLumpName(DEH_String(detailNames[detailLevel]),
			              PU_CACHE));

    V_DrawPatchDirect(OptionsDef.x + 120, OptionsDef.y + LINEHEIGHT * messages,
                      W_CacheLumpName(DEH_String(msgNames[showMessages]),
                                      PU_CACHE));

    // Goblin Dice Rollaz: Draw RPG Mode toggle state (text)
    M_WriteText(OptionsDef.x + 120, OptionsDef.y + LINEHEIGHT * 2,
                rpgModeNames[rpg_mode]);

    M_DrawThermo(OptionsDef.x, OptionsDef.y + LINEHEIGHT * (mousesens + 1),
		  10, mouseSensitivity);

    M_DrawThermo(OptionsDef.x,OptionsDef.y+LINEHEIGHT*(scrnsize+1),
		  9,screenSize);
}

void M_Options(int choice)
{
    M_SetupNextMenu(&OptionsDef);
}

//
// M_ContentWarning - Open the content warning menu
//
void M_ContentWarning(int choice)
{
    M_SetupNextMenu(&ContentWarningDef);
}

//
// M_ChangeRegion - Cycle through region options
//
void M_ChangeRegion(int choice)
{
    player_region++;
    if (player_region > 3)
        player_region = 0;
    S_StartSound(NULL, sfx_pstop);
}

//
// M_ShowContentWarning - Display the content warning for current region
//
void M_ShowContentWarning(int choice)
{
    const char* region_name;
    const char* warning_text;
    char warning_msg[1024];
    
    switch(player_region)
    {
    case 0:
        region_name = DEH_String(REGION_NA);
        warning_text = DEH_String(CONTENT_WARN_NA);
        break;
    case 1:
        region_name = DEH_String(REGION_EU);
        warning_text = DEH_String(CONTENT_WARN_EU);
        break;
    case 2:
        region_name = DEH_String(REGION_AU);
        warning_text = DEH_String(CONTENT_WARN_AU);
        break;
    case 3:
    default:
        region_name = DEH_String(REGION_JP);
        warning_text = DEH_String(CONTENT_WARN_JP);
        break;
    }
    
    DEH_snprintf(warning_msg, sizeof(warning_msg), "%s\n\n%s\n\n%s",
                 DEH_String(CONTENT_WARNING_TITLE),
                 region_name,
                 warning_text);
    
    players[consoleplayer].message = warning_msg;
    S_StartSound(NULL, sfx_swtchn);
}

//
// M_AcceptContentWarning - Acknowledge the content warning
//
void M_AcceptContentWarning(int choice)
{
    content_warning_accepted = 1;
    S_StartSound(NULL, sfx_swtchn);
    M_SetupNextMenu(&MainDef);
}

//
// M_DrawContentWarning - Draw the content warning menu
//
void M_DrawContentWarning(void)
{
    V_DrawPatchDirect(72, 15, W_CacheLumpName(DEH_String("M_OPTTTL"),
                                               PU_CACHE));
    
    // Region selection
    M_WriteText(ContentWarningDef.x - 80, ContentWarningDef.y, "Region");
    switch(player_region)
    {
    case 0:
        M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y, "N. America");
        break;
    case 1:
        M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y, "Europe");
        break;
    case 2:
        M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y, "Australia");
        break;
    case 3:
        M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y, "Japan");
        break;
    }
    
    // View warning button
    M_WriteText(ContentWarningDef.x - 80, ContentWarningDef.y + LINEHEIGHT * 1, "View");
    M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y + LINEHEIGHT * 1, "Warning");
    
    // Acknowledge button
    M_WriteText(ContentWarningDef.x - 80, ContentWarningDef.y + LINEHEIGHT * 2, "Accept");
    if (content_warning_accepted)
        M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y + LINEHEIGHT * 2, "YES");
    else
        M_WriteText(ContentWarningDef.x + 120, ContentWarningDef.y + LINEHEIGHT * 2, "NO");
}

//
// M_LevelUp - Open the level up menu
//
void M_LevelUp(int choice)
{
    M_SetupNextMenu(&LevelUpDef);
}

/* GDR STUB: Stat allocation menu callbacks - delegate to M_AllocateStat */
static void M_LevelUpStrength(int choice) { M_AllocateStat(0); }
static void M_LevelUpDexterity(int choice) { M_AllocateStat(1); }
static void M_LevelUpVitality(int choice) { M_AllocateStat(2); }
static void M_LevelUpLuck(int choice) { M_AllocateStat(3); }
static void M_CloseLevelUp(int choice) { M_ClearMenus(); } /* GDR STUB: close level up menu */

//
// M_AllocateStat - Allocate a stat point
//
void M_AllocateStat(int choice)
{
    player_t* player = &players[consoleplayer];

    if (!rpg_mode || !player)
        return;

    if (player->stat_points <= 0)
        return;

    switch(choice)
    {
    case 0:
        G_AllocateStatPoint(player, 0);
        break;
    case 1:
        G_AllocateStatPoint(player, 1);
        break;
    case 2:
        G_AllocateStatPoint(player, 2);
        break;
    case 3:
        G_AllocateStatPoint(player, 3);
        break;
    }
}

//
// M_DrawLevelUp - Draw the level up stat allocation menu
//
void M_DrawLevelUp(void)
{
    player_t* player = &players[consoleplayer];
    char str[32];
    int i;
    static const char* statNames[] = {"STR", "DEX", "VIT", "LUK"};
    static const char* statDesc[] = {"Damage", "Speed", "Health", "Crit %"};

    V_DrawPatchDirect(109, 15, W_CacheLumpName(DEH_String("M_LEVTTL"),
                                               PU_CACHE));

    if (!player)
        return;

    snprintf(str, sizeof(str), "Level %d", player->level);
    M_WriteText(60, 40, str);

    snprintf(str, sizeof(str), "Points: %d", player->stat_points);
    M_WriteText(60, 55, str);

    for (i = 0; i < 4; i++)
    {
        int y = 80 + i * 25;

        M_WriteText(60, y, statNames[i]);

        snprintf(str, sizeof(str), "%d", *((int*)player + 17 + i));
        M_WriteText(180, y, str);

        M_DrawThermo(100, y + 8, 10, *((int*)player + 17 + i));
    }

    M_WriteText(60, 190, "Press ENTER when done");
}

void M_Leaderboard(int choice)
{
    choice = 0;
    M_SetupNextMenu(&LeaderboardDef);
}

void M_CloseLeaderboard(int choice)
{
    choice = 0;
    M_SetupNextMenu(&MainDef);
}

void M_DrawLeaderboard(void)
{
    V_DrawPatchDirect(109, 15, W_CacheLumpName(DEH_String("M_LEADTTL"), PU_CACHE));
    M_WriteText(60, 40, "Select a category");
}

void M_LeaderboardSurvival(int choice)
{
    choice = 0;
    leaderboard_t *lb = G_GetLeaderboard(LEADERBOARD_SURVIVAL);
    char str[64];
    int i;

    DEH_printf("\n=== SURVIVAL LEADERBOARD ===\n");
    if (!lb || lb->num_entries == 0)
    {
        DEH_printf("No entries yet!\n");
    }
    else
    {
        for (i = 0; i < lb->num_entries; i++)
        {
            snprintf(str, sizeof(str), "%d. %s - Wave %d, %d kills (Score: %d)",
                     i + 1, lb->entries[i].player_name,
                     lb->entries[i].wave, lb->entries[i].time,
                     lb->entries[i].score);
            DEH_printf("%s\n", str);
        }
    }
    DEH_printf("============================\n");
}

void M_LeaderboardTimeAttack(int choice)
{
    choice = 0;
    leaderboard_t *lb = G_GetLeaderboard(LEADERBOARD_TIMEATTACK);
    char str[64];
    int i;
    int minutes, seconds;

    DEH_printf("\n=== TIME ATTACK LEADERBOARD ===\n");
    if (!lb || lb->num_entries == 0)
    {
        DEH_printf("No entries yet!\n");
    }
    else
    {
        for (i = 0; i < lb->num_entries; i++)
        {
            minutes = lb->entries[i].time / 3600;
            seconds = (lb->entries[i].time % 3600) / 60;
            snprintf(str, sizeof(str), "%d. %s - %d:%02d (Maps: %d)",
                     i + 1, lb->entries[i].player_name,
                     minutes, seconds, lb->entries[i].wave);
            DEH_printf("%s\n", str);
        }
    }
    DEH_printf("==============================\n");
}

void M_LeaderboardCritOnly(int choice)
{
    choice = 0;
    leaderboard_t *lb = G_GetLeaderboard(LEADERBOARD_CHALLENGE_CRITONLY);
    char str[64];
    int i;

    DEH_printf("\n=== CRIT ONLY CHALLENGE LEADERBOARD ===\n");
    if (!lb || lb->num_entries == 0)
    {
        DEH_printf("No entries yet!\n");
    }
    else
    {
        for (i = 0; i < lb->num_entries; i++)
        {
            snprintf(str, sizeof(str), "%d. %s - Wave %d (Score: %d)",
                     i + 1, lb->entries[i].player_name,
                     lb->entries[i].wave, lb->entries[i].score);
            DEH_printf("%s\n", str);
        }
    }
    DEH_printf("======================================\n");
}

void M_LeaderboardNoPowerups(int choice)
{
    choice = 0;
    leaderboard_t *lb = G_GetLeaderboard(LEADERBOARD_CHALLENGE_NOPOWERUPS);
    char str[64];
    int i;

    DEH_printf("\n=== NO POWERUPS CHALLENGE LEADERBOARD ===\n");
    if (!lb || lb->num_entries == 0)
    {
        DEH_printf("No entries yet!\n");
    }
    else
    {
        for (i = 0; i < lb->num_entries; i++)
        {
            snprintf(str, sizeof(str), "%d. %s - Wave %d (Score: %d)",
                     i + 1, lb->entries[i].player_name,
                     lb->entries[i].wave, lb->entries[i].score);
            DEH_printf("%s\n", str);
        }
    }
    DEH_printf("=========================================\n");
}

void M_LeaderboardHardcore(int choice)
{
    choice = 0;
    leaderboard_t *lb = G_GetLeaderboard(LEADERBOARD_CHALLENGE_HARDCORE);
    char str[64];
    int i;

    DEH_printf("\n=== HARDCORE CHALLENGE LEADERBOARD ===\n");
    if (!lb || lb->num_entries == 0)
    {
        DEH_printf("No entries yet!\n");
    }
    else
    {
        for (i = 0; i < lb->num_entries; i++)
        {
            snprintf(str, sizeof(str), "%d. %s - Wave %d (Score: %d)",
                     i + 1, lb->entries[i].player_name,
                     lb->entries[i].wave, lb->entries[i].score);
            DEH_printf("%s\n", str);
        }
    }
    DEH_printf("=======================================\n");
}

void M_ClearLeaderboards(int choice)
{
    choice = 0;
    G_ClearAllLeaderboards();
    players[consoleplayer].message = "Leaderboards cleared";
}




//
//      Toggle messages on/off
//
void M_ChangeMessages(int choice)
{
    // warning: unused parameter `int choice'
    choice = 0;
    showMessages = 1 - showMessages;
	
    if (!showMessages)
	players[consoleplayer].message = DEH_String(MSGOFF);
    else
	players[consoleplayer].message = DEH_String(MSGON);

    message_dontfuckwithme = true;
}


//
//      Toggle RPG Progression Mode on/off
//
void M_ChangeRPGMode(int choice)
{
    choice = 0;
    rpg_mode = 1 - rpg_mode;
	
    if (!rpg_mode)
	players[consoleplayer].message = "RPG Mode OFF";
    else
	players[consoleplayer].message = "RPG Mode ON";

    message_dontfuckwithme = true;
}


//
// M_EndGame
//
void M_EndGameResponse(int key)
{
    if (key != key_menu_confirm)
	return;
		
    currentMenu->lastOn = itemOn;
    M_ClearMenus ();
    D_StartTitle ();
}

void M_EndGame(int choice)
{
    choice = 0;
    if (!usergame)
    {
	S_StartSound(NULL,sfx_oof);
	return;
    }
	
    if (netgame)
    {
	M_StartMessage(DEH_String(NETEND),NULL,false);
	return;
    }
	
    M_StartMessage(DEH_String(ENDGAME),M_EndGameResponse,true);
}




//
// M_ReadThis
//
void M_ReadThis(int choice)
{
    choice = 0;
    M_SetupNextMenu(&ReadDef1);
}

void M_ReadThis2(int choice)
{
    choice = 0;
    M_SetupNextMenu(&ReadDef2);
}

void M_FinishReadThis(int choice)
{
    choice = 0;
    M_SetupNextMenu(&MainDef);
}




//
// M_QuitDOOM
//
int     quitsounds[8] =
{
    sfx_pldeth,
    sfx_dmpain,
    sfx_popain,
    sfx_slop,
    sfx_telept,
    sfx_posit1,
    sfx_posit3,
    sfx_sgtatk
};

int     quitsounds2[8] =
{
    sfx_vilact,
    sfx_getpow,
    sfx_boscub,
    sfx_slop,
    sfx_skeswg,
    sfx_kntdth,
    sfx_bspact,
    sfx_sgtatk
};



void M_QuitResponse(int key)
{
    if (key != key_menu_confirm)
	return;
    if (!netgame)
    {
	if (gamemode == commercial)
	    S_StartSound(NULL,quitsounds2[(gametic>>2)&7]);
	else
	    S_StartSound(NULL,quitsounds[(gametic>>2)&7]);
	I_WaitVBL(105);
    }
    I_Quit ();
}


static const char *M_SelectEndMessage(void)
{
    const char **endmsg;

    if (logical_gamemission == doom)
    {
        // Doom 1

        endmsg = doom1_endmsg;
    }
    else
    {
        // Doom 2
        
        endmsg = doom2_endmsg;
    }

    return endmsg[gametic % NUM_QUITMESSAGES];
}


void M_QuitDOOM(int choice)
{
    DEH_snprintf(endstring, sizeof(endstring), "%s\n\n" DOSY,
                 DEH_String(M_SelectEndMessage()));

    M_StartMessage(endstring,M_QuitResponse,true);
}




void M_ChangeSensitivity(int choice)
{
    switch(choice)
    {
      case 0:
	if (mouseSensitivity)
	    mouseSensitivity--;
	break;
      case 1:
	if (mouseSensitivity < 9)
	    mouseSensitivity++;
	break;
    }
}




void M_ChangeDetail(int choice)
{
    choice = 0;
    // Goblin Dice Rollaz: detail level removed - always high detail
    players[consoleplayer].message = DEH_String("High Detail");
}




void M_SizeDisplay(int choice)
{
    switch(choice)
    {
      case 0:
	if (screenSize > 0)
	{
	    screenblocks--;
	    screenSize--;
	}
	break;
      case 1:
	if (screenSize < 8)
	{
	    screenblocks++;
	    screenSize++;
	}
	break;
    }
	

    R_SetViewSize (screenblocks, 0);
}




//
//      Menu Functions
//
void
M_DrawThermo
( int	x,
  int	y,
  int	thermWidth,
  int	thermDot )
{
    int barW = thermWidth * 8;
    // Background
    V_DrawFilledBox(x, y, barW, 8, 0);
    // Filled portion
    V_DrawFilledBox(x, y, thermDot * 8, 8, 200);
    // Border
    V_DrawFilledBox(x, y,         barW, 1, 224);
    V_DrawFilledBox(x, y + 7,     barW, 1, 224);
    V_DrawFilledBox(x, y,         1,    8, 224);
    V_DrawFilledBox(x + barW - 1, y,    1, 8,  224);
}


void
M_StartMessage
( const char	*string,
  void*		routine,
  boolean	input )
{
    messageLastMenuActive = menuactive;
    messageToPrint = 1;
    messageString = string;
    messageRoutine = routine;
    messageNeedsInput = input;
    menuactive = true;
    return;
}


//
// Find string width from hu_font chars
//
int M_StringWidth(const char *string)
{
    size_t             i;
    int             w = 0;
    int             c;
	
    for (i = 0;i < strlen(string);i++)
    {
	c = toupper(string[i]) - HU_FONTSTART;
	if (c < 0 || c >= HU_FONTSIZE)
	    w += 4;
	else
	    w += SHORT (hu_font[c]->width);
    }
		
    return w;
}



//
//      Find string height from hu_font chars
//
int M_StringHeight(const char* string)
{
    size_t             i;
    int             h;
    int             height = SHORT(hu_font[0]->height);
	
    h = height;
    for (i = 0;i < strlen(string);i++)
	if (string[i] == '\n')
	    h += height;
		
    return h;
}


//
//      Write a string using the hu_font
//
void
M_WriteText
( int		x,
  int		y,
  const char *string)
{
    int		w;
    const char *ch;
    int		c;
    int		cx;
    int		cy;
		

    ch = string;
    cx = x;
    cy = y;
	
    while(1)
    {
	c = *ch++;
	if (!c)
	    break;
	if (c == '\n')
	{
	    cx = x;
	    cy += 12;
	    continue;
	}
		
	c = toupper(c) - HU_FONTSTART;
	if (c < 0 || c>= HU_FONTSIZE)
	{
	    cx += 4;
	    continue;
	}
		
	w = SHORT (hu_font[c]->width);
	if (cx+w > SCREENWIDTH)
	    break;
	V_DrawPatchDirect(cx, cy, hu_font[c]);
	cx+=w;
    }
}

// These keys evaluate to a "null" key in Vanilla Doom that allows weird
// jumping in the menus. Preserve this behavior for accuracy.

static boolean IsNullKey(int key)
{
    return key == KEY_PAUSE || key == KEY_CAPSLOCK
        || key == KEY_SCRLCK || key == KEY_NUMLOCK;
}

//
// CONTROL PANEL
//

//
// M_Responder
//
boolean M_Responder (event_t* ev)
{
    int             ch;
    int             key;
    int             i;
    static  int     mousewait = 0;
    static  int     mousey = 0;
    static  int     lasty = 0;
    static  int     mousex = 0;
    static  int     lastx = 0;
    int dir;

    // In testcontrols mode, none of the function keys should do anything
    // - the only key is escape to quit.

    if (testcontrols)
    {
        if (ev->type == ev_quit
         || (ev->type == ev_keydown
          && (ev->data1 == key_menu_activate || ev->data1 == key_menu_quit)))
        {
            I_Quit();
            return true;
        }

        return false;
    }

    // "close" button pressed on window?
    if (ev->type == ev_quit)
    {
        // First click on close button = bring up quit confirm message.
        // Second click on close button = confirm quit

        if (menuactive && messageToPrint && messageRoutine == M_QuitResponse)
        {
            M_QuitResponse(key_menu_confirm);
        }
        else
        {
            S_StartSound(NULL,sfx_swtchn);
            M_QuitDOOM(0);
        }

        return true;
    }

    // key is the key pressed, ch is the actual character typed
  
    ch = 0;
    key = -1;
	
    if (ev->type == ev_joystick)
    {
        // Simulate key presses from joystick events to interact with the menu.

        if (menuactive)
        {
            if (JOY_GET_DPAD(ev->data6) != JOY_DIR_NONE)
            {
                dir = JOY_GET_DPAD(ev->data6);
            }
            else if (JOY_GET_LSTICK(ev->data6) != JOY_DIR_NONE)
            {
                dir = JOY_GET_LSTICK(ev->data6);
            }
            else
            {
                dir = JOY_GET_RSTICK(ev->data6);
            }

            if (dir & JOY_DIR_UP)
            {
                key = key_menu_up;
                joywait = I_GetTime() + 5;
            }
            else if (dir & JOY_DIR_DOWN)
            {
                key = key_menu_down;
                joywait = I_GetTime() + 5;
            }
            if (dir & JOY_DIR_LEFT)
            {
                key = key_menu_left;
                joywait = I_GetTime() + 5;
            }
            else if (dir & JOY_DIR_RIGHT)
            {
                key = key_menu_right;
                joywait = I_GetTime() + 5;
            }

#define JOY_BUTTON_MAPPED(x) ((x) >= 0)
#define JOY_BUTTON_PRESSED(x) (JOY_BUTTON_MAPPED(x) && (ev->data1 & (1 << (x))) != 0)

            if (JOY_BUTTON_PRESSED(joybfire))
            {
                // Simulate a 'Y' keypress when Doom show a Y/N dialog with Fire button.
                if (messageToPrint && messageNeedsInput)
                {
                    key = key_menu_confirm;
                }
                // Simulate pressing "Enter" when we are supplying a save slot name
                else if (saveStringEnter)
                {
                    key = KEY_ENTER;
                }
                else
                {
                    // if selecting a save slot via joypad, set a flag
                    if (currentMenu == &SaveDef)
                    {
                        joypadSave = true;
                    }
                    key = key_menu_forward;
                }
                joywait = I_GetTime() + 5;
            }
            if (JOY_BUTTON_PRESSED(joybuse))
            {
                // Simulate a 'N' keypress when Doom show a Y/N dialog with Use button.
                if (messageToPrint && messageNeedsInput)
                {
                    key = key_menu_abort;
                }
                // If user was entering a save name, back out
                else if (saveStringEnter)
                {
                    key = KEY_ESCAPE;
                }
                else
                {
                    key = key_menu_back;
                }
                joywait = I_GetTime() + 5;
            }
        }
        if (JOY_BUTTON_PRESSED(joybmenu))
        {
            key = key_menu_activate;
            joywait = I_GetTime() + 5;
        }
    }
    else
    {
	if (ev->type == ev_mouse && mousewait < I_GetTime() && menuactive)
	{
	    mousey += ev->data3;
	    if (mousey < lasty-30)
	    {
		key = key_menu_down;
		mousewait = I_GetTime() + 5;
		mousey = lasty -= 30;
	    }
	    else if (mousey > lasty+30)
	    {
		key = key_menu_up;
		mousewait = I_GetTime() + 5;
		mousey = lasty += 30;
	    }
		
	    mousex += ev->data2;
	    if (mousex < lastx-30)
	    {
		key = key_menu_left;
		mousewait = I_GetTime() + 5;
		mousex = lastx -= 30;
	    }
	    else if (mousex > lastx+30)
	    {
		key = key_menu_right;
		mousewait = I_GetTime() + 5;
		mousex = lastx += 30;
	    }
		
	    if (ev->data1&1)
	    {
		key = key_menu_forward;
		mousewait = I_GetTime() + 15;
	    }
			
	    if (ev->data1&2)
	    {
		key = key_menu_back;
		mousewait = I_GetTime() + 15;
	    }
	}
	else
	{
	    if (ev->type == ev_keydown)
	    {
		key = ev->data1;
		ch = ev->data2;
	    }
	}
    }
    
    if (key == -1)
	return false;

    // Save Game string input
    if (saveStringEnter)
    {
	switch(key)
	{
	  case KEY_BACKSPACE:
	    if (editingComment)
	    {
		if (saveCharIndex > 0)
		{
		    saveCharIndex--;
		    savecomments[saveSlot][saveCharIndex] = 0;
		}
	    }
	    else
	    {
		if (saveCharIndex > 0)
		{
		    saveCharIndex--;
		    savegamestrings[saveSlot][saveCharIndex] = 0;
		}
	    }
	    break;

          case KEY_ESCAPE:
            saveStringEnter = 0;
            editingComment = false;
            I_StopTextInput();
            M_StringCopy(savegamestrings[saveSlot], saveOldString,
                         SAVESTRINGSIZE);
            M_StringCopy(savecomments[saveSlot], saveOldComment,
                         SAVESTRINGSIZE);
            break;

	  case KEY_ENTER:
	    saveStringEnter = 0;
            editingComment = false;
            I_StopTextInput();
	    if (savegamestrings[saveSlot][0])
		M_DoSave(saveSlot);
	    break;

	  case KEY_TAB:
	    editingComment = !editingComment;
	    if (editingComment)
		saveCharIndex = strlen(savecomments[saveSlot]);
	    else
		saveCharIndex = strlen(savegamestrings[saveSlot]);
	    break;

	  default:
            // Savegame name entry using native keyboard mapping.
            if (ev->type != ev_keydown)
            {
                break;
            }
            ch = ev->data3;

            ch = toupper(ch);

            if (ch != ' '
             && (ch - HU_FONTSTART < 0 || ch - HU_FONTSTART >= HU_FONTSIZE))
            {
                break;
            }

	    if (ch >= 32 && ch <= 127 &&
		saveCharIndex < SAVESTRINGSIZE-1)
	    {
		if (editingComment)
		{
		    if (M_StringWidth(savecomments[saveSlot]) <
			(SAVESTRINGSIZE-2)*8)
		    {
			savecomments[saveSlot][saveCharIndex++] = ch;
			savecomments[saveSlot][saveCharIndex] = 0;
		    }
		}
		else
		{
		    if (M_StringWidth(savegamestrings[saveSlot]) <
			(SAVESTRINGSIZE-2)*8)
		    {
			savegamestrings[saveSlot][saveCharIndex++] = ch;
			savegamestrings[saveSlot][saveCharIndex] = 0;
		    }
		}
	    }
	    break;
	}
	return true;
    }

    // Take care of any messages that need input
    if (messageToPrint)
    {
	if (messageNeedsInput)
        {
            if (key != ' ' && key != KEY_ESCAPE
             && key != key_menu_confirm && key != key_menu_abort)
            {
                return false;
            }
	}

	menuactive = messageLastMenuActive;
	messageToPrint = 0;
	if (messageRoutine)
	    messageRoutine(key);

	menuactive = false;
	S_StartSound(NULL,sfx_swtchx);
	return true;
    }

    if ((devparm && key == key_menu_help) ||
        (key != 0 && key == key_menu_screenshot))
    {
	G_ScreenShot ();
	return true;
    }

    // F-Keys
    if (!menuactive)
    {
	if (key == key_menu_decscreen)      // Screen size down
        {
	    if (automapactive || chat_on)
		return false;
	    M_SizeDisplay(0);
	    S_StartSound(NULL,sfx_stnmov);
	    return true;
	}
        else if (key == key_menu_incscreen) // Screen size up
        {
	    if (automapactive || chat_on)
		return false;
	    M_SizeDisplay(1);
	    S_StartSound(NULL,sfx_stnmov);
	    return true;
	}
        else if (key == key_menu_help)     // Help key
        {
	    M_StartControlPanel ();

	    currentMenu = &ReadDef2;

	    itemOn = 0;
	    S_StartSound(NULL,sfx_swtchn);
	    return true;
	}
        else if (key == key_menu_save)     // Save
        {
	    M_StartControlPanel();
	    S_StartSound(NULL,sfx_swtchn);
	    M_SaveGame(0);
	    return true;
        }
        else if (key == key_menu_load)     // Load
        {
	    M_StartControlPanel();
	    S_StartSound(NULL,sfx_swtchn);
	    M_LoadGame(0);
	    return true;
        }
        else if (key == key_menu_volume)   // Sound Volume
        {
	    M_StartControlPanel ();
	    currentMenu = &SoundDef;
	    itemOn = sfx_vol;
	    S_StartSound(NULL,sfx_swtchn);
	    return true;
	}
        else if (key == key_menu_detail)   // Detail toggle
        {
	    M_ChangeDetail(0);
	    S_StartSound(NULL,sfx_swtchn);
	    return true;
        }
        else if (key == key_menu_qsave)    // Quicksave
        {
	    S_StartSound(NULL,sfx_swtchn);
	    M_QuickSave();
	    return true;
        }
        else if (key == key_menu_qsave2)   // Alternate Quicksave (slot 2)
        {
	    S_StartSound(NULL,sfx_swtchn);
	    M_QuickSave2();
	    return true;
        }
        else if (key == key_menu_endgame)  // End game
        {
	    S_StartSound(NULL,sfx_swtchn);
	    M_EndGame(0);
	    return true;
        }
        else if (key == key_menu_messages) // Toggle messages
        {
	    M_ChangeMessages(0);
	    S_StartSound(NULL,sfx_swtchn);
	    return true;
        }
        else if (key == key_menu_qload)    // Quickload
        {
	    S_StartSound(NULL,sfx_swtchn);
	    M_QuickLoad();
	    return true;
        }
        else if (key == key_menu_qload2)   // Alternate Quickload (slot 2)
        {
	    S_StartSound(NULL,sfx_swtchn);
	    M_QuickLoad2();
	    return true;
        }
        else if (key == key_menu_quit)     // Quit Game
        {
	    S_StartSound(NULL,sfx_swtchn);
	    M_QuitDOOM(0);
	    return true;
        }
        else if (key == key_menu_gamma)    // gamma toggle
        {
	    usegamma++;
	    if (usegamma > 4)
		usegamma = 0;
	    players[consoleplayer].message = DEH_String(gammamsg[usegamma]);
            I_SetPalette (W_CacheLumpName (DEH_String("PLAYPAL"),PU_CACHE));
	    return true;
	}
        else if (key == key_menu_hud)    // Goblin Dice Rollaz: HUD toggle
        {
	    ST_ToggleHUD();
	    S_StartSound(NULL,sfx_swtchn);
	    return true;
        }
    }

    // Pop-up menu?
    if (!menuactive)
    {
	if (key == key_menu_activate)
	{
	    M_StartControlPanel ();
	    S_StartSound(NULL,sfx_swtchn);
	    return true;
	}
	return false;
    }

    // Keys usable within menu

    if (key == key_menu_down)
    {
        // Move down to next item

        do
	{
	    if (itemOn+1 > currentMenu->numitems-1)
		itemOn = 0;
	    else itemOn++;
	    S_StartSound(NULL,sfx_pstop);
	} while(currentMenu->menuitems[itemOn].status==-1);

	M_AnnounceMenuItem();
	return true;
    }
    else if (key == key_menu_up)
    {
        // Move back up to previous item

	do
	{
	    if (!itemOn)
		itemOn = currentMenu->numitems-1;
	    else itemOn--;
	    S_StartSound(NULL,sfx_pstop);
	} while(currentMenu->menuitems[itemOn].status==-1);

	M_AnnounceMenuItem();
	return true;
    }
    else if (key == key_menu_left)
    {
        // Slide slider left

	if (currentMenu->menuitems[itemOn].routine &&
	    currentMenu->menuitems[itemOn].status == 2)
	{
	    S_StartSound(NULL,sfx_stnmov);
	    currentMenu->menuitems[itemOn].routine(0);
	}
	return true;
    }
    else if (key == key_menu_right)
    {
        // Slide slider right

	if (currentMenu->menuitems[itemOn].routine &&
	    currentMenu->menuitems[itemOn].status == 2)
	{
	    S_StartSound(NULL,sfx_stnmov);
	    currentMenu->menuitems[itemOn].routine(1);
	}
	return true;
    }
    else if (key == key_menu_forward)
    {
        // Activate menu item

	if (currentMenu->menuitems[itemOn].routine &&
	    currentMenu->menuitems[itemOn].status)
	{
	    currentMenu->lastOn = itemOn;
	    if (currentMenu->menuitems[itemOn].status == 2)
	    {
		currentMenu->menuitems[itemOn].routine(1);      // right arrow
		S_StartSound(NULL,sfx_stnmov);
	    }
	    else
	    {
		currentMenu->menuitems[itemOn].routine(itemOn);
		S_StartSound(NULL,sfx_pistol);
	    }
	}
	return true;
    }
    else if (key == key_menu_activate)
    {
        // Deactivate menu

	currentMenu->lastOn = itemOn;
	M_ClearMenus ();
	S_StartSound(NULL,sfx_swtchx);
	return true;
    }
    else if (key == key_menu_back)
    {
        // Go back to previous menu

	currentMenu->lastOn = itemOn;
	if (currentMenu->prevMenu)
	{
	    currentMenu = currentMenu->prevMenu;
	    itemOn = currentMenu->lastOn;
	    S_StartSound(NULL,sfx_swtchn);
	}
	return true;
    }

    // Keyboard shortcut?
    // Vanilla Doom has a weird behavior where it jumps to the scroll bars
    // when the certain keys are pressed, so emulate this.

    else if (ch != 0 || IsNullKey(key))
    {
	for (i = itemOn+1;i < currentMenu->numitems;i++)
        {
	    if (currentMenu->menuitems[i].alphaKey == ch)
	    {
		itemOn = i;
		S_StartSound(NULL,sfx_pstop);
		return true;
	    }
        }

	for (i = 0;i <= itemOn;i++)
        {
	    if (currentMenu->menuitems[i].alphaKey == ch)
	    {
		itemOn = i;
		S_StartSound(NULL,sfx_pstop);
		return true;
	    }
        }
    }

    return false;
}



//
// M_StartControlPanel
//
void M_StartControlPanel (void)
{
    // intro might call this repeatedly
    if (menuactive)
	return;
    
    menuactive = 1;
    currentMenu = &MainDef;         // JDC
    itemOn = currentMenu->lastOn;   // JDC
    M_AnnounceMenuItem();
}

// Display OPL debug messages - hack for GENMIDI development.

static void M_DrawOPLDev(void)
{
    char debug[1024];
    char *curr, *p;
    int line;

    I_OPL_DevMessages(debug, sizeof(debug));
    curr = debug;
    line = 0;

    for (;;)
    {
        p = strchr(curr, '\n');

        if (p != NULL)
        {
            *p = '\0';
        }

        M_WriteText(0, line * 8, curr);
        ++line;

        if (p == NULL)
        {
            break;
        }

        curr = p + 1;
    }
}

//
// M_Drawer
// Called after the view has been rendered,
// but before it has been blitted.
//
void M_Drawer (void)
{
    static short	x;
    static short	y;
    unsigned int	i;
    unsigned int	max;
    char		string[80];
    const char          *name;
    int			start;

    inhelpscreens = false;
    
    // Horiz. & Vertically center string and print it.
    if (messageToPrint)
    {
	start = 0;
	y = SCREENHEIGHT/2 - M_StringHeight(messageString) / 2;
	while (messageString[start] != '\0')
	{
	    boolean foundnewline = false;

            for (i = 0; messageString[start + i] != '\0'; i++)
            {
                if (messageString[start + i] == '\n')
                {
                    M_StringCopy(string, messageString + start,
                                 sizeof(string));
                    if (i < sizeof(string))
                    {
                        string[i] = '\0';
                    }

                    foundnewline = true;
                    start += i + 1;
                    break;
                }
            }

            if (!foundnewline)
            {
                M_StringCopy(string, messageString + start, sizeof(string));
                start += strlen(string);
            }

	    x = SCREENWIDTH/2 - M_StringWidth(string) / 2;
	    M_WriteText(x, y, string);
	    y += SHORT(hu_font[0]->height);
	}

	return;
    }

    if (opldev)
    {
        M_DrawOPLDev();
    }

    if (!menuactive)
	return;

    if (currentMenu->routine)
	currentMenu->routine();         // call Draw routine
    
    // DRAW MENU
    x = currentMenu->x;
    y = currentMenu->y;
    max = currentMenu->numitems;

    for (i=0;i<max;i++)
    {
        name = DEH_String(currentMenu->menuitems[i].name);

	if (name[0] && W_CheckNumForName(name) >= 0)
	{
	    V_DrawPatchDirect (x, y, W_CacheLumpName(name, PU_CACHE));
	}
	else if (name[0])
	{
	    // Lump missing — look up a proper display name, fall back to lump name
	    static const struct { const char *lump; const char *label; } lump_labels[] = {
	        { "M_NGAME",  "New Game"       },
	        { "M_SURVIV", "Survival"       },
	        { "M_TIMATT", "Time Attack"    },
	        { "M_LEADER", "Leaderboard"    },
	        { "M_OPTION", "Options"        },
	        { "M_GAMESET","Game Settings"  },
	        { "M_ACCESS", "Accessibility"  },
	        { "M_LOADG",  "Load Game"      },
	        { "M_SAVEG",  "Save Game"      },
	        { "M_RDTHIS", "Read This!"     },
	        { "M_QUITG",  "Quit Game"      },
	        { "M_CUSTM",  "Customize"      },
	        { "M_MSGS",   "Messages"       },
	        { "M_SNDvol", "Sound Volume"   },
	        { "M_SENS",   "Mouse Sensitivity" },
	        { "M_QUICK",  "Quick Save"     },
	        { "M_ENDGC",  "End Game"       },
	        { NULL, NULL }
	    };
	    const char *label = NULL;
	    int li;
	    for (li = 0; lump_labels[li].lump; li++)
	    {
	        if (!strcasecmp(name, lump_labels[li].lump))
	        {
	            label = lump_labels[li].label;
	            break;
	        }
	    }
	    if (!label) label = name;
	    M_WriteText(x, y + 1, label);
	    fprintf(stderr, "[STUB] Menu lump missing: %s — showing '%s'\n", name, label);
	}
	y += LINEHEIGHT;
    }

    
    // DRAW DICE CURSOR (replaces skull)
    {
        static int dice_face = 1;
        static int dice_timer = 0;
        int cx = currentMenu->x + SKULLXOFF;
        int cy = currentMenu->y - 4 + itemOn * LINEHEIGHT;
        int sz = 16;
        int dot = sz / 5;
        int dr, dc;
        // Animate: cycle face 1-6 every 8 ticks
        if (++dice_timer >= 8) { dice_timer = 0; if (++dice_face > 6) dice_face = 1; }
        // Die body
        V_DrawFilledBox(cx, cy, sz, sz, 4);
        // Die border
        V_DrawFilledBox(cx,        cy,        sz, 1, 0);
        V_DrawFilledBox(cx,        cy + sz-1, sz, 1, 0);
        V_DrawFilledBox(cx,        cy,        1, sz, 0);
        V_DrawFilledBox(cx + sz-1, cy,        1, sz, 0);
        // Draw pips based on face value
        #define PIP(px, py) V_DrawFilledBox(cx+(px), cy+(py), dot, dot, 0)
        switch (dice_face) {
            case 1: PIP(6, 6); break;
            case 2: PIP(2, 2); PIP(9, 9); break;
            case 3: PIP(2, 2); PIP(6, 6); PIP(9, 9); break;
            case 4: PIP(2, 2); PIP(9, 2); PIP(2, 9); PIP(9, 9); break;
            case 5: PIP(2, 2); PIP(9, 2); PIP(6, 6); PIP(2, 9); PIP(9, 9); break;
            case 6: PIP(2, 2); PIP(9, 2); PIP(2, 6); PIP(9, 6); PIP(2, 10); PIP(9, 10); break;
        }
        #undef PIP
    }
}


//
// M_ClearMenus
//
void M_ClearMenus (void)
{
    menuactive = 0;
    // if (!netgame && usergame && paused)
    //       sendpause = true;
}




//
// M_SetupNextMenu
//
void M_SetupNextMenu(menu_t *menudef)
{
    currentMenu = menudef;
    itemOn = currentMenu->lastOn;
    M_AnnounceMenuItem();
}


//
// M_Ticker
//
void M_Ticker (void)
{
    if (--skullAnimCounter <= 0)
    {
	whichSkull ^= 1;
	skullAnimCounter = 8;
    }
}


//
// M_Init
//
void M_Init (void)
{
    currentMenu = &MainDef;
    menuactive = 0;
    itemOn = currentMenu->lastOn;
    whichSkull = 0;
    skullAnimCounter = 10;
    screenSize = screenblocks - 3;
    messageToPrint = 0;
    messageString = NULL;
    messageLastMenuActive = menuactive;
    quickSaveSlot = -1;
    quickSaveSlot2 = -1;

    // Here we could catch other version dependencies,
    //  like HELP1/2, and four episodes.

    // The same hacks were used in the original Doom EXEs.

    MainMenu[readthis].routine = M_ReadThis2;
    ReadDef2.prevMenu = NULL;

    if (gameversion >= exe_final && gameversion <= exe_final2)
    {
        ReadDef2.routine = M_DrawReadThisCommercial;
    }

    if (gamemode == commercial)
    {
        MainMenu[readthis] = MainMenu[quitdoom];
        MainDef.numitems--;
        MainDef.y += 8;
        NewDef.prevMenu = &MainDef;
        ReadDef1.routine = M_DrawReadThisCommercial;
        ReadDef1.x = 330;
        ReadDef1.y = 165;
        ReadMenu1[rdthsempty1].routine = M_FinishReadThis;
    }

    // Versions of doom.exe before the Ultimate Doom release only had
    // three episodes; if we're emulating one of those then don't try
    // to show episode four. If we are, then do show episode four
    // (should crash if missing).
    if (gameversion < exe_ultimate)
    {
        EpiDef.numitems--;
    }
    // chex.exe shows only one episode.
    else if (gameversion == exe_chex)
    {
        EpiDef.numitems = 1;
    }

    opldev = M_CheckParm("-opldev") > 0;
}

