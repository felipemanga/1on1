#pragma once
#include <Femto>
#include <cstdint>
#include <cstring>
#include <string_view>
#include "Camera3D.h"
#include "Renderer.h"
#include "meshes.h"
#include "Ghost.h"
#include "Settings.hpp"

constexpr u32 shipCount = 2;
constexpr u32 currentVersion = 3;

struct Recording {
    u32 version;
    char name[32];
    u32 pod;
    u32 color;
    u32 trackId;
    u32 time;
    u32 sampleCount;
    u8 data[];
};

Recording* const recordingBanks[] = {
#ifdef TARGET_LPC11U68
    reinterpret_cast<Recording*>(0x20000000),
    reinterpret_cast<Recording*>(0x20004000)
#else
    reinterpret_cast<Recording*>(new u8[0x800]),
    reinterpret_cast<Recording*>(new u8[0x800])
#endif
};

inline constexpr const u32 maxBankSize = 0x800;

class Ship {
public:
    static inline Ship* player = nullptr;
    static inline u32 nextUID = 0;
    const u32 uid;
    Recording* recording;
    const Stats* stats;
    f32 thrust;
    Point3D speed;
    Point3D position;

    f32 rotation;
    f32 rotationSpeed;
    u32 HP;
    s32 boostMeter;
    bool isBoosting;
    u32 nextCheckpoint;
    static inline u32 lapStart;
    static inline u32 lapTime;
    static inline u32 startFrame;
    u32 bestLap = ~u32{};

    Point3D relativePosition;
    f32 cameraZ;
    f32 skew;

    static inline Point2D checkPoints[4];

    void init(u32 trackId) {
        bestLap = ~u32{};
        startFrame = frame;
        lapStart = getTimeMicro() / 1000;
        nextCheckpoint = 1;
        position = Point3D{0,0,0};
        speed = Point3D{0,0,0};
        thrust = 0;
        rotation = PI;
        rotationSpeed = 0;
        recording->trackId = trackId;
        if (isPlayer()) {
            recording->sampleCount = 0;
            strcpy(recording->name, Settings::name);
            recording->pod = Settings::pod % shipStatsCount;
            recording->color = Settings::color;
            recording->version = currentVersion;
        } else if (!loadGhost(trackId, recording, maxBankSize) || recording->version != currentVersion) {
            LOG("Invalid recording version: ", recording->version, "\n");
            MemOps::set(recording, 0, maxBankSize);
        // } else {
        //     save(5);
        }
    }

    Ship() : uid{nextUID++} {
        recording = recordingBanks[uid];
        if (!player){
            player = this;
        }
    }

    bool isPlayer() {
        return this == player;
    }

    s32 radius() {
        return 16;
    }

    void spawn() {
    }

    bool isDead() {
        return HP <= 0;
    }

    bool dead() {
        if (HP > 0)
            return false;
        return true;
    }

    void aiPlayer() {
        PROFILER;
        u32 prevTS = lapTime >> 8;
        f32 w = s24q8ToF32(lapTime - (prevTS << 8));
        f32 iw = 1 - w;

        Point2D lerp;
        Point2D prev, next;
        if (recording->sampleCount > 0) {
            prevTS %= recording->sampleCount;
            prev.x = recording->data[prevTS * 2];
            prev.y = recording->data[prevTS * 2 + 1];

            u32 nextTS = prevTS + 1;
            if (nextTS >= recording->sampleCount)
                nextTS -= recording->sampleCount;

            next.x = recording->data[nextTS * 2];
            next.y = recording->data[nextTS * 2 + 1];

            next = Ground::relative(next, prev);
            lerp = prev * iw + next * w;

            // LOG(prevTS, " x:", s32(lerp.x), " y:", s32(lerp.y), "\n");
        } else {
            lerp = prev = next = Point2D{0, 0};
        }

        // Graphics::print("\n\n\nxy:", s32(lerp.x), " x ", s32(lerp.y));

        auto oldPosition = position;
        position.x = lerp.x;
        position.z = lerp.y;

        auto newPosition = Ground::relative(position, oldPosition);
        auto& terrain = gameRenderer->get<Ground>();
        s32 groundHeight = terrain.getHeightAtPoint({position.x, position.z});

        isInShade = groundHeight & 1;
        position.y = tweenTo(position.y, groundHeight + 20, 2);

        if ((oldPosition - newPosition).lengthSquared()) {
            f32 newRotation = atan2(next.y - prev.y, next.x - prev.x);
            rotation += angleDelta(rotation, newRotation) * f32(0.25f);
        }
    }

    void writeRecordSample() {
        PROFILER;
        s32 x = position.x;
        s32 z = position.z;
        if (x == 0 && z == 0 && recording->sampleCount <= 0) {
            lapStart = getTimeMicro() / 1000;
            startFrame = (frame << 3);
            lapTime = 0;
            return;
        }
        if (recording->sampleCount == 0) {
            lapStart = getTimeMicro() / 1000;
            startFrame = (frame << 3);
            lapTime = 0;
        }

        lapTime = (frame << 3) - startFrame; // getTimeMicro() / 1000 - lapStart;
        u32 prevTS = lapTime >> 8;
        if (prevTS == recording->sampleCount - 1)
            return;

        if (reinterpret_cast<uintptr_t>(&recording->data[prevTS * 2 + 1]) >= reinterpret_cast<uintptr_t>(recording) + maxBankSize) {
            return;
        }

        if (x < 0) x = Ground::mapWidth + x;
        else if (x >= Ground::mapWidth) x -= Ground::mapWidth;
        if (z < 0) z = Ground::mapHeight + z;
        else if (z >= Ground::mapHeight) z -= Ground::mapHeight;
        if (x < 0) x = 0;
        else if (x > 255) x = 255;
        if (z < 0) z = 0;
        else if (z > 255) z = 255;

        // LOG(recording->sampleCount, " x:", x, " y:", z, "\n");
        recording->sampleCount++;
        recording->data[prevTS * 2] = x;
        recording->data[prevTS * 2 + 1] = z;
    }

    void save(u32 trackId) {
        PROFILER;
        auto &text = gameRenderer->get<TextLayer>();
        u32 lapTime = getTimeMicro()/1000 - lapStart;
        u32 s = lapTime / 1000;
        u32 ms = lapTime - s * 1000;
        text.bindText();
        Graphics::clearText();
        Graphics::setCursor(0, screenHeight - 80);
        Graphics::print(" v.", recording->version, " ");
#if defined(TARGET_LPC11U6X)
        Graphics::print(((volatile uint32_t *) 0xE000ED00)[0] != 1947 ? "HW" : "EMU");
#endif
        Graphics::print("\n Pod: ", stats->name);
        Graphics::setCursor(5, screenHeight - 30);
        Graphics::print(recording->name);
        Graphics::print("\n Time:  ", s, " s ", ms, " ms ");
        saveGhost(trackId, recordingBanks[1], maxBankSize);
        Graphics::clearText();
        delay(1000);
    }

    void localPlayer() {
        PROFILER;
        static bool prevRight = false;
        static bool prevDown = false;
        static bool prevLeft = false;
        static bool prevUp = false;
        static bool prevB = false;
        static u32 timeRight = 0;
        static u32 timeDown = 0;
        static u32 timeLeft = 0;
        static u32 timeUp = 0;
        static bool boost = false;

        writeRecordSample();

        u32 deltaRight = 500;
        u32 deltaDown = 500;
        u32 deltaLeft = 500;
        u32 deltaUp = 500;

        f32 targetThrust = stats->thrust;

        Graphics::print(nextCheckpoint);

        if (isBoosting) {
            boostMeter -= frame & 1;
            if (boostMeter < 0) boostMeter = 0;
            if (boostMeter == 0 && isPressed(Button::Left)) {
            } else if (!boostMeter || isPressed(Button::Right)) {
                isBoosting = false;
                boostMeter = 0;
                LOG("Boost end\n");
            } else {
                targetThrust = stats->boostThrust;
            }
        } else {
            if (isPressed(Button::Left)) {
                boostMeter += 2;
                if (boostMeter >= stats->boostMax) {
                    isBoosting = true;
                    boostMeter = stats->boostMax;
                    LOG("Boost start\n");
                }
            } else if (boostMeter > 0) {
                boostMeter--;
            }
        }

        BoostLayer::level = boostMeter * 128 / stats->boostMax;

        rotationSpeed = tweenTo(rotationSpeed, (isPressed(Button::Up) - isPressed(Button::Down)) * stats->turnRate * (isBoosting ? f32(1.0f) : f32(1.3f)), f32(0.25f));

        if (Settings::autoAccelerate) {
            if (isPressed(Button::Right)) {
                thrust = tweenTo(thrust, f32(-80.0f), 4);
            } else {
                thrust = tweenTo(thrust, targetThrust, 4 - isBoosting);
            }
        } else {
            if (isPressed(Button::A)) {
                thrust = tweenTo(thrust, targetThrust, 4 - isBoosting);
            } else if (isPressed(Button::B)) {
                thrust = tweenTo(thrust, f32(-80.0f), 4);
            }
        }

        auto& terrain = gameRenderer->get<Ground>();
        s32 groundHeight = terrain.getHeightAtPoint({position.x, position.z});

        isInShade = groundHeight & 1;

        if (groundHeight > position.y)
            position.y = groundHeight;

        groundHeight += 20;
        f32 delta = (groundHeight - position.y) * f32(3.0f);

        speed.y += delta;

        if (delta > 15) {
            auto groundNormal = terrain.getNormalAtPoint({position.x, position.z}).normalize();
            f32 w = f32(100.0f) - std::abs(groundNormal.y) * f32(100.0f);
            speed += groundNormal * w;
        }

        if (!(frame&3)) {
            Point3D forward{thrust, 0, 0};
            forward.rotateXZ(rotation);
            speed += forward;
            thrust *= f32(0.9f);
        }
        speed *= f32(0.8f);
        position += speed * f32(0.01f);
        rotation += rotationSpeed * f32(0.004f);
        if (std::abs(f32ToS24q8(rotationSpeed)) < 50) rotationSpeed = 0;

        auto rpos = -position.xz();

        auto checkpointDelta = Ground::relative(checkPoints[nextCheckpoint], rpos) - rpos;

        // if (!checkpointDelta.distanceCheck(128)) {
        //     checkpointDelta = (checkPoints[nextCheckpoint] - Point2D{
        //             ((s32(rpos.x) & Ground::widthMask) - Ground::mapWidth),
        //             ((s32(rpos.y) & Ground::heightMask) - Ground::mapHeight)
        //         });
        // }

        if (checkpointDelta.distanceCheck(15)) {
            if (nextCheckpoint == 0) {
                u32 now = getTimeMicro() / 1000;
                if (lapStart) {
                    recording->time = now - lapStart;
                    auto other = reinterpret_cast<Recording*>(recordingBanks[1]);
                    if (!other->sampleCount || recording->time < other->time) {
                        MemOps::copy(recordingBanks[1], recording, maxBankSize);
                        recording->sampleCount = 0;
                        save(recording->trackId);
                    }
                }
                lapStart = now;
                startFrame = frame << 3;
                lapTime = 0;
            }
            nextCheckpoint = (nextCheckpoint + 1) & 3;
            // LOG(uid, "] Next checkpoint: ", nextCheckpoint, "\n");
        }

        // if (isPlayer() && !(frame & 0xF))
        //     LOG(nextCheckpoint, " distance: ", s32(checkpointDelta.length()), " @{", s32(rpos.x), ",", s32(rpos.y), "}\n");
    }

    bool isInShade;

    void update(u32 frame){
        PROFILER;
        stats = &shipStats[recording->pod];

        if (isPlayer()) localPlayer();
        else aiPlayer();

        if (this->position.x > Ground::mapWidth) {
            this->position.x -= Ground::mapWidth;
            if (isPlayer())
                camera3D.position.x -= Ground::mapWidth;
        } else if (this->position.x < 0) {
            this->position.x += Ground::mapWidth;
            if (isPlayer())
                camera3D.position.x += Ground::mapWidth;
        }

        if (this->position.z > Ground::mapHeight) {
            this->position.z -= Ground::mapHeight;
            if (isPlayer())
                camera3D.position.z -= Ground::mapWidth;
        } else if (this->position.z < 0) {
            this->position.z += Ground::mapHeight;
            if (isPlayer())
                camera3D.position.z += Ground::mapWidth;
        }

        // Graphics::setCursor(0, s32(30 + uid * 10));
        // Graphics::print(uid, ": ", s32(this->position.x), ", ", s32(this->position.z), " d:", s32(sqrt(f32(camera3D.distanceTo(this->position)))));

        auto position = this->position;
        auto camPosition = camera3D.position;

        relativePosition = Ground::relative(position - camPosition, Point3D{0, 0, 0});
        relativePosition.rotateXZ(-camera3D.rotation);

        if (relativePosition.z >= 3 && relativePosition.z < Ground::maxDrawDist) {
            cameraZ = relativePosition.length();
            skew = sin(atan2(-relativePosition.y, relativePosition.z));
            s32 fz = (1 << 24) / f32ToS24q8(cameraZ);
            relativePosition.x *= s24q8ToF32(fz); // s24q8ToF32(f32ToS24q8(relativePosition.x) * fz);
            relativePosition.y *= s24q8ToF32(fz >> 2); // s24q8ToF32(f32ToS24q8(relativePosition.y) * fz);
            relativePosition.z = s24q8ToF32(fz);
        } else {
            cameraZ = 512;
        }
    }

    void postUpdate() {
        draw();
    }

    void draw(){
        PROFILER;

        if (cameraZ > Ground::maxDrawDist) {
            return;
        }

        if (gameRenderer->get<Ground>().isObstructed({
                    position.x,
                    (Ground::horizon >> 8) - relativePosition.y,
                    position.z
                }))
            return;

        gameRenderer->get<Ground>().plotShadow(position.x + f32(0.5f), position.z + f32(0.5f), 5);

        f32 fz = relativePosition.z;// * f32(1.5f);

        f32 scale = s24q8ToF32(f32ToS24q8(fz) >> 3);

        constexpr const f32 maxScale = f32(1.75f);
        bool tooBig = scale > maxScale;
        if (tooBig) {
            scale = s24q8ToF32(f32ToS24q8(fz) >> 4);
            if (scale > maxScale){
                // return;
                // scale = f32(1.75f);
            }
        }

        constexpr const s32 r = 64;
        auto position2D = Point2D{
            50 - relativePosition.y,
            screenHeight/2 + relativePosition.x
        } - (tooBig ? r : r/2);

        if (auto bitmap = drawMesh(stats->mesh,
                                   scale,
                                   camera3D.rotation - rotation,
                                   rotationSpeed * f32(0.03f),
                                   recording->color,
                                   skew,
                                   isInShade)) {
            if (tooBig) Graphics::draw<true, true>(*bitmap, position2D);
            else        Graphics::draw<true, false>(*bitmap, position2D);
        }
    }

    static void broadcast(void (Ship::*method)(u32), u32);

} ships[shipCount];


inline void Ship::broadcast(void (Ship::*method)(u32), u32 data) {
    PROFILER;
    for (u32 i = 0; i < shipCount; ++i) {
        (ships[i].*method)(data);
    }
}
