#pragma once

/// Header file contains Emscripten bindings of lzw.hpp:
///  - std::wstring lzw_encode(std::wstring in);
///  - std::wstring lzw_decode(std::wstring in);

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

/// Standalone hpp implementation file (lib/...)
#include <lzw/lzw.hpp>

namespace screeps {

    std::wstring lzw_encode(std::wstring in) {
        using codec = lzw::codecs::string_to_UTF16;
        
        std::wstring out;
        out.reserve(in.size());
        codec::encode(in.begin(), in.end(), std::back_inserter(out));
        return out;
    }

    std::wstring lzw_decode(std::wstring in) {
        using codec = lzw::codecs::string_to_UTF16;
        
        std::wstring out;
        out.reserve(in.size()*2);
        codec::decode(in.begin(), in.end(), std::back_inserter(out));
        return out;
    }

    EMSCRIPTEN_BINDINGS(lzw) {
        emscripten::function("zlw_encode", &lzw_encode);
        emscripten::function("zlw_decode", &lzw_decode);
    }
    
}
