#pragma once

#include <cstddef>
#include <cstdint>
#include <concepts>

namespace utils {
    template<typename T>
    constexpr T limit(T x, T min, T max) {
        return x < min ? min : (x > max ? max : x);
    }

    constexpr float degToRad(float angle) {
        return angle * 0.01745329252f;
    }

    constexpr float radToDeg(float angle) {
        return angle * 57.29577951f;
    }

    template<typename T>
    constexpr T align4(T x) {
        return (x + 3) & ~3;
    }

    template<typename T>
    constexpr T align32(T x) {
        return (x + 31) & ~31;
    }

    constexpr uint16_t le16(uint16_t i) {
        return ((i & 0xFF) << 8) | ((i & 0xFF00) >> 8);
    }

    constexpr uint32_t le32(uint32_t i) {
        return ((static_cast<uint32_t>(le16(i & 0xFFFF))) << 16) | 
               (static_cast<uint32_t>(le16((i & 0xFFFF0000) >> 16)));
    }

    constexpr uint64_t le64(uint64_t i) {
        return ((static_cast<uint64_t>(le32(i & 0xFFFFFFFFLL))) << 32) | 
               (static_cast<uint64_t>(le32((i & 0xFFFFFFFF00000000LL) >> 32)));
    }

    void dumpHex(const void* data, std::size_t size);
}
