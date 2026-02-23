//
// Goblin Dice Rollaz - Demo Test Suite
// Provides reproducible demo testing for deterministic validation
//

#include "g_demo_test.h"
#include "g_game.h"
#include "doomstat.h"
#include "p_mobj.h"
#include "p_user.h"
#include "m_random.h"
#include "i_system.h"
#include <stdio.h>
#include <string.h>

#define GDT_MAX_CHECKSUM_ENTRIES 128

typedef struct
{
    int gametic;
    unsigned int state_checksum;
} gdt_checkum_entry_t;

static boolean gdt_initialized = false;
static boolean gdt_recording = false;
static boolean gdt_verifying = false;
static unsigned int gdt_recorded_checksum = 0;
static unsigned int gdt_verify_checksum = 0;
static int gdt_test_passes = 0;
static int gdt_test_fails = 0;
static char gdt_current_demo[256];

void GDT_Init(void)
{
    gdt_initialized = true;
    gdt_recording = false;
    gdt_verifying = false;
    gdt_recorded_checksum = 0;
    gdt_verify_checksum = 0;
    gdt_test_passes = 0;
    gdt_test_fails = 0;

    // Register console variables
    // Note: Using M_BindIntVariable would require m_menu.c integration
    // For now, functions provide programmatic access
}

void GDT_ComputeStateChecksum(unsigned int *checksum)
{
    unsigned int sum = 0;
    int i;

    if (!checksum || !gdt_initialized)
        return;

    sum ^= (gametic & 0xFFFFFFFF);

    sum ^= ((gameepisode << 16) & 0xFFFF0000);
    sum ^= (gamemap & 0xFFFF);

    sum ^= ((gameskill << 24) & 0xFF000000);

    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            player_t *p = &players[i];

            sum ^= ((p->mo->x >> 16) & 0xFFFF);
            sum ^= (p->mo->y & 0xFFFF);

            sum ^= ((p->mo->angle << 16) & 0xFFFF0000);

            sum ^= ((p->health << 8) & 0xFF00);
            sum ^= (p->armorpoints & 0xFF);

            sum ^= ((p->attackdown << 24) & 0xFF000000);
            sum ^= ((p->powers[0]) & 0x00FFFFFF);
        }
    }

    sum ^= (P_Random() & 0xFFFF);
    sum ^= ((M_Random() << 16) & 0xFFFF0000);

    sum ^= ((totalkills << 16) & 0xFFFF0000);
    sum ^= (totalitems & 0xFFFF);

    *checksum = sum;
}

boolean GDT_CompareChecksums(unsigned int a, unsigned int b)
{
    return (a == b);
}

void GDT_RecordDemo(const char *demoname)
{
    if (!gdt_initialized || !demoname)
        return;

    gdt_recording = true;
    gdt_verifying = false;

    strncpy(gdt_current_demo, demoname, sizeof(gdt_current_demo) - 1);
    gdt_current_demo[sizeof(gdt_current_demo) - 1] = '\0';

    GDT_ComputeStateChecksum(&gdt_recorded_checksum);

    // G_RecordDemo is called externally to start actual recording
}

void GDT_PlayAndVerifyDemo(const char *demoname)
{
    unsigned int playback_checksum = 0;

    if (!gdt_initialized || !demoname)
        return;

    gdt_recording = false;
    gdt_verifying = true;

    strncpy(gdt_current_demo, demoname, sizeof(gdt_current_demo) - 1);
    gdt_current_demo[sizeof(gdt_current_demo) - 1] = '\0';

    GDT_ComputeStateChecksum(&gdt_verify_checksum);

    if (GDT_CompareChecksums(gdt_recorded_checksum, gdt_verify_checksum))
    {
        gdt_test_passes++;
    }
    else
    {
        gdt_test_fails++;
    }
}
