#include "utils/utils.hpp"
#include "utils/logger.h"
#include <string>
#include <array>
#include <format>

namespace utils {
    void dumpHex(const void* data, std::size_t size) {
        std::array<char, 17> ascii{};
        const auto* bytes = static_cast<const unsigned char*>(data);
        
        DEBUG_FUNCTION_LINE_WRITE("0x{:08X} (0x0000): ", static_cast<const void*>(data));
        
        for (std::size_t i = 0; i < size; ++i) {
            WHBLogWritef("%02X ", bytes[i]);
            
            ascii[i % 16] = (bytes[i] >= ' ' && bytes[i] <= '~') ? bytes[i] : '.';
            
            if ((i + 1) % 8 == 0 || i + 1 == size) {
                WHBLogWritef(" ");
                if ((i + 1) % 16 == 0) {
                    ascii[16] = '\0';
                    WHBLogWritef("|  %s \n", ascii.data());
                    if (i + 1 < size) {
                        WHBLogWritef("0x%08X (0x%04X); ", 
                            static_cast<const void*>(bytes + i + 1), i + 1);
                    }
                } else if (i + 1 == size) {
                    ascii[(i + 1) % 16] = '\0';
                    
                    if ((i + 1) % 16 <= 8) {
                        WHBLogWritef(" ");
                    }
                    
                    for (std::size_t j = (i + 1) % 16; j < 16; ++j) {
                        WHBLogWritef("   ");
                    }
                    
                    WHBLogWritef("|  %s \n", ascii.data());
                }
            }
        }
    }
}
