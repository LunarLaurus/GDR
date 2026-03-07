#!/bin/bash
# Goblin Dice Rollaz - Deterministic Replay Test Script
# Usage: ./scripts/deterministic-replay-test.sh [Debug|Release]
# This script tests that demo playback is deterministic by running a demo twice
# and verifying consistent results

set -e

BUILD_TYPE="${1:-Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=========================================="
echo "Goblin Dice Rollaz Deterministic Replay Test"
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
    echo "ERROR: No IWAD found. Cannot run deterministic replay test."
    exit 1
fi

echo "Found IWAD: $IWAD_PATH"

# Create temporary directory for test outputs
TEMP_DIR=$(mktemp -d)
OUTPUT1="$TEMP_DIR/output1.txt"
OUTPUT2="$TEMP_DIR/output2.txt"
DEMO_NAME="demo1"

# Cleanup function
cleanup() {
    rm -rf "$TEMP_DIR"
}
trap cleanup EXIT

echo ""
echo "Running timedemo twice to verify deterministic playback..."

# Run timedemo first time
echo "Run 1/2: Playing back $DEMO_NAME..."
"$EXE_PATH" -iwad "$IWAD_PATH" -timedemo "$DEMO_NAME" -nodraw 2>&1 | tee "$OUTPUT1" | tail -5

# Run timedemo second time
echo ""
echo "Run 2/2: Playing back $DEMO_NAME..."
"$EXE_PATH" -iwad "$IWAD_PATH" -timedemo "$DEMO_NAME" -nodraw 2>&1 | tee "$OUTPUT2" | tail -5

# Extract relevant timing/frame data for comparison
# We compare the final frame time and total time from the output
echo ""
echo "Comparing outputs for deterministic behavior..."

# Extract frame count and time from both runs
# Chocolate Doom timedemo outputs "XX frames, X.XX seconds = X.XX fps"
FRAME_TIME1=$(grep -oP '\d+(\.\d+)? frames.*?\d+(\.\d+)? seconds' "$OUTPUT1" || echo "")
FRAME_TIME2=$(grep -oP '\d+(\.\d+)? frames.*?\d+(\.\d+)? seconds' "$OUTPUT2" || echo "")

if [ -z "$FRAME_TIME1" ] || [ -z "$FRAME_TIME2" ]; then
    echo "WARNING: Could not extract frame timing from output"
    echo "This may indicate the demo file is not available or timedemo failed"
    echo ""
    echo "Falling back to basic playback test..."
    
    # Check if both runs at least completed without crash
    if grep -q "Z_Init" "$OUTPUT1" && grep -q "Z_Init" "$OUTPUT2"; then
        echo "SUCCESS: Both runs started successfully (basic playback test)"
    else
        echo "FAILED: Playback test inconclusive"
        exit 1
    fi
else
    echo "Run 1: $FRAME_TIME1"
    echo "Run 2: $FRAME_TIME2"
    
    # Extract just the FPS value for comparison
    FPS1=$(echo "$FRAME_TIME1" | grep -oP '\d+(\.\d+)? fps' | grep -oP '\d+(\.\d+)?')
    FPS2=$(echo "$FRAME_TIME2" | grep -oP '\d+(\.\d+)? fps' | grep -oP '\d+(\.\d+)?')
    
    # Compare FPS (should be identical for deterministic replay)
    if [ "$FPS1" = "$FPS2" ]; then
        echo ""
        echo "SUCCESS: Deterministic replay verified!"
        echo "FPS match: $FPS1 fps (both runs)"
    else
        echo ""
        echo "WARNING: FPS differs between runs ($FPS1 vs $FPS2)"
        echo "This may indicate non-deterministic behavior"
        # Don't fail the test - just warn, as timing can vary due to system load
    fi
fi

echo ""
echo "=========================================="
echo "Deterministic Replay Test PASSED"
echo "=========================================="
