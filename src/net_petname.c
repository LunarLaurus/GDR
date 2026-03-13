//
// Copyright(C) 2019 Jonathan Dowland
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
//     Generate a randomized, private, memorable name for a Player
//

#include <stdlib.h>
#include "doomtype.h"
#include "m_misc.h"
#include "doom/m_random.h"

static const char * const adjectives [] = {
    "Grumpy",
    "Ecstatic",
    "Surly",
    "Prepared",
    "Crafty",
    "Alert",
    "Sluggish",
    "Testy",
    "Reluctant",
    "Languid",
    "Passive",
    "Pacifist",
    "Aggressive",
    "Hostile",
    "Bubbly",
    "Giggly",
    "Laughing",
    "Crying",
    "Frowning",
    "Torpid",
    "Lethargic",
    "Manic",
    "Patient",
    "Protective",
    "Philosophical",
    "Enquiring",
    "Debating",
    "Furious",
    "Laid-Back",
    "Easy-Going",
    "Cromulent",
    "Excitable",
    "Tired",
    "Exhausted",
    "Ruminating",
    "Redundant",
    "Sporty",
    "Ginger",
    "Scary",
    "Posh",
    "Baby",
};

static const char * const nouns[] = {
    "Frad",
    // Doom
    "Cacodemon",
    "Arch-Vile",
    "Cyberdemon",
    "Imp",
    "Demon",
    "Mancubus",
    "Arachnotron",
    "Baron",
    "Knight",
    "Revenant",
    // Hexen
    "Ettin",
    "Maulotaur",
    "Centaur",
    "Afrit",
    "Serpent",
    // Heretic
    "Disciple",
    "Gargoyle",
    "Golem",
    "Lich",
    // Strife
    "Sentinel",
    "Acolyte",
    "Templar",
    "Reaver",
    "Spectre",
};

char *NET_GetRandomPetName()
{
    const char *a, *n;

    a = adjectives[M_Random() % arrlen(adjectives)];
    n = nouns[M_Random() % arrlen(nouns)];

    return M_StringJoin(a, " ", n, NULL);
}
