#!/bin/bash
# Goblin Dice Rollaz - Memory Leak Detection Test
# Usage: ./scripts/memory-leak-test.sh [Debug|Release]
# This script runs the game with -debugmem flag and checks for memory leaks

set -e

BUILD_TYPE="${1:-Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
REPORT_FILE="$PROJECT_ROOT/goblin-doom-memleaks.txt"

echo "=========================================="
echo "Goblin Dice Rollaz Memory Leak Test"
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

# Remove any previous report
rm -f "$REPORT_FILE"

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
    # Test with minimal startup
    timeout 5s "$EXE_PATH" -nodraw -devparm -debugmem +quit 2>&1 || true
else
    echo "Found IWAD: $IWAD_PATH"
    
    # Run the game briefly with memory debugging enabled
    # -debugmem enables memory leak tracking
    # Run for 3 seconds then quit - enough to initialize and check for obvious leaks
    echo ""
    echo "Running memory leak test on MAP01..."
    
    timeout 10s "$EXE_PATH" -iwad "$IWAD_PATH" -warp 1 -nodraw -debugmem +quit 2>&1 || {
        EXIT_CODE=$?
        if [ $EXIT_CODE -eq 124 ]; then
            echo "Game timed out (expected)"
        elif [ $EXIT_CODE -eq 0 ]; then
            echo "Game exited cleanly"
        else
            echo "Game exited with code: $EXIT_CODE"
        fi
    }
fi

echo ""
echo "=========================================="
echo "Memory Leak Report Analysis"
echo "=========================================="

if [ -f "$REPORT_FILE" ]; then
    echo "Report file found: $REPORT_FILE"
    echo ""
    cat "$REPORT_FILE"
    echo ""
    
    # Check for actual leaks
    if grep -q "Leaked blocks: 0" "$REPORT_FILE"; then
        echo "=========================================="
        echo "RESULT: PASS - No memory leaks detected!"
        echo "=========================================="
        rm -f "$REPORT_FILE"
        exit 0
    else
        # Extract leak count for summary
        LEAK_COUNT=$(grep "Leaked blocks:" "$REPORT_FILE" | awk '{print $3}')
        echo "=========================================="
        echo "RESULT: FAIL - Memory leaks detected!"
        echo "Leaked blocks: $LEAK_COUNT"
        echo "=========================================="
        exit 1
    fi
else
    echo "ERROR: No report file generated!"
    echo "This may indicate the game crashed before shutdown."
    exit 1
fi
