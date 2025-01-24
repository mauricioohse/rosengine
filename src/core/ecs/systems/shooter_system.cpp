#include "shooter_system.h"
#include <math.h>
#include "../../input.h"

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
    entities->AddComponentToEntity(quill, COMPONENT_SPRITE);  // Assuming you have quill sprite
    entities->AddComponentToEntity(quill, COMPONENT_COLLIDER);
    
    // Initialize transform
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(quill, COMPONENT_TRANSFORM);
    transform->Init(x, y, atan2f(dirY, dirX));
    
    // Initialize physics
    PhysicsComponent* physics = 
        (PhysicsComponent*)components->GetComponentData(quill, COMPONENT_PHYSICS);
    physics->Init(0.1f, 0.0f);  // Light mass, no friction
    physics->velocityX = dirX * speed;
    physics->velocityY = dirY * speed;
    
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
        }
    }
}

void ShooterSystem::Destroy() {
    printf("ShooterSystem destroyed\n");
} 