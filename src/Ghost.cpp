#include <Femto>
#include <File>
#include "Renderer.h"
#include "Ghost.h"

class PNGWriter {
public:
    File file;
    u32 width, height, positionX = 0, positionY = 0;
    u32 remaining, lineSize;
    u16 deflateFilled = 0;
    u32 crc = 0;
    u32 adler = 1;
    static constexpr inline u32 DEFLATE_MAX_BLOCK_SIZE = 65535;

    void crc32(const u8* data, u32 len) {
	crc = ~crc;
	for (u32 i = 0; i < len; i++) {
            for (u32 j = 0; j < 8; j++) {  // Inefficient bitwise implementation, instead of table-based
                u32 bit = (crc ^ (data[i] >> j)) & 1;
                crc = (crc >> 1) ^ ((-bit) & UINT32_C(0xEDB88320));
            }
	}
	crc = ~crc;
    }

    void adler32(const u8* data, u32 len) {
	u32 s1 = adler & 0xFFFF;
	u32 s2 = adler >> 16;
	for (u32 i = 0; i < len; i++) {
            s1 = (s1 + data[i]) % 65521;
            s2 = (s2 + s1) % 65521;
	}
	adler = s2 << 16 | s1;
    }

    void writeInt32BE(u32 val, u8* array) {
        array[0] = val >> 24;
        array[1] = val >> 16;
        array[2] = val >> 8;
        array[3] = val;
    }

#define BIGEND(V) u8(V >> 24), u8(V >> 16), u8(V >> 8), u8(V)

    PNGWriter(const char* name, u32 w, u32 h) : width(w), height(h) {
        file.openRW(name, true, false);
        lineSize = w * 3 + 1;
        remaining = lineSize * h;
        u32 numBlocks = remaining / DEFLATE_MAX_BLOCK_SIZE;
	if (remaining % DEFLATE_MAX_BLOCK_SIZE != 0)
		numBlocks++;  // Round up

        // 5 bytes per DEFLATE uncompressed block header, 2 bytes for zlib header, 4 bytes for zlib Adler-32 footer
	u32 idatSize = numBlocks * 5 + 6 + remaining;

	// Write header (not a pure header, but a couple of things concatenated together)
	u8 header[] = {  // 43 bytes long
		// PNG header
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,

		// IHDR chunk
		0x00, 0x00, 0x00, 0x0D,
		0x49, 0x48, 0x44, 0x52,
		BIGEND(w),
                BIGEND(h),
		0x08, // bit depth
                0x02, // color type. 2 = color used
                0x00, 0x00, 0x00, // compression, filter, interlace
		0, 0, 0, 0,  // IHDR CRC-32 placeholder
        };

// // eXif chunk
// 0x00, 0x00, 0x00, 0x5a, // size
// 0x65, 0x58, 0x49, 0x66, // eXIf
// 0x4d, 0x4d, 0x00, 0x2a,
// 0x00, 0x00, 0x00, 0x08,
// 0x00, 0x05, 0x01, 0x12, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x06, 0x00, 0x00, 0x01, 0x1a,
// 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4a, 0x01, 0x1b, 0x00, 0x05, 0x00, 0x00,
// 0x00, 0x01, 0x00, 0x00, 0x00, 0x52, 0x01, 0x28, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02,
// 0x00, 0x00, 0x02, 0x13, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00,
// 0x00, 0x01,
// 0x74, 0x6c, 0xe5, 0x6f,

        u8 idat[] = {
		// IDAT chunk
		BIGEND(idatSize),  // 'idatSize' placeholder
		0x49, 0x44, 0x41, 0x54,
		// DEFLATE data
		0x08, 0x1D,
	};

        crc = 0;
	crc32(&header[12], 17);
	writeInt32BE(crc, &header[29]);

	file.write(header);
        file.write(idat);
        crc = 0;
	crc32(&idat[4], 6);  // 0xD7245B6B
    }


    void write(const u8* pixels, u32 count) {
	count *= 3;  // Convert pixel count to byte count
	while (count > 0) {
            if (deflateFilled == 0) {  // Start DEFLATE block
                auto size = DEFLATE_MAX_BLOCK_SIZE;
                if (remaining < size)
                    size = remaining;
                const u8 header[] = {  // 5 bytes long
                    static_cast<u8>(remaining <= DEFLATE_MAX_BLOCK_SIZE),
                    static_cast<u8>(size >> 0),
                    static_cast<u8>(size >> 8),
                    static_cast<u8>((size >> 0) ^ 0xFF),
                    static_cast<u8>((size >> 8) ^ 0xFF),
                };
                file.write(header);
                crc32(header, sizeof(header) / sizeof(header[0]));
            }

            if (positionX == 0) {  // Beginning of line - write filter method byte
                u8 b[] = {0};
                file.write(b);
                crc32(b, 1);
                adler32(b, 1);
                positionX++;
                remaining--;
                deflateFilled++;
            } else {  // Write some pixel bytes for current line
                u32 n = DEFLATE_MAX_BLOCK_SIZE - deflateFilled;
                n = std::min(lineSize - positionX, n);
                n = std::min(n, count);
                file.write(pixels, n);

                // Update checksums
                crc32(pixels, n);
                adler32(pixels, n);

                // Increment positions
                count -= n;
                pixels += n;
                positionX += n;
                remaining -= n;
                deflateFilled += n;
            }

            if (deflateFilled >= DEFLATE_MAX_BLOCK_SIZE)
                deflateFilled = 0;  // End current block

            if (positionX == lineSize) {  // Increment line
                positionX = 0;
                positionY++;
                if (positionY == height) {  // Reached end of pixels
                    u8 footer[] = {  // 20 bytes long
                        0, 0, 0, 0,  // DEFLATE Adler-32 placeholder
                        0, 0, 0, 0,  // IDAT CRC-32 placeholder

                        // IEND chunk
                        0x00, 0x00, 0x00, 0x00,
                        0x49, 0x45, 0x4E, 0x44,
                        0xAE, 0x42, 0x60, 0x82,
                    };
                    writeInt32BE(adler, &footer[0]);
                    crc32(&footer[0], 4);
                    writeInt32BE(crc, &footer[4]);
                    file.write(footer);
                }
            }
	}
    }
};

bool loadGhost(u32 trackId, void* data, u32 size) {
    auto bdata = reinterpret_cast<u8*>(data);
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "data/%d.png", trackId);
    File ghost;
    if (!ghost.openRO(fileName)) {
        LOG("No ghost for track ", fileName, "\n");
        return false;
    }
    ghost.seek(0x30);

    for (u32 y = 0; y < 176; ++y) {
        u8 b;
        ghost >> b;
        for (u32 x = 0; x < 220; ++x) {
            ghost >> b;
            u8 o = b & 7;
            ghost >> b;
            o |= (b & 3) << 3;
            ghost >> b;
            o |= (b & 7) << 5;
            *bdata++ = o;
            size--;
            if (!size) return true;
        }
    }

    return true;
}

bool saveGhost(u32 trackId, void* data, u32 size) {
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "data/%d.png", trackId);
    PNGWriter png(fileName, 220, 176);
    if (!png.file)
        return false;

    auto& ground = gameRenderer->get<Ground>();
    auto fillers = static_cast<Graphics::DynamicRenderer*>(gameRenderer)->fillers;
    auto fillerCount = static_cast<Graphics::DynamicRenderer*>(gameRenderer)->fillerCount;
    auto bdata = reinterpret_cast<u8*>(data);

    auto &text = gameRenderer->get<TextLayer>();
    File ooo;
    ooo.openRO("data/1on1.i16");
    u8 w = 0, h = 0;
    ooo >> w >> h;

    for (int y = 0; y < 176; ++y) {
        Schedule::runUpdateHooks(false, getTimeMicro());
        u8 pixels[220*3];
        u16 line[220 + 16];
        for (u32 x = 0; x < 220; ++x)
            line[x + 8] = ground.palette[ground.heightmap[(Ground::mapWidth/2 - screenWidth/2) + x + Ground::mapWidth * (y + (Ground::mapHeight/2 - screenHeight/2))]];

        if (y < h) {
            for (u32 x = 0; x < w; ++x){
                if (u16 c = ooo.read<u16>()) {
                    line[x + 8 + (screenWidth/2 - w/2)] = c;
                }
            }
        }

        if (y > screenHeight - 45) {
            for (u32 x = 8; x < 228; ++x) {
                line[x] = (line[x] >> 2) & 0x39e7;
            }
        }

        text(line + 8, y);
        // for (u32 i = 0; i < fillerCount; ++i) {
        //     fillers[i](line + 8, y);
        // }

        for (u32 x = 0; x < 220; ++x) {
            u8 db;
            if (size) {
                db = *bdata;
                bdata++;
                size--;
            } else {
                db = 0;
            }
            u32 R = (line[x + 8] >> 11 << 3);
            u32 G = (((line[x + 8] >> 5) & 0x3F) << 2);
            u32 B = ((line[x + 8] & 0x1F) << 3);

            R |= (db & 7); db >>= 3;
            G |= (db & 3); db >>= 2;
            B |= (db & 7);

            pixels[x * 3    ] = R;
            pixels[x * 3 + 1] = G;
            pixels[x * 3 + 2] = B;
        }
        // flushLine16(line + 8);
        png.write(pixels, 220);
    }
    // File ghost(fileName, true);
    // if (ghost) {
    //     ghost.write(data, size);
    //     return true;
    // }
    // return false;
    return true;
}
