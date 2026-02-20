# Goblin Dice Rollaz – Ralph Build Mode (Improved)

You are an autonomous coding agent using opencode. Follow the Ralph methodology strictly.

---

## Context

- Project root: `GoblinDiceRollaz/`
- Engine base: Chocolate Doom
- Task source: `IMPLEMENTATION_PLAN.md`
- Version control: **Git is mandatory**
- Work increment: **Exactly one task per loop**

---

## Execution Contract

You must complete exactly one task per iteration.

### Step 1 – Load Plan
- Open `IMPLEMENTATION_PLAN.md`
- Scan top to bottom
- Identify the **first unchecked task** (`- [ ]`)

Priority is strictly positional. Do not reorder tasks.

---

### Step 2 – Task Resolution

If an unchecked task exists:
- Implement that single task only.
- Do not partially complete multiple tasks.
- Do not skip tasks.
- If the task is ambiguous, infer intent conservatively using existing code patterns.

If no unchecked tasks exist:
- Generate 5–10 new tasks.
- Append them to the end of the file.
- Use the proper phase structure.
- Do not implement anything in the same loop after generating tasks.

---

## Implementation Rules

- Modify only files required for the selected task.
- Preserve Chocolate Doom architecture and style.
- Follow existing naming conventions.
- Avoid speculative refactors unless required by the task.
- Keep deterministic behavior intact (especially RNG and netplay).

---

## Commit Rules (Mandatory)

After completing a task:

1. Stage only relevant files.
2. Commit using format:

```
feat: complete Phase X – <task description>
```

If it is a fix:

```
fix: complete Phase X – <task description>
```

3. Do not squash.
4. One commit per task.

---

## Task Completion Protocol

After implementation:

- Edit `IMPLEMENTATION_PLAN.md`
- Change:

```
- [ ] Task description
```

to:

```
- [x] Task description
```

- Do not alter other tasks.
- Do not reformat the file.

---

## If Task Cannot Be Completed

Only allowed reasons:

- Missing upstream dependency
- Build failure unrelated to your change
- Ambiguous task that requires human clarification

In that case:
- Add a new sub-task directly below it:

```
- [ ] Resolve blocker: <short description>
```

- Do not mark original task complete.
- Exit loop.

---

## New Task Generation Guidelines

If all tasks are complete, append a new phase.

### Rules:
- Generate 5–10 tasks.
- Group them under a new `### Phase X: <Name>` section.
- X must increment from the last existing phase.
- Tasks must be specific and implementable.
- No vague tasks like “Improve gameplay”.

### Acceptable Categories

Engine:
- Deterministic RNG audit
- Add debug console commands
- Add new CVARs
- Rendering optimizations

Gameplay:
- New dice weapons
- New dwarf/goblin variants
- Boss mechanics
- New powerups

Content:
- WAD documentation
- Sprite pipeline docs
- DEHACKED/BEX specs

UI:
- HUD indicators
- Dice roll overlays
- Status bar extensions

---

## Known High-Impact Files

- `src/CMakeLists.txt`
- `src/doom/m_menu.c`
- `src/doom/d_englsh.h`
- `src/doom/info.c`
- `src/doom/d_items.c`
- `src/doom/st_stuff.c`

Search before editing. Follow surrounding code style.

---

## Safety Constraints

- Do not introduce floating-point nondeterminism.
- Do not modify core engine behavior unless explicitly required.
- Keep Chocolate Doom philosophy intact.

---

## Loop Summary

1. Open `IMPLEMENTATION_PLAN.md`
2. Find first `- [ ]`
3. Implement only that task
4. Mark it `[x]`
5. Commit
6. Exit

If none exist:

1. Append new phase with 5–10 tasks
2. Commit
3. Exit