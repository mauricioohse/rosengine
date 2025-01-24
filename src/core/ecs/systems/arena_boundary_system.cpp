#include "arena_boundary_system.h"
#include <math.h>

void ArenaBoundarySystem::Init() {
    printf("ArenaBoundarySystem initialized\n");
    porcupineEntity = 0;
    isOutOfBounds = false;
}

void ArenaBoundarySystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    if (!porcupineEntity) return;

    // Get porcupine components
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(porcupineEntity, COMPONENT_TRANSFORM);
    PhysicsComponent* physics = 
        (PhysicsComponent*)components->GetComponentData(porcupineEntity, COMPONENT_PHYSICS);
    
    if (!transform || !physics) return;

    // Calculate distance from arena center
    float distance = CalculateDistanceFromCenter(transform->x, transform->y);
    
    // Update out of bounds status
    isOutOfBounds = distance > ARENA_RADIUS;
    
    // Apply boundary force if outside safe radius
    if (distance > SAFE_RADIUS) {
        ApplyBoundaryForce(physics, distance, transform->x, transform->y);
    }
}

float ArenaBoundarySystem::CalculateDistanceFromCenter(float x, float y) {
    // Assuming arena is centered at (0,0)
    return sqrt(x * x + y * y);
}

void ArenaBoundarySystem::ApplyBoundaryForce(PhysicsComponent* physics, float distance, float x, float y) {
    // Calculate normalized direction to center
    float dx = -x / distance;  // Negative because we want to push towards center
    float dy = -y / distance;
    
    // Scale force based on how far past safe radius
    float forceMagnitude = BOUNDARY_FORCE * pow(FORCE_SCALE, distance - SAFE_RADIUS);
    
    // Apply force
    physics->velocityX += dx * forceMagnitude * physics->mass;
    physics->velocityY += dy * forceMagnitude * physics->mass;
}

void ArenaBoundarySystem::Destroy() {
    printf("ArenaBoundarySystem destroyed\n");
} 