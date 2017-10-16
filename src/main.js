'use strict';

/// Utils library
const wasm_utils = require('wasm_utils')

/// Module by XXX.js and XXX.wasm files
const mod = wasm_utils.load_module('loop_mod', 'loop');


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


function duration(f, ...args) {
    const start = Game.cpu.getUsed();
    const ret = f(...args);
    return [ret, Game.cpu.getUsed() - start];
}


function displayCostMatrix(costMatrix, color = '#ff0000') {
    var vis = new RoomVisual();
    
    var bounds = [0, 50];
    
    var max = 1;
    for (var y = bounds[0]; y < bounds[1]; ++y) {
        for (var x = bounds[0]; x < bounds[1]; ++x) {
            max = Math.max(max, costMatrix.get(x, y));
        }
    }

    for (var y = bounds[0]; y < bounds[1]; ++y) {
        for (var x = bounds[0]; x < bounds[1]; ++x) {
            var value = costMatrix.get(x, y);
            if (value > 0) {
                vis.circle(x, y, {radius:value/max/2, fill:color});
                vis.text(value, x, y + 0.3, { color: '#666666' });
            }
        }
    }
}


module.exports.loop = function(){
    let t = Game.cpu.getUsed();
    let dt = duration(mod.loop);
    console.log(`Preload = ${t} CPU`);
    console.log(`Loop = ${dt} CPU`);
    
    // LZW
    if(1) {
        let src = "ololo, string for lzw, TOBEORNOTTOBEORTOBEORNOT";
        let enc, dec;
        
        [enc, dt] = duration(mod.lzw_encode, src);
        
        console.log(`LZW encode = ${dt} CPU, ${1048576*dt/src.length} CPU/MIB`);
        console.log(`           => {${enc}}, k = ${enc.length/src.length}`);
        
        [dec, dt] = duration(mod.lzw_decode, enc);
        
        console.log(`LZW decode = ${dt} CPU, ${1048576*dt/enc.length} CPU/MIB`);
        console.log(`           => {${dec}}`);
        
        // Serialization test
        if(1) {
            const mem = Memory.__lz || (Memory.__lz = {});
            mem.test = "Some unicode: あああ😀😀😀";
            mem.src = RawMemory.get().substring(0, 128);
            
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
    }
    
    
    // DT
    if(1) {
        for(let sname in Game.spawns) {
            const rname = Game.spawns[sname].room.name;
            let path, map, bits, dt;
            
            [path,dt] = duration(()=>{
                return PathFinder.search(
                    Game.spawns.Base2.pos,
                    Game.spawns.Base3.pos);
            });
            console.log(`PF ${Game.spawns.Base2.room.name+'->'+Game.spawns.Base3.room.name}, time = ${dt.toFixed(3)} CPU, len = ${path.path.length}`);
            
            [map,dt] = duration(()=>{
                const map = new Uint8Array(2500);
                for(let x = 0; x < 50; ++x) {
                    for(let y = 0; y < 50; ++y) {
                        const offset = 50*x + y;
                        switch(Game.map.getTerrainAt(x,y,rname)) {
                            case 'wall':  map[offset] = 255; break;
                            case 'swamp': map[offset] =   4; break;
                            default: map[offset] = 1;
                        }
                    }
                }
                return map;
            });
            console.log(`TERRAIN ${rname}, time = ${dt.toFixed(3)} CPU`);
            
            [bits, dt] = duration(mod.distance_transform, rname);
            //console.log(`ret = ${JSON.stringify(bits)}`);
            
            const m = new PathFinder.CostMatrix();
            //console.log(`m = ${JSON.stringify(m)}`);
            
            m._bits = bits;
            //console.log(`m = ${JSON.stringify(m)}`);
            
            m.set(25, 25, 10);
            displayCostMatrix(m);
            console.log(`DT AT ${rname}, time = ${dt.toFixed(3)} CPU`);
            
            console.log(`MAPS ${rname}, eq = ${map == bits}`);
            
            break;
        }
    }
    
    
    // Memory
    if(1) {
        
        [32, 128, 1024, 1024*32].forEach((sz) => {
            const jsArr = new Uint8Array(sz);
            let [ret, dur] = duration(wasm_utils.share_to_heap, jsArr, mod);
            console.log(`share_to_heap: ${sz}, ${dur.toFixed(3)} CPU`);
            [,dur] = duration(ret.free);
            console.log(`         free: ${sz}, ${dur.toFixed(3)} CPU`);
        });
        
        const str = "Initial string";
        const initArray = new Int8Array(32);
        console.log(`${initArray}`);
        
        const ret = wasm_utils.share_to_heap(initArray, mod);
        console.log(`${ret.typedArray}`);
        
        mod.test(ret.typedArray.byteOffset);
        console.log(`${ret.typedArray}`);
        
        ret.free();
    }
    
    
    console.log(`END = ${Game.cpu.getUsed().toFixed(3)} CPU`);
}
