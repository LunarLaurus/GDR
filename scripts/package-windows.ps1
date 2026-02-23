# Goblin Dice Rollaz - Windows Release Packaging Script
# Usage: .\scripts\package-windows.ps1 [-Version <version>]
# Example: .\scripts\package-windows.ps1 -Version "1.0.0"

param(
    [string]$Version = "1.0.0"
)

$ProjectName = "goblin-dice-rollaz"
$ArchiveName = "$ProjectName-windows-$Version"
$OutputDir = "release"

Write-Host "Building Goblin Dice Rollaz Windows release package v$Version..."

# Create output directory
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

# Build the project
Write-Host "Building project..."
if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
}
New-Item -ItemType Directory -Path "build" | Out-Null
Set-Location "build"

# Try to find SDL2 and other dependencies via vcpkg or system
$cmakeCmd = "cmake .. -G `"Unix Makefiles`" -DCMAKE_BUILD_TYPE=Release"
Write-Host "Running: $cmakeCmd"
Invoke-Expression $cmakeCmd

$buildCmd = "cmake --build . --config Release"
Write-Host "Running: $buildCmd"
Invoke-Expression $buildCmd

Set-Location ".."

# Create release directory
$ReleaseDir = "$OutputDir\$ArchiveName"
if (Test-Path $ReleaseDir) {
    Remove-Item -Recurse -Force $ReleaseDir
}
New-Item -ItemType Directory -Path $ReleaseDir | Out-Null

# Copy binaries
Write-Host "Copying binaries..."
Copy-Item "build\src\Release\goblin-doom.exe" "$ReleaseDir\"
Copy-Item "build\src\Release\goblin-setup.exe" "$ReleaseDir\"
Copy-Item "build\src\Release\goblin-server.exe" "$ReleaseDir\"

# Copy SDL2 DLLs if they exist (common locations)
$sdl2Paths = @(
    "C:\vcpkg\installed\x64-windows\bin\SDL2.dll",
    "C:\SDL2-2.28.0\lib\x64\SDL2.dll",
    "$env:SDL2_PATH\lib\x64\SDL2.dll"
)

foreach ($path in $sdl2Paths) {
    if (Test-Path $path) {
        Copy-Item $path "$ReleaseDir\"
        Write-Host "Copied SDL2.dll from $path"
    }
}

# Copy SDL2_mixer and SDL2_net if they exist
$sdl2mixerPaths = @(
    "C:\vcpkg\installed\x64-windows\bin\SDL2_mixer.dll",
    "C:\SDL2_mixer-2.6.0\lib\x64\SDL2_mixer.dll"
)

foreach ($path in $sdl2mixerPaths) {
    if (Test-Path $path) {
        Copy-Item $path "$ReleaseDir\"
    }
}

$sdl2netPaths = @(
    "C:\vcpkg\installed\x64-windows\bin\SDL2_net.dll",
    "C:\SDL2_net-2.2.0\lib\x64\SDL2_net.dll"
)

foreach ($path in $sdl2netPaths) {
    if (Test-Path $path) {
        Copy-Item $path "$ReleaseDir\"
    }
}

# Create ZIP archive
Write-Host "Creating archive..."
Set-Location $OutputDir
Compress-Archive -Path $ArchiveName -DestinationPath "$ArchiveName.zip" -Force
Set-Location ".."

Write-Host "Windows release package created: $OutputDir\$ArchiveName.zip"

# List contents
Write-Host "Package contents:"
Get-ChildItem $ReleaseDir | ForEach-Object { Write-Host "  $($_.Name)" }

Write-Host "Done!"
