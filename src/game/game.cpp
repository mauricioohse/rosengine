#include "game.h"
#include "../core/resource_manager.h"
#include "../core/window.h"
#include "../core/input.h"
#include "cloud_init.h"
#include "peanut_init.h"
#include <math.h>

Game g_Game;

bool Game::Init() {
    // Initialize systems
    // renderSystem.Init();
    // squirrelSystem.Init();
    // cameraSystem.Init();
    // cloudSystem.Init();
    // backgroundSystem.Init();
    // peanutSystem.Init();
    // collisionSystem.Init();
    // musicSystem.Init();
    
    
    g_Engine.systemManager.RegisterSystem(&backgroundSystem);
    g_Engine.systemManager.RegisterSystem(&renderSystem);
    //g_Engine.systemManager.RegisterSystem(&squirrelSystem);
    g_Engine.systemManager.RegisterSystem(&cameraSystem);
    g_Engine.systemManager.RegisterSystem(&cloudSystem);
    g_Engine.systemManager.RegisterSystem(&peanutSystem);
    g_Engine.systemManager.RegisterSystem(&collisionSystem);
    g_Engine.systemManager.RegisterSystem(&musicSystem);
    g_Engine.systemManager.RegisterSystem(new WASDControllerSystem());
    g_Engine.systemManager.RegisterSystem(new IcePhysicsSystem());
    g_Engine.systemManager.RegisterSystem(new ShooterSystem());

    // Create background
    backgroundEntity = g_Engine.entityManager.CreateEntity();
    Texture* backgroundTexture = ResourceManager::GetTexture(TEXTURE_BACKGROUND_MIDDLE);
    ADD_TRANSFORM(backgroundEntity, -600.0f, 0.0f, 0.0f, 1.0f);
    ADD_SPRITE(backgroundEntity, backgroundTexture);
    ADD_BACKGROUND(backgroundEntity, 0.5f);  // 0.5 parallax factor for medium depth


    // Create squirrel entity
    squirrelEntity = g_Engine.entityManager.CreateEntity();
    
    Texture* squirrelTexture = ResourceManager::GetTexture(TEXTURE_SQUIRREL_OPEN);    
    // Add basic components
    ADD_TRANSFORM(squirrelEntity, 1200.0f, 100.0f, 0.0f, 1.0f);  // Center-top of screen
    ADD_PHYSICS(squirrelEntity, 100, 2);
    ADD_SPRITE(squirrelEntity, squirrelTexture);
    ADD_WASD_CONTROLLER(squirrelEntity, 400, 1);
    ADD_COLLIDER(squirrelEntity, 32, 32, 0, 0);
    ADD_SHOOTER(squirrelEntity, 150.0f, 300.0f, 1.0f);

    // create camera
    cameraEntity = g_Engine.entityManager.CreateEntity();
    ADD_TRANSFORM(cameraEntity, 1200.0f, 100.0f, 0.0f, 1.0f);
    ADD_CAMERA(cameraEntity, WINDOW_WIDTH, WINDOW_HEIGHT, squirrelEntity);

    // Store IDs for later use
    hitSoundID = SOUND_HIT;
    fpsFontID = FONT_FPS;
    

    gameTimer = 0.0f;


    gameState = GAME_STATE_PLAYING;
    bestTime = 999999.0f;  // Some high number
    isNewRecord = false;

    // Create arrow entity
    arrowEntity = g_Engine.entityManager.CreateEntity();
    Texture* arrowTexture = ResourceManager::GetTexture(TEXTURE_ARROW);
    ADD_TRANSFORM(arrowEntity, 0.0f, 0.0f, 0.0f, 1.0f);
    ADD_SPRITE(arrowEntity, arrowTexture);

    return true;
}

// logic related to inputs on the game should go here
void Game::HandleInput(){

    // Play sound on mouse click
    if (Input::mouseButtonsPressed[0]) {  // Left click
        Sound* hitSound = ResourceManager::GetSound(SOUND_HIT);
        if (hitSound) {
            Mix_PlayChannel(-1, hitSound->sdlChunk, 0);
        }
    }
    
    // Add reset on 'R' key press
    if (Input::IsKeyPressed(SDL_SCANCODE_R)) {
        Reset();
    }
}

void Game::Update(float deltaTime) {
    HandleInput();

    // Get squirrel position
    SquirrelComponent *squirrel =
        (SquirrelComponent *)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_SQUIRREL);

}

void Game::Render() {
    // Systems will handle rendering of entities
    g_Engine.systemManager.UpdateSystems(g_Engine.deltaTime, &g_Engine.entityManager, &g_Engine.componentArrays);
    
}

void Game::Cleanup() {
    
    // Resources will be cleaned up by ResourceManager
}

void Game::Reset() {
    // Reset game state
    gameState = GAME_STATE_PLAYING;
    gameTimer = 0.0f;
    isNewRecord = false;
    
}

