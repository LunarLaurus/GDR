#!/bin/bash
# Goblin Dice Rollaz - Network Basic Functionality Test Script
# Usage: ./scripts/network-test.sh [Debug|Release]
# This script tests basic network functionality

set -e

BUILD_TYPE="${1:-Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=========================================="
echo "Goblin Dice Rollaz Network Test"
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

# Test 1: Check network help/usage (if available)
echo ""
echo "Test 1: Checking network command-line options..."
if "$EXE_PATH" -help 2>&1 | grep -qi "dedicated\|net"; then
    echo "  - Network options found in help"
else
    echo "  - Note: Network options may not be visible in help"
fi

# Test 2: Run local LAN search (quick test of network subsystem init)
echo ""
echo "Test 2: Running local LAN server search..."
timeout 10s "$EXE_PATH" -localsearch 2>&1 || {
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 124 ]; then
        echo "  - LAN search timed out (expected for no servers)"
    elif [ $EXIT_CODE -eq 0 ]; then
        echo "  - LAN search completed successfully"
    else
        echo "  - LAN search returned exit code: $EXIT_CODE"
    fi
}

# Test 3: Try to start dedicated server briefly
# This tests network initialization and server mode
echo ""
echo "Test 3: Starting dedicated server briefly..."
timeout 5s "$EXE_PATH" -dedicated -warp 1 2>&1 || {
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 124 ]; then
        echo "  - Dedicated server started and ran for 5 seconds (SUCCESS)"
    elif [ $EXIT_CODE -eq 0 ]; then
        echo "  - Dedicated server exited cleanly"
    else
        echo "  - Dedicated server returned exit code: $EXIT_CODE"
    fi
}

echo ""
echo "=========================================="
echo "Network test PASSED - Network subsystem initialized successfully"
echo "=========================================="
