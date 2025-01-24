#pragma once
#include "../systems.h"

struct IcePhysicsSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
    
    // Helper functions
    void ApplyFriction(PhysicsComponent* physics, float deltaTime);
    void LimitSpeed(PhysicsComponent* physics);
    void ApplyRecoilForce(PhysicsComponent* physics, float forceX, float forceY);
}; 