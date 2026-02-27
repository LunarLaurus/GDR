#!/bin/bash
# Goblin Dice Rollaz - Build Test Script
# Usage: ./scripts/build-test.sh [Debug|Release]
# This script performs an automated build test to verify the project compiles

set -e

BUILD_TYPE="${1:-Release}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=========================================="
echo "Goblin Dice Rollaz Build Test"
echo "Build Type: $BUILD_TYPE"
echo "=========================================="

# Change to project root
cd "$PROJECT_ROOT"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure with CMake
echo ""
echo "[1/3] Configuring CMake..."
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build the project
echo ""
echo "[2/3] Building project..."
cmake --build . -j$(nproc)

# Verify the executable exists
echo ""
echo "[3/3] Verifying build output..."
EXE_NAME="goblin-doom"
if [ -f "$BUILD_DIR/$EXE_NAME" ]; then
    echo "SUCCESS: $EXE_NAME found"
    ls -la "$BUILD_DIR/$EXE_NAME"
elif [ -f "$BUILD_DIR/bin/$EXE_NAME" ]; then
    echo "SUCCESS: $EXE_NAME found in bin/"
    ls -la "$BUILD_DIR/bin/$EXE_NAME"
else
    echo "ERROR: $EXE_NAME not found in build output"
    find "$BUILD_DIR" -name "$EXE_NAME" -type f 2>/dev/null || true
    exit 1
fi

echo ""
echo "=========================================="
echo "Build test PASSED"
echo "=========================================="
