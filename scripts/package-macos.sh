#!/bin/bash
# Goblin Dice Rollaz - macOS Release Packaging Script
# Usage: ./scripts/package-macos.sh [version]

set -e

VERSION="${1:-1.0.0}"
PROJECT_NAME="goblin-dice-rollaz"
APP_NAME="Goblin Dice Rollaz"
ARCHIVE_NAME="${PROJECT_NAME}-macos-${VERSION}"
OUTPUT_DIR="release"

echo "Building Goblin Dice Rollaz macOS release package v${VERSION}..."

# Check for macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Warning: This script is designed to run on macOS"
fi

# Create output directory
mkdir -p "${OUTPUT_DIR}"

# Build the project
echo "Building project..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu 2>/dev/null || echo 4)
cd ..

# Create app bundle structure
BUNDLE_DIR="${OUTPUT_DIR}/${APP_NAME}.app"
mkdir -p "${BUNDLE_DIR}/Contents/MacOS"
mkdir -p "${BUNDLE_DIR}/Contents/Resources"

# Copy binaries
echo "Creating app bundle..."
cp build/src/goblin-doom "${BUNDLE_DIR}/Contents/MacOS/"
cp build/src/goblin-setup "${BUNDLE_DIR}/Contents/Resources/"

# Copy SDL2 frameworks (if needed - for distribution builds)
# Typically SDL2 is either embedded or expected to be installed

# Create Info.plist
cat > "${BUNDLE_DIR}/Contents/Info.plist" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>goblin-doom</string>
    <key>CFBundleIdentifier</key>
    <string>com.goblindicerollaz.game</string>
    <key>CFBundleName</key>
    <string>Goblin Dice Rollaz</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
EOF

# Create DMG
echo "Creating DMG..."
cd "${OUTPUT_DIR}"
hdiutil create -volname "Goblin Dice Rollaz" -srcfolder "${APP_NAME}.app" -ov -format UDZO "${ARCHIVE_NAME}.dmg"
cd ../..

echo "macOS release package created: ${OUTPUT_DIR}/${ARCHIVE_NAME}.dmg"

# Also create a zip archive as alternative
cd "${OUTPUT_DIR}"
zip -r "${ARCHIVE_NAME}.zip" "${APP_NAME}.app"
cd ../..

echo "macOS archive created: ${OUTPUT_DIR}/${ARCHIVE_NAME}.zip"

echo "Done!"
