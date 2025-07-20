#!/bin/bash

# Build script for Base64 Decoder

echo "Building Base64 Decoder..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release

echo "Build complete!"

# Platform-specific instructions
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS: Executable is in build/Base64Decoder.app/Contents/MacOS/Base64Decoder"
    echo "Or run: open Base64Decoder.app"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    echo "Windows: Executable is in build/Release/Base64Decoder.exe"
else
    echo "Linux: Executable is in build/Base64Decoder"
fi