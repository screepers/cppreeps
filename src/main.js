'use strict';

const wasm_loader = require('wasm_loader')

/// Module by XXX.js and XXX.wasm files
const mod = wasm_loader('loop_mod', 'loop');

module.exports.loop = function(){
    let t = Game.cpu.getUsed();
    mod.loop();
    let dt = Game.cpu.getUsed() - t;
    console.log(`Preload = ${t} CPU`);
    console.log(`Loop = ${dt} CPU`);
    
    if(1) {
        const src = "ololo, string for lzw, TOBEORNOTTOBEORTOBEORNOT"
        t = Game.cpu.getUsed();
        let enc = mod.zlw_encode(src);
        dt = Game.cpu.getUsed() - t;
        
        console.log(`LZW encode = ${dt} CPU, ${1048576*dt/src.length} CPU/MIB`);
        console.log(`           => {${enc}}, k = ${enc.length/src.length}`);
        
        t = Game.cpu.getUsed();
        let dec = mod.zlw_decode(enc);
        dt = Game.cpu.getUsed() - t;
        
        console.log(`LZW decode = ${dt} CPU, ${1048576*dt/enc.length} CPU/MIB`);
        console.log(`           => {${dec}}`);
        
        // RawMemory.zip
        if(0) {
            let src = RawMemory.get();
            src = src.substring(0, src.length/2);
            t = Game.cpu.getUsed();
            let enc = mod.zlw_encode(src);
            dt = Game.cpu.getUsed() - t;
            
            console.log(`LZW encode = ${dt} CPU, ${1048576*dt/src.length} CPU/MIB`);
            console.log(`           => k = ${enc.length/src.length}, len = ${src.length}`);
            
            t = Game.cpu.getUsed();
            let dec = mod.zlw_decode(enc);
            dt = Game.cpu.getUsed() - t;
            
            console.log(`LZW decode = ${dt} CPU, ${1048576*dt/enc.length} CPU/MIB`);
        }
    }
    
    console.log(`END = ${Game.cpu.getUsed().toFixed(3)} CPU`);
}
