# GDR Build Fix Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Get GDR compiling from 519 errors to zero by fixing header enums first, then source files.

**Architecture:** Enum-first strategy. Fix `p_mobj.h`, `sounds.h`, `info.h`, `info.c` in one pass to eliminate cascading errors, then address per-file issues. Stubs marked `/* GDR STUB: <reason> */` for future tracking. Boss flags use a new `flags2` field to avoid 32-bit exhaustion without touching hundreds of static initializers.

**Tech Stack:** C99, MSVC via CMake. Build command: `cmake --build D:\code\GDR\build --config Debug 2>&1`. Error count check: pipe to `grep "error C" | wc -l`.

**Build reference:** `D:\tools\compile_test.ps1` compiles a single file quickly for iteration.

---

### Task 1: Add `flags2` and missing flags to `p_mobj.h`

**Why flags2:** The 32-bit `flags` int is exhausted. `MF_BOSS` cannot be assigned a clean bit without aliasing `MF_SKULLFLY` (which controls skull-fly collision physics — destructive alias). `flags2` avoids touching the hundreds of static initializers in `info.c`.

**Files:**
- Modify: `src/doom/p_mobj.h`

**Step 1: Add `MF_ALWAYSPUFF` alias and `MF2_*` defines after the mobjflag_t enum (around line 230)**

In `p_mobj.h`, after the closing `} mobjflag_t;` line, add:

```c
// Goblin Dice Rollaz: MF_ALWAYSPUFF - alias for MF_NOBLOOD.
// Puff-only projectiles are semantically no-blood things.
// TODO: Give distinct bit value when flags2 system is extended to mobjinfo_t.
#define MF_ALWAYSPUFF   MF_NOBLOOD

// Goblin Dice Rollaz: Extended flags (flags2 field in mobj_t).
// These avoid the 32-bit exhaustion of the flags field.
#define MF2_BOSS        0x00000001  // This mobj is a boss (5x XP, phase triggers)
#define MF2_FRIENDLY    0x00000002  /* GDR STUB: coop ally flag, not yet implemented */
```

**Step 2: Add `flags2`, `special1`, `special2`, `speed`, `damage` to `mobj_t` struct**

In `p_mobj.h`, inside `struct mobj_s`, after the existing `int flags;` field (around line 281), add:

```c
    int         flags2;     // Goblin Dice Rollaz: Extended flags (MF2_* values)

    // Goblin Dice Rollaz: Per-actor scratch fields for phase tracking and counters.
    // Equivalent to Heretic/Hexen special1/special2.
    int         special1;
    int         special2;

    // Goblin Dice Rollaz: Runtime overrides for speed and damage.
    // info->speed / info->damage are the base values from mobjinfo.
    // These allow stun/slow/buff effects to modify an actor's values temporarily.
    int         speed;      // runtime speed override (0 = use info->speed)
    int         damage;     // runtime damage override (0 = use info->damage)
```

**Step 3: Build a single file to check no new errors in p_mobj.h itself**

```powershell
powershell -File D:\tools\compile_test.ps1
```

Expected: output should mention `p_mobj.h` with no errors about the new fields.

**Step 4: Commit**

```bash
cd D:/code/GDR
git add src/doom/p_mobj.h
git commit -m "fix: add flags2/special1/special2/speed/damage to mobj_t, MF_ALWAYSPUFF alias"
```

---

### Task 2: Restore missing sfx entries in `sounds.h`

**Why:** `info.c` references `sfx_posdie`, `sfx_bossdth`, `sfx_bmact`, `sfx_iceskl`, `sfx_pop`, and `ssfx_ssdth` (note the typo — double-s). These were in vanilla DOOM but were dropped, plus `ssfx_ssdth` is a typo for `sfx_ssdth` (which already exists).

**Files:**
- Modify: `src/doom/sounds.h`
- Modify: `src/doom/info.c` (fix typo `ssfx_ssdth` → `sfx_ssdth`)

**Step 1: Add missing sfx to `sfxenum_t` in `sounds.h`**

In `sounds.h`, find the line `sfx_radio,` (around line 226) and add the missing vanilla sfx before the GDR custom sfx block:

```c
    sfx_radio,
    // Restored vanilla DOOM sfx accidentally dropped during GDR refactor:
    sfx_posdie,     // Possessed soldier death (also used for mechanical deaths)
    sfx_bossdth,    // Boss death sound
    sfx_bmact,      // Baron of Hell active sound
    sfx_iceskl,     // Ice skeleton sound (used for ice-themed enemy)
    sfx_pop,        // Generic pop/break sound
    // GDR custom sfx:
    sfx_critup,     // ...existing line...
```

Note: `sfx_ssdth` (Wolfenstein SS death) already exists at line ~222. The reference `ssfx_ssdth` in `info.c` line 2043 is a **typo** — fix it in info.c, not here.

**Step 2: Fix typo in `info.c`**

In `src/doom/info.c` at line 2043, change:
```c
	ssfx_ssdth,		// deathsound
```
to:
```c
	sfx_ssdth,		// deathsound
```

**Step 3: Build check**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "sounds.h\|sfx_" | head -20
```

Expected: no more undefined `sfx_posdie` / `sfx_bossdth` etc.

**Step 4: Commit**

```bash
git add src/doom/sounds.h src/doom/info.c
git commit -m "fix: restore dropped vanilla sfx entries, fix ssfx_ssdth typo"
```

---

### Task 3: Add missing sprites and states to `info.h`

**Why:** `info.c` uses `SPR_ICE` (for the D24 Hourglass Cannon weapon — it uses ice imagery), and references states `S_D2_4`, `S_ICE`, `S_FIRE`, `S_BOSS_XDIE1`, `S_IFOG00` that aren't in `statenum_t`. The D2 weapon sequence has `S_D2_1`, `S_D2_2`, `S_D2_3` but is missing `S_D2_4` — the refire frame. All others are stubs.

**Files:**
- Modify: `src/doom/info.h`

**Step 1: Add `SPR_ICE` to `spritenum_t`**

In `info.h`, find `NUMSPRITES` (line 188) and add before it:

```c
    SPR_ICE,        /* GDR STUB: Ice/Hourglass Cannon weapon sprite, needs WAD asset */
    NUMSPRITES
```

**Step 2: Add missing states to `statenum_t`**

In `info.h`, find `NUMSTATES` (line 1443) and add before it:

```c
    // Goblin Dice Rollaz: Missing weapon state (D2 refire frame)
    S_D2_4,
    // Goblin Dice Rollaz: Environmental effect states (stubs)
    S_ICE,          /* GDR STUB: Ice effect spawn state */
    S_FIRE,         /* GDR STUB: Fire effect spawn state */
    // Goblin Dice Rollaz: Boss death states (stubs)
    S_BOSS_XDIE1,   /* GDR STUB: Boss gib death state */
    // Goblin Dice Rollaz: Item fog states (vanilla variant names)
    S_IFOG00,       /* Variant name for item fog; maps to same animation as S_IFOG */
    NUMSTATES
```

**Step 3: Build check**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "SPR_ICE\|S_D2_4\|S_ICE\|S_FIRE\|S_BOSS\|S_IFOG00" | head -20
```

Expected: no more undeclared identifier errors for these symbols.

**Step 4: Commit**

```bash
git add src/doom/info.h
git commit -m "fix: add SPR_ICE sprite and missing statenum_t entries (S_D2_4, S_ICE, S_FIRE, S_BOSS_XDIE1, S_IFOG00)"
```

---

### Task 4: Add matching `state_t` entries and fix `info.c` forward declarations

**Why:** Every entry added to `statenum_t` needs a corresponding initializer in the `states[]` array in `info.c`. MSVC C2078 "too many initializers" means the array has more entries than `NUMSTATES` allows — adding the new enum values fixes this by growing `NUMSTATES`. Also, `A_FireD100Plus1` is defined in `p_pspr.c` but not forward-declared in `info.c`'s preamble.

**Files:**
- Modify: `src/doom/info.c`

**Step 1: Add `A_FireD100Plus1` forward declaration**

In `info.c`, find the forward declaration block (around line 51-158). Add after `A_FireRunicBinding();`:

```c
void A_FireD100Plus1();     // Goblin Dice Rollaz: D100+1 Fumble Finder (defined in p_pspr.c)
```

**Step 2: Add state_t entries for new statenum_t values**

In `info.c`, find the end of the `states[]` array (just before the closing `};`). Add these entries — they must appear in the same order as the enum:

```c
    // S_D2_4 - D2 weapon refire frame (completes the 4-frame sequence)
    {SPR_D6B1,1,5,{A_ReFire},S_D2,0,0},

    // S_ICE - Ice effect state stub
    /* GDR STUB: needs proper sprite frames when ice WAD assets are ready */
    {SPR_PUFF,0,1,{NULL},S_NULL,0,0},

    // S_FIRE - Fire effect state stub
    /* GDR STUB: needs proper sprite frames when fire effect WAD assets are ready */
    {SPR_PUFF,0,1,{NULL},S_NULL,0,0},

    // S_BOSS_XDIE1 - Boss gib death stub
    /* GDR STUB: needs boss gib sprite sequence */
    {SPR_PUFF,0,1,{NULL},S_NULL,0,0},

    // S_IFOG00 - Item fog variant (same animation as S_IFOG)
    {SPR_IFOG,0,6,{NULL},S_IFOG01,0,0},
```

**Step 3: Also add "PFIR" and "PFTR" sprite names to `sprnames[]` if SPR_ICE is new**

In `info.c`, find the `sprnames[]` array and verify `"ICE\0"` is at the same index position as `SPR_ICE` in the enum. Add `"ICE "` (4 chars + null) at the correct position. The array and enum must stay in sync.

Count: `SPR_ICE` is added just before `NUMSPRITES`. Count the existing sprites in `sprnames[]` and the enum to ensure alignment. Add `"ICE "` before the terminating `NULL`:

```c
    "ICE ", NULL   /* SPR_ICE added for D24 Hourglass Cannon */
};
```

**Step 4: Build check — count errors**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "error C" | wc -l
```

Expected: significant error reduction from 519. Target: under 100.

**Step 5: Commit**

```bash
git add src/doom/info.c
git commit -m "fix: add state_t entries for new states, A_FireD100Plus1 forward decl, SPR_ICE in sprnames"
```

---

### Task 5: Fix `MF_BOSS` and `MF_FRIENDLY` in consuming files

**Why:** `info.c` sets `MF_BOSS` in mobjinfo static initializers; `g_rpg.c` and `p_inter.c` check it at runtime. With `flags2` + `MF2_BOSS` now defined, we need to:
1. Remove `MF_BOSS` from the static `mobjinfo[]` flags (these are initial spawn flags, not runtime)
2. Remove the placeholder `#define MF_BOSS 0` hacks from `g_rpg.c`
3. Set `flags2 |= MF2_BOSS` in `P_SpawnMobj` for boss types
4. Update runtime checks to use `flags2`

**Files:**
- Modify: `src/doom/info.c`
- Modify: `src/doom/g_rpg.c`
- Modify: `src/doom/p_inter.c`
- Modify: `src/doom/p_mobj.c`

**Step 1: Remove `MF_BOSS` from `info.c` static initializers**

In `info.c`, search for all occurrences of `MF_BOSS` (there are 3: lines ~4210, ~4241, ~4443). Remove `|MF_BOSS` from each flags field. The boss type is identified by `mobjtype_t` — the runtime flag will be set in `P_SpawnMobj`.

Example change:
```c
// Before:
MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL|MF_BOSS,    // flags

// After:
MF_SOLID|MF_SHOOTABLE|MF_COUNTKILL,            // flags (MF2_BOSS set at spawn time via P_SpawnMobj)
```

**Step 2: Remove `#define` stubs from `g_rpg.c`**

In `src/doom/g_rpg.c`, remove these lines (around line 30-31):
```c
#define MF_FRIENDLY 0
#define MF_BOSS     0
```

**Step 3: Update `g_rpg.c` runtime checks to use `flags2`**

In `g_rpg.c`, change the two flag checks:
```c
// Line ~111 - Before:
if (victim->flags & MF_FRIENDLY)
// After:
if (victim->flags2 & MF2_FRIENDLY)

// Line ~125 - Before:
if (victim->flags & MF_BOSS)
// After:
if (victim->flags2 & MF2_BOSS)
```

**Step 4: Update `p_inter.c` runtime checks**

In `src/doom/p_inter.c`, change the MF_BOSS checks (lines ~1158, ~2206, ~2211, ~2217):
```c
// Before:
if (target->flags & MF_BOSS)
// After:
if (target->flags2 & MF2_BOSS)

// Before:
if (source && (source->flags & MF_BOSS))
// After:
if (source && (source->flags2 & MF2_BOSS))
```

**Step 5: Set `flags2 |= MF2_BOSS` in `P_SpawnMobj`**

In `src/doom/p_mobj.c`, find `P_SpawnMobj` function. After the mobj is initialized (after `mobj->flags = info->flags;`), add:

```c
    // Goblin Dice Rollaz: Set extended flags for boss types
    mobj->flags2 = 0;
    if (type == MT_GOBLIN_KING ||
        type == MT_DWARVEN_WAR_MACHINE ||
        type == MT_DWARF_WARLORD)
    {
        mobj->flags2 |= MF2_BOSS;
    }
```

**Step 6: Zero-initialize new fields in P_SpawnMobj**

In the same function, after `mobj->flags2 = 0;`, also initialize the new scratch fields:
```c
    mobj->special1 = 0;
    mobj->special2 = 0;
    mobj->speed = 0;
    mobj->damage = 0;
```

**Step 7: Build check**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "error C" | wc -l
```

Expected: further significant reduction. MF_BOSS-related errors should be gone.

**Step 8: Commit**

```bash
git add src/doom/info.c src/doom/g_rpg.c src/doom/p_inter.c src/doom/p_mobj.c
git commit -m "fix: move MF_BOSS to flags2 system, zero-init new mobj_t fields in P_SpawnMobj"
```

---

### Task 6: Run full build and triage remaining errors

**Why:** After Tasks 1-5, the majority of cascading errors should be resolved. This task does a full build, categorizes remaining errors by file, and determines if more tasks are needed.

**Files:** None modified — diagnostic only.

**Step 1: Full build with error capture**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 > D:/code/GDR/build_errors.txt
grep "error C" D:/code/GDR/build_errors.txt | wc -l
```

**Step 2: Group by file**

```bash
grep "error C" D:/code/GDR/build_errors.txt | sed 's/.*src.doom.//' | cut -d'(' -f1 | sort | uniq -c | sort -rn | head -30
```

Expected output: a ranked list of files with remaining errors.

**Step 3: Review top 5 error files**

```bash
grep "error C" D:/code/GDR/build_errors.txt | grep "p_inter.c\|p_enemy.c\|m_menu\|p_user\|p_pspr" | head -30
```

**Step 4: Create follow-on tasks**

Based on the output, create additional tasks for each cluster of errors. Common expected remaining issues:
- `p_pspr.c`: weapon fire functions referencing undefined locals or missing includes
- `m_menu_gameopts.c`, `m_menu_accessibility.c`: new menu files with missing CVAR declarations
- `p_siege_weapon.h`: header-only struct or type issues
- `p_user.c`, `p_enemy.c`: per-actor field access issues now resolved by Task 1

If error count is under 50 and clustered in 2-3 files, continue inline. If over 50, spawn a new plan pass.

---

### Task 7: Fix remaining per-file errors (iterate)

This task is a template — repeat for each file cluster identified in Task 6.

**For each file with errors:**

**Step 1: Isolate errors for one file**

```bash
grep "error C" D:/code/GDR/build_errors.txt | grep "filename.c"
```

**Step 2: Read the relevant section of the file**

Use Read tool on the file around the error line numbers.

**Step 3: Apply the minimal fix**

Common patterns:
- Missing `#include`: add the header that defines the missing type
- Missing `extern` declaration: add `extern type name;` at top of file
- Identifier typo: fix in place
- Undeclared function: add forward declaration before first use

**Step 4: Build the single file to verify fix**

Edit `D:\tools\compile_test.ps1` to target the file being fixed, then run:
```powershell
powershell -File D:\tools\compile_test.ps1
```

Expected: no errors for that file.

**Step 5: Full build check and commit**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "error C" | wc -l
git add src/doom/filename.c
git commit -m "fix: resolve compile errors in filename.c"
```

Repeat until error count = 0.

---

### Task 8: Final verification

**Step 1: Clean build**

```bash
cmake --build D:/code/GDR/build --config Debug --clean-first 2>&1 | tail -5
```

Expected: `Build succeeded` or linker errors only (linker errors are acceptable — they indicate missing stub implementations, not header errors).

**Step 2: Count any remaining compiler errors**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "error C" | wc -l
```

Expected: 0

**Step 3: List any linker errors for documentation**

```bash
cmake --build D:/code/GDR/build --config Debug 2>&1 | grep "LNK" | head -20
```

Document any linker errors in `docs/plans/2026-03-13-build-fix-design.md` under a new "Linker Stubs" section.

**Step 4: Final commit**

```bash
git add -A
git commit -m "fix: GDR project now compiles clean (linker stubs documented)"
```

---

## Stub Tracking

After completion, find all documented stubs:
```bash
grep -r "GDR STUB" src/ | wc -l
grep -r "GDR STUB" src/
```

These are the items deferred for future implementation sprints.
