#include "balloon_system.h"
#include "../components.h"
#include <math.h>

void BalloonSystem::Init() {
    printf("BalloonSystem initialized\n");
}

void BalloonSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_BALLOON | COMPONENT_TRANSFORM | COMPONENT_PHYSICS)) {
            BalloonComponent* balloon = 
                (BalloonComponent*)components->GetComponentData(entity, COMPONENT_BALLOON);
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            PhysicsComponent* physics = 
                (PhysicsComponent*)components->GetComponentData(entity, COMPONENT_PHYSICS);
                
            if (!balloon || !transform || !physics) continue;
            
            // Get target transform
            TransformComponent* targetTransform = 
                (TransformComponent*)components->GetComponentData(balloon->targetEntity, COMPONENT_TRANSFORM);
            if (!targetTransform) continue;
            
            // Handle behavior based on balloon type
            switch (balloon->type) {
                case BALLOON_RED:
                    UpdateRedBalloon(deltaTime, balloon, transform, physics, targetTransform);
                    break;
                    
                // Other balloon types will be added here later
                default:
                    break;
            }
        }
    }
}

void BalloonSystem::UpdateRedBalloon(float deltaTime, 
                                   BalloonComponent* balloon,
                                   TransformComponent* transform,
                                   PhysicsComponent* physics,
                                   TransformComponent* targetTransform) 
{
    // Calculate direction to target
    float dx = targetTransform->x - transform->x;
    float dy = targetTransform->y - transform->y;
    
    // Normalize direction
    float distance = sqrt(dx*dx + dy*dy);
    if (distance > 0) {
        dx /= distance;
        dy /= distance;
    }
    
    // Apply force towards target through physics component
    float force = balloon->moveSpeed * physics->mass;
    physics->velocityX += dx * force * deltaTime;
    physics->velocityY += dy * force * deltaTime;
}

void BalloonSystem::Destroy() {
    printf("BalloonSystem destroyed\n");
} 