#pragma once

#include <Femto>
#include "../meshes/BasePod.h"
#include "../meshes/Pod2.h"
#include "../meshes/Pod3.h"
#include "../meshes/Pod4.h"
#include "../meshes/Pod5.h"
#include "../meshes/Pod6.h"

constexpr const char* const shipNames[] = {
    "Seneus",
    "Mazion",
    "Atus",
    "Baanella",
    "Tashtronia",
    "Diaronia"
};

constexpr const u8* const shipMeshes[] = {
    BasePod,
    Pod2,
    Pod3,
    Pod4,
    Pod5,
    Pod6,
};

constexpr const u32 shipMeshCount = sizeof(shipMeshes) / sizeof(shipMeshes[0]);
