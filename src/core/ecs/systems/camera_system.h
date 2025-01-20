#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

struct CameraSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
}; 