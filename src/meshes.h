#pragma once

#include <Femto>
#include "../meshes/BasePod.h"
#include "../meshes/Pod2.h"
#include "../meshes/Pod3.h"
#include "../meshes/Pod4.h"
#include "../meshes/Pod5.h"
#include "../meshes/Pod6.h"

struct Stats {
    const u8* mesh;
    const char* name;
    u32 boostMax;
    f32 thrust;
    f32 boostThrust;
    u32 maxHP;
    f32 turnRate;
    f32 grip;
};

constexpr const Stats shipStats[] = {
    {
        .mesh=BasePod,
        .name="Seneus",
        .boostMax=60,
        .thrust=f32(170.0f),
        .boostThrust=f32(250.0f),
        .maxHP=100,
        .turnRate=f32(16.0f),
        .grip=f32(1.0f)
    }, {
        .mesh=Pod2,
        .name="Mazion",
        .boostMax=80,
        .thrust=f32(190.0f),
        .boostThrust=f32(270.0f),
        .maxHP=100,
        .turnRate=f32(15.0f),
        .grip=f32(1.0f)
    }, {
        .mesh=Pod3,
        .name="Atus",
        .boostMax=60,
        .thrust=f32(170.0f),
        .boostThrust=f32(250.0f),
        .maxHP=100,
        .turnRate=f32(16.0f),
        .grip=f32(1.0f)
    }, {
        .mesh=Pod4,
        .name="Baanella",
        .boostMax=60,
        .thrust=f32(170.0f),
        .boostThrust=f32(250.0f),
        .maxHP=100,
        .turnRate=f32(16.0f),
        .grip=f32(1.0f)
    }, {
        .mesh=Pod5,
        .name="Tashtronia",
        .boostMax=60,
        .thrust=f32(170.0f),
        .boostThrust=f32(250.0f),
        .maxHP=100,
        .turnRate=f32(16.0f),
        .grip=f32(1.0f)
    }, {
        .mesh=Pod6,
        .name="Diaronia",
        .boostMax=60,
        .thrust=f32(170.0f),
        .boostThrust=f32(250.0f),
        .maxHP=100,
        .turnRate=f32(16.0f),
        .grip=f32(1.0f)
    },
};

constexpr const u32 shipStatsCount = sizeof(shipStats) / sizeof(shipStats[0]);
