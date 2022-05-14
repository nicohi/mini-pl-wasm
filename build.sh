#!/usr/bin/env bash

rm -r build
mkdir -p build
cd build
cmake ../
cmake --build .
