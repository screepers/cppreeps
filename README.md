# CPPREEPS

Example of C++ API, bindings and utilities for [Screeps](https://screeps.com/) game. Native C++ source code can be compiled to [WebAssembly (WASM)](http://webassembly.org/) format using [Emscripten](http://kripken.github.io/emscripten-site/docs/api_reference/bind.h.html) compiler.

> WebAssembly (or wasm) is a low-level bytecode format for in-browser client-side scripting, evolved from JavaScript. Its initial aim is to support compilation from C and C++, though other source languages such as Rust are also supported... [WIKI](https://en.wikipedia.org/wiki/WebAssembly)

> Emscripten is an Open Source LLVM to JavaScript compiler. Using Emscripten you can: compile C and C++ code into JavaScript, compile any other code that can be translated into LLVM bitcode into JavaScript, compile the C/C++ runtimes of other languages into JavaScript, and then run code in those other languages in an indirect way (this has been done for Python and Lua)!.. [DOCS](http://kripken.github.io/emscripten-site/docs/introducing_emscripten/about_emscripten.html)

WASM modules (`.wasm` files) can be relatively easily loaded within `.js` code. After that, native C++ functions can be called from JS, example:

```javascript
const wasm_loader = require("wasm_loader");
const module = wasm_loader("js_filename", "wasm_filename");
/* * * * */
let result = module.native_function(arg1, arg2);
let some_sinus = module.sin(9001);
let packed_mem = module.lzw_encode(RawMemory.get());
let path = module.find_path(creep1, creep2);
```

## Getting started (_under construction_)

1. Download and install `emsdk` (Emscripten SDK, compiler toolchain): [guide](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).
    
    > NOTE: the simplest way to get started with WASM is to use **portable** `emsdk` package: it easy to install and work with, just follow installation guide *very carefully*.
    
    This step briefly (Linux/iOS syntax):
    
    ```bash
    $ #Download emsdk-portable.tar.gz && Unpack archive
    $ ./emsdk update
    $ ./emsdk install latest
    $ ./emsdk activate latest
    ```
    
2. Prepare building environment:
    * Linux/iOS: `$ source ./emsdk_env.sh`
    * Windows: `> emsdk_env.bat`
    * OR by configuring own building system (see [official example](https://kripken.github.io/emscripten-site/docs/compiling/Building-Projects.html)).
        
        > NOTE: configuring own building system (cmake + make etc.) is **the most painful way** to get started with Emscripten, so **we really need help here** to complete this guide =)
    
3. Build project using `em++` to WASM module (pair of `.wasm` and `.js` files):
    * Linux/iOS: `$ ./create.sh` (see script file example)
    * Windows: `> create.bat` (see script file example)
    * OR using your own building system (`make` etc.)
    
4. Push generated files from `/dist` folder to Screeps/PTR (using `grunt`, `gulp`, or whatever), see Screeps docs: [1](http://docs.screeps.com/commit.html#Using-Grunt-task), [2](https://screeps.com/forum/post/8464).
    
    > NOTE: WASM is an experimental feature, and for now it only available on [PTR](http://docs.screeps.com/ptr.html).
    
    > NOTE: for now binary GUI uploading isn't implemented yet.

See `src/loop.cpp`, `src/main.js` for WASM usage examples.

Current repo can be cloned by: `git clone --recursive git://***.git`

Submodules updating can be performed by: `git submodule update --init --recursive`

***

# Utility pack:

## LZW-based codec (_[submodule](https://github.com/Mototroller/ax.lzw)_) [![Build Status](https://travis-ci.org/Mototroller/ax.lzw.svg?branch=master)](https://travis-ci.org/Mototroller/ax.lzw)
Header-only library **lib/lzw/lzw.hpp** contains implementation of original [LZW](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch) compression/decompression algorithms. It's a submodule, see my [source repo](https://github.com/Mototroller/ax.lzw) for details.

Header file **include/lzw.hpp** contains wrapper around `lzw_codec<ASCII_128_common, UTF16_pack>` codec functions and `EMSCRIPTEN_BINDINGS(lzw){...}` block exporting them to `WASM` module making them easy to use. Their native signatures:

```cpp
std::wstring lzw_encode(std::wstring in);
std::wstring lzw_decode(std::wstring in);
```

...and example of JS usage (see sources for other examples):

```javascript
const src = "Ololo, some string!";
const enc = mod.lzw_encode(src);
const dec = mod.lzw_decode(enc);
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