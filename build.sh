#!/bin/bash

VERSION=$1

echo "Building in $MODE mode..."
gcc -std=c2x -Wall -Wextra -D$VERSION -O3 -o main platform/main.c $(pkg-config --libs --cflags raylib)

echo "Build complete."
