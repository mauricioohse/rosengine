#include "collision_system.h"
#include <stdio.h>
#include <stdlib.h>

void CollisionSystem::Init() {
    printf("CollisionSystem initialized\n");
    collisionCount = 0;
}

bool CollisionSystem::CheckCollision(
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

void CollisionSystem::ResolveCollision(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float penetrationX, float penetrationY) 
{
    if (colliderA->isTrigger || colliderB->isTrigger) {
        return;
    }

    // Calculate centers of both objects
    float centerAx = transformA->x + colliderA->width * 0.5f;
    float centerAy = transformA->y + colliderA->height * 0.5f;
    float centerBx = transformB->x + colliderB->width * 0.5f;
    float centerBy = transformB->y + colliderB->height * 0.5f;

    // Determine direction of collision
    float directionX = centerAx - centerBx;  // Positive if A is to the right of B
    float directionY = centerAy - centerBy;  // Positive if A is below B

    // Determine which axis has the smaller penetration
    if (abs(penetrationX) < abs(penetrationY)) {
        // Resolve on X axis
        float moveX = (directionX > 0) ? penetrationX : -penetrationX;
        
        if (!colliderA->isStatic && !colliderB->isStatic) {
            transformA->x += moveX * 0.5f;
            transformB->x -= moveX * 0.5f;
        } else if (!colliderA->isStatic) {
            transformA->x += moveX;
        } else if (!colliderB->isStatic) {
            transformB->x -= moveX;
        }
    } else {
        // Resolve on Y axis
        float moveY = (directionY > 0) ? penetrationY : -penetrationY;
        
        if (!colliderA->isStatic && !colliderB->isStatic) {
            transformA->y += moveY * 0.5f;
            transformB->y -= moveY * 0.5f;
        } else if (!colliderA->isStatic) {
            transformA->y += moveY;
        } else if (!colliderB->isStatic) {
            transformB->y -= moveY;
        }
    }
}

void CollisionSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    collisionCount = 0;
    
    // Check collisions between all entities with colliders
    for (EntityID entityA = 1; entityA < MAX_ENTITIES; entityA++) {
        if (!entities->HasComponent(entityA, COMPONENT_TRANSFORM | COMPONENT_COLLIDER)) {
            continue;
        }
        
        TransformComponent* transformA = 
            (TransformComponent*)components->GetComponentData(entityA, COMPONENT_TRANSFORM);
        ColliderComponent* colliderA = 
            (ColliderComponent*)components->GetComponentData(entityA, COMPONENT_COLLIDER);
            
        for (EntityID entityB = entityA + 1; entityB < MAX_ENTITIES; entityB++) {
            if (!entities->HasComponent(entityB, COMPONENT_TRANSFORM | COMPONENT_COLLIDER)) {
                continue;
            }
            
            TransformComponent* transformB = 
                (TransformComponent*)components->GetComponentData(entityB, COMPONENT_TRANSFORM);
            ColliderComponent* colliderB = 
                (ColliderComponent*)components->GetComponentData(entityB, COMPONENT_COLLIDER);
                
            float penetrationX, penetrationY;
            if (CheckCollision(transformA, colliderA, transformB, colliderB, 
                             penetrationX, penetrationY)) 
            {
                // Store collision
                if (collisionCount < MAX_COLLISIONS) {
                    collisions[collisionCount].entityA = entityA;
                    collisions[collisionCount].entityB = entityB;
                    collisions[collisionCount].penetrationX = penetrationX;
                    collisions[collisionCount].penetrationY = penetrationY;
                    collisionCount++;
                }
                
                // Resolve collision
                ResolveCollision(transformA, colliderA, transformB, colliderB,
                               penetrationX, penetrationY);
            }
        }
    }
}

void CollisionSystem::Destroy() {
    printf("CollisionSystem destroyed\n");
} 