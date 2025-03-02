#include "pause_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"

PauseScene g_pauseScene;

void PauseScene::OnLoad()
{
    EntityManager* EM = &g_Engine.entityManager;

    // Create semi-transparent overlay covering the whole screen
    EntityID overlay = RegisterEntity();
    EM->AddComponentToEntity(overlay, COMPONENT_TRANSFORM | COMPONENT_UIBOX);
    
    // Setup transform for full screen
    g_Engine.componentArrays.Transforms[overlay].Init(0, 0);
    
    // Setup UI box with semi-transparent dark background
    g_Engine.componentArrays.UIBoxs[overlay].Init(
        g_Engine.window->width,
        g_Engine.window->height,
        SDL_Color{30, 30, 30, 30},    // Semi-transparent black
        SDL_Color{0, 0, 0, 0},      // No border
        0.0f,                       // No border width
        nullptr                     // No callback
    );

    // Create "PAUSED" text
    EntityID pausedText = RegisterEntity();
    ADD_TRANSFORM(pausedText, g_Engine.window->width / 2, g_Engine.window->height / 4, 0, 1.5f);
    ADD_TEXT(pausedText, ResourceManager::GetFont(FONT_FPS), "PAUSED");

    // Create Resume button
    EntityID resumeButton = RegisterEntity();
    ADD_TRANSFORM(resumeButton, g_Engine.window->width / 2, g_Engine.window->height / 2, 0, 1.5f);
    ADD_TEXT(resumeButton, ResourceManager::GetFont(FONT_FPS), "Resume");
    g_Engine.componentArrays.UIBoxs[resumeButton].Init(200, 50, SDL_Color{255, 255, 255, 255}, SDL_Color{0, 0, 0, 0}, 0.0f, nullptr);


    
    // Initialize button position and size


    state = SceneState::INACTIVE;
} 