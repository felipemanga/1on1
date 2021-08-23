#pragma once

#include <Femto>
#include <LibProfiler>
#include <layers/Tiles.hpp>

#include "../img-src/map1.h"
#include "../img-src/map1h.h"
#include "miloslav.h"
#include "ztables.h"

template<u32 _mapWidth, u32 _mapHeight, u32 _maxDrawDist = 80>
class Terrain {
public:
    static inline constexpr const u32 mapWidth = _mapWidth;
    static inline constexpr const u32 mapHeight = _mapHeight;
    static inline constexpr const u32 widthMask = mapWidth - 1;
    static inline constexpr const u32 heightMask = mapHeight - 1;
    static inline constexpr const s32 horizon = 50 << 8;
    static inline constexpr const u32 maxDrawDist = _maxDrawDist;

    u32 shadowMap[(mapWidth/32)*mapHeight];

    const u8* heightmap = map1h + 2;
    const u16* palette = map1;

    template<s32 d>
    static f32 relative(f32 c, f32 r) {
        bool cn = c < 0;
        bool rn = r < 0;
        if (cn) c = -c;
        if (rn) r = -r;
        c = s24q8ToF32((f32ToS24q8(c) % (d << 8)));
        r = s24q8ToF32((f32ToS24q8(r) % (d << 8)));
        if (cn) c = -c;
        if (rn) r = -r;
        f32 m = c - r;
        if (m >= d/2) c -= d;
        else if(m < -d/2) c += d;
        return c;
    }

    static Point2D relative(Point2D c, const Point2D& r) {
        c.x = relative<mapWidth>(c.x, r.x);
        c.y = relative<mapHeight>(c.y, r.y);
        return c;
    }

    static Point3D relative(Point3D c, const Point3D& r) {
        c.x = relative<mapWidth>(c.x, r.x);
        c.z = relative<mapHeight>(c.z, r.z);
        return c;
    }

    void clearShadows() {
        PROFILER;
        for (u32 i = 0; i < (mapWidth/32)*mapHeight; ++i)
            shadowMap[i] = 0;
    }

    void plotShadow(s32 x, s32 y, s32 size){
        PROFILER;
        x = -x;
        y = -y;
        s32 rad = size / 2 - (size & 1);
        s32 sy = y - rad;
        s32 sx = x - rad;
        rad *= rad;
        for(s32 iy = sy; iy < sy+size; ++iy){
            for(s32 ix = sx; ix < sx+size; ++ix){
                if ((ix - x)*(ix - x) + (iy - y)*(iy - y) <= rad) {
                    shadowMap[((ix & widthMask) >> 5) + (iy & heightMask) * (mapWidth >> 5)] |= 1 << (ix & 0x1F);
                }
            }
        }
    }

    s32 getHeightAtPoint(const Point2D& point) {
        PROFILER;
        s32 sgx = (f32ToS24q8(-point.x) + (1<<7));
        s32 sgy = (f32ToS24q8(-point.y) + (1<<7));
        return heightmap[((sgx >> 8) & widthMask) + ((sgy >> 8) & heightMask) * mapWidth];
    }

    Point3D getNormalAtPoint(const Point2D point) {
        s32 Ax = (f32ToS24q8(-point.x) + (1<<7)) >> 8;
        s32 Az = (f32ToS24q8(-point.y) + (1<<7)) >> 8;
        s32 Ay =  heightmap[(Ax & widthMask) + (Az & heightMask) * mapWidth];
        s32 Bx = Ax - 2;
        s32 Bz = Az - 2;
        s32 By =  heightmap[(Bx & widthMask) + (Bz & heightMask) * mapWidth];
        s32 Cx = Ax + 2;
        s32 Cz = Az - 2;
        s32 Cy =  heightmap[(Cx & widthMask) + (Cz & heightMask) * mapWidth];
        return {
            (Ay - Cy) * (Bz - Cz) - (Az - Cz) * (By - Cy),
            (Az - Cz) * (Bx - Cx) - (Ax - Cx) * (Bz - Cz),
            (Ax - Cx) * (By - Cy) - (Ay - Cy) * (Bx - Cx)
        };
    }

    bool isObstructed(Point3D target) {
        // if (target.x - camera3D.position.x >= 128) target.x = (mapWidth - (target.x - camera3D.position.x));
        // if (target.x - camera3D.position.x < -128) target.x = (mapWidth + (target.x - camera3D.position.x));
        // if (target.z - camera3D.position.z >= 128) target.z = (mapHeight - (target.z - camera3D.position.z));
        // if (target.z - camera3D.position.z < -128) target.z = (mapHeight + (target.z - camera3D.position.z));
        // target = relative(target, camera3D.position);

        s32 sgx = (f32ToS24q8(-camera3D.position.x) + (1<<7)) << 8;
        s32 sgy = (f32ToS24q8(-camera3D.position.z) + (1<<7)) << 8;
        s32 height = camera3D.position.y;
        s32 prevSampleY = screenWidth;
        f32 angle = camera3D.angleTo(target);
        Point2D forward{0.0f, -1.0f};
        forward.rotateXY(angle);
        s32 fx = f32ToS24q8(forward.x);
        s32 fy = f32ToS24q8(forward.y);
        s32 targetDist = camera3D.distanceTo(target);

        for (u32 step = 5; step < maxDrawDist; step++) {
            s32 z = ztable[step];
            s32 iz = iztable[step];
            if ((z >> 8) * (z >> 8) > targetDist)
                return false;
            s32 px = (fx * z + sgx) >> 16;
            s32 py = (fy * z + sgy) >> 16;

            // shadowMap[((px & widthMask) >> 5) + (py & heightMask) * (mapWidth >> 5)] |= 1 << (px & 0x1F);

            s32 sampleIndex = (px & widthMask) + (py & heightMask) * mapWidth;
            s32 sample = heightmap[sampleIndex];
            s32 sampleY = ((height - sample) * iz + (horizon + (1 << 7))) >> 8;
            if (sampleY < target.y)
                return true;
        }
        return false;
    }

    void operator () (u16* line, s32 column) {
        PROFILER;
        s32 sgx = (f32ToS24q8(-camera3D.position.x) + (1<<7)) << 8;
        s32 sgy = (f32ToS24q8(-camera3D.position.z) + (1<<7)) << 8;
        s32 height = camera3D.position.y;
        s32 prevSampleY = screenWidth;
        const f32 angle = camera3D.rotation - ((column - s32(screenHeight/2))*(f32(1.0f/(screenHeight*PI/4.0f))));
        Point2D forward{0.0f, -1.0f};
        forward.rotateXY(angle);
        s32 fx = f32ToS24q8(forward.x);
        s32 fy = f32ToS24q8(forward.y);
        // s32 z = 2560 - 128;
        for (u32 step = 0; step < maxDrawDist; step++) {
            // z += 128 + step;
            s32 z = ztable[step];
            s32 iz = iztable[step];
            s32 px = (fx * z + sgx) >> 16;
            s32 py = (fy * z + sgy) >> 16;
            s32 sampleIndex = (px & widthMask) + (py & heightMask) * mapWidth;
            s32 sample = heightmap[sampleIndex];
            s32 sampleY = ((height - sample) * iz + (horizon + (1 << 7))) >> 8;
            if (sampleY >= prevSampleY)
                continue;
            if (sampleY < 0)
                sampleY = 0;

            sample |= (shadowMap[((px & widthMask) >> 5) + (py & heightMask) * (mapWidth >> 5)] >> (px & 0x1F)) & 1;

            s32 diff = sampleY - prevSampleY;
            auto point = line + prevSampleY;
            prevSampleY = sampleY;
            sample = palette[sample];



#ifdef TARGET_CORTEX_M
            diff <<= 1;
            __asm__ volatile (
                ".syntax unified" "\n"
                "1:strh %2, [%1, %0] \n"
                "adds %0, 2 \n"
                "bne 1b \n"
                : "+l" (diff)
                : "l" (point), "l" (sample)
                : "cc"
                );
#else
            while (diff) {
                point[++diff] = sample;
            }
#endif
            if (sampleY == 0)
                return;
        }

#ifdef TARGET_CORTEX_M
        if (prevSampleY) {
            u32 sample = palette[255];
            auto point = line + prevSampleY - 1;
            prevSampleY = -prevSampleY << 1;
            __asm__ volatile (
                ".syntax unified" "\n"
                "1: adds %0, 2 \n"
                "strh %2, [%1, %0] \n"
                "bne 1b \n"
                : "+l" (prevSampleY), "+l" (point)
                : "l" (sample)
                : "cc"
                );
        }
#else
        for (s32 i = prevSampleY; i;) {
            line[--i] = palette[255];
        }
#endif
    }
};
