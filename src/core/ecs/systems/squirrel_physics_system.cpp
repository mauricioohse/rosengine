#include "squirrel_physics_system.h"
#include <math.h>
#include <stdio.h>
#include "../../engine_constants.h"
#include <algorithm>


void SquirrelPhysicsSystem::Init() {
    // Nothing to initialize
}

void SquirrelPhysicsSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_SQUIRREL | COMPONENT_SPRITE)) {
            SquirrelComponent* squirrel = 
                (SquirrelComponent*)components->GetComponentData(entity, COMPONENT_SQUIRREL);
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            SpriteComponent* sprite =
                (SpriteComponent*)components->GetComponentData(entity, COMPONENT_SPRITE);

            // Handle all state-related logic in one place
            HandleSquirrelState(squirrel, sprite, deltaTime);
            HandleMovementInput(squirrel, deltaTime);

            // Apply physics
            ApplyGravity(squirrel, deltaTime);
            LimitVerticalSpeed(squirrel);

            // Update position
            if(squirrel->state != SQUIRREL_STATE_DROPPING) {
                transform->x += squirrel->velocityX * deltaTime;
                transform->y += squirrel->velocityY * deltaTime;
            }

            // Keep rotation at zero (except for wiggle state)
            if (squirrel->state == SQUIRREL_STATE_WIGGLING) {
                float wiggleAngle = 30.0f * sinf(squirrel->wiggleTimer * 15.0f);
                transform->rotation = wiggleAngle;
            } else {
                transform->rotation = 0;
            }
        }
    }
}

void SquirrelPhysicsSystem::HandleMovementInput(SquirrelComponent* squirrel, float deltaTime) {
    const float BASE_HORIZONTAL_SPEED = 300.0f;  // Base speed when arms are closed
    const float MAX_HORIZONTAL_SPEED = 800.0f;  // Maximum possible horizontal speed
    float currentHorizontalSpeed;
    
    // Calculate horizontal speed based on maxSpeed
    float speedRatio = 1;  // How much faster than base are we?
    currentHorizontalSpeed = std::min(BASE_HORIZONTAL_SPEED * speedRatio, MAX_HORIZONTAL_SPEED);
    
    // State-specific modifications
    switch (squirrel->state) {
        case SQUIRREL_STATE_OPEN_ARMS:
            currentHorizontalSpeed *= 3.0f;  // Double speed when arms are open
            break;
        case SQUIRREL_STATE_WIGGLING:
            break;
        default:
            break;
    }
    
    // Reset horizontal velocity each frame (for crisp movement)
    squirrel->velocityX = 0;

    // // Don't allow movement during wiggle state
    // if (squirrel->state == SQUIRREL_STATE_WIGGLING) {
    //     return;
    // }

    // Move left
    if (Input::IsKeyDown(SDL_SCANCODE_A) || Input::IsKeyDown(SDL_SCANCODE_LEFT)) {
        squirrel->velocityX = -currentHorizontalSpeed;
    }
    
    // Move right
    if (Input::IsKeyDown(SDL_SCANCODE_D) || Input::IsKeyDown(SDL_SCANCODE_RIGHT)) {
        squirrel->velocityX = currentHorizontalSpeed;
    }
}

void SquirrelPhysicsSystem::ApplyGravity(SquirrelComponent* squirrel, float deltaTime) {
    squirrel->velocityY += squirrel->currentGravity * deltaTime;
}

void SquirrelPhysicsSystem::LimitVerticalSpeed(SquirrelComponent* squirrel) {
    
    if (fabsf(squirrel->velocityY) > squirrel->maxSpeed) {
        float targetSpeed = squirrel->maxSpeed * (squirrel->velocityY > 0 ? 1.0f : -1.0f);
        // Lerp between current velocity and target speed
        squirrel->velocityY = squirrel->velocityY + (targetSpeed - squirrel->velocityY) * SMOOTHING_FACTOR;
    }
}

void SquirrelPhysicsSystem::Destroy() {}

void SquirrelPhysicsSystem::HandleSquirrelState(SquirrelComponent* squirrel, 
                                               SpriteComponent* sprite, 
                                               float deltaTime) {
    // Handle state timers first
    if (squirrel->state == SQUIRREL_STATE_WIGGLING) {
        squirrel->wiggleTimer += deltaTime;
        if (squirrel->wiggleTimer >= SQUIRREL_WIGGLE_DURATION) {
            // Exit wiggle state
            squirrel->state = SQUIRREL_STATE_OPEN_ARMS;
            squirrel->wiggleTimer = 0.0f;
            squirrel->graceTimer = SQUIRREL_GRACE_PERIOD;
        }
    } else if (squirrel->graceTimer > 0.0f) {
        squirrel->graceTimer -= deltaTime;
    }

    // Handle state transitions
    switch (squirrel->state) {

        case SQUIRREL_STATE_DROPPING:
            if ((Input::IsKeyDown(SDL_SCANCODE_SPACE) || Input::IsKeyDown(SDL_SCANCODE_W) || Input::IsKeyDown(SDL_SCANCODE_UP))   ) squirrel->state = SQUIRREL_STATE_OPEN_ARMS;
            // Don't apply gravity or controls while dropping
            
            sprite->ChangeTexture(ResourceManager::GetTexture(TEXTURE_SQUIRREL_SITTING));
            squirrel->velocityX = 0;
            squirrel->velocityY = 0;
            squirrel->currentGravity =squirrel->gravity; 
            break;

        case SQUIRREL_STATE_OPEN_ARMS:
            squirrel->maxSpeed = SQUIRREL_OPEN_ARMS_MAX_SPEED + squirrel->speedBoost/2;
            sprite->ChangeTexture(ResourceManager::GetTexture(TEXTURE_SQUIRREL_OPEN));
            if (!(Input::IsKeyDown(SDL_SCANCODE_SPACE) || Input::IsKeyDown(SDL_SCANCODE_W) || Input::IsKeyDown(SDL_SCANCODE_UP)) && squirrel->state == SQUIRREL_STATE_OPEN_ARMS) {
                squirrel->state = SQUIRREL_STATE_CLOSED_ARMS;
            }
            squirrel->currentGravity =squirrel->gravity; 
            break;

        case SQUIRREL_STATE_CLOSED_ARMS:
            squirrel->maxSpeed = SQUIRREL_CLOSED_ARMS_MAX_SPEED + squirrel->speedBoost;
            sprite->ChangeTexture(ResourceManager::GetTexture(TEXTURE_SQUIRREL_CLOSED));
            if ((Input::IsKeyDown(SDL_SCANCODE_SPACE) || Input::IsKeyDown(SDL_SCANCODE_W) || Input::IsKeyDown(SDL_SCANCODE_UP)) && squirrel->state == SQUIRREL_STATE_CLOSED_ARMS) {
                squirrel->state = SQUIRREL_STATE_OPEN_ARMS;
            }
            squirrel->currentGravity = 2 * squirrel->gravity; 
            break;

        case SQUIRREL_STATE_WIGGLING:
            squirrel->maxSpeed = SQUIRREL_WIGGLE_MAX_SPEED + squirrel->speedBoost/5;
            sprite->ChangeTexture(ResourceManager::GetTexture(TEXTURE_SQUIRREL_OPEN));
            squirrel->currentGravity =squirrel->gravity; 

                // Handle wiggle state timing and transitions
            if (squirrel->state == SQUIRREL_STATE_WIGGLING) {

                squirrel->wiggleTimer += deltaTime;

                if (squirrel->wiggleTimer >= SQUIRREL_WIGGLE_DURATION) {
                    // Exit wiggle state
                    squirrel->state = SQUIRREL_STATE_OPEN_ARMS;
                    squirrel->wiggleTimer = 0.0f;
                }
            }

            break;
    }
}


