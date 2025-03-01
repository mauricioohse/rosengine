#include "menu_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"
#include "main_game_scene.h"

MenuScene g_menu;   

// Callback function
void StartGameButtonClicked() {
    g_menu.state =  SceneState::DESTROYED;
    g_mainGame.state = SceneState::ACTIVE;
}

void MenuScene::OnLoad()
{
    EntityManager* EM = &g_Engine.entityManager;

    // Create centered game title
    EntityID titleText = RegisterEntity();
    ADD_TRANSFORM(titleText, g_Engine.window->width / 2, g_Engine.window->height / 3, 0, 1);
    ADD_TEXT(titleText, ResourceManager::GetFont(FONT_FPS), "Game Name");

    // Create Start Game button
    EntityID startButton = RegisterEntity();
    EM->AddComponentToEntity(startButton, COMPONENT_TRANSFORM | COMPONENT_UIBOX | COMPONENT_TEXT);
    
    // Initialize button position and size
    float buttonWidth = 200;
    float buttonHeight = 50;
    float buttonX = (g_Engine.window->width - buttonWidth) / 2;  // Center horizontally
    float buttonY = g_Engine.window->height / 2;                 // Place below title
    
    // Setup transform
    g_Engine.componentArrays.Transforms[startButton].Init(buttonX, buttonY);
    
    // Setup UI box with callback
    g_Engine.componentArrays.UIBoxs[startButton].Init(
        buttonWidth, 
        buttonHeight,
        SDL_Color{50, 50, 50, 255},      // Dark gray background
        SDL_Color{200, 200, 200, 255},   // Light gray border
        2.0f,                            // Border width
        StartGameButtonClicked           // Click callback
    );
    
    // Setup text
    g_Engine.componentArrays.Texts[startButton].Init(
        ResourceManager::GetFont(FONT_FPS),
        "Start Game",
        TEXT_CENTER
    );

    state = SceneState::ACTIVE;
}


