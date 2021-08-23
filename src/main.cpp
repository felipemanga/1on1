#include <Femto>
#include <LibProfiler>

#include <SFXVolumeSource.hpp>
#include "miloslav.h"
// #include "Particles.h"
#include "Serialize.hpp"
#include "drawMesh.h"
#include "Renderer.h"
#include "meshes.h"
#include "ship.h"
#include "cutscene.h"

Audio::Sink<7, 10000> audio;
Point2D refCamera;

void fadeOut() {
    for (s32 i = backlight; i >= 0; i -= 11) {
        setBacklight(s24q8ToF32(i));
        delay(3);
    }
    backlight = 0;
    setBacklight(backlight);
}

bool updateEnter(GameState state) {
    targetBacklight = 0;
    if (backlight != 0)
        return false;
    targetBacklight = 255;
    gameState = state;
    return true;
}

void updateCamera(f32 speed) {
    // auto target = Ship::player->position - Point2D{s32(screenWidth/2), s32(screenHeight/2)};
    // refCamera -= (refCamera - target) * speed;
    // Graphics::camera = refCamera + (Point2D{shake, 0}).rotateXY(frame++ * f32(2.5f));
}

void init(){
    Audio::init();
    Audio::setVolume(0);

    Graphics::textMode = Graphics::TextMode::Clip;
    // Graphics::palette = Graphics::generalPalette;
    Graphics::palette = miloslav;
    setMaxFPS(30);
    Graphics::primaryColor = colorFromRGB(0xFFFFFF);
    Serialize::init();
    gameRenderer = &std::get<GameRenderer>(renderer);
    LOG("Free RAM: ", getFreeRAM(), "\n");
    if (!Settings::load()) {
        LOG("Could not load settings\n");
    } else {
        LOG("Loaded settings\n");
    }
    Ship::checkPoints[0] = Point2D{0, 0};
    Ship::checkPoints[1] = Point2D{130, 54};
    Ship::checkPoints[2] = Point2D{133, 184};
    Ship::checkPoints[3] = Point2D{37, 155};
}

u32 paletteAnimFrame = 0;
void updatePalette() {
    PROFILER;
    if (paletteAnimFrame++ < 5)
        return;
    paletteAnimFrame = 0;

    u32 cycleStart = 2, cycleEnd = 8;
    auto of1 = map1[cycleStart];

/* * /
    u32 i;
    for(i = cycleStart; i < cycleEnd - 1; ++i)
        map1[i] = map1[i+1];
    map1[i++] = of1;
/*/
    auto of2 = map1[cycleStart + 1];
    u32 i;
    for(i = cycleStart; i < cycleEnd - 2; ++i)
        map1[i] = map1[i+2];
    map1[i++] = of1;
    map1[i++] = of2;
/**/
}


void playMusic() {
    std::array musicList{
        "music/JeffII-Jetfire.raw",
        "music/ScottHolmesMusic-Hotshot.raw"
    };
    static u32 chosen = musicList.size();
    chosen++;
    if (chosen >= musicList.size())
        chosen = random(0, musicList.size());
    if (auto music = Audio::play(musicList[chosen])) {
        music->setLoop(true);
        LOG("Music started\n");
    } else LOG("Music '", chosen, "' not found\n");
}

bool raceStarted = false;

void stats() {
    PROFILER;
    using namespace Graphics;
    setCursor(0, 10);
    print(getFPS(), " ", profiler::getBottleneck());
}

void drawShips() {
    PROFILER;
    Ship* drawList[shipCount];
    for (u32 i = 0; i < shipCount; ++i) {
        Ship* current = &ships[i];
        for (u32 j = 0; j < i; ++j) {
            if (current->cameraZ > drawList[j]->cameraZ) {
                auto tmp = drawList[j];
                drawList[j] = current;
                current = tmp;
            }
        }
        drawList[i] = current;
    }
    for (u32 i = 0; i < shipCount; ++i)
        drawList[i]->postUpdate();
}

void initFrame() {
    PROFILER;
    Graphics::clear();
    usedBufferCount = 0;
}

void updateSpace() {
    PROFILER;
    if (!raceStarted) {
        Ship::broadcast(&Ship::init, 0);
        raceStarted = true;
        playMusic();
    }

    initFrame();
    updatePalette();
    auto& terrain = gameRenderer->get<Ground>();
    terrain.clearShadows();
    camera3D.follow(Ship::player->position, Ship::player->rotation);
    Ship::broadcast(&Ship::update, frame);
    drawShips();
    stats();
}

void updateStart(){
    gameState = GameState::Space;
    backlight = 0;
    targetBacklight = 255;
    // universe.load();
}

void updateLogo(){
    // using namespace Graphics;
    // gameRenderer->bind<HUDLayer>();
    // BitmapFrame<4> bmp{logo};
    // draw(bmp, screenWidth / 2 - bmp.width() / 2, screenHeight / 2 - bmp.height() / 2, (frame - 30) * f32(0.01f));
    // camera.y = -f32(frame);
    // gameRenderer->get<Background>().init(bg);

    // if (targetBacklight == 255) {
    //     if (isPressed(Button::A) || isPressed(Button::B) || isPressed(Button::C))
    //         targetBacklight = 0;
    // } else {
    //     if (backlight == 0) {
    //         gameState = GameState::Space;
    //         targetBacklight = 255;
    //     }
    // }
}

void update(){
    INIT_PROFILER;
    PROFILER;

    using namespace Graphics;
    frame++;
    clearText();
    // Particles::clear();
    // gameRenderer->bind<HUDLayer>();
    // clear();

    if (backlight != targetBacklight) {
        if (backlight > targetBacklight) {
            backlight -= 11;
            if (backlight < targetBacklight)
                backlight = targetBacklight;
        } else {
            backlight += 11;
            if (backlight > targetBacklight)
                backlight = targetBacklight;
        }
        if (backlight > 255) backlight = 255;
        setBacklight(s24q8ToF32(backlight));
    }

    if (streamedEffect) {
        static Audio::RAWFileSource* source = nullptr;
        static u32 lastSoundTime = 0;
        auto now = getTime();
        if ((!source || source->ended()) && (now - lastSoundTime > effectPriority)) {
            source = Audio::play<6>(streamedEffect);
            if (source) source->setLoop(false);
            lastSoundTime = now;
            effectPriority = ~u32{};
        }
        streamedEffect = nullptr;
    }

    switch (gameState) {
    case GameState::Start:         updateStart();         break;
    case GameState::Logo:          updateLogo();          break;
    case GameState::Space:         updateSpace();         break;
    case GameState::EnterCutScene: updateEnterCutScene(); break;
    case GameState::CutScene:      updateCutScene();      break;
    }

    // Graphics::setCursor(0, 20);
    // Graphics::print(profiler::getBottleneck());
}
