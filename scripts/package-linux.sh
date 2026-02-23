#!/bin/bash
# Goblin Dice Rollaz - Linux Release Packaging Script
# Usage: ./scripts/package-linux.sh [version]

set -e

VERSION="${1:-1.0.0}"
PROJECT_NAME="goblin-dice-rollaz"
ARCHIVE_NAME="${PROJECT_NAME}-linux-${VERSION}"
OUTPUT_DIR="release"

echo "Building Goblin Dice Rollaz Linux release package v${VERSION}..."

# Create output directory
mkdir -p "${OUTPUT_DIR}"

# Build the project
echo "Building project..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
cd ..

# Create release directory structure
RELEASE_DIR="${OUTPUT_DIR}/${ARCHIVE_NAME}"
mkdir -p "${RELEASE_DIR}"

# Copy binaries
echo "Copying binaries..."
cp build/src/goblin-doom "${RELEASE_DIR}/"
cp build/src/goblin-setup "${RELEASE_DIR}/"
cp build/src/goblin-server "${RELEASE_DIR}/"

# Copy required libraries (if statically linked, this may be empty)
# SDL2 and other dependencies are typically statically linked or installed system-wide

# Create archive
echo "Creating archive..."
cd "${OUTPUT_DIR}"
tar -cvzf "${ARCHIVE_NAME}.tar.gz" "${ARCHIVE_NAME}/"
cd ..

# Create tar.bz2 as alternative
cd "${OUTPUT_DIR}"
tar -cvjf "${ARCHIVE_NAME}.tar.bz2" "${ARCHIVE_NAME}/"
cd ..

echo "Linux release package created: ${OUTPUT_DIR}/${ARCHIVE_NAME}.tar.gz"
echo "Linux release package created: ${OUTPUT_DIR}/${ARCHIVE_NAME}.tar.bz2"

# Clean up build directory (optional, comment out to keep)
# rm -rf build

echo "Done!"
