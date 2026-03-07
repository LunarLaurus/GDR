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
//     Common code to parse command line, identifying WAD files to load.
//

#include <stdlib.h>
#include <stdarg.h>

#include "config.h"
#include "d_iwad.h"
#include "i_glob.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "w_main.h"
#include "w_merge.h"
#include "w_wad.h"
#include "z_zone.h"

// Parse the command line, merging WAD files that are sppecified.
// Returns true if at least one file was added.
boolean W_ParseCommandLine(void)
{
    boolean modifiedgame = false;
    int p;

    // Merged PWADs are loaded first, because they are supposed to be 
    // modified IWADs.

    //!
    // @arg <files>
    // @category mod
    //
    // Simulates the behavior of deutex's -merge option, merging a PWAD
    // into the main IWAD.  Multiple files may be specified.
    //

    p = M_CheckParmWithArgs("-merge", 1);

    if (p > 0)
    {
        for (p = p + 1; p<myargc && myargv[p][0] != '-'; ++p)
        {
            char *filename;

            modifiedgame = true;

            filename = D_TryFindWADByName(myargv[p]);

            printf(" merging %s\n", filename);
            W_MergeFile(filename);
            free(filename);
        }
    }

    // NWT-style merging:

    // NWT's -merge option:

    //!
    // @arg <files>
    // @category mod
    //
    // Simulates the behavior of NWT's -merge option.  Multiple files
    // may be specified.

    p = M_CheckParmWithArgs("-nwtmerge", 1);

    if (p > 0)
    {
        for (p = p + 1; p<myargc && myargv[p][0] != '-'; ++p)
        {
            char *filename;

            modifiedgame = true;

            filename = D_TryFindWADByName(myargv[p]);

            printf(" performing NWT-style merge of %s\n", filename);
            W_NWTDashMerge(filename);
            free(filename);
        }
    }
    
    // Add flats

    //!
    // @arg <files>
    // @category mod
    //
    // Simulates the behavior of NWT's -af option, merging flats into
    // the main IWAD directory.  Multiple files may be specified.
    //

    p = M_CheckParmWithArgs("-af", 1);

    if (p > 0)
    {
        for (p = p + 1; p<myargc && myargv[p][0] != '-'; ++p)
        {
            char *filename;

            modifiedgame = true;

            filename = D_TryFindWADByName(myargv[p]);

            printf(" merging flats from %s\n", filename);
            W_NWTMergeFile(filename, W_NWT_MERGE_FLATS);
            free(filename);
        }
    }

    //!
    // @arg <files>
    // @category mod
    //
    // Simulates the behavior of NWT's -as option, merging sprites
    // into the main IWAD directory.  Multiple files may be specified.
    //

    p = M_CheckParmWithArgs("-as", 1);

    if (p > 0)
    {
        for (p = p + 1; p<myargc && myargv[p][0] != '-'; ++p)
        {
            char *filename;

            modifiedgame = true;
            filename = D_TryFindWADByName(myargv[p]);

            printf(" merging sprites from %s\n", filename);
            W_NWTMergeFile(filename, W_NWT_MERGE_SPRITES);
            free(filename);
        }
    }

    //!
    // @arg <files>
    // @category mod
    //
    // Equivalent to "-af <files> -as <files>".
    //

    p = M_CheckParmWithArgs("-aa", 1);

    if (p > 0)
    {
        for (p = p + 1; p<myargc && myargv[p][0] != '-'; ++p)
        {
            char *filename;

            modifiedgame = true;

            filename = D_TryFindWADByName(myargv[p]);

            printf(" merging sprites and flats from %s\n", filename);
            W_NWTMergeFile(filename, W_NWT_MERGE_SPRITES | W_NWT_MERGE_FLATS);
            free(filename);
        }
    }

    //!
    // @arg <files>
    // @vanilla
    //
    // Load the specified PWAD files.  Each succeeding argument is
    // treated as a PWAD file name until one starts with a dash or the
    // argument list is exhausted.
    //

    p = M_CheckParmWithArgs ("-file", 1);
    if (p)
    {
	// the parms after p are wadfile/lump names,
	// until end of parms or another - preceded parm
	modifiedgame = true;            // homebrew levels
	while (++p != myargc && myargv[p][0] != '-')
        {
            char *filename;

            filename = D_TryFindWADByName(myargv[p]);

            printf(" adding %s\n", filename);
	    W_AddFile(filename);
            W_IncrementWadFileCount();
            free(filename);
        }
    }

//    W_PrintDirectory();

    return modifiedgame;
}

// Load all WAD files from the given directory.
void W_AutoLoadWADs(const char *path)
{
    glob_t *glob;
    const char *filename;

    glob = I_StartMultiGlob(path, GLOB_FLAG_NOCASE|GLOB_FLAG_SORTED,
                            "*.wad", "*.lmp", NULL);
    for (;;)
    {
        filename = I_NextGlob(glob);
        if (filename == NULL)
        {
            break;
        }
        printf(" [autoload] merging %s\n", filename);
        W_MergeFile(filename);
    }

    I_EndGlob(glob);
}

// Lump names that are unique to particular game types. This lets us check
// the user is not trying to play with the wrong executable, eg.
// chocolate-doom -iwad hexen.wad.
static const struct
{
    GameMission_t mission;
    const char *lumpname;
} unique_lumps[] = {
    { doom,    "POSSA1" },
    { heretic, "IMPXA1" },
    { hexen,   "ETTNA1" },
    { strife,  "AGRDA1" },
};

void W_CheckCorrectIWAD(GameMission_t mission)
{
    int i;
    lumpindex_t lumpnum;

    for (i = 0; i < arrlen(unique_lumps); ++i)
    {
        if (mission != unique_lumps[i].mission)
        {
            lumpnum = W_CheckNumForName(unique_lumps[i].lumpname);

            if (lumpnum >= 0)
            {
                I_Error("\nYou are trying to use a %s IWAD file with "
                        "the %s%s binary.\nThis isn't going to work.\n"
                        "You probably want to use the %s%s binary.",
                        D_SuggestGameName(unique_lumps[i].mission,
                                          indetermined),
                        PROGRAM_PREFIX,
                        D_GameMissionString(mission),
                        PROGRAM_PREFIX,
                        D_GameMissionString(unique_lumps[i].mission));
            }
        }
    }
}

// Load WAD files from the mod_load_order config variable.
// This is called after W_ParseCommandLine to load mods specified in config.
void W_LoadModsFromConfig(void)
{
    char *mod_list;
    char *mod_copy;
    char *token;
    boolean modifiedgame = false;

    mod_list = M_GetStringVariable("goblin_mod_load_order");
    if (mod_list == NULL || *mod_list == '\0')
    {
        return;
    }

    mod_copy = M_StringDuplicate(mod_list);
    if (mod_copy == NULL)
    {
        return;
    }

    token = strtok(mod_copy, ",");
    while (token != NULL)
    {
        while (*token == ' ' || *token == '\t')
        {
            token++;
        }

        char *end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r'))
        {
            *end = '\0';
            end--;
        }

        if (*token != '\0')
        {
            char *filename;

            filename = D_TryFindWADByName(token);

            if (filename != NULL)
            {
                printf(" [config] loading mod: %s\n", filename);
                W_AddFile(filename);
                free(filename);
                modifiedgame = true;
            }
            else
            {
                printf(" [config] warning: could not find mod: %s\n", token);
            }
        }

        token = strtok(NULL, ",");
    }

    free(mod_copy);

    if (modifiedgame)
    {
        printf("\n");
    }
}

// Compatibility check results
typedef struct
{
    char message[256];
    mod_compat_result_t level;
} mod_compat_warning_t;

#define MAX_MOD_COMPAT_WARNINGS 32

static mod_compat_warning_t mod_compat_warnings[MAX_MOD_COMPAT_WARNINGS];
static int num_mod_compat_warnings = 0;

void W_AddModCompatWarning(mod_compat_result_t level, const char *fmt, ...)
{
    va_list args;

    if (num_mod_compat_warnings >= MAX_MOD_COMPAT_WARNINGS)
        return;

    va_start(args, fmt);
    vsnprintf(mod_compat_warnings[num_mod_compat_warnings].message,
               sizeof(mod_compat_warnings[num_mod_compat_warnings].message),
               fmt, args);
    va_end(args);

    mod_compat_warnings[num_mod_compat_warnings].level = level;
    num_mod_compat_warnings++;
}

void W_ClearModCompatWarnings(void)
{
    num_mod_compat_warnings = 0;
}

int W_GetNumModCompatWarnings(void)
{
    return num_mod_compat_warnings;
}

void W_GetModCompatWarning(int index, mod_compat_result_t *level, char *message)
{
    if (index < 0 || index >= num_mod_compat_warnings)
        return;

    if (level)
        *level = mod_compat_warnings[index].level;
    if (message)
        M_StringCopy(message, mod_compat_warnings[index].message,
                      sizeof(mod_compat_warnings[index].message));
}

static unsigned int num_wad_files_loaded = 0;

void W_IncrementWadFileCount(void)
{
    num_wad_files_loaded++;
}

static void W_CheckModCompat_WADS(void)
{
    if (num_wad_files_loaded <= 1)
        return;

    if (num_wad_files_loaded > 10)
    {
        W_AddModCompatWarning(MODCOMPAT_WARNING,
            "Loading %u WAD files - may cause performance issues or conflicts",
            num_wad_files_loaded);
    }
}

static void W_CheckModCompat_Sprites(void)
{
    unsigned int i;
    static const char *required_sprites[] = {
        "PLAY", "TROO", "SHTG", "MGUN", "LAUN", "PLAS", "BFGG"
    };
    int missing_count = 0;

    for (i = 0; i < arrlen(required_sprites); ++i)
    {
        char spritename[9];
        M_StringCopy(spritename, required_sprites[i], sizeof(spritename));

        if (W_CheckNumForName(spritename) < 0)
        {
            missing_count++;
        }
    }

    if (missing_count > 0)
    {
        W_AddModCompatWarning(MODCOMPAT_WARNING,
            "%d required sprite(s) missing - game may not render correctly",
            missing_count);
    }
}

static void W_CheckModCompat_Textures(void)
{
    unsigned int i;
    static const char *required_textures[] = {
        "FLAT01", "FLAT02", "FLOOR0_1", "CEIL1_1", "COMPBLUE", "SLADWALL"
    };
    int missing_count = 0;

    for (i = 0; i < arrlen(required_textures); ++i)
    {
        char texname[9];
        M_StringCopy(texname, required_textures[i], sizeof(texname));

        if (W_CheckNumForName(texname) < 0)
        {
            missing_count++;
        }
    }

    if (missing_count > 3)
    {
        W_AddModCompatWarning(MODCOMPAT_WARNING,
            "%d common texture(s) missing - some areas may not render correctly",
            missing_count);
    }
}

static void W_CheckModCompat_Dehacked(void)
{
    unsigned int i;
    int dehack_count = 0;

    for (i = 0; i < numlumps; ++i)
    {
        if (strncmp(lumpinfo[i]->name, "DEHACKED", 8) == 0)
        {
            dehack_count++;
        }
    }

    if (dehack_count > 5)
    {
        W_AddModCompatWarning(MODCOMPAT_WARNING,
            "%d DEHACKED patches loaded - may cause compatibility issues",
            dehack_count);
    }
}

void W_RunModCompatibilityCheck(void)
{
    W_ClearModCompatWarnings();

    W_CheckModCompat_WADS();
    W_CheckModCompat_Sprites();
    W_CheckModCompat_Textures();
    W_CheckModCompat_Dehacked();
}

void W_PrintModCompatibilityReport(void)
{
    int i;
    mod_compat_result_t level;
    char message[256];

    if (num_mod_compat_warnings == 0)
        return;

    printf("\n=== Mod Compatibility Report ===\n");

    for (i = 0; i < num_mod_compat_warnings; ++i)
    {
        W_GetModCompatWarning(i, &level, message);

        switch (level)
        {
        case MODCOMPAT_ERROR:
            printf("ERROR: ");
            break;
        case MODCOMPAT_WARNING:
            printf("WARNING: ");
            break;
        default:
            printf("INFO: ");
            break;
        }

        printf("%s\n", message);
    }

    printf("================================\n\n");
}

