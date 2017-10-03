#!/bin/bash

mkdir -p dist
mkdir -p build

em++ \
    --std=c++11 \
    --bind \
    -s BINARYEN_ASYNC_COMPILATION=0 \
    -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORTED_FUNCTIONS='[]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -I include \
    -O3 -Wall -pedantic \
    src/loop.cpp -o build/loop.js

cp build/loop.wasm      dist/
cp build/loop.js        dist/loop_mod.js
cp src/main.js          dist/
cp src/wasm_loader.js   dist/
