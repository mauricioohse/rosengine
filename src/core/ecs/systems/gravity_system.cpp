#include "gravity_system.h"

void GravitySystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_GRAVITY)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            GravityComponent* gravity = 
                (GravityComponent*)components->GetComponentData(entity, COMPONENT_GRAVITY);
            
            if (!gravity->isGrounded) {
                // Apply gravity
                gravity->velocityY += GRAVITY * gravity->gravityScale * deltaTime;
                
                // Update position
                transform->y += gravity->velocityY * deltaTime;
            }
            
            // Reset grounded state each frame
            // (CollisionSystem will set it to true if needed)
            gravity->isGrounded = false;
        }
    }
}

void GravitySystem::Init() {}
void GravitySystem::Destroy() {}

