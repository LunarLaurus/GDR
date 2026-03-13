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

// On Windows, include platform headers FIRST to prevent boolean/BOOLEAN
// typedef conflicts between the Windows SDK (rpcndr.h/winnt.h) and doomtype.h.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "i_cloud.h"
#include "m_misc.h"
#include "i_system.h"

#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

static char cloud_dir[512];
static boolean cloud_initialized = false;
static boolean cloud_available = false;

void I_CloudInit(void)
{
    if (cloud_initialized)
    {
        return;
    }

    cloud_initialized = true;

#ifdef _WIN32
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, cloud_dir)))
    {
        M_snprintf(cloud_dir + strlen(cloud_dir),
                   sizeof(cloud_dir) - strlen(cloud_dir),
                   "\\GoblinDiceRollaz\\CloudSaves");
        CreateDirectoryA(cloud_dir, NULL);
        cloud_available = true;
    }
#else
    const char *home = getenv("HOME");
    if (home != NULL)
    {
        M_snprintf(cloud_dir, sizeof(cloud_dir),
                   "%s/.goblin-dice-rollaz/cloud_saves", home);
        mkdir(cloud_dir, 0755);
        cloud_available = true;
    }
#endif

    if (!cloud_available)
    {
        fprintf(stderr, "Cloud storage not available - using local saves only\n");
    }
}

int I_CloudAvailable(void)
{
    if (!cloud_initialized)
    {
        I_CloudInit();
    }
    return cloud_available;
}

int I_CloudSave(const char *local_path, const char *save_name)
{
    FILE *src, *dst;
    char dst_path[512];
    int result = -1;

    if (!I_CloudAvailable())
    {
        return -1;
    }

    M_snprintf(dst_path, sizeof(dst_path), "%s/%s.dsg",
               cloud_dir, save_name);

    src = M_fopen(local_path, "rb");
    if (src == NULL)
    {
        return -1;
    }

    dst = M_fopen(dst_path, "wb");
    if (dst == NULL)
    {
        fclose(src);
        return -1;
    }

    {
        byte buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
        {
            fwrite(buffer, 1, bytes, dst);
        }
    }

    fclose(src);
    fclose(dst);
    result = 0;

    return result;
}

int I_CloudLoad(const char *save_name, const char *local_path)
{
    FILE *src, *dst;
    char src_path[512];
    int result = -1;

    if (!I_CloudAvailable())
    {
        return -1;
    }

    M_snprintf(src_path, sizeof(src_path), "%s/%s.dsg",
               cloud_dir, save_name);

    src = M_fopen(src_path, "rb");
    if (src == NULL)
    {
        return -1;
    }

    dst = M_fopen(local_path, "wb");
    if (dst == NULL)
    {
        fclose(src);
        return -1;
    }

    {
        byte buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
        {
            fwrite(buffer, 1, bytes, dst);
        }
    }

    fclose(src);
    fclose(dst);
    result = 0;

    return result;
}

int I_CloudListSaves(cloud_save_info_t *saves, int max_saves)
{
    int count = 0;

    if (!I_CloudAvailable() || saves == NULL || max_saves <= 0)
    {
        return 0;
    }

#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE handle;
    char search_path[512];

    M_snprintf(search_path, sizeof(search_path), "%s\\*.dsg", cloud_dir);
    handle = FindFirstFileA(search_path, &find_data);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (count < max_saves)
            {
                size_t name_len = strlen(find_data.cFileName) - 4;
                if (name_len < CLOUD_SAVE_NAME_LEN)
                {
                    strncpy(saves[count].filename, find_data.cFileName, CLOUD_SAVE_NAME_LEN - 1);
                    saves[count].filename[CLOUD_SAVE_NAME_LEN - 1] = '\0';
                    strncpy(saves[count].name, find_data.cFileName, CLOUD_SAVE_NAME_LEN - 1);
                    saves[count].name[name_len] = '\0';
                    saves[count].size = find_data.nFileSizeLow;
                    saves[count].timestamp = 0;
                    count++;
                }
            }
        } while (FindNextFileA(handle, &find_data));
        FindClose(handle);
    }
#else
    DIR *dir = opendir(cloud_dir);
    if (dir != NULL)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL && count < max_saves)
        {
            size_t name_len = strlen(entry->d_name) - 4;
            if (name_len > 0 && name_len < CLOUD_SAVE_NAME_LEN &&
                strstr(entry->d_name, ".dsg") != NULL)
            {
                strncpy(saves[count].filename, entry->d_name, CLOUD_SAVE_NAME_LEN - 1);
                saves[count].filename[CLOUD_SAVE_NAME_LEN - 1] = '\0';
                strncpy(saves[count].name, entry->d_name, CLOUD_SAVE_NAME_LEN - 1);
                saves[count].name[name_len] = '\0';
                saves[count].size = 0;
                saves[count].timestamp = 0;
                count++;
            }
        }
        closedir(dir);
    }
#endif

    return count;
}

int I_CloudDelete(const char *save_name)
{
    char filepath[512];
    int result = -1;

    if (!I_CloudAvailable())
    {
        return -1;
    }

    M_snprintf(filepath, sizeof(filepath), "%s/%s.dsg",
               cloud_dir, save_name);

    if (M_remove(filepath) == 0)
    {
        result = 0;
    }

    return result;
}

void I_CloudSync(void)
{
    if (!I_CloudAvailable())
    {
        return;
    }

    fprintf(stderr, "Cloud sync: Stub implementation - no automatic sync\n");
}
