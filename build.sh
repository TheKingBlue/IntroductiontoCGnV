#!/bin/bash

# Create "build" directory (unless it already exists).
mkdir -p build

# Change to the "build" directory (or exit if that fails).
cd build || exit

# Create Makefiles for a release build.
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project using Make. The "4" indicates the number of jobs for building in parallel.
make -j4
