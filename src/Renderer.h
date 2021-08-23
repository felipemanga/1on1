#pragma once

#include <variant>
#include <donut7x10.hpp>
#include <MonoText.hpp>
#include "Camera3D.h"
#include "Terrain.h"


/* inline Particles shots; */
using SpriteLayer = Graphics::layer::FastDrawList<10, 5>;
using HUDLayer = Graphics::layer::DrawList<80, fontDonut>;
using Ground = Terrain<256, 256, 70>;
using SpriteLayerParent = SpriteLayer;
using TextLayer = Graphics::layer::MonoText<fontDonut>;
using GameRenderer = Graphics::Renderer<
    Ground,
    SpriteLayerParent,
    TextLayer
    >;

inline GameRenderer* gameRenderer;
inline std::variant<GameRenderer> renderer;

using cAction_t = void (*)(uptr);

inline u32 frame;
inline f32 shake;
inline const c8* streamedEffect = nullptr;
inline u32 effectPriority = 0;
inline char missionText[100];
inline bool ignoreAction = false;
inline void cActionNOP(uptr) {}
inline cAction_t cAction = cActionNOP;
inline uptr cActionArg;
inline const c8* featureCaption;

enum class GameState {
    Start,
    Logo,
    Space,
    EnterCutScene,
    CutScene
} inline gameState = GameState::Start;

inline s32 backlight = 0, targetBacklight = 255;

void fadeOut();
bool updateEnter(GameState state);
