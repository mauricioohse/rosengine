#pragma once
#include "../systems.h"
#include "../../window.h"
#include "../../engine.h"

struct RenderSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
    
    // Camera properties (we can expand this later)
    float cameraX = 0.0f;
    float cameraY = 0.0f;

private:
    void RenderEntity(TransformComponent* transform, SpriteComponent* sprite);
    void RenderAnimatedEntity(TransformComponent *transform, AnimationComponent *anim);
};