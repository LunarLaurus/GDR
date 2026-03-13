# GDR Build Fix Design
**Date**: 2026-03-13
**Status**: Approved
**Strategy**: Enum-First, Hybrid Stubs

## Problem

The GDR project has ~519 compilation errors across ~30 source files. The ralph loop added code referencing enum values, struct fields, and action functions that were never added to the corresponding headers. The majority of errors cascade from a small number of root-cause header gaps.

## Goal

Get the project compiling to a linkable binary. Fix what the code actually references today. Document stubs clearly so content can be completed later without hunting for them.

## Stub Convention

All placeholder/stub additions are marked:
```c
/* GDR STUB: <reason> */
```
These are greppable: `grep -r "GDR STUB" src/` shows everything still pending.

## Strategy: Three Phases

### Phase 1 — Header Pass (fixes ~80% of errors)

Fix each header file once. Every `.c` file that includes it inherits the fix automatically.

#### `p_mobj.h` — `mobjflag_t` additions

| Flag | Value | Status | Notes |
|------|-------|--------|-------|
| `MF_ALWAYSPUFF` | `0x00080000` | Proper | Vanilla unused bit; spawns puff on non-wall hits |
| `MF_BOSS` | `0x00100000` | Proper | Boss XP multiplier, vanilla-safe unused bit |
| `MF_FRIENDLY` | `0x00200000` | Stub | Coop ally; proper implementation deferred |

#### `p_mobj.h` — `mobj_t` struct additions

| Field | Type | Status | Notes |
|-------|------|--------|-------|
| `special1` | `int` | Stub | Per-actor scratch field (phase tracking, etc.) |
| `special2` | `int` | Stub | Per-actor scratch field |
| `speed` | `int` | Proper | Runtime speed override (stun/slow effects modify this) |
| `damage` | `int` | Proper | Runtime damage override |

#### `sounds.h` — missing `sfxenum_t` entries

**Restored vanilla entries** (accidentally dropped):
- `sfx_posdie`, `sfx_bossdth`, `sfx_bmact`, `sfx_iceskl`, `sfx_pop`

**GDR stubs** (needs WAD assets before these are functional):
- War Machine: `sfx_warmchl`, `sfx_warmcpn`, `sfx_warmcgun`, `sfx_warmcpain`, `sfx_warmcdt`, `sfx_warmcflm`, `sfx_warmcrok`, `sfx_warmcrep`
- Goblin King: per boss-goblin-king.md spec

#### `info.h` — `spritenum_t` additions

| Sprite | Status | Notes |
|--------|--------|-------|
| `SPR_ICE` | Stub | Ice effect; needs WAD sprite |
| `SPR_WARMC` | Stub | Dwarven War Machine; per spec |
| `SPR_GOBK` | Stub | Goblin King; per spec |

#### `info.h` — `statenum_t` additions

| State | Status | Notes |
|-------|--------|-------|
| `S_D2_4` | Proper | D2 weapon was missing its 4th frame (all other dice weapons have 4) |
| `S_ICE` | Stub | Ice effect state; next=S_NULL |
| `S_FIRE` | Stub | Fire effect state; next=S_NULL |
| `S_BOSS_XDIE1` | Stub | Boss gib death; next=S_NULL |
| `S_IFOG00`, `S_IFOG01` | Proper | Restore vanilla item fog states that were dropped |
| War Machine states | Stub | Full list per boss-dwarven-war-machine.md spec |

#### `info.h` — `mobjflag_t` note

`MF_BOSS` and `MF_FRIENDLY` are in `p_mobj.h`. Remove the `#define MF_BOSS 0` / `#define MF_FRIENDLY 0` placeholder hacks from `g_rpg.c` and `g_powerup.c` once the real flags are in place.

#### `info.c` — matching `state_t` entries

Every new `statenum_t` entry needs a corresponding `state_t` in the `states[]` array in `info.c`. Stubs use `{SPR_PUFF, 0, -1, {NULL}, S_NULL, 0, 0}` (invisible/static).

#### `info.c` — `A_FireD100Plus1`

Forward declaration in `info.h`, stub body in `p_inter.c` or dedicated `p_d100plus1.c`:
```c
void A_FireD100Plus1(player_t* player, pspdef_t* psp)
{
    /* GDR STUB: D100+1 weapon fire logic not yet implemented */
}
```

### Phase 2 — Source File Pass

After Phase 1, rebuild and collect remaining errors. Expected categories:

- **Wrong struct field access**: code accessing `mo->speed` expecting runtime field (now added)
- **Missing externs**: variables defined in one `.c` but not declared extern where used
- **Identifier typos**: e.g. `ssfx_ssdth` (double-s typo for `sfx_ssdth`)
- **File-specific issues**: each of the ~30 files addressed individually

Files with known unique issues (from pre-fix audit):
`p_particles.c`, `p_doors.c`, `p_pspr.c`, `p_inter.c`, `m_menu_gameopts.c`,
`m_menu_accessibility.c`, `p_siege_weapon.h`, `p_user.c`, `p_enemy.c`, `info.c`

### Phase 3 — Compile Verification

After each phase:
1. Run full build via CMake
2. Count remaining errors
3. Treat new errors as feedback, not failure
4. Declare success when error count reaches 0 (or linker errors only — those come next)

## What This Does NOT Fix

- Linker errors (missing function bodies for stubs) — intentional, documented
- Runtime correctness of stubbed systems — deferred
- WAD/sprite assets for new enemies — separate art pipeline task

## Flagged as Coming Next (not added now)

Per the pragmatic scope agreement, these are documented here but not added to enums today:
- Full Goblin King state machine (specs/boss-goblin-king.md)
- Full War Machine state machine (specs/boss-dwarven-war-machine.md)
- All dwarf enemy variant states (~20 MT_ types in mobjtype_t, already added but states not yet defined)
- Music entries for boss encounters (already in musicenum_t — ✓ done)
