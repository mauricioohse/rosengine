#pragma once
#include "../systems.h"

struct BackgroundSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
}; 