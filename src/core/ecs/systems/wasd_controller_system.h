#pragma once
#include "../systems.h"
#include "../../input.h"

struct WASDControllerSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
}; 