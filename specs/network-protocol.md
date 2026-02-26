# Goblin Dice Rollaz Network Protocol

This document describes the network protocol used by Goblin Dice Rollaz for multiplayer gameplay. The protocol is based on Chocolate Doom's networking stack with modifications for Goblin Dice Rollaz.

---

## Protocol Version

- **Protocol Name**: `CHOCOLATE_DOOM_0`
- **Magic Number**: `1454104972U` (0x569A_9714)
- **Maximum Players**: 8
- **Maximum Nodes**: 16

---

## Network Architecture

### Connection Flow

1. Client broadcasts `NET_PACKET_TYPE_QUERY` to discover servers
2. Server responds with `NET_PACKET_TYPE_QUERY_RESPONSE`
3. Client sends `NET_PACKET_TYPE_SYN` to initiate connection
4. Server validates and responds with `NET_PACKET_TYPE_WAITING_DATA` or `NET_PACKET_TYPE_REJECTED`
5. Once all players ready, server sends `NET_PACKET_TYPE_GAMESTART`
6. During gameplay, `NET_PACKET_TYPE_GAMEDATA` packets exchange ticcmds
7. On disconnect, `NET_PACKET_TYPE_DISCONNECT` is exchanged

### Reliable vs Unreliable Packets

- **Unreliable**: Game data packets (can be lost; newer packets supersede)
- **Reliable**: Connection management (retransmitted until acknowledged)
- Reliable packets use the `NET_RELIABLE_PACKET` flag (1 << 15) in the header

---

## Packet Types

| Type | Value | Description |
|------|-------|-------------|
| NET_PACKET_TYPE_SYN | 0 | Connection request |
| NET_PACKET_TYPE_REJECTED | 1 | Connection rejected |
| NET_PACKET_TYPE_KEEPALIVE | 2 | Keep-alive ping |
| NET_PACKET_TYPE_WAITING_DATA | 3 | Lobby state update |
| NET_PACKET_TYPE_GAMESTART | 4 | Game is starting |
| NET_PACKET_TYPE_GAMEDATA | 5 | Game state/ticcmd |
| NET_PACKET_TYPE_GAMEDATA_ACK | 6 | Acknowledge gamedata |
| NET_PACKET_TYPE_DISCONNECT | 7 | Disconnect request |
| NET_PACKET_TYPE_DISCONNECT_ACK | 8 | Disconnect acknowledged |
| NET_PACKET_TYPE_RELIABLE_ACK | 9 | Reliable packet ACK |
| NET_PACKET_TYPE_GAMEDATA_RESEND | 10 | Request resend |
| NET_PACKET_TYPE_CONSOLE_MESSAGE | 11 | Chat message |
| NET_PACKET_TYPE_QUERY | 12 | Server discovery |
| NET_PACKET_TYPE_QUERY_RESPONSE | 13 | Server info response |
| NET_PACKET_TYPE_LAUNCH | 14 | Launch external server |
| NET_PACKET_TYPE_NAT_HOLE_PUNCH | 15 | NAT traversal |

---

## Packet Structures

### Connect Data (net_connect_data_t)

Sent during connection negotiation:

| Field | Type | Size | Description |
|-------|------|------|-------------|
| gamemode | uint8 | 1 | Game mode (shareware, registered, retail, commercial) |
| gamemission | uint8 | 1 | Game mission (doom, doom2, pack_tnt, etc.) |
| lowres_turn | uint8 | 1 | Use low-resolution turning (1 = yes) |
| drone | uint8 | 1 | Drone mode (1 = passive observer) |
| max_players | uint8 | 1 | Maximum players allowed |
| is_freedoom | uint8 | 1 | Using FreeDoom content |
| wad_sha1sum | bytes | 20 | SHA1 hash of WAD file |
| deh_sha1sum | bytes | 20 | SHA1 hash of DEHACKED patch |
| player_class | uint8 | 1 | Player class (Doom/Heretic/Hexen) |

**Total Size**: 27 bytes

### Game Settings (net_gamesettings_t)

Broadcast when game starts:

| Field | Type | Size | Description |
|-------|------|------|-------------|
| ticdup | uint8 | 1 | Ticdup factor |
| extratics | uint8 | 1 | Extra tic multiplier |
| deathmatch | uint8 | 1 | Deathmatch mode |
| nomonsters | uint8 | 1 | No monsters |
| fast_monsters | uint8 | 1 | Fast monsters |
| respawn_monsters | uint8 | 1 | Respawn monsters |
| episode | uint8 | 1 | Episode number |
| map | uint8 | 1 | Map number |
| skill | int8 | 1 | Skill level (1-5) |
| gameversion | uint8 | 1 | Game version |
| lowres_turn | uint8 | 1 | Low-res turning |
| new_sync | uint8 | 1 | New sync algorithm |
| timelimit | uint32 | 4 | Time limit in minutes |
| loadgame | int8 | 1 | Load game slot (-1 = new game) |
| random | uint8 | 1 | Random seed mode |
| num_players | uint8 | 1 | Number of players |
| consoleplayer | int8 | 1 | Local player number |
| player_classes | uint8[] | 8 | Player class per slot |

### Ticcmd Diff (net_ticdiff_t)

Player input is delta-encoded to reduce bandwidth:

| Flag | Bit | Description |
|------|-----|-------------|
| NET_TICDIFF_FORWARD | 0 | Forward movement changed |
| NET_TICDIFF_SIDE | 1 | Side movement changed |
| NET_TICDIFF_TURN | 2 | Turning changed |
| NET_TICDIFF_BUTTONS | 3 | Button state changed |
| NET_TICDIFF_CONSISTANCY | 4 | Consistency checksum changed |
| NET_TICDIFF_CHATCHAR | 5 | Chat character present |

When a flag is set, the corresponding field follows:
- `forwardmove`: int8 (signed)
- `sidemove`: int8 (signed)
- `angleturn`: int8 (lowres) or int16 (hires)
- `buttons`: uint8
- `consistancy`: uint8
- `chatchar`: uint8

### Full Ticcmd (net_full_ticcmd_t)

Complete frame data for all players:

| Field | Type | Size | Description |
|-------|------|------|-------------|
| latency | int16 | 2 | Milliseconds since sent |
| playeringame | bitfield | 1 | 8-bit mask of active players |
| cmds | ticdiff[] | variable | Per-player ticcmd diffs |

### Query Data (net_querydata_t)

Server broadcast response:

| Field | Type | Size | Description |
|-------|------|------|-------------|
| version | string | variable | Version string (e.g., "GoblinDiceRollaz 1.0") |
| server_state | uint8 | 1 | 0 = waiting, 1 = playing |
| num_players | uint8 | 1 | Current player count |
| max_players | uint8 | 1 | Maximum players |
| gamemode | uint8 | 1 | Game mode |
| gamemission | uint8 | 1 | Game mission |
| description | string | variable | Server description |
| protocol | uint8 | 1 | Protocol version |

---

## Ticcmd Structure (ticcmd_t)

Player input command (defined in d_ticcmd.h):

```
struct ticcmd_s
{
    signed int forwardmove;  // Forward/backward (+/- 255)
    signed int sidemove;     // Left/right (+/- 255)
    unsigned int angleturn;  // Angle turn (0-65535)
    unsigned int buttons;    // Button bitfield
    unsigned char chatchar;  // Chat character
    unsigned char consistancy; // World consistency check
};
```

Button bits:
- BT_ATTACK (1)
- BT_USE (2)
- BT_CHANGEWEAPON (4)
- BT_SPECIAL (128)
- BTSPECIALMASK (3)
- BT_WEAPONMASK (31 << 4)
- BT_WEAPONSHIFT (4)

---

## Networking Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| BACKUPTICS | 128 | Input buffer size |
| NET_TIC_RATE | 35 | Tics per second |
| MAXPACKETLENGTH | 1024 | Maximum packet size |

---

## Sliding Window Protocol

The network code uses a sliding window for reliable packet delivery:

- **Send Window**: Outstanding unacknowledged packets
- **Receive Window**: Buffer for out-of-order packets
- **Resend Timeout**: Configurable, typically 1000ms

---

## Compatibility Notes

- Protocol string `CHOCOLATE_DOOM_0` is used for backward compatibility
- Newer versions should maintain this for cross-version play
- The protocol supports both old ( Doom v1.2 ) and new sync methods
- Lowres turn uses 1 byte per turn; hires uses 2 bytes

---

## Security Considerations

- `NET_ReadSafeString` filters non-printable characters
- SHA1 hashes verify WAD/DEH content integrity
- No encryption (local network play only)
- No authentication (trusted local network assumed)

---

## References

- Source: `src/net_packet.c` - Packet read/write functions
- Source: `src/net_structrw.c` - Structure serialization
- Source: `src/net_defs.h` - Type definitions
- Source: `src/net_client.c` - Client implementation
- Source: `src/net_server.c` - Server implementation
