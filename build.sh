#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build