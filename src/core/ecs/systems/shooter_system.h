#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

class ShooterSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;

private:
    void SpawnQuill(EntityManager* entities, ComponentArrays* components,
                    float x, float y, float dirX, float dirY, 
                    float speed, EntityID owner);
    
    void UpdateQuills(float deltaTime, EntityManager* entities, ComponentArrays* components);
}; 