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
//      Zone Memory Allocation.
//

#ifndef Z_ZONE_H
#define Z_ZONE_H

#include <stdio.h>

//
// ZONE MEMORY
// PU - purge tags.

enum
{
    PU_STATIC = 1,                  // static entire execution time
    PU_SOUND,                       // static while playing
    PU_MUSIC,                       // static while playing
    PU_FREE,                        // a free block
    PU_LEVEL,                       // static until level exited
    PU_LEVSPEC,                     // a special thinker in a level

    // Tags >= PU_PURGELEVEL are purgable whenever needed.

    PU_PURGELEVEL,
    PU_CACHE,

    // GDR extension tags
    PU_AUDIO,                       // audio resources
    PU_OPENGL,                      // OpenGL resources

    // Total number of different tag types

    PU_NUM_TAGS
};

void    Z_Init (void);
void*   Z_Malloc (int size, int tag, void *ptr);
void    Z_Free (void *ptr);
void    Z_FreeTags (int lowtag, int hightag);
void    Z_CheckHeap (void);
void    Z_ChangeTag2 (void *ptr, int tag, const char *file, int line);
int     Z_FreeMemory (void);
void    Z_DumpMemoryLeaks(FILE *fp);
int     Z_GetAllocCount(void);
void    Z_ResetAllocCount(void);

#define Z_ChangeTag(p,t) \
    Z_ChangeTag2((p), (t), __FILE__, __LINE__)

#endif
