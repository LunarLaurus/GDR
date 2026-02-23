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

#ifndef __G_REPLAY__
#define __G_REPLAY__

#include <stdio.h>

#define MAX_SHARED_DEMOS 32
#define MAX_DEMO_NAME_LEN 64

typedef struct {
    char filename[MAX_DEMO_NAME_LEN];
    char mapname[16];
    int skill;
    int kills;
    int time;
} shared_demo_t;

char *G_GetSharedDemosDir(void);
int G_ExportDemo(const char *demoname, const char *sharename);
int G_ListSharedDemos(shared_demo_t *demos, int max_demos);
int G_LoadSharedDemo(const char *filename);
void G_ShareDemo(const char *sharename);

#endif
