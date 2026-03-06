#!/bin/bash
# Goblin Dice Rollaz - Smoke Test Script
# Usage: ./scripts/smoke-test.sh [Debug|Release]
# This script runs a quick gameplay test to verify the game starts without crashing

set -e

BUILD_TYPE="${1:-Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=========================================="
echo "Goblin Dice Rollaz Smoke Test"
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
    echo "WARNING: No IWAD found. Running with -nodraw for startup test only."
    # Just verify the executable runs without crashing
    timeout 5s "$EXE_PATH" -nodraw -devparm 2>&1 || {
        if [ $? -eq 124 ]; then
            echo "SUCCESS: Executable started and ran for 5 seconds without crash"
        else
            echo "FAILED: Executable crashed or errored"
            exit 1
        fi
    }
else
    echo "Found IWAD: $IWAD_PATH"
    
    # Run the game on map 1 with a short timeout
    # -warp 1 starts on map 1
    # -nodraw disables rendering (for headless testing)
    # -timedemo demo1 would run a demo and exit
    echo ""
    echo "Running smoke test on MAP01..."
    
    # Run for 5 seconds then kill - should not crash
    timeout 5s "$EXE_PATH" -iwad "$IWAD_PATH" -warp 1 -nodraw +quit 2>&1 || {
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 124 ]; then
            echo "SUCCESS: Game ran for 5 seconds without crash"
        elif [ $EXIT_CODE -eq 0 ]; then
            echo "SUCCESS: Game exited cleanly"
        else
            echo "FAILED: Game crashed or errored (exit code: $EXIT_CODE)"
            exit 1
        fi
    }
fi

echo ""
echo "=========================================="
echo "Smoke test PASSED"
echo "=========================================="
