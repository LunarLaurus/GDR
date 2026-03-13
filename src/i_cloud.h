//
// Copyright(C) 2024 Goblin Dice Rollaz
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
//  Cloud save interface stubs.
//

#ifndef __I_CLOUD__
#define __I_CLOUD__

#include <stddef.h>

#define MAX_CLOUD_SAVES 8
#define CLOUD_SAVE_NAME_LEN 32

typedef struct
{
    char name[CLOUD_SAVE_NAME_LEN];
    char filename[CLOUD_SAVE_NAME_LEN];
    unsigned long size;
    unsigned int timestamp;
} cloud_save_info_t;

void I_CloudInit(void);

/* Returns non-zero if cloud storage is available. Uses int to avoid
   boolean/BOOLEAN conflicts with the Windows SDK. */
int I_CloudAvailable(void);

int I_CloudSave(const char *local_path, const char *save_name);

int I_CloudLoad(const char *save_name, const char *local_path);

int I_CloudListSaves(cloud_save_info_t *saves, int max_saves);

int I_CloudDelete(const char *save_name);

void I_CloudSync(void);

#endif
