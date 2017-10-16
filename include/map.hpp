#pragma once

#include <cppreeps.hpp>

#include <algorithm>
#include <array>
#include <vector>
#include <queue>


namespace screeps { namespace map {
    
    
    enum : int {
        MAP_SIZE = 50,                  // Height == width
        MAP_AREA = MAP_SIZE * MAP_SIZE  // Area (height * width)
    };
    
    
    /// Type of CostMatrix weight, range = [0,255]
    using cost_matrix_weight_type = uint8_t;
    
    
    /// Native CostMatrix
    class cost_matrix {
    private:
        using underlying_type = std::array<cost_matrix_weight_type, MAP_AREA>;
        
        underlying_type data_; // ~Uint8Array (JS)
        
    public:
        cost_matrix() : data_{{0}} {}
        
        
        /// Access:
        
        inline auto at(int x, int y) const -> decltype(data_[x + y]) {
            return data_[x * MAP_SIZE + y]; }
        
        inline auto at(int x, int y) -> decltype(data_[x + y]) {
            return data_[x * MAP_SIZE + y]; }
        
        /// Iterations:
        
        inline auto begin() const -> decltype(data_.begin()) {
            return data_.begin(); }
        
        inline auto begin() -> decltype(data_.begin()) {
            return data_.begin(); }
        
        inline auto end() const -> decltype(data_.end()) {
            return data_.end(); }
        
        inline auto end() -> decltype(data_.end()) {
            return data_.end(); }
        
        
        /// @returns JS typed array (Uint8Array) as emscripten::val
        val to_Uint8Array() const {
            using namespace emscripten;
            return val(typed_memory_view(data_.size(), data_.data()));
        }
        
        /// @returns
        std::string to_string() const {
            std::string res; res.resize(MAP_AREA);
            std::copy(data_.begin(), data_.end(), res.begin());
            return res;
        }
    };
    
    
    /// Default terrain weights
    enum DFLT_COSTS : cost_matrix_weight_type {
        PLAIN   = 1,
        SWAMP   = 4,
        WALL    = 255
    };
    
    
    /// @returns reference to cached terrain matrix
    cost_matrix const& get_terrain_matrix(std::string const& room_name) {
        using namespace std;
        using terrain_cache_map = unordered_map<string, cost_matrix>;
        
        static terrain_cache_map terrain_ = []{
            terrain_cache_map res; res.reserve(128);
            return res;
        }();
        
        auto it = terrain_.find(room_name);
        
        // Update cache
        if(it == terrain_.end()) {
            cost_matrix res;
        
            auto getTerrainAt = tick->Game["map"]["getTerrainAt"];
            string terr;
            
            for(int x = 0; x < MAP_SIZE; ++x) {
                for(int y = 0; y < MAP_SIZE; ++y) {
                    terr = getTerrainAt(x, y, room_name).as<string>();
                    auto& weight = res.at(x, y);
                    
                    if(terr == "wall")
                        weight = DFLT_COSTS::WALL;
                    else if(terr == "swamp")
                        weight = DFLT_COSTS::SWAMP;
                    else // "plain"
                        weight = DFLT_COSTS::PLAIN;
                }
            }
            
            it = terrain_.emplace(room_name, res).first;
        }
        
        return it->second;
    }
    
    
    val distance_transform(std::string const& room_name) {
        cost_matrix res = get_terrain_matrix(room_name);
        return res.to_Uint8Array();
        
        constexpr int OOB = DFLT_COSTS::WALL;
        
        for(auto& w : res) // inverse
            w = (w == DFLT_COSTS::WALL ? 0 : DFLT_COSTS::WALL);
        
        for(int x = 1; x < MAP_SIZE; ++x) {
            for(int y = 1; y < MAP_SIZE; ++y) {
                res.at(x, y) = std::min(std::initializer_list<int>{
                    res.at(x  , y  ),
                    res.at(x  , y-1) + 1,
                    res.at(x-1, y  ) + 1,
                    res.at(x-1, y-1) + 1,
                    (x != MAP_SIZE-1 ? res.at(x+1, y-1) + 1 : OOB)
                });
            }
        }
        
        for(int x = MAP_SIZE-1; x-- > 0;) {
            for(int y = MAP_SIZE-1; y-- > 0;) {
                res.at(x, y) = std::min(std::initializer_list<int>{
                    res.at(x  , y  ),
                    res.at(x  , y+1) + 1,
                    res.at(x+1, y  ) + 1,
                    res.at(x+1, y+1) + 1,
                    (x != 0 ? res.at(x-1, y+1) + 1 : OOB)
                });
            }
        }
        
        return res.to_Uint8Array();
    }
    
    
    
    
    EMSCRIPTEN_BINDINGS(dt_bindings) {
        /// Distance Transform main function
        emscripten::function("distance_transform", &distance_transform);
    }
    
} // map
} // screeps
