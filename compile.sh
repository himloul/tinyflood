#!/bin/bash

# Configuration
gameName="flooder"
loveExePath="/c/Program Files/LOVE/love.exe"
gameFolderPath="$(pwd)"
outputPath="$(pwd)/dist"

# Create output directory if it doesn't exist
mkdir -p "$outputPath"

# Create .love file
zip -9 -r "$outputPath/$gameName.love" . -x "*.git*" "dist/*" "*.ps1"

# Copy LÖVE executable
cp "$loveExePath" "$outputPath/$gameName.exe"

# Append .love file to the executable
cat "$outputPath/$gameName.love" >> "$outputPath/$gameName.exe"

# Copy required DLLs and license
dllsToCopy=("SDL2.dll" "OpenAL32.dll" "love.dll" "lua51.dll" "mpg123.dll" "msvcp120.dll" "msvcr120.dll")
for dll in "${dllsToCopy[@]}"; do
    cp "/c/Program Files/LOVE/$dll" "$outputPath"
done
cp "/c/Program Files/LOVE/license.txt" "$outputPath"

# Clean up
rm "$outputPath/$gameName.love"

echo "Game compiled successfully to $outputPath/$gameName.exe"
