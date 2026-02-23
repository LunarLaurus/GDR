#ifndef G_DEMO_TEST_H
#define G_DEMO_TEST_H

#include "doomtype.h"

void GDT_Init(void);
void GDT_RecordDemo(const char *demoname);
void GDT_PlayAndVerifyDemo(const char *demoname);
void GDT_ComputeStateChecksum(unsigned int *checksum);
boolean GDT_CompareChecksums(unsigned int a, unsigned int b);

#endif
