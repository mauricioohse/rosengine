#include "menu_scene.h"

void MenuScene::OnLoad()
{
    EntityID box = entityManager.CreateEntity();
    entityManager.AddComponentToEntity(box, COMPONENT_TRANSFORM | COMPONENT_SPRITE);

    // do I need to use pointers?
    TransformComponent *tc = &components.Transforms[box];
    tc->Init(300,300,0,1);

    SpriteComponent *sc = &components.Sprites[box];
    Texture * t = ResourceManager::GetTexture(TEXTURE_BOX);
    sc->Init(t);

    state = SceneState::ACTIVE;

}


