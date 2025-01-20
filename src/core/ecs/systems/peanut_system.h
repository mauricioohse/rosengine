#pragma once
#include "../systems.h"

class PeanutSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
}; 