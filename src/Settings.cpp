#include "Settings.hpp"
#include <cstring>

static void save() {
    File file;
    file.openRW("data/1on1Settings.ini", true, false);

    file << "autoAccelerate " << (Settings::autoAccelerate ? "true" : "false") << "\n";

    file << "name ";
    file.write(Settings::name, strlen(Settings::name));
    file << "\n";

    char buff[32];
    snprintf(buff, sizeof(buff), "%d", Settings::pod);
    file << "pod ";
    file.write(buff, strlen(buff));
    file << "\n";

    snprintf(buff, sizeof(buff), "%d", Settings::color);
    file << "color ";
    file.write(buff, strlen(buff));
    file << "\n";
}

static u32 readKey(File& file) {
    u32 v1 = 5381, v2 = 2166136261;
    u8 c;

    do {
        file >> c;
    } while (c && c <= ' ');

    while(c > ' '){
        if (c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
        v1 = (v1 * 251) ^ c;
        v2 = (v2 ^ c) * 16777619;
        file >> c;
    }

    return v1 * 13 + v2;
}

static u32 readString(File& file, char* out, u32 maxSize) {
    u32 pos = 0;
    u8 c;
    do {
        file >> c;
    } while (c && c <= ' ');

    while(c && c != '\n' && pos < (maxSize - 1)){
        out[pos++] = c;
        file >> c;
    }
    out[pos] = 0;
    return pos;
}

static bool readBool(File& file) {
    u8 c;
    do {
        file >> c;
    } while (c && c <= ' ');

    while(file.read<u8>() > ' ');

    return c == 't' || c == 'T' || c == 'y' || c == 'Y';
}

static u32 readNumber(File& file) {
    u8 c;
    do {
        file >> c;
    } while (c && c <= ' ');

    u32 n = 0;
    while (c >= '0' && c <= '9') {
        n = n * 10 + (c - '0');
        file >> c;
    }

    return n;
}

bool read() {
    File file;
    if (!file.openRO("data/1on1Settings.ini")) {
        return false;
    }
    auto size = file.size();
    while (file.tell() < size) {
        switch (readKey(file)) {
        case "autoaccelerate"_hash:
            Settings::autoAccelerate = readBool(file);
            LOG("autoAccelerate ", Settings::autoAccelerate, "\n");
            break;

        case "name"_hash:
            readString(file, Settings::name, sizeof(Settings::name));
            LOG("name ", Settings::name, "\n");
            break;

        case "pod"_hash:
            Settings::pod = readNumber(file);
            LOG("pod ", Settings::pod, "\n");
            break;

        case "color"_hash:
        case "colour"_hash:
            Settings::color = readNumber(file);
            LOG("color ", Settings::color, "\n");
            break;

        default:
            LOG("Unknown key\n");
            break;
        }
    }
    return true;
}

bool Settings::load() {
    if (!read()){
        save();
        return false;
    }
    return true;
}
