#include "main_game_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"

MainGameScene g_mainGame;

void MainGameScene::OnLoad() 
{
    EntityManager* EM = &g_Engine.entityManager;
    
    // Create the box entity (moved from menu scene)
    EntityID box = RegisterEntity();
    EM->AddComponentToEntity(box, COMPONENT_TRANSFORM | COMPONENT_SPRITE | COMPONENT_WASD_CONTROLLER);
    g_Engine.componentArrays.Transforms[box].Init(300, 300);
    g_Engine.componentArrays.Sprites[box].Init(ResourceManager::GetTexture(TEXTURE_BOX));
    g_Engine.componentArrays.wasdControllers[box].Init(600);

    state = SceneState::ACTIVE;
}
