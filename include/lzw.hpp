#pragma once

#include <algorithm>
#include <climits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>


namespace lzw {
    
    /// Large unsigned type
    using size_t = std::size_t;
    
    /// String type to be used as lzw I/O storage
    using string_type = std::string;
    
    /// Default dictionary bounds
    enum : size_t {
        DFLT_DICT_FROM  = 0,
        DFLT_DICT_TO    = 128
    };
    
    enum : bool {
        /// Force to use static buffers, UB if access is multithreaded (concurrent)
        USE_CACHED_BUFFERS = false,
        
        // Force to use all bits of signed byte to pack data, UB in case of signed "char"
        UNSAFE_SIGNED_PACK = false,
    };
    
    
    /// Implementation details
    namespace details {
        
        using char_type     = string_type::value_type;              // underlying string character
        using uchar_type    = std::make_unsigned<char_type>::type;  // == safe byte type
        
        using en_dict_type  = std::unordered_map<string_type, size_t>;  // map {phrase, code}
        using de_dict_type  = std::vector<string_type>;                 // vector {phrase}
        using codes_vec     = std::vector<size_t>;                      // vector {codes}
        
        enum : size_t {
            DFLT_DICT_SIZE  = DFLT_DICT_TO - DFLT_DICT_FROM,
            BITS_IN_CHAR    = CHAR_BIT,
            ALLOWED_BITS    = std::is_unsigned<char_type>::value || UNSAFE_SIGNED_PACK ?
                              BITS_IN_CHAR : (BITS_IN_CHAR - 1)
        };
        
        
        /// @returns cached copy of encoding dictionary
        en_dict_type ascii_encode_dict() {
            static const en_dict_type d_ = []{
                en_dict_type dict; dict.reserve(DFLT_DICT_SIZE);
                for(size_t code = 0; code < DFLT_DICT_SIZE; ++code)
                    dict.emplace(string_type(1, char_type(code + DFLT_DICT_FROM)), code);
                return dict;
            }();
            return d_;
        }
        
        /// @returns cached copy of decoding dictionary (reversed encoding dictionary)
        de_dict_type ascii_decode_dict() {
            static const de_dict_type d_ = []{
                de_dict_type rdict; rdict.reserve(DFLT_DICT_SIZE);
                for(size_t code = 0; code < DFLT_DICT_SIZE; ++code)
                    rdict.emplace_back(string_type(1, char_type(code + DFLT_DICT_FROM)));
                return rdict;
            }();
            return d_;
        }

        /// @returns ~floor(log2(x)) == number of bits to represent "x"
        inline size_t bits_needed(size_t x) {
            size_t res = 1;
            while((x >>= 1)) ++res;
            return res;
        }

        void pack_bits(string_type& dst, codes_vec const& src, size_t bit_depth) {
            const size_t bits_needed = bit_depth*src.size();
            const size_t payload_len = (bits_needed - 1)/ALLOWED_BITS + 1; // ceil
            const size_t mod = payload_len*ALLOWED_BITS - bits_needed;
            
            // len*[payload] + [depth][mod]
            dst.clear(); dst.resize(payload_len + 2, char_type{0});
            
            size_t idx = 0;     // current symbol index
            size_t offset = 0;  // offset of next free bits to store code
            
            // Example, depth=11:
            // [11111111][11122222][222222..][........][........][........][........]
            // [11111111][11122222][22222233][........][........][........][........]
            // [11111111][11122222][22222233][33333333][........][........][........]
            // [11111111][11122222][22222233][33333333][3.......][........][........]
            
            // cycle over codes
            for(size_t code : src) {
                
                // cycle over code bits
                for(size_t code_done = 0; code_done < bit_depth;) {
                    
                    size_t free_bits = ALLOWED_BITS - offset;
                    size_t code_left = bit_depth - code_done;
                    
                    size_t bits_to_write = std::min(free_bits, code_left);
                    
                    uchar_type mask = (code >> code_done);  // abcd|ef  => abcd
                    mask &= (1UL << bits_to_write) - 1UL;   // abcd     => 00cd
                    mask <<= offset;                        // 00cd     => 00cd0000
                    
                    uchar_type data = dst[idx];
                    dst[idx] = data | mask;
                    
                    code_done += bits_to_write;
                    offset    += bits_to_write;
                    
                    if(offset == ALLOWED_BITS)
                        ++idx, offset = 0;
                }
            }
            
            dst[dst.size() - 2] = char_type(bit_depth);
            dst[dst.size() - 1] = char_type(mod);
        }
        
        void unpack_bits(codes_vec& dst, string_type const& src) {
            const size_t bit_depth = src[src.size() - 2];
            const size_t mod       = src[src.size() - 1];
            
            const size_t payload_bits = ALLOWED_BITS*(src.size() - 2);
            const size_t out_length = (payload_bits - mod)/bit_depth;
            dst.clear(); dst.resize(out_length, 0);
            
            size_t idx = 0;
            size_t offset = 0;
            
            // [11111111][11122222][22222233][33333333][3.......]>>
            // ...........................^......................>>
            
            // cycle over codes
            for(auto& code : dst) {
                
                size_t acc = 0;
                
                // cycle over code bits
                for(size_t code_done = 0; code_done < bit_depth;) {
                    
                    size_t bits_left = ALLOWED_BITS - offset;
                    size_t code_left = bit_depth - code_done;
                    
                    size_t bits_to_read = std::min(bits_left, code_left);
                    
                    size_t data = src[idx];
                    data >>= offset;                     // abcde|fg => abcde
                    data &= (1UL << bits_to_read) - 1UL; // abcde    => 00cde
                    data <<= code_done;                  // 00cde    => 00cde000
                    acc |= data;
                    
                    code_done += bits_to_read;
                    offset    += bits_to_read;
                    
                    if(offset == ALLOWED_BITS)
                        ++idx, offset = 0;
                }
                
                code = acc;
            }
        }
        
        /// @returns reference to cached static string
        string_type& cached_string() {
            static string_type s_; return s_; }
        
        /// @returns reference to cached static codes_vec
        codes_vec& cached_codes_vec() {
            static codes_vec v_; return v_; }
    }
    
    
    string_type lzw_encode(string_type const& s) {
        if(s.empty()) return s;
        
        auto dict = details::ascii_encode_dict();
        dict.reserve(dict.size()*3/2);
        
        size_t next_code = dict.size();
        size_t max_code = next_code - 1;
        
        details::codes_vec codes_;
        auto& codes = USE_CACHED_BUFFERS ? details::cached_codes_vec() : codes_;
        
        codes.clear();
        codes.reserve(s.size()*3/2);
        
        string_type phrase(1, s[0]); // X
        string_type tmp;
        
        /// Single emplace step
        auto emplace_code = [&dict, &phrase, &max_code, &codes]{
            auto code = dict.at(phrase);
            max_code = std::max(max_code, code);
            codes.emplace_back(code); };
        
        for(size_t i = 1, sz = s.size(); i < sz; ++i) {
            auto curr_char = s[i];      // Y
            tmp.assign(phrase);
            tmp.push_back(curr_char);   // XY
            
            // already exists, no insertion
            if(!dict.emplace(tmp, next_code).second) {
                phrase.assign(tmp);
            
            // not exists, inserted
            } else {
                ++next_code;
                emplace_code();
                phrase.assign(1, curr_char);
            }
        }
        
        // last step
        emplace_code();
        
        string_type out_;
        auto& out = USE_CACHED_BUFFERS ? details::cached_string() : out_;
        
        // Binary packing
        auto bit_depth = details::bits_needed(max_code);
        details::pack_bits(out, codes, bit_depth);
        
        return out;
    }
    
    string_type lzw_decode(string_type const& s) {
        if(s.empty()) return s;
        
        // Binary unpacking
        details::codes_vec codes_;
        auto& codes = USE_CACHED_BUFFERS ? details::cached_codes_vec() : codes_;
        details::unpack_bits(codes, s);
        
        string_type out_;
        auto& out = USE_CACHED_BUFFERS ? details::cached_string() : out_;
        
        out.clear();
        out.reserve(s.size()*3/2);
        
        auto rdict = details::ascii_decode_dict();
        rdict.reserve(rdict.size() + s.size() + 1);
        
        size_t code = codes[0];
        size_t old = code;
        
        string_type tmp;
        
        out.append(rdict[code]);
        for(size_t i = 1, sz = codes.size(); i < sz; ++i) {
            code = codes[i];
            tmp = rdict[old];
            
            if(code < rdict.size()) {
                auto const& entry = rdict[code];
                tmp.push_back(entry[0]);
                out.append(entry);
                rdict.emplace_back(std::move(tmp));
            } else {
                tmp.push_back(tmp[0]);
                out.append(tmp);
                rdict.emplace_back(std::move(tmp));
            }
            
            old = code;
        }
        
        return out;
    }
    
} // lzw
