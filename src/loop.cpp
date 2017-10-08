#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

/// Main Screeps-CPP API
#include <cppreeps.hpp>

/// Exporting lzw_xxcode() to current module
#include <lzw.hpp>

void loop() {
    using namespace utils;
    using namespace screeps;
    
    INIT();
    
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
