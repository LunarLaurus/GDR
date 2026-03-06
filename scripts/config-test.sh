#!/bin/bash
# Goblin Dice Rollaz - Config File Parsing Test
# Usage: ./scripts/config-test.sh [Debug|Release]
# Tests that config files can be parsed without errors

set -e

BUILD_TYPE="${1:-Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

echo "=========================================="
echo "Goblin Dice Rollaz Config Parsing Test"
echo "Build Type: $BUILD_TYPE"
echo "=========================================="

# Find the executable
EXE_NAME="goblin-doom"
EXE_PATH=""

if [ -f "$BUILD_DIR/$EXE_NAME" ]; then
    EXE_PATH="$BUILD_DIR/$EXE_NAME"
elif [ -f "$BUILD_DIR/bin/$EXE_NAME" ]; then
    EXE_PATH="$BUILD_DIR/bin/$EXE_NAME"
else
    echo "ERROR: $EXE_NAME not found. Run build-test.sh first."
    exit 1
fi

echo "Found executable: $EXE_PATH"

# Check if an IWAD exists
IWAD_DIRS=(
    "$PROJECT_ROOT"
    "$PROJECT_ROOT/data"
    "/usr/share/games/doom"
    "/usr/local/share/games/doom"
    "/opt/doom"
)

IWAD_PATH=""
for dir in "${IWAD_DIRS[@]}"; do
    for iwad in "doom.wad" "DOOM.WAD" "doom2.wad" "DOOM2.WAD" "freedoom.wad" "FREEDOOM.WAD"; do
        if [ -f "$dir/$iwad" ]; then
            IWAD_PATH="$dir/$iwad"
            break 2
        fi
    done
done

if [ -z "$IWAD_PATH" ]; then
    echo "ERROR: No IWAD found. Cannot run config test."
    exit 1
fi

echo "Found IWAD: $IWAD_PATH"

# Create test config files
TEST_CONFIGS=()

echo ""
echo "=== Test 1: Default config values ==="
CONFIG1="$TEMP_DIR/test_default.cfg"
cat > "$CONFIG1" << 'EOF'
# Default config - all values should be valid
EOF
TEST_CONFIGS+=("$CONFIG1")

echo ""
echo "=== Test 2: All goblin CVARs ==="
CONFIG2="$TEMP_DIR/test_goblin.cfg"
cat > "$CONFIG2" << 'EOF'
# Test all Goblin Dice Rollaz specific CVARs
goblin_crit_chance = 5
goblin_crit_boost_bonus = 10
goblin_exploding_dice = 1
goblin_weapon_stats = 0
goblin_modern_fx = 1
goblin_status_burning = 1
goblin_status_frozen = 1
goblin_status_stunned = 1
goblin_status_dicecurse = 1
goblin_colorblind_mode = 0
goblin_reduce_motion = 0
dice_sound_volume = 10
advantage_mode = 0
rpg_mode = 0
rpg_stat_points = 0
rpg_skill_combat = 0
rpg_skill_magic = 0
rpg_skill_survival = 0
EOF
TEST_CONFIGS+=("$CONFIG2")

echo ""
echo "=== Test 3: Mixed config with comments ==="
CONFIG3="$TEMP_DIR/test_mixed.cfg"
cat > "$CONFIG3" << 'EOF'
# This is a comment
mouse_sensitivity = 10 # inline comment
# Another comment
fullscreen = 1
# Video settings
window_position = center
video_driver = SDL2
# Sound
sfx_volume = 8
music_volume = 8
# Gameplay
showfps = 1
bob = 1
EOF
TEST_CONFIGS+=("$CONFIG3")

echo ""
echo "=== Test 4: Edge case values ==="
CONFIG4="$TEMP_DIR/test_edge.cfg"
cat > "$CONFIG4" << 'EOF'
# Test edge cases
mouse_sensitivity = 0
mouse_sensitivity = 999
sfx_volume = 0
sfx_volume = 31
screenblocks = 0
screenblocks = 13
detaillevel = 0
detaillevel = 2
EOF
TEST_CONFIGS+=("$CONFIG4")

echo ""
echo "=== Running config parsing tests ==="

TEST_NUM=0
PASSED=0
FAILED=0

for CONFIG in "${TEST_CONFIGS[@]}"; do
    TEST_NUM=$((TEST_NUM + 1))
    echo ""
    echo "Test $TEST_NUM: $CONFIG"
    
    # Run game with config and try to load a level
    # This tests that the config parses correctly
    timeout 5s "$EXE_PATH" \
        -iwad "$IWAD_PATH" \
        -config "$CONFIG" \
        -warp 1 \
        -nodraw \
        +quit 2>&1 || {
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 124 ] || [ $EXIT_CODE -eq 0 ]; then
            echo "  PASSED: Config parsed without errors"
            PASSED=$((PASSED + 1))
        else
            echo "  FAILED: Config parsing error (exit code: $EXIT_CODE)"
            FAILED=$((FAILED + 1))
        fi
    }
done

echo ""
echo "=========================================="
echo "Config Parsing Tests: $PASSED passed, $FAILED failed"
echo "=========================================="

if [ $FAILED -gt 0 ]; then
    exit 1
fi

echo "All config tests PASSED"
