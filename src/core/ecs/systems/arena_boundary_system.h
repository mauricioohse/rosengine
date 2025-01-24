#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

class ArenaBoundarySystem : public System {
public:
    static constexpr float ARENA_RADIUS = 800.0f;
    static constexpr float SAFE_RADIUS = 750.0f;
    static constexpr float BOUNDARY_FORCE = 1000.0f;  // Base force to push back
    static constexpr float FORCE_SCALE = 2.0f;       // How quickly force increases with distance

    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;

    // Get reference to porcupine entity from game
    void SetPorcupineEntity(EntityID entity) { porcupineEntity = entity; }
    bool IsOutOfBounds() const { return isOutOfBounds; }

private:
    EntityID porcupineEntity;
    bool isOutOfBounds;

    float CalculateDistanceFromCenter(float x, float y);
    void ApplyBoundaryForce(PhysicsComponent* physics, float distance, float x, float y);
}; 