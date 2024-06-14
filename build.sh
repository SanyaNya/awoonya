#!/bin/sh

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=$1 -S . -B build
cmake --build build
mv build/compile_commands.json .
