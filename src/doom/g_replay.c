//
// Copyright(C) 2024 Brian
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
//  Goblin Dice Rollaz - Replay sharing system
//

#include "g_replay.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "doomdef.h"
#include "doomstat.h"
#include "d_main.h"
#include "g_game.h"
#include "m_misc.h"
#include "m_argv.h"

static char shared_demos_dir[256] = {0};

char *G_GetSharedDemosDir(void)
{
    if (shared_demos_dir[0] == '\0' && savegamedir)
    {
        M_snprintf(shared_demos_dir, sizeof(shared_demos_dir),
                  "%sgoblin_dice_rollaz_replays", savegamedir);
    }
    return shared_demos_dir;
}

int G_ExportDemo(const char *demoname, const char *sharename)
{
    char src_path[256];
    char dst_path[256];
    char *shared_dir;
    byte *demo_data;
    int demo_length;

    if (!demoname || !sharename)
        return -1;

    M_snprintf(src_path, sizeof(src_path), "%s.lmp", demoname);
    shared_dir = G_GetSharedDemosDir();
    M_snprintf(dst_path, sizeof(dst_path), "%s/%s.lmp", shared_dir, sharename);

    if (!M_FileExists(src_path))
    {
        printf("Demo file not found: %s\n", src_path);
        return -1;
    }

    if (!M_ReadFile(src_path, &demo_data))
    {
        printf("Failed to read demo: %s\n", src_path);
        return -1;
    }

    demo_length = M_FileLength(M_fopen(src_path, "rb"));

    M_MakeDirectory(shared_dir);

    if (!M_WriteFile(dst_path, demo_data, demo_length))
    {
        free(demo_data);
        printf("Failed to write shared demo: %s\n", dst_path);
        return -1;
    }

    free(demo_data);

    printf("Demo exported to: %s\n", dst_path);
    return 0;
}

int G_ListSharedDemos(shared_demo_t *demos, int max_demos)
{
    return 0;
}

int G_LoadSharedDemo(const char *filename)
{
    char path[256];
    char *shared_dir = G_GetSharedDemosDir();

    M_snprintf(path, sizeof(path), "%s/%s.lmp", shared_dir, filename);

    if (!M_FileExists(path))
    {
        printf("Shared demo not found: %s\n", path);
        return -1;
    }

    G_PlayDemo(path);
    return 0;
}

void G_ShareDemo(const char *sharename)
{
    if (demorecording && demoname)
    {
        char default_name[64];
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);

        if (!sharename || sharename[0] == '\0')
        {
            M_snprintf(default_name, sizeof(default_name),
                      "replay_%04d%02d%02d_%02d%02d",
                      tm_info->tm_year + 1900,
                      tm_info->tm_mon + 1,
                      tm_info->tm_mday,
                      tm_info->tm_hour,
                      tm_info->tm_min);
            sharename = default_name;
        }

        G_ExportDemo(demoname, sharename);
    }
    else
    {
        printf("Not currently recording a demo.\n");
    }
}
