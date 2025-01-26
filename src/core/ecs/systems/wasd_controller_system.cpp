#include "wasd_controller_system.h"
#include <stdio.h>
#include <math.h>

void WASDControllerSystem::Init() {
    printf("WASDControllerSystem initialized\n");
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

            // Calculate movement based on input
            float moveX = 0.0f;
            float moveY = 0.0f;

            // Get keyboard state
            const Uint8* keyState = SDL_GetKeyboardState(NULL);

            // WASD movement
            if (keyState[SDL_SCANCODE_W]) {
                moveY -= 1.0f;
            }
            if (keyState[SDL_SCANCODE_S]) {
                moveY += 1.0f;
            }
            if (keyState[SDL_SCANCODE_A]) {
                moveX -= 1.0f;
            }
            if (keyState[SDL_SCANCODE_D]) {
                moveX += 1.0f;
            }

            // Normalize diagonal movement
            if (moveX != 0.0f && moveY != 0.0f) {
                float length = sqrt(moveX * moveX + moveY * moveY);
                moveX /= length;
                moveY /= length;
            }

            controller->moveX = moveX;
            controller->moveY = moveY;

            // Apply acceleration to physics velocity
            const float ACCELERATION = controller->moveSpeed * 1.0f;  // Adjust this multiplier as needed
            physics->velocityX += moveX * ACCELERATION * deltaTime;
            physics->velocityY += moveY * ACCELERATION * deltaTime;
        }
    }
}

void WASDControllerSystem::Destroy() {
    printf("WASDControllerSystem destroyed\n");
} 