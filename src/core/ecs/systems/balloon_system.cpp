#include "balloon_system.h"
#include "../components.h"
#include <math.h>

void BalloonSystem::Init() {
    printf("BalloonSystem initialized\n");
}

bool BalloonCheckCollision(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float& penetrationX, float& penetrationY) 
{
    // Calculate boundaries for first box
    float leftA = transformA->x;
    float rightA = transformA->x + colliderA->width;
    float topA = transformA->y;
    float bottomA = transformA->y + colliderA->height;
    
    // Calculate boundaries for second box
    float leftB = transformB->x;
    float rightB = transformB->x + colliderB->width;
    float topB = transformB->y;
    float bottomB = transformB->y + colliderB->height;
    
    // Check if boxes overlap
    if (leftA < rightB && rightA > leftB &&
        topA < bottomB && bottomA > topB) {
        
        // Calculate penetration depths
        penetrationX = (rightA > rightB) ? 
            rightB - leftA : 
            rightA - leftB;
            
        penetrationY = (bottomA > bottomB) ? 
            bottomB - topA : 
            bottomA - topB;
            
        return true;
    }
    
    return false;
}

void BalloonSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_BALLOON | COMPONENT_TRANSFORM | COMPONENT_PHYSICS)) {
            BalloonComponent* balloon = 
                (BalloonComponent*)components->GetComponentData(entity, COMPONENT_BALLOON);
            TransformComponent* balloonTransform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            PhysicsComponent* physics = 
                (PhysicsComponent*)components->GetComponentData(entity, COMPONENT_PHYSICS);
                
            if (!balloon || !balloonTransform || !physics) continue;
            
            // Get target (porcupine) components
            TransformComponent* porcupineTransform = 
                (TransformComponent*)components->GetComponentData(balloon->targetEntity, COMPONENT_TRANSFORM);
            PhysicsComponent* porcupinePhysics = 
                (PhysicsComponent*)components->GetComponentData(balloon->targetEntity, COMPONENT_PHYSICS);
            if (!porcupineTransform || !porcupinePhysics) continue;
            
            // Check for collision with porcupine
            float penetrationX, penetrationY;
            ColliderComponent* balloonCollider = 
                (ColliderComponent*)components->GetComponentData(entity, COMPONENT_COLLIDER);
            ColliderComponent* porcupineCollider = 
                (ColliderComponent*)components->GetComponentData(balloon->targetEntity, COMPONENT_COLLIDER);
                
            if (balloonCollider && porcupineCollider) {
                if (BalloonCheckCollision(balloonTransform, balloonCollider,
                                 porcupineTransform, porcupineCollider,
                                 penetrationX, penetrationY)) 
                {
                    HandleBalloonCollision(entity, balloonTransform, 
                                         porcupineTransform, porcupinePhysics, 
                                         entities);
                    continue;  // Skip movement update since balloon is destroyed
                }
            }
            
            // Handle behavior based on balloon type
            switch (balloon->type) {
                case BALLOON_RED:
                    UpdateRedBalloon(deltaTime, balloon, balloonTransform, physics, porcupineTransform);
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

void BalloonSystem::HandleBalloonCollision(EntityID balloonEntity,
                                         TransformComponent* balloonTransform,
                                         TransformComponent* porcupineTransform,
                                         PhysicsComponent* porcupinePhysics,
                                         EntityManager* entities) 
{
    // Calculate direction from balloon to porcupine
    float dx = porcupineTransform->x - balloonTransform->x;
    float dy = porcupineTransform->y - balloonTransform->y;
    
    // Normalize direction
    float distance = sqrt(dx*dx + dy*dy);
    if (distance > 0) {
        dx /= distance;
        dy /= distance;
    }
    
    // Increase damage percentage
    porcupinePhysics->damagePercent += 10.0f; // Each balloon hit adds 10%
    
    // Calculate knockback multiplier based on damage percentage
    // Formula: 1.0 + (damage% / 100) gives us values like:
    // 0% damage = 1.0x knockback
    // 50% damage = 1.5x knockback
    // 100% damage = 2.0x knockback
    porcupinePhysics->knockbackMultiplier = 1.0f + (porcupinePhysics->damagePercent / 100.0f);
    
    // Apply scaled knockback force to porcupine (away from balloon)
    float scaledKnockback = KNOCKBACK_FORCE * porcupinePhysics->knockbackMultiplier;
    porcupinePhysics->velocityX += dx * scaledKnockback;
    porcupinePhysics->velocityY += dy * scaledKnockback;
    
    // Destroy the balloon
    ExplodeBalloon(balloonEntity, entities);
}

void BalloonSystem::ExplodeBalloon(EntityID balloonEntity, EntityManager* entities) {
    // For now, just destroy the entity
    // Later we can add particle effects, sound, etc.
    entities->DestroyEntity(balloonEntity);
}

void BalloonSystem::Destroy() {
    printf("BalloonSystem destroyed\n");
} 