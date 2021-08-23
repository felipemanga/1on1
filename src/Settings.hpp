#pragma once

#include <Femto>
#include <File>

class Settings {
public:
    static inline bool autoAccelerate = true;
    static inline char name[32] = {'P', 'l', 'a', 'y', 'e', 'r', 0};
    static inline u32 pod = 0;
    static inline u32 color = 0;
    static bool load();
};
