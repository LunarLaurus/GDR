#!/bin/bash
# Goblin Dice Rollaz - Universal Release Packaging Script
# Usage: ./scripts/release.sh [version]
# This script calls the appropriate platform-specific script

set -e

VERSION="${1:-1.0.0}"

echo "=========================================="
echo "Goblin Dice Rollaz Release Packaging"
echo "Version: $VERSION"
echo "=========================================="

# Detect platform
case "$(uname -s)" in
    Linux*)
        echo "Detected: Linux"
        if [ -f "./scripts/package-linux.sh" ]; then
            chmod +x ./scripts/package-linux.sh
            ./scripts/package-linux.sh "$VERSION"
        else
            echo "Error: package-linux.sh not found"
            exit 1
        fi
        ;;
    Darwin*)
        echo "Detected: macOS"
        if [ -f "./scripts/package-macos.sh" ]; then
            chmod +x ./scripts/package-macos.sh
            ./scripts/package-macos.sh "$VERSION"
        else
            echo "Error: package-macos.sh not found"
            exit 1
        fi
        ;;
    MINGW*|MSYS*|CYGWIN*)
        echo "Detected: Windows (Git Bash/MSYS)"
        if [ -f "./scripts/package-windows.ps1" ]; then
            powershell.exe -ExecutionPolicy Bypass -File "./scripts/package-windows.ps1" -Version "$VERSION"
        else
            echo "Error: package-windows.ps1 not found"
            exit 1
        fi
        ;;
    *)
        echo "Unknown platform. Running platform detection..."
        echo ""
        echo "Available platform scripts:"
        echo "  - Linux:   ./scripts/package-linux.sh"
        echo "  - macOS:   ./scripts/package-macos.sh"
        echo "  - Windows: ./scripts/package-windows.ps1"
        echo ""
        echo "Usage:"
        echo "  Linux/macOS: ./scripts/package-<platform>.sh [version]"
        echo "  Windows:     powershell -File ./scripts/package-windows.ps1 -Version <version>"
        exit 1
        ;;
esac

echo ""
echo "=========================================="
echo "Release packaging complete!"
echo "Output directory: release/"
echo "=========================================="
