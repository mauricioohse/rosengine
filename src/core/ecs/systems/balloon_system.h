#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

class BalloonSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;

private:
    void UpdateRedBalloon(float deltaTime, 
                         BalloonComponent* balloon,
                         TransformComponent* transform,
                         PhysicsComponent* physics,
                         TransformComponent* targetTransform);
}; 