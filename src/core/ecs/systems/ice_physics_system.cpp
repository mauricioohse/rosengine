#include "ice_physics_system.h"
#include <math.h>

void IcePhysicsSystem::Init() {
    printf("IcePhysicsSystem initialized\n");
    gripMultiplier = 1.0f;
    knockbackResistance = 1.0f;
}

void IcePhysicsSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_PHYSICS)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            PhysicsComponent* physics = 
                (PhysicsComponent*)components->GetComponentData(entity, COMPONENT_PHYSICS);
            
            if (!transform || !physics) continue;
            
            // Apply ice physics
            ApplyFriction(physics, deltaTime);
            LimitSpeed(physics);
            
            // Update position based on velocity
            transform->x += physics->velocityX * deltaTime;
            transform->y += physics->velocityY * deltaTime;
        }
    }
}

void IcePhysicsSystem::ApplyFriction(PhysicsComponent* physics, float deltaTime) {
    // Apply grip multiplier to friction
    float frictionForce = physics->friction * deltaTime * gripMultiplier;
    
    // Calculate velocity magnitude
    float velocityMagnitude = sqrtf(
        physics->velocityX * physics->velocityX + 
        physics->velocityY * physics->velocityY
    );
    
    if (velocityMagnitude > 0) {
        // Calculate friction direction (opposite to movement)
        float frictionDirX = -physics->velocityX / velocityMagnitude;
        float frictionDirY = -physics->velocityY / velocityMagnitude;
        
        // Apply friction force
        physics->velocityX += frictionDirX * frictionForce;
        physics->velocityY += frictionDirY * frictionForce;
        
        // Stop completely if moving very slowly
        if (fabs(physics->velocityX) < 1.0f) physics->velocityX = 0;
        if (fabs(physics->velocityY) < 1.0f) physics->velocityY = 0;
    }
}

void IcePhysicsSystem::LimitSpeed(PhysicsComponent* physics) {
    float currentSpeed = sqrtf(
        physics->velocityX * physics->velocityX + 
        physics->velocityY * physics->velocityY
    );
    
    if (currentSpeed > physics->maxSpeed) {
        float scale = physics->maxSpeed / currentSpeed;
        physics->velocityX *= scale;
        physics->velocityY *= scale;
    }
}

void IcePhysicsSystem::ApplyRecoilForce(PhysicsComponent* physics, float forceX, float forceY) {
    // Apply knockback resistance to recoil
    physics->velocityX -= (forceX / physics->mass) * knockbackResistance;
    physics->velocityY -= (forceY / physics->mass) * knockbackResistance;
}

void IcePhysicsSystem::ApplyUpgrade(UpgradeType type, float value) {
    switch (type) {
        case UPGRADE_GRIP:
            gripMultiplier *= value;  // Improve ice grip
            break;
            
        case UPGRADE_RECOIL_RES:
            knockbackResistance *= value;  // Reduce recoil
            break;
    }
}

void IcePhysicsSystem::Destroy() {
    printf("IcePhysicsSystem destroyed\n");
} 