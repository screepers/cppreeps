# CPPREEPS

Example of C++ API, bindings and utilities for [Screeps](https://screeps.com/) game based on Emscripten [Embind](http://kripken.github.io/emscripten-site/docs/api_reference/bind.h.html) and [`val`](http://kripken.github.io/emscripten-site/docs/api_reference/val.h.html) tools.

## Getting started (_under construction_)

1. Install `emsdk` ([guide](http://webassembly.org/getting-started/developers-guide/)).
    
    > NOTE: it may take a long time to build toolchain for Emscripten compiler, and there are known issues with memory overflow while compiling and linking LLVM libraries. Please, see official [Emscripten](https://kripken.github.io/emscripten-site/index.html) and [WASM](http://webassembly.org/docs/high-level-goals/) documentation.
    
2. Prepare building environment:
    * `$ source path_to_emsdk_dir/emsdk_env.sh` (Win: `emsdk_env.bat`)
    * OR by configuring own build system (see [example](https://kripken.github.io/emscripten-site/docs/compiling/Building-Projects.html)).
    
3. Build project using `em++` to JS module (`.wasm` and `.js` files):
    * `$ ./create.sh` (see script file example)
    * OR using your own building system (`make` etc.)
    
4. Pull generated files to Screeps/PTR (using `grunt`, `gulp`, or whatever).
    
    > NOTE: WASM is an experimental feature, and for now it only available on [PTR](http://docs.screeps.com/ptr.html).
    
    > NOTE: for now binary GUI uploading isn't implemented yet.

See `src/loop.cpp`, `src/main.js` for WASM usage examples.

***

# Utility pack:

## LZW-based codec (_[submodule](https://github.com/Mototroller/ax.lzw)_) [![Build Status](https://travis-ci.org/Mototroller/ax.lzw.svg?branch=master)](https://travis-ci.org/Mototroller/ax.lzw)
Header-only library **lib/lzw/lzw.hpp** contains implementation of original [LZW](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch) compression/decompression algorithms. It's a submodule, see my [source repo](https://github.com/Mototroller/ax.lzw) for details.

Header file **include/lzw.hpp** contains `EMSCRIPTEN_BINDINGS(lzw){...}` block exporting two codec functions to `WASM` module making them easy to use. Their native signatures:

```cpp
std::wstring lzw_encode(std::wstring in);
std::wstring lzw_decode(std::wstring in);
```

...and example of JS usage (see sources for other examples):

```javascript
const src = "Ololo, some string!";
const enc = mod.zlw_encode(src);
const dec = mod.zlw_decode(enc);
// assert(src == dec);
```


### Native performance test (1 KIB string):

```
LZW encode = 94 us/KIB, decode = 94 us/KIB
ZIP ratio = 0.0527344 (enc/src) str="AAAAAAAAAAAAAAAA..."
LZW encode = 117 us/KIB, decode = 20 us/KIB
ZIP ratio = 0.538086 (enc/src) str="{"key1":42, "key2":"val1"..." (~JSON)
LZW encode = 151 us/KIB, decode = 16 us/KIB
ZIP ratio = 1.25977 (enc/src) str="v21ny6E5624VjTk8..." (random)
```

### JS performance tests, `lzw_xxcode(RawMemory.get())`, ~600 KIB:

```
LZW encode = 167.497 CPU, 244.144 CPU/MIB
           => ratio = 0.229, len = 597102
LZW decode = 185.010 CPU, 1413.525 CPU/MIB
```

### In-game usage result:

```
Memory view:

1.1 KB
src :   {\"stats\":{\"profiler.findInRange\":0.06027972683740784,..."

0.3 KB
enc :  **࢛ກҔຣ࢓ݡ䂠๣ᢏൃᒌ๣㑎෤⒄෢Ѳ෣ᒇᄃ㡐ء塐݁呕ځ呒ۡ䁘݁䣌ځじѡ䂃...
```


***

Thanks @ags131, @primus, @tedivm for their examples and experience =)