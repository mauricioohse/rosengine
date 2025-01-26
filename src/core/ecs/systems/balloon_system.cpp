#include "balloon_system.h"
#include "../components.h"
#include "../component_macros.h"
#include "../../../game/game.h"
#include "../../utils.h"
#include <math.h>

void BalloonSystem::Init() {
    printf("BalloonSystem initialized\n");
}

bool BalloonCheckCollision(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float& penetrationX, float& penetrationY) 
{
    return CheckCollisionCentered(transformA, colliderA, transformB, colliderB, 
                                penetrationX, penetrationY);
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
                    
                case BALLOON_GREEN:
                    UpdateGreenBalloon(deltaTime, balloon, balloonTransform, physics, entities, components);
                    break;
                    
                default:
                    break;
            }
        }
    }

    // Update projectiles
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_PROJECTILE | COMPONENT_TRANSFORM | COMPONENT_COLLIDER)) {
            ProjectileComponent* projectile = 
                (ProjectileComponent*)components->GetComponentData(entity, COMPONENT_PROJECTILE);
            TransformComponent* projectileTransform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            ColliderComponent* projectileCollider = 
                (ColliderComponent*)components->GetComponentData(entity, COMPONENT_COLLIDER);
                
            if (!projectile || !projectileTransform || !projectileCollider) continue;
            
            // Update lifetime
            projectile->lifetime -= deltaTime;
            if (projectile->lifetime <= 0) {
                entities->DestroyEntity(entity);
                continue;
            }
            
            // Check collision with porcupine
            TransformComponent* porcupineTransform = 
                (TransformComponent*)components->GetComponentData(g_Game.squirrelEntity, COMPONENT_TRANSFORM);
            ColliderComponent* porcupineCollider = 
                (ColliderComponent*)components->GetComponentData(g_Game.squirrelEntity, COMPONENT_COLLIDER);
            PhysicsComponent* porcupinePhysics = 
                (PhysicsComponent*)components->GetComponentData(g_Game.squirrelEntity, COMPONENT_PHYSICS);
                
            if (!porcupineTransform || !porcupineCollider || !porcupinePhysics) continue;
            
            float penetrationX, penetrationY;
            if (BalloonCheckCollision(projectileTransform, projectileCollider,
                                    porcupineTransform, porcupineCollider,
                                    penetrationX, penetrationY)) 
            {
                // Increase damage percentage
                porcupinePhysics->damagePercent += projectile->damage;
                
                // Calculate knockback multiplier
                porcupinePhysics->knockbackMultiplier = 1.0f + (porcupinePhysics->damagePercent / 100.0f);
                
                // Apply knockback in laser's direction
                float scaledKnockback = KNOCKBACK_FORCE * porcupinePhysics->knockbackMultiplier;
                porcupinePhysics->velocityX += projectile->directionX * scaledKnockback;
                porcupinePhysics->velocityY += projectile->directionY * scaledKnockback;
                
                // Destroy the laser
                entities->DestroyEntity(entity);
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

void BalloonSystem::UpdateGreenBalloon(float deltaTime, 
                                     BalloonComponent* balloon,
                                     TransformComponent* transform,
                                     PhysicsComponent* physics,
                                     EntityManager* entities,
                                     ComponentArrays* components) 
{
    // Update shoot timer
    balloon->shootTimer += deltaTime;
    
    // Circular movement
    balloon->orbitAngle += deltaTime * balloon->moveSpeed / 500.0f;  // Adjust speed as needed
    float orbitRadius = 600.0f;  // Distance from center
    
    // Calculate desired position on circle
    float targetX = cos(balloon->orbitAngle) * orbitRadius;
    float targetY = sin(balloon->orbitAngle) * orbitRadius;
    
    // Calculate direction to target position
    float dx = targetX - transform->x;
    float dy = targetY - transform->y;
    
    // Normalize and apply movement
    float distance = sqrt(dx*dx + dy*dy);
    if (distance > 0) {
        dx /= distance;
        dy /= distance;
        physics->velocityX = dx * balloon->moveSpeed;
        physics->velocityY = dy * balloon->moveSpeed;
    }
    
    // Shoot laser every second
    if (balloon->shootTimer >= 1.0f) {
        balloon->shootTimer = 0.0f;
        ShootLasers(transform->x, transform->y, entities, components);
    }
}

void BalloonSystem::ShootLasers(float x, float y, 
                               EntityManager* entities, 
                               ComponentArrays* components) 
{
    // Directions for orthogonal and diagonal shots
    const int NUM_DIRECTIONS = 8;
    const float LASER_SPEED = 200.0f;
    const float LASER_DAMAGE = 5.0f;
    const float LASER_LIFETIME = 5.0f;
    const float LASER_KNOCKBACK = 500.0f;
    
    const float DIRECTIONS[8][2] = {
        {1, 0},   // Right
        {1, 1},   // Down-Right
        {0, 1},   // Down
        {-1, 1},  // Down-Left
        {-1, 0},  // Left
        {-1, -1}, // Up-Left
        {0, -1},  // Up
        {1, -1}   // Up-Right
    };
    
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        // Create laser entity
        EntityID laser = entities->CreateEntity();
        
        float dx = DIRECTIONS[i][0];
        float dy = DIRECTIONS[i][1];
        // Normalize diagonal vectors
        if (dx != 0 && dy != 0) {
            float norm = sqrt(2.0f);
            dx /= norm;
            dy /= norm;
        }
        
        // Add components
        ADD_TRANSFORM(laser, x, y, 0.0f, 1.0f);
        ADD_SPRITE(laser, ResourceManager::GetTexture(TEXTURE_BOX_ANIM_SHEET));
        ADD_PHYSICS(laser, 1.0f, 0.0f);  // Light mass, no friction
        ADD_COLLIDER(laser, 16, 16, 0, 1);  // Adjust size as needed
        ADD_PROJECTILE(laser, LASER_DAMAGE, LASER_LIFETIME, dx, dy, 0);
        
        // Initialize physics for movement
        PhysicsComponent* physics = 
            (PhysicsComponent*)components->GetComponentData(laser, COMPONENT_PHYSICS);
        if (physics) {
            physics->velocityX = dx * LASER_SPEED;
            physics->velocityY = dy * LASER_SPEED;
        }
    }
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

void ExplodeBalloon(EntityID balloonEntity, EntityManager* entities) {
    // For now, just destroy the entity
    // Later we can add particle effects, sound, etc.
    entities->DestroyEntity(balloonEntity);
}

void BalloonSystem::Destroy() {
    printf("BalloonSystem destroyed\n");
} 