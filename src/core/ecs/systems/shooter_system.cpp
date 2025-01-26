#include "shooter_system.h"
#include <math.h>
#include "../../input.h"
#include "../component_macros.h"
#include "balloon_system.h"

void ShooterSystem::Init() {
    printf("ShooterSystem initialized\n");
}

void ShooterSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    // Get mouse position relative to screen
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Calculate screen center
    int screenCenterX = WINDOW_WIDTH / 2;
    int screenCenterY = WINDOW_HEIGHT / 2;
    
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_SHOOTER | COMPONENT_PHYSICS)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            ShooterComponent* shooter = 
                (ShooterComponent*)components->GetComponentData(entity, COMPONENT_SHOOTER);
            PhysicsComponent* physics = 
                (PhysicsComponent*)components->GetComponentData(entity, COMPONENT_PHYSICS);
            
            if (!transform || !shooter || !physics || !shooter->canShoot) {
                continue;
            }

            // Handle shooting
            if (Input::mouseButtonsPressed[0]) {  // Left click
                float currentTime = SDL_GetTicks() / 1000.0f;
                if (currentTime - shooter->lastShotTime >= shooter->fireRate) {
                    // Calculate direction from screen center to mouse
                    float dirX = mouseX - screenCenterX;
                    float dirY = mouseY - screenCenterY;
                    
                    // Normalize direction
                    float length = sqrt(dirX * dirX + dirY * dirY);
                    if (length > 0) {
                        dirX /= length;
                        dirY /= length;
                        
                        // Spawn quill from the actual entity position
                        SpawnQuill(entities, components, transform->x, transform->y, 
                                 dirX, dirY, shooter->quillSpeed, entity);

                        physics->velocityX -= dirX * shooter->recoilForce;
                        physics->velocityY -= dirY * shooter->recoilForce;

                        printf("shooter entity %d mouse (%d, %d) screen center (%d, %d) force (%f,%f) \n", 
                        entity, mouseX, mouseY, screenCenterX, screenCenterY,
                        dirX * shooter->recoilForce, dirY * shooter->recoilForce);
                        
                        // Update last shot time
                        shooter->lastShotTime = currentTime;
                    }
                }
            }
        }
    }

    // Update existing quills
    UpdateQuills(deltaTime, entities, components);
}

void ShooterSystem::SpawnQuill(EntityManager* entities, ComponentArrays* components,
                              float x, float y, float dirX, float dirY, 
                              float speed, EntityID owner) {
    EntityID quill = entities->CreateEntity();
    
    // Add components
    entities->AddComponentToEntity(quill, COMPONENT_TRANSFORM);
    entities->AddComponentToEntity(quill, COMPONENT_QUILL);
    entities->AddComponentToEntity(quill, COMPONENT_PHYSICS);
    entities->AddComponentToEntity(quill, COMPONENT_COLLIDER);
    
    // Initialize sprite with peanut texture
    Texture *quillTexture = ResourceManager::GetTexture(TEXTURE_PEANUT);
    ADD_SPRITE(quill, quillTexture);

    // Initialize transform
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(quill, COMPONENT_TRANSFORM);
    // Convert to degrees and add 90 to align sprite
    float angle = (atan2f(dirY, dirX) * 180.0f / 3.1415) + 90.0f;
    transform->Init(x, y, angle);

    // Get owner's physics component to add its velocity to the quill
    PhysicsComponent* ownerPhysics = 
        (PhysicsComponent*)components->GetComponentData(owner, COMPONENT_PHYSICS);
    
    // Initialize physics
    PhysicsComponent* physics = 
        (PhysicsComponent*)components->GetComponentData(quill, COMPONENT_PHYSICS);
    physics->Init(0.1f, 0.0f);  // Light mass, no friction
    
    // Add owner's velocity to quill's initial velocity
    if (ownerPhysics) {
        physics->velocityX = (dirX * speed) + ownerPhysics->velocityX;
        physics->velocityY = (dirY * speed) + ownerPhysics->velocityY;
    } else {
        physics->velocityX = dirX * speed;
        physics->velocityY = dirY * speed;
    }
    
    // Initialize quill properties
    QuillComponent* quillComp = 
        (QuillComponent*)components->GetComponentData(quill, COMPONENT_QUILL);
    quillComp->Init(10.0f, 2.0f, owner);
    
    // Initialize collider
    ColliderComponent* collider = 
        (ColliderComponent*)components->GetComponentData(quill, COMPONENT_COLLIDER);
    collider->Init(8, 8, false, true);  // Small trigger collider
}

void ShooterSystem::UpdateQuills(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_QUILL)) {
            QuillComponent* quill = 
                (QuillComponent*)components->GetComponentData(entity, COMPONENT_QUILL);
            
            if (!quill) continue;
            
            // Update lifetime
            quill->currentTime += deltaTime;
            if (quill->currentTime >= quill->lifetime) {
                entities->DestroyEntity(entity);
                continue;
            }

            // Check for collision with balloons
            TransformComponent* quillTransform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            ColliderComponent* quillCollider = 
                (ColliderComponent*)components->GetComponentData(entity, COMPONENT_COLLIDER);

            if (!quillTransform || !quillCollider) continue;

            // Check against all balloons
            for (EntityID balloonEntity = 1; balloonEntity < MAX_ENTITIES; balloonEntity++) {
                if (entities->HasComponent(balloonEntity, COMPONENT_BALLOON)) {
                    TransformComponent* balloonTransform = 
                        (TransformComponent*)components->GetComponentData(balloonEntity, COMPONENT_TRANSFORM);
                    ColliderComponent* balloonCollider = 
                        (ColliderComponent*)components->GetComponentData(balloonEntity, COMPONENT_COLLIDER);
                    PhysicsComponent* balloonPhysics = 
                        (PhysicsComponent*)components->GetComponentData(balloonEntity, COMPONENT_PHYSICS);

                    if (!balloonTransform || !balloonCollider || !balloonPhysics) continue;

                    float penetrationX, penetrationY;
                    if (BalloonCheckCollision(quillTransform, quillCollider,
                                            balloonTransform, balloonCollider,
                                            penetrationX, penetrationY)) 
                    {
                        // Apply a small knockback to balloon before destroying (visual effect)
                        PhysicsComponent* quillPhysics = 
                            (PhysicsComponent*)components->GetComponentData(entity, COMPONENT_PHYSICS);
                        if (quillPhysics) {
                            const float KNOCKBACK_FORCE = 500.0f;
                            balloonPhysics->velocityX += (quillPhysics->velocityX / 
                                quillPhysics->mass) * KNOCKBACK_FORCE;
                            balloonPhysics->velocityY += (quillPhysics->velocityY / 
                                quillPhysics->mass) * KNOCKBACK_FORCE;
                        }

                        // Destroy both the balloon and the quill
                        ExplodeBalloon(balloonEntity, entities);
                        entities->DestroyEntity(entity);
                        break; // Exit balloon loop since quill is destroyed
                    }
                }
            }
        }
    }
}

void ShooterSystem::Destroy() {
    printf("ShooterSystem destroyed\n");
} 