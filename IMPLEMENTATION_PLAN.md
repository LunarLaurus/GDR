# IMPLEMENTATION_PLAN.md - Goblin Dice Rollaz

## Priority Tasks

### Phase 1: Engine Setup (Complete)
- [x] Clone Chocolate Doom source
- [x] Set up CMakeLists.txt with project name
- [x] Create Dockerfile
- [x] Create GitHub Actions workflow

### Phase 2: Game Identity
- [x] Update d_main.c with "Goblin Dice Rollaz" title
- [x] Update m_menu.c title screen text
- [x] Update binary output names in CMakeLists.txt
- [ ] Test build succeeds

### Phase 3: CI/CD Validation  
- [ ] Push to GitHub and verify Actions pass
- [ ] Verify Linux artifact builds
- [ ] Verify Windows artifact builds
- [ ] Verify Docker image builds

### Phase 4: Content (Future)
- [ ] Create goblin player sprite replacement
- [ ] Create dwarf enemy sprites
- [ ] Create dice weapon sprites
- [ ] Update monster definitions in info.c

---

## Current Status
**Phase 1 Complete** - Engine setup done, ready for identity updates.

## Notes
- Chocolate Doom base provides SDL2 cross-platform support
- Need Doom IWAD file to run actual game
- Content changes require WAD file creation (future phase)
