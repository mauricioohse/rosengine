#include "menu_scene.h"
#include "../core/engine.h"
#include "../core/window.h"

void MenuScene::OnLoad()
{
    EntityManager* EM = &g_Engine.entityManager;
    EntityID box = RegisterEntity();
    EM->AddComponentToEntity(box, COMPONENT_TRANSFORM | COMPONENT_SPRITE | COMPONENT_WASD_CONTROLLER);
    g_Engine.componentArrays.Transforms[box].Init(300,300);
    g_Engine.componentArrays.Sprites[box].Init(ResourceManager::GetTexture(TEXTURE_BOX));
    g_Engine.componentArrays.wasdControllers[box].Init(600);

    // Create centered game title
    EntityID titleText = RegisterEntity();
    ADD_TRANSFORM(titleText, g_Engine.window->width / 2, g_Engine.window->height / 3, 0, 1);
    ADD_TEXT(titleText, ResourceManager::GetFont(FONT_FPS), "Game Name");

    state = SceneState::ACTIVE;
}


