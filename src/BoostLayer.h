#pragma once

#include <Femto>
#include "../img-src/Gauge.h"

class BoostLayer {
public:
    static inline u32 level = 0x7C;
    
    void operator () (u16* line, s32 y) {
        y -= screenHeight - Gauge[1];
        if (y < 0)
            return;
        auto bmp = Gauge + 2 + y * Gauge[0];
        line += screenWidth - Gauge[0];
        s32 i = 0;
        for (; i < Gauge[0]; ++i) {
            if (bmp[i])
                break;
        }
        for (; i < Gauge[0]; ++i) {
            u8 c = bmp[i];
            if (c && c <= level)
                line[i] = c >> 3;
        }
    }
};
