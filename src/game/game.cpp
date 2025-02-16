#include "game.h"
#include "../core/resource_manager.h"
#include "../core/window.h"
#include "../core/input.h"
#include <math.h>
#include <algorithm>

Game g_Game;

bool Game::Init() {
    g_Engine.systemManager.RegisterSystem(&renderSystem);
    g_Engine.systemManager.RegisterSystem(&cameraSystem);
    g_Engine.systemManager.RegisterSystem(&collisionSystem);
    g_Engine.systemManager.RegisterSystem(&musicSystem);
    g_Engine.systemManager.RegisterSystem(new WASDControllerSystem());

    // Create background
    EntityID backgroundEntity = g_Engine.entityManager.CreateEntity();
    Texture* backgroundTexture = ResourceManager::GetTexture(TEXTURE_BACKGROUND_MIDDLE);
    ADD_TRANSFORM(backgroundEntity, 0.0f, 0.0f, 0.0f, 1.0f);
    ADD_SPRITE(backgroundEntity, backgroundTexture);
    ADD_BACKGROUND(backgroundEntity, 0.5f);  // 0.5 parallax factor for medium depth


    // Create squirrel entity
    squirrelEntity = g_Engine.entityManager.CreateEntity();
    
    Texture* squirrelTexture = ResourceManager::GetTexture(TEXTURE_PORCUPINE_LEFT);    
    // Add basic components
    ADD_TRANSFORM(squirrelEntity, 200.0f, 200.0f, 0.0f, 1.0f);  // Center-top of screen
    ADD_SPRITE(squirrelEntity, squirrelTexture);
    ADD_WASD_CONTROLLER(squirrelEntity, 600, 1);
    ADD_COLLIDER(squirrelEntity, 32, 32, 0, 0);
    
    return true;
}

// logic related to inputs on the game should go here
void Game::HandleInput(){

}

void Game::Update(float deltaTime) {

}


void Game::Render() {
    g_Engine.systemManager.UpdateSystems(g_Engine.deltaTime, &g_Engine.entityManager, &g_Engine.componentArrays);
    
}

void Game::Cleanup() {
    
}

void Game::Reset() {

    gameState = GAME_STATE_START;

}


