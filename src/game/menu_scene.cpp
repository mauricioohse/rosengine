#include "menu_scene.h"
#include "../core/engine.h"
#include "../core/window.h"

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

    // Create centered game title
    EntityID titleText = RegisterEntity();
    EM->AddComponentToEntity(titleText, COMPONENT_TRANSFORM | COMPONENT_TEXT);

    TransformComponent* titleTransform = &g_Engine.componentArrays.Transforms[titleText];
    titleTransform->Init(
        g_Engine.window->width / 2,  // Center X
        g_Engine.window->height / 3,  // One-third from top
        0,  // No rotation
        1   // Normal scale
    );

    TextComponent* titleTextComp = &g_Engine.componentArrays.Texts[titleText];
    titleTextComp->Init(
        ResourceManager::GetFont(FONT_FPS),
        "Game Name",
        TEXT_CENTER
    );

    state = SceneState::ACTIVE;
}


