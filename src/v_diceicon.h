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
//  Dice load indicator - Goblin Dice Rollaz themed loading animation.
//

#ifndef __V_DICEICON__
#define __V_DICEICON__

#define DICE_ICON_W 32
#define DICE_ICON_H 32

void V_EnableDiceIcon(void);
void V_DrawDiceIcon(void);
void V_RestoreDiceBackground(void);
void V_DisableDiceIcon(void);

#endif
