'use strict';

const wasm_loader = require('wasm_loader')

/// Module by XXX.js and XXX.wasm files
const mod = wasm_loader('loop_mod', 'loop');

global.byteLength = function(str) {
    str = str + "";
    let len = str.length;
    for(let i = str.length; i-- > 0;) {
        const code = str.charCodeAt(i);
        if      (code > 0x7f    && code <=  0x7ff) ++len;
        else if (code > 0x7ff   && code <= 0xffff) len += 2;
        if      (code >= 0xDC00 && code <= 0xDFFF) --i; //trail surrogate
    }
    return len;
};

module.exports.loop = function(){
    let t = Game.cpu.getUsed();
    mod.loop();
    let dt = Game.cpu.getUsed() - t;
    console.log(`Preload = ${t} CPU`);
    console.log(`Loop = ${dt} CPU`);
    
    if(1) {
        const src = "ololo, string for lzw, TOBEORNOTTOBEORTOBEORNOT";
        t = Game.cpu.getUsed();
        let enc = mod.lzw_encode(src);
        dt = Game.cpu.getUsed() - t;
        
        console.log(`LZW encode = ${dt} CPU, ${1048576*dt/src.length} CPU/MIB`);
        console.log(`           => {${enc}}, k = ${enc.length/src.length}`);
        
        t = Game.cpu.getUsed();
        let dec = mod.lzw_decode(enc);
        dt = Game.cpu.getUsed() - t;
        
        console.log(`LZW decode = ${dt} CPU, ${1048576*dt/enc.length} CPU/MIB`);
        console.log(`           => {${dec}}`);
        
        // Serialization test
        if(1) {
            const mem = Memory.__lz || (Memory.__lz = {});
            mem.test = "Some unicode: あああ😀😀😀";
            mem.src = RawMemory.get().substring(0, 1024);
            
            const tick = Game.time;
            if(tick % 2 == 0 && mem.src) {
                mem.enc = mod.lzw_encode(mem.src);
                
                mem.enc_codes = [];
                for(let i in mem.enc)
                    mem.enc_codes.push(mem.enc.charCodeAt(i));
                
            } else if(mem.enc) {
                mem.dec = mod.lzw_decode(mem.enc);
                
                mem.dec_codes = [];
                for(let i in mem.dec)
                    mem.dec_codes.push(mem.dec.charCodeAt(i));
            }
            
            _.forEach([mem.src, mem.enc, mem.dec], (s)=>{
                if(s) console.log(`length=${s.length}, bytes=${byteLength(s)}: "${s.substring(0, 16)}..."\n`);
            });
            
            console.log(`${mem.dec == mem.src}\n`);
            console.log(`length ratio = ${(mem.enc.length/mem.src.length).toFixed(3)}`);
            console.log(`bytesz ratio = ${(byteLength(mem.enc)/byteLength(mem.src)).toFixed(3)}`);
        }
        
        // RawMemory.zip
        if(0) {
            let src = RawMemory.get();
            src = src.substring(0, src.length/2);
            t = Game.cpu.getUsed();
            let enc = mod.lzw_encode(src);
            dt = Game.cpu.getUsed() - t;
            
            console.log(`LZW encode = ${dt} CPU, ${1048576*dt/src.length} CPU/MIB`);
            console.log(`           => k = ${enc.length/src.length}, len = ${src.length}`);
            
            t = Game.cpu.getUsed();
            let dec = mod.lzw_decode(enc);
            dt = Game.cpu.getUsed() - t;
            
            console.log(`LZW decode = ${dt} CPU, ${1048576*dt/enc.length} CPU/MIB`);
        }
    }
    
    console.log(`END = ${Game.cpu.getUsed().toFixed(3)} CPU`);
}
