#pragma once
#include "../systems.h"
#include "wave_system.h"

struct IcePhysicsSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
    
    // Helper functions
    void ApplyFriction(PhysicsComponent* physics, float deltaTime);
    void LimitSpeed(PhysicsComponent* physics);
    void ApplyRecoilForce(PhysicsComponent* physics, float forceX, float forceY);

    void ApplyUpgrade(UpgradeType type, float value);

    // Getters for stats
    float GetGripMultiplier() const { return gripMultiplier; }
    float GetKnockbackResistance() const { return knockbackResistance; }

private:
    float gripMultiplier;
    float knockbackResistance;
}; 