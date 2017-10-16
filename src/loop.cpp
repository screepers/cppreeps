#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

/// Main Screeps-CPP API
#include <cppreeps.hpp>

/// Exporting lzw_xxcode() to current module
#include <lzw.hpp>

/// Exporting Distance Transform lib to current module
#include <map.hpp>


void loop() {
    using namespace utils;
    using namespace screeps;
    
    RECACHE_TICK();
    
    std::printf("Updated Game.time = %d\n",
        tick->Game["time"].as<int>());
    
    std::printf("RawMemory.length = %d\n",
        tick->RawMemory.call<val>("get")["length"].as<int>());
    
    std::printf("[OK, MOVE, RESOURCE_ENERGY] = [%d, %s, %s]\n",
        gCONST("OK").as<int>(),
        gCONST("MOVE").as<std::string>().c_str(),
        gCONST("RESOURCE_ENERGY").as<std::string>().c_str());
    
    EM_ASM({
        console.log("ASM: Game time  = " + Game.time);
        console.log("ASM: Creeps num = " + _.size(Game.creeps));
    });
    
    auto creeps_map = js_object_to_map(tick->Game["creeps"]);
    for(auto const& kv : creeps_map) {
        auto const& name  = kv.first;
        auto const& creep = kv.second;
        creep.call<int>("say", name);
    }
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

#include <cstring>
void test(unsigned ptr_num) {
    auto ptr = reinterpret_cast<char*>(ptr_num);
    std::printf("got ptr=%p, str=%s\n", (void*)(ptr), ptr);
    std::strcpy(ptr, "Modified!");
    std::printf("ret ptr=%p, str=%s\n", (void*)(ptr), ptr);
}
EMSCRIPTEN_BINDINGS(test) {
    emscripten::function("test", &test, emscripten::allow_raw_pointers());
}

