#pragma once

#include <Femto>
#include <cstdint>

using RenderTexture = Bitmap<8, 2+64*64>;

inline RenderTexture buffers[] = {
    {64, 64},
    {64, 64}
};

constexpr inline u32 bufferCount = sizeof(buffers) / sizeof(buffers[0]);
inline u32 usedBufferCount = 0;

inline u32 bufferHash[bufferCount];

RenderTexture* drawMesh(const u8* node, f32 scale = 1, f32 yRotation = 0, f32 zRotation = 0, u32 recolor = 0, f32 fskew = 0, bool isInShade = false) {
    PROFILER;
    if (usedBufferCount >= bufferCount){
        usedBufferCount++;
        LOG("Ran out of textures ", usedBufferCount, "\n");
        return nullptr;
    }

    s32 syRotation = f32ToS24q8(yRotation) >> 4;
    s32 szRotation = f32ToS24q8(zRotation) >> 4;

    u32 hash = reinterpret_cast<uintptr_t>(node) +
        syRotation * 991 +
        szRotation * 463 +
        recolor * 253 +
        f32ToS24q8(scale) * 1001 +
        f32ToS24q8(fskew) * 971;

    if (bufferHash[usedBufferCount] == hash) {
        return &buffers[usedBufferCount++];
    }

    bufferHash[usedBufferCount] = hash;
    auto& bitmap = buffers[usedBufferCount++];
    bitmap.fill(0);

    u32 width = bitmap.width();
    u32 height = bitmap.height();
    s32 centerX = width / 2;
    s32 centerY = height / 2;
    s32 iscale = std::min(width, height) * scale;

    // for (s32 y = 0; y < iscale; ++y) {
    //     for (s32 x = 0; x < iscale; ++x) {
    //         bitmap.ptr()[2 + (centerX - iscale/2) + x + ((y + (centerY - iscale/2)) * width)] = 1;
    //     }
    // }
    // return &bitmap;

    u32 faceCount = (node[0] << 8) | node[1];
    u32 vertexCount = node[2];
    s8  vtxCache[node[2] * 2];

    s32 crY = f32ToS24q8(cos(yRotation));
    s32 srY = f32ToS24q8(sin(yRotation));
    s32 crZ = f32ToS24q8(cos(zRotation));
    s32 srZ = f32ToS24q8(sin(zRotation));

    auto vtx = reinterpret_cast<const s8*>(node + 3 + faceCount * 4);

    f32 fiskew = f32(1.0f) - fskew;

    s32 skew = f32ToS24q8(fskew);
    s32 iskew = f32ToS24q8(f32(1.0f) - fskew);

    for (u32 vtxId = 0; vtxId < vertexCount; ++vtxId) {
        auto ptr = vtx + vtxId * 3;
        s32 Ax = *ptr++ * iscale;
        s32 Ay = *ptr++ * iscale;
        s32 Az = *ptr * iscale;
        s32 T = (crZ * Ax - srZ * Ay) >> 8;
        Ay = (crZ * Ay + srZ * Ax) >> 8;
        Ax = T;

        T = (crY * Ax - srY * Az) >> 8;
        Az = crY * Az + srY * Ax;
        Ax = T;

        vtxCache[vtxId * 2] = -(Ay*iskew - Ax*skew) >> 16;
        vtxCache[vtxId * 2 + 1] = Az >> 16;
    }

    for (u32 face = 0; face < faceCount; ++face) {
        u32 index = 3 + face * 4;
        u32 color = u8(node[index++]) + (recolor << 3);
        auto indexA = vtxCache + node[index++] * 2;
        auto indexB = vtxCache + node[index++] * 2;
        auto indexC = vtxCache + node[index  ] * 2;

        s32 Ax = indexA[0];
        s32 Ay = indexA[1];
        // s32 Az = indexA[2];
        s32 Bx = indexB[0];
        s32 By = indexB[1];
        // s32 Bz = indexB[2];
        s32 Cx = indexC[0];
        s32 Cy = indexC[1];
        // s32 Cz = indexC[2];

        // s32 Nx = (Ay - Cy) * (Bz - Cz) - (Az - Cz) * (By - Cy);
        // s32 Ny = (Az - Cz) * (Bx - Cx) - (Ax - Cx) * (Bz - Cz);
        // s32 Nz = (Ax - Cx) * (By - Cy) - (Ay - Cy) * (Bx - Cx);

        // Ay = Ay/2 - Az;
        // By = By/2 - Bz;
        // Cy = Cy/2 - Cz;

        s32 Nx = (Ax - Bx)*(Ay - Cy) - (Ay - By)*(Ax - Cx);
        if (Nx > 0)
            continue;
        Nx = -Nx;

        if (isInShade)
            Nx >>= 1;

        u32 hue = (color >> 3) << 3;
        s32 lum = (color & 7) + (Nx >> 3) - 2;
        if (lum < 0) lum = 0;
        else if (lum > 7) lum = 7;
        /* s32 lum = 5; */
        color = hue + lum;

        bitmap.fillTriangle(
            centerX + Ax, centerY - Ay,
            centerX + Bx, centerY - By,
            centerX + Cx, centerY - Cy,
            color
            );
    }

    return &bitmap;
}
