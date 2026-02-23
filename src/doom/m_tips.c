#include "doomdef.h"
#include "doomstat.h"
#include "hu_stuff.h"
#include "m_random.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"
#include "d_main.h"
#include "g_game.h"

#define NUM_TIPS 15

static const char *dice_tips[NUM_TIPS] = {
    "TIP: Rolling a 20 always crits!",
    "TIP: D4 throws are fast but low damage.",
    "TIP: D12 has the highest crit multiplier!",
    "TIP: D100 can roll anywhere from 1 to 100!",
    "TIP: Combine Critical Hit powerups for devastation.",
    "TIP: Dwarves have high armor - aim for their weak points!",
    "TIP: The Goblin Shaman casts healing spells on enemies!",
    "TIP: Use the d6 blaster for balanced damage output.",
    "TIP: The d20 cannon fires slow but hits hard.",
    "TIP: Combo kills build your crit multiplier!",
    "TIP: D4 throwing knives have a high crit chance.",
    "TIP: Watch for the CRIT indicator on your HUD!",
    "TIP: The Dwarven War Machine boss has a weak rear armor.",
    "TIP: Explosive dice can damage multiple enemies at once!",
    "TIP: The Goblin King has three attack phases!"
};

static int current_tip_index = -1;
static boolean tips_initialized = false;

void GDR_InitTips(void)
{
    if (!tips_initialized)
    {
        current_tip_index = M_Random() % NUM_TIPS;
        tips_initialized = true;
    }
}

const char *GDR_GetCurrentTip(void)
{
    if (!tips_initialized)
    {
        GDR_InitTips();
    }
    return dice_tips[current_tip_index];
}

void GDR_NextTip(void)
{
    if (!tips_initialized)
    {
        GDR_InitTips();
    }
    current_tip_index = (current_tip_index + 1) % NUM_TIPS;
}

void GDR_RandomizeTip(void)
{
    if (!tips_initialized)
    {
        GDR_InitTips();
    }
    current_tip_index = M_Random() % NUM_TIPS;
}

void GDR_DrawTip(int x, int y)
{
    const char *tip;
    int w;
    const char *ch;
    int c;
    int cx;
    int cy;

    tip = GDR_GetCurrentTip();
    if (!tip)
        return;

    ch = tip;
    cx = x;
    cy = y;

    while(1)
    {
        c = *ch++;
        if (!c)
            break;
        
        c = toupper(c) - HU_FONTSTART;
        if (c < 0 || c >= HU_FONTSIZE)
        {
            cx += 4;
            continue;
        }
        
        w = SHORT(hu_font[c]->width);
        if (cx + w > SCREENWIDTH)
            break;
        V_DrawPatchDirect(cx, cy, hu_font[c]);
        cx += w;
    }
}
