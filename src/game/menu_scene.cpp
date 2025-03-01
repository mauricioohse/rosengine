#include "menu_scene.h"
#include "../core/engine.h"

void MenuScene::OnLoad()
{
    EntityManager* EM = &g_Engine.entityManager;
    EntityID box = RegisterEntity();
    EM->AddComponentToEntity(box, COMPONENT_TRANSFORM | COMPONENT_SPRITE | COMPONENT_WASD_CONTROLLER);

    TransformComponent *tc = &g_Engine.componentArrays.Transforms[box];
    tc->Init(300,300,0,1);

    SpriteComponent *sc = &g_Engine.componentArrays.Sprites[box];
    Texture * t = ResourceManager::GetTexture(TEXTURE_BOX);
    sc->Init(t);

    WASDControllerComponent* wc = &g_Engine.componentArrays.wasdControllers[box];
    wc->canMove = 1;
    wc->moveSpeed = 600;

    state = SceneState::ACTIVE;

}


