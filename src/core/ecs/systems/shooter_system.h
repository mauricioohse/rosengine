#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"
#include "wave_system.h"


class ShooterSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
    void ApplyUpgrade(UpgradeType type, float value);

    // Getters for stats
    float GetFireRateMultiplier() const { return fireRateMultiplier; }
    int GetExtraProjectiles() const { return extraProjectiles; }
    float GetDamageMultiplier() const { return damageMultiplier; }

private:
    void SpawnQuill(EntityManager* entities, ComponentArrays* components,
                    float x, float y, float dirX, float dirY, 
                    float speed, EntityID owner);
    
    void UpdateQuills(float deltaTime, EntityManager* entities, ComponentArrays* components);

    float damageMultiplier;
    int extraProjectiles;
    float fireRateMultiplier;
}; 