#ifndef __UTILS_H_
#define __UTILS_H_

#include <malloc.h>
#include <string_view>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#define LIMIT(x, min, max)                                   \
    ({                                                       \
        typeof(x) _x     = x;                                \
        typeof(min) _min = min;                              \
        typeof(max) _max = max;                              \
        (((_x) < (_min)) ? (_min) : ((_x) > (_max)) ? (_max) \
                                                    : (_x)); \
    })

#define DegToRad(a) ((a) * 0.01745329252f)
#define RadToDeg(a) ((a) * 57.29577951f)

#define ALIGN4(x)   (((x) + 3) & ~3)
#define ALIGN32(x)  (((x) + 31) & ~31)

#define le16(i)     ((((uint16_t) ((i) & 0xFF)) << 8) | ((uint16_t) (((i) & 0xFF00) >> 8)))
#define le32(i)     ((((uint32_t) le16((i) & 0xFFFF)) << 16) | ((uint32_t) le16(((i) & 0xFFFF0000) >> 16)))
#define le64(i)     ((((uint64_t) le32((i) & 0xFFFFFFFFLL)) << 32) | ((uint64_t) le32(((i) & 0xFFFFFFFF00000000LL) >> 32)))

//Needs to have log_init() called beforehand.
void dumpHex(const void *data, size_t size);


#ifdef __cplusplus
}

#endif

#ifdef __cplusplus
namespace Utils {
    bool LoadFileIntoBuffer(std::string_view path, std::vector<uint8_t> &buffer);
}
#endif

#endif // __UTILS_H_
