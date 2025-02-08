#include "shooter_system.h"
#include <math.h>
#include "../../input.h"
#include "../component_macros.h"
#include "balloon_system.h"
#include "../../../game/game.h"

void ShooterSystem::Init() {
    printf("ShooterSystem initialized\n");
    damageMultiplier = 1.0f;
    extraProjectiles = 0;
    fireRateMultiplier = 1.0f;
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
            SpriteComponent* sprite =
                (SpriteComponent*)components->GetComponentData(entity, COMPONENT_SPRITE);
            WASDControllerComponent* controller =
                (WASDControllerComponent*)components->GetComponentData(entity, COMPONENT_WASD_CONTROLLER);
            
            if (!transform || !shooter || !physics || !sprite || !controller || !shooter->canShoot) {
                continue;
            }

            // Handle shooting with fire rate multiplier
            if (Input::mouseButtonsPressed[0]) {  // Left click
                float currentTime = SDL_GetTicks() / 1000.0f;
                if (currentTime - shooter->lastShotTime >= shooter->fireRate * fireRateMultiplier) {
                    // Calculate direction from screen center to mouse
                    float dirX = mouseX - screenCenterX;
                    float dirY = mouseY - screenCenterY;
                    
                    // Normalize direction
                    float length = sqrt(dirX * dirX + dirY * dirY);
                    if (length > 0) {
                        dirX /= length;
                        dirY /= length;

                        // Set shooting animation state
                        shooter->isShooting = true;
                        shooter->shootingAnimTimer = 0.2f; // 200ms animation duration
                        

                        // Spawn main quill
                        SpawnQuill(entities, components, transform->x, transform->y, 
                                 dirX, dirY, shooter->quillSpeed, entity);

                        // Apply recoil for main shot
                        physics->velocityX -= dirX * shooter->recoilForce;
                        physics->velocityY -= dirY * shooter->recoilForce;

                        // Apply additional 80% recoil for each extra projectile
                        for(int i = 0; i < extraProjectiles; i++) {
                            physics->velocityX -= dirX * shooter->recoilForce*.8;
                            physics->velocityY -= dirY * shooter->recoilForce*.8;
                        }
                        
                        // Update last shot time
                        shooter->lastShotTime = currentTime;
                    }
                }
            } 

            // Update shooting animation timer
            if (shooter->shootingAnimTimer > 0) {
                shooter->shootingAnimTimer -= deltaTime;
                if (shooter->shootingAnimTimer <= 0) {
                    shooter->isShooting = false;
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
    // Play quill shooting sound with random pitch
    Sound* quillSound = ResourceManager::GetSound(SOUND_QUILL);
    if (quillSound) {
        int channel = Mix_PlayChannel(-1, quillSound->sdlChunk, 0);
        if (channel != -1) {
            // Random pitch between 0.8 and 1.2 (80% to 120% of normal speed)
            int randomPitch = 80 + (rand() % 41);  // Random value between 80 and 120
            Mix_SetDistance(channel, randomPitch);
        }
    }

    EntityID quill = entities->CreateEntity();
    
    // Add components
    entities->AddComponentToEntity(quill, COMPONENT_TRANSFORM);
    entities->AddComponentToEntity(quill, COMPONENT_QUILL);
    entities->AddComponentToEntity(quill, COMPONENT_PHYSICS);
    entities->AddComponentToEntity(quill, COMPONENT_COLLIDER);
    
    // Initialize sprite with peanut texture
    Texture *quillTexture = ResourceManager::GetTexture(TEXTURE_PEANUT);
    ADD_SPRITE(quill, quillTexture);

    SpriteComponent* sp = 
        (SpriteComponent*)components->GetComponentData(quill, COMPONENT_SPRITE);

    sp->width = 16;
    sp->height = 32;

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
    if (0) { //ownerPhysics) {
        physics->velocityX = (dirX * speed) + ownerPhysics->velocityX;
        physics->velocityY = (dirY * speed) + ownerPhysics->velocityY;
    } else {
        physics->velocityX = dirX * speed;
        physics->velocityY = dirY * speed;
    }
    
    // Initialize quill properties with damage multiplier
    QuillComponent* quillComp = 
        (QuillComponent*)components->GetComponentData(quill, COMPONENT_QUILL);
    quillComp->Init(10.0f * damageMultiplier, 2.0f, owner);  // Apply damage multiplier
    
    // Initialize collider
    ColliderComponent* collider = 
        (ColliderComponent*)components->GetComponentData(quill, COMPONENT_COLLIDER);
    collider->Init(14, 20, false, true);  // Small trigger collider

    // Spawn extra projectiles if we have any
    for(int i = 0; i < extraProjectiles; i++) {
        printf("Spawning extra projectile %d/%d\n", i + 1, extraProjectiles);
        // Calculate spread angle (15 degrees per extra projectile)
        float spreadAngle = (i + 1) * 15.0f * (3.14159f / 180.0f);
        float spreadDirX = dirX * cosf(spreadAngle) - dirY * sinf(spreadAngle);
        float spreadDirY = dirX * sinf(spreadAngle) + dirY * cosf(spreadAngle);
        
        // Spawn additional quill with spread
        EntityID extraQuill = entities->CreateEntity();
        
        // Add components
        entities->AddComponentToEntity(extraQuill, COMPONENT_TRANSFORM);
        entities->AddComponentToEntity(extraQuill, COMPONENT_QUILL);
        entities->AddComponentToEntity(extraQuill, COMPONENT_PHYSICS);
        entities->AddComponentToEntity(extraQuill, COMPONENT_COLLIDER);
        
        // Initialize sprite with peanut texture
        Texture *quillTexture = ResourceManager::GetTexture(TEXTURE_PEANUT);
        ADD_SPRITE(extraQuill, quillTexture);

        // Initialize transform
        TransformComponent* transform = 
            (TransformComponent*)components->GetComponentData(extraQuill, COMPONENT_TRANSFORM);
        // Convert to degrees and add 90 to align sprite
        float angle = (atan2f(spreadDirY, spreadDirX) * 180.0f / 3.1415) + 90.0f;
        transform->Init(x, y, angle);

        // Initialize physics
        PhysicsComponent* physics = 
            (PhysicsComponent*)components->GetComponentData(extraQuill, COMPONENT_PHYSICS);
        physics->Init(0.1f, 0.0f);  // Light mass, no friction
        
        // Add owner's velocity to quill's initial velocity
        if (0) { //ownerPhysics) {
            physics->velocityX = (spreadDirX * speed) + ownerPhysics->velocityX;
            physics->velocityY = (spreadDirY * speed) + ownerPhysics->velocityY;
        } else {
            physics->velocityX = spreadDirX * speed;
            physics->velocityY = spreadDirY * speed;
        }
        
        // Initialize quill properties with damage multiplier
        QuillComponent* quillComp = 
            (QuillComponent*)components->GetComponentData(extraQuill, COMPONENT_QUILL);
        quillComp->Init(10.0f * damageMultiplier, 2.0f, owner);
        
        // Initialize collider
        ColliderComponent* collider = 
            (ColliderComponent*)components->GetComponentData(extraQuill, COMPONENT_COLLIDER);
        collider->Init(8, 8, false, true);  // Small trigger collider
    }
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
                        // Get balloon type before destroying it
                        BalloonComponent *balloon =
                            (BalloonComponent *)g_Engine.componentArrays.GetComponentData(balloonEntity, COMPONENT_BALLOON);
                        if (balloon)
                        {
                            // Add score based on balloon type
                            g_Game.AddScore(balloon->type);
                        }

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
void ShooterSystem::ApplyUpgrade(UpgradeType type, float value) {
    printf("ShooterSystem applying upgrade: ");
    switch (type) {
        case UPGRADE_FIRE_RATE:
            printf("Fire Rate %.2fx\n", value);
            fireRateMultiplier *= value;  // Reduce fire rate (value < 1.0)
            break;
            
        case UPGRADE_MULTI_SHOT:
            printf("Multi Shot +%d\n", (int)value);
            extraProjectiles += (int)value;  // Add extra projectiles
            break;
                        
        case UPGRADE_RECOIL_RES:
            printf("Recoil Resistance (handled by IcePhysicsSystem)\n");
            // This will be handled by IcePhysicsSystem
            break;
    }
} 