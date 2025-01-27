#include "wasd_controller_system.h"
#include <stdio.h>
#include <math.h>

void WASDControllerSystem::Init() {
    printf("WASDControllerSystem initialized\n");
    speedMultiplier = 1.0f;
}

void WASDControllerSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    // Loop through all entities
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        // Check if entity has both transform and WASD controller components
        if (entities->HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_WASD_CONTROLLER | COMPONENT_PHYSICS)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            WASDControllerComponent* controller = 
                (WASDControllerComponent*)components->GetComponentData(entity, COMPONENT_WASD_CONTROLLER);
            PhysicsComponent* physics =
                (PhysicsComponent*)components->GetComponentData(entity, COMPONENT_PHYSICS);
            
            if (!transform || !controller || !physics || !controller->canMove) {
                continue;
            }

            // Apply input forces with speed multiplier
            float moveForce = controller->moveSpeed * speedMultiplier;
            
            if (Input::IsKeyDown(SDL_SCANCODE_W)) physics->velocityY -= moveForce * deltaTime;
            if (Input::IsKeyDown(SDL_SCANCODE_S)) physics->velocityY += moveForce * deltaTime;
            if (Input::IsKeyDown(SDL_SCANCODE_A)) physics->velocityX -= moveForce * deltaTime;
            if (Input::IsKeyDown(SDL_SCANCODE_D)) physics->velocityX += moveForce * deltaTime;
        }
    }
}

void WASDControllerSystem::ApplyUpgrade(UpgradeType type, float value) {
    switch (type) {
        case UPGRADE_SPEED:
            speedMultiplier *= value;  // Increase movement speed
            break;
    }
}

void WASDControllerSystem::Destroy() {
    printf("WASDControllerSystem destroyed\n");
} 