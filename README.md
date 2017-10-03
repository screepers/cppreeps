# CPPREEPS

Example of C++ API, bindings and utilities for [Screeps](https://screeps.com/) game based on Emscripten [Embind](http://kripken.github.io/emscripten-site/docs/api_reference/bind.h.html) and [`val`](http://kripken.github.io/emscripten-site/docs/api_reference/val.h.html) tools.

## Getting started

1. Install `emsdk` ([guide](http://webassembly.org/getting-started/developers-guide/)).
    
    > NOTE: it may take a long time to build toolchain for Emscripten compiler, and there are known issues with memory overflow while compiling and linking LLVM libraries. Please, see official [Emscripten](https://kripken.github.io/emscripten-site/index.html) and [WASM](http://webassembly.org/docs/high-level-goals/) documentation.
    
2. Prepare building environment:
    * with `$ source path_to_emsdk_dir/emsdk_env.sh` (Win: `emsdk_env.bat`)
    * OR by configuring own build system (see [example](https://kripken.github.io/emscripten-site/docs/compiling/Building-Projects.html)).
    
3. Build project using `em++` to JS module (`.wasm` and `.js` files):
    * `$ ./create.sh` (see script file example)
    * OR using your own building system (`make` etc.)
    
4. Pull generated files to Screeps/PTR (using `grunt`, `gulp`, or whatever).
    
    > NOTE: WASM is an experimental feature, and for now it only available on [PTR](http://docs.screeps.com/ptr.html).
    
    > NOTE: for now binary GUI uploading isn't implemented yet.

See `src/loop.cpp`, `src/main.js` for WASM usage examples.

**_UNDER CONSTRUCTION_**

***

## Utility pack:

### LZW string encoder/decoder
Header-only library `lzw.hpp` contains implementation of original [LZW](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch) compression/decompression algorithms. Features:

* Initial dictionary can be chosen by range of character codes.
    
    _Default_: 0-128, standard ASCII Chart _aka_ Unicode.Basic_Latin.
    
* Underlying string type can be chosen by standard C++ STL typedef ([Embind limitations](http://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html#built-in-type-conversions)).
    
    _Default_: `std::string`. Not tested for now: `std::wstring`.
    
* Possibility to use static cached buffers (may increase performance).
    
    _Default_: `USE_CACHED_BUFFERS = false` to avoid data races in multithreaded environment.
    
* Encoding with fixed bit depth (choses by maximum used code).
    
* Dense bit packing (uses binary operations with resulting string bytes).
    
* Compile time tuning to avoid UB while binary packing/unpacking with signed character types, so library _seems to be_ portable and stable :)
    
* Header-only and STL-only open source.


#### Native performance test (1 KIB string):

```
LZW encode = 94 us/KIB, decode = 94 us/KIB
ZIP ratio = 0.0527344 (enc/src) str="AAAAAAAAAAAAAAAA..."
LZW encode = 117 us/KIB, decode = 20 us/KIB
ZIP ratio = 0.538086 (enc/src) str="{"key1":42, "key2":"val1"..." (~JSON)
LZW encode = 151 us/KIB, decode = 16 us/KIB
ZIP ratio = 1.25977 (enc/src) str="v21ny6E5624VjTk8..." (random)
```

#### JS performance tests, `lzw_xxcode(RawMemory.get())`, ~600 KIB:

```
LZW encode = 167.497 CPU, 244.144 CPU/MIB
           => ratio = 0.229, len = 597102
LZW decode = 185.010 CPU, 1413.525 CPU/MIB
```

Library can be exported to JS using [Emscripten Embind API](http://kripken.github.io/emscripten-site/docs/api_reference/bind.h.html):

```cpp
EMSCRIPTEN_BINDINGS(lzw) {
    emscripten::function("zlw_encode", &lzw::lzw_encode);
    emscripten::function("zlw_decode", &lzw::lzw_decode);
}
```

Example of usage from JS (see sources for complete examples):

```javascript
const src = "Ololo, some string!";
const enc = mod.zlw_encode(src);
const dec = mod.zlw_decode(enc);
assert(src == dec);
```

***

Thanks @ags131 for his examples and initial experiments =)