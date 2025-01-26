#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

bool BalloonCheckCollision(TransformComponent *transformA, ColliderComponent *colliderA, TransformComponent *transformB, ColliderComponent *colliderB, float &penetrationX, float &penetrationY);
void ExplodeBalloon(EntityID balloonEntity, EntityManager* entities);


class BalloonSystem : public System {
public:
    static constexpr float KNOCKBACK_FORCE = 300.0f;  // Force applied to porcupine on collision

    void Init() override;
    void Update(float deltaTime, EntityManager *entities, ComponentArrays *components) override;
    void Destroy() override;

private:
    void UpdateRedBalloon(float deltaTime, 
                         BalloonComponent* balloon,
                         TransformComponent* transform,
                         PhysicsComponent* physics,
                         TransformComponent* targetTransform);

    void UpdateGreenBalloon(float deltaTime, BalloonComponent *balloon, TransformComponent *transform, PhysicsComponent *physics, EntityManager *entities, ComponentArrays *components);

    void ShootLasers(float x, float y, EntityManager *entities, ComponentArrays *components);

    void HandleBalloonCollision(EntityID balloonEntity,
                                TransformComponent *balloonTransform,
                                TransformComponent *porcupineTransform,
                                PhysicsComponent *porcupinePhysics,
                                EntityManager *entities);

}; 