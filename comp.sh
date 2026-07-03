#!/bin/bash
set -e

echo "Removing old build folder..."
rm -rf build

echo "Configuring CMake..."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo "Building project..."
cmake --build build -j$(nproc)

echo "Build finished successfully."
