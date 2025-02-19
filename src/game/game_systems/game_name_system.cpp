#include "game_name_system.h"
#include "../../core/engine.h"
#include "../../core/resource_manager.h"

void game_name_system::Init() {
    // for testing purposes, creating a small text with game title
    titleEntity = g_Engine.entityManager.CreateEntity();
    
    ADD_TRANSFORM(titleEntity,400,300,0,0);
    ADD_SPRITE(titleEntity, ResourceManager::GetTextTexture(ResourceManager::GetFont(FONT_FPS), "game title!",  {255, 215, 0}));
}

void game_name_system::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {

}

void game_name_system::Destroy(){

}