//
// Copyright(C) 2026 Goblin Dice Rollaz
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
// DESCRIPTION:  Client-side prediction system for dice roll results
//

#ifndef __DICE_PREDICT_H__
#define __DICE_PREDICT_H__

#include "doomdef.h"

#define MAX_PREDICTED_DAMAGE 64
#define MAX_DICE_EVENTS_PER_TIC 8

// Goblin Dice Rollaz: Optimized network packet for dice roll events
// Uses minimal byte packing to reduce network bandwidth
typedef struct __attribute__((packed)) dice_event_net_s {
    uint8_t predict_id;        // 1 byte (0-255, wraps around)
    uint8_t weapon;            // 1 byte (weapon index)
    int16_t damage;            // 2 bytes (can be negative for healing)
    uint8_t crit_roll;         // 1 byte (0 = no crit, 1-20 = crit roll)
    uint8_t flags;             // 1 byte (bitfield: 0=normal, 1=critical, 2=validated)
    uint8_t reserved;          // 1 byte (padding for alignment)
    int16_t target_x_delta;   // 2 bytes (delta from player position)
    int16_t target_y_delta;   // 2 bytes (delta from player position)
} dice_event_net_t;            // Total: 10 bytes per event

// Flags for dice_event_net_t.flags
#define DICE_EVENT_FLAG_CRITICAL  0x01
#define DICE_EVENT_FLAG_VALIDATED 0x02
#define DICE_EVENT_FLAG_MISFIRE   0x04

typedef struct predicted_damage_s {
    int id;
    int target_x;
    int target_y;
    int predicted_damage;
    int actual_damage;
    boolean predicted_critical;
    boolean actual_critical;
    int predicted_crit_roll;
    int actual_crit_roll;
    int weapon;
    int tic;
    boolean pending;
    boolean validated;
    boolean desynced;
} predicted_damage_t;

void PREDICT_Init(void);

int PREDICT_RecordDamage(int x, int y, int damage, boolean critical, 
                         int crit_roll, int weapon);

void PREDICT_ValidateDamage(int predict_id, int actual_damage, 
                            boolean actual_critical, int actual_crit_roll);

void PREDICT_Clear(void);

int PREDICT_GetPendingCount(void);

int PREDICT_GetDesyncCount(void);

void PREDICT_Ticker(void);

// Server-authoritative damage validation
int PREDICT_CalculateServerDamage(int weapon, player_t *player, mobj_t *target);
void PREDICT_ValidateServerDamage(int predict_id, int weapon, player_t *player, mobj_t *target);

// Goblin Dice Rollaz: Network packet optimization functions
// Encode/decode dice events for efficient network transmission
void PREDICT_EncodeEvent(dice_event_net_t *packet, int predict_id, 
                         int player_x, int player_y,
                         int target_x, int target_y,
                         int damage, boolean critical, int crit_roll, int weapon);

boolean PREDICT_DecodeEvent(const dice_event_net_t *packet, int player_x, int player_y,
                            int *out_predict_id, int *out_target_x, int *out_target_y,
                            int *out_damage, boolean *out_critical, int *out_crit_roll, 
                            int *out_weapon);

int PREDICT_GetEventSize(void);

#endif
