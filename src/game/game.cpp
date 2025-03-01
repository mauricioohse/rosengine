#include "game.h"
#include "../core/resource_manager.h"
#include "../core/window.h"
#include "../core/input.h"
#include "../core/resource_manager.h"
#include "../core/ecs/systems/render_system.h"
#include "../core/ecs/systems/wasd_controller_system.h"
#include "../core/ecs/systems/collision_system.h"
#include "../core/ecs/systems/camera_system.h"
#include "../core/ecs/systems/background_system.h"
#include "../core/ecs/systems/music_system.h"
#include "../core/ecs/systems/text_update_system.h"
#include "../core/ecs/systems/ui_system.h"
#include <math.h>
#include <algorithm>
#include "menu_scene.h"

Game g_Game;
MenuScene menu;

bool Game::Init() {
    g_Engine.systemManager.RegisterSystem(new RenderSystem());
    g_Engine.systemManager.RegisterSystem(new WASDControllerSystem());
    g_Engine.systemManager.RegisterSystem(new TextUpdateSystem());
    g_Engine.systemManager.RegisterSystem(new UISystem());


    // // Create background
    // EntityID backgroundEntity = g_Engine.entityManager.CreateEntity();
    // Texture* backgroundTexture = ResourceManager::GetTexture(TEXTURE_BACKGROUND_MIDDLE);
    // ADD_TRANSFORM(backgroundEntity, 0.0f, 0.0f, 0.0f, 1.0f);
    // ADD_SPRITE(backgroundEntity, backgroundTexture);
    // ADD_BACKGROUND(backgroundEntity, 0.5f);  // 0.5 parallax factor for medium depth


    // // Create squirrel entity
    // playerEntity = g_Engine.entityManager.CreateEntity();
    
    // Texture* playerTex = ResourceManager::GetTexture(TEXTURE_PORCUPINE_LEFT);    
    // // Add basic components
    // ADD_TRANSFORM(playerEntity, 200.0f, 200.0f, 0.0f, 1.0f);  // Center-top of screen
    // ADD_SPRITE(playerEntity, playerTex);
    // ADD_WASD_CONTROLLER(playerEntity, 600, 1);
    // ADD_COLLIDER(playerEntity, 32, 32, 0, 0);

    g_Engine.sceneManager.PushScene(&menu);

    menu.OnLoad();
    
    return true;
}

// logic related to inputs on the game should go here
void Game::HandleInput(){

}

void Game::Update(float deltaTime) {

    g_Engine.sceneManager.Update(deltaTime);

}


void Game::Render() {
    //g_Engine.systemManager.UpdateSystems(g_Engine.deltaTime, g_Engine.entityManager.activeEntities, &g_Engine.componentArrays);



}

void Game::Cleanup() {
    
}

void Game::Reset() {

    gameState = GAME_STATE_START;

}


