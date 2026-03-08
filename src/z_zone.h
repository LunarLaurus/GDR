#ifndef Z_ZONE_H
#define Z_ZONE_H

#include <stdio.h>

void    Z_CheckHeap (void);
int     Z_FreeMemory (void);
void    Z_DumpMemoryLeaks(FILE *fp);
int     Z_GetAllocCount(void);
void    Z_ResetAllocCount(void);

#endif