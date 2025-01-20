#pragma once
#include "../systems.h"

struct Collision {
    EntityID entityA;
    EntityID entityB;
    float penetrationX;
    float penetrationY;
};

struct CollisionSystem : System {
    static const int MAX_COLLISIONS = 1024;
    Collision collisions[MAX_COLLISIONS];
    int collisionCount;

    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;

private:
    bool CheckCollision(
        TransformComponent* transformA, ColliderComponent* colliderA,
        TransformComponent* transformB, ColliderComponent* colliderB,
        float& penetrationX, float& penetrationY);
    
    void ResolveCollision(
        TransformComponent* transformA, ColliderComponent* colliderA,
        TransformComponent* transformB, ColliderComponent* colliderB,
        float penetrationX, float penetrationY);
}; 