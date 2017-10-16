'use strict';

/// @returns WASM module object by *.js and *.wasm files
function load_module(mod_js, mod_wasm, opts) {
    
    const mod_file = require(mod_js);
    const bin_file = require(mod_wasm);
    
    opts = opts || {};
    
    opts.ENVIRONMENT = 'SHELL';
    
    opts.wasmBinary = bin_file;
    opts.print      = opts.print    || ((text) => console.log(`[STDOUT]: ${text}`));
    opts.printErr   = opts.printErr || ((text) => console.log(`[STDERR]: ${text}`));
    opts.onAbort    = opts.onAbort  || (() => console.log(`[ABORT]: WASM Aborted!`));
    
    // == don't call main()
    if(typeof opts.noInitialRun === "undefined")
        opts.noInitialRun = true;
    
    // == don't terminate after returning from main()
    if(typeof opts.noExitRuntime === "undefined")
        opts.noExitRuntime = true;
    
    return mod_file(opts);
}

function heap_for_array(typedArray, Module) {
    switch(typedArray.BYTES_PER_ELEMENT) {
        case 1:
            if(typedArray instanceof Int8Array )        return Module.HEAP8;
            if(typedArray instanceof Uint8Array)        return Module.HEAPU8;
            if(typedArray instanceof Uint8ClampedArray) return Module.HEAPU8;
        case 2:
            if(typedArray instanceof Int16Array )       return Module.HEAP16;
            if(typedArray instanceof Uint16Array)       return Module.HEAPU16;
        case 4:
            if(typedArray instanceof Int32Array )       return Module.HEAP32;
            if(typedArray instanceof Uint32Array)       return Module.HEAPU32;
            if(typedArray instanceof Float32Array)      return Module.HEAPF32;
        case 8:
            if(typedArray instanceof Float64Array)      return Module.HEAPF64;
        default:
            return null; // typedArray is invalid
    }
}

/// @returns TODO
function share_to_heap(typedArray, Module) {
    let heap = heap_for_array(typedArray, Module);
    if(!heap) {
        // DANGER ZONE: implicit conversion to Uint32Array
        typedArray = new Uint32Array(typedArray.buffer || typedArray);
        heap = heap_for_array(typedArray, Module);
    }
    
    const ctor = typedArray.constructor; // for cloning
    const numBytes = typedArray.length * typedArray.BYTES_PER_ELEMENT;
    
    // WASM memory allocation
    const ptr = Module._malloc(numBytes);
    
    // Creation of new memory view pointing to allocated space
    const heapArray = new ctor(heap.buffer, ptr, typedArray.length);
    
    // Data copying
    heapArray.set(typedArray);
    //heapArray.set(new ctor(typedArray.buffer)); // TODO: what the right way?
    
    return {
        typedArray: heapArray,                  // typedArray object
        free:       (()=>(Module._free(ptr)))   // deallocation function
    };
    
    return heapArray;
}

module.exports = {
    load_module:    load_module,
    share_to_heap:  share_to_heap
};