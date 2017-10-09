@echo off
if not exist "build\" mkdir build
if not exist "dist\"  mkdir dist

cmd /C emcc --std=c++11 --bind -s WASM=1 -s MODULARIZE=1 -s ALLOW_MEMORY_GROWTH=1 -Iinclude -Ilib -O3 -Wall -pedantic src\loop.cpp -o build\loop.js

copy build\loop.wasm    dist\
copy build\loop.js      dist\loop_mod.js
copy src\main.js        dist\
copy src\wasm_loader.js	dist\
