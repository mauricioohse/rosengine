#include "ice_physics_system.h"
#include <math.h>

void IcePhysicsSystem::Init() {
    printf("IcePhysicsSystem initialized\n");
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
    // Gradually reduce velocity based on friction
    float frictionForce = physics->friction * deltaTime;
    
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
    // Apply force in opposite direction of shooting
    physics->velocityX -= forceX / physics->mass;
    physics->velocityY -= forceY / physics->mass;
}

void IcePhysicsSystem::Destroy() {
    printf("IcePhysicsSystem destroyed\n");
} 