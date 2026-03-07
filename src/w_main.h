//
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

#ifndef W_MAIN_H
#define W_MAIN_H

#include "d_mode.h"

boolean W_ParseCommandLine(void);
void W_CheckCorrectIWAD(GameMission_t mission);

// Autoload all .wad files from the given directory:
void W_AutoLoadWADs(const char *path);

// Load WAD files from the mod_load_order config variable:
void W_LoadModsFromConfig(void);

// Mod compatibility checking:
void W_IncrementWadFileCount(void);
void W_RunModCompatibilityCheck(void);
void W_PrintModCompatibilityReport(void);
void W_ClearModCompatWarnings(void);
int W_GetNumModCompatWarnings(void);

typedef enum
{
    MODCOMPAT_OK,
    MODCOMPAT_WARNING,
    MODCOMPAT_ERROR
} mod_compat_result_t;

void W_GetModCompatWarning(int index, mod_compat_result_t *level, char *message);

#endif /* #ifndef W_MAIN_H */

