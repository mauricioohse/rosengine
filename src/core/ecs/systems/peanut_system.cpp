#include "peanut_system.h"
#include "../components.h"
#include "../../engine.h"
#include "../../../game/game.h"

void PeanutSystem::Init() {
    printf("PeanutSystem initialized\n");
}

void PeanutSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    // Get squirrel components first
    TransformComponent* squirrelTransform = 
        (TransformComponent*)g_Engine.componentArrays.GetComponentData(g_Game.squirrelEntity, COMPONENT_TRANSFORM);
    SquirrelComponent* squirrel = 
        (SquirrelComponent*)g_Engine.componentArrays.GetComponentData(g_Game.squirrelEntity, COMPONENT_SQUIRREL);
    CameraComponent* camera = 
        (CameraComponent*)g_Engine.componentArrays.GetComponentData(g_Game.cameraEntity, COMPONENT_CAMERA);
    


    if (!squirrelTransform || !squirrel) return;

    // Update powerup timers
    if (squirrel->hasShield) {
        squirrel->shieldTimer -= deltaTime;
        if (squirrel->shieldTimer <= 0) {
            squirrel->hasShield = false;
        }
    }

    if (squirrel->hasSuperMode) {
        squirrel->superTimer -= deltaTime;
        if (squirrel->superTimer <= 0) {
            squirrel->hasSuperMode = false;
            squirrel->speedBoost = 0.0f;  // Remove speed boost when super mode ends
        }
    }

    // Check for collisions with peanuts
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_PEANUT | COMPONENT_TRANSFORM | COMPONENT_SPRITE)) {
            PeanutComponent* peanut = &components->peanuts[entity];
            if (peanut->wasCollected) continue;  // Skip already collected peanuts

            TransformComponent* peanutTransform = &components->transforms[entity];
            SpriteComponent* peanutSprite = &components->sprites[entity];

            // Simple AABB collision check
            bool collision = 
                squirrelTransform->x < peanutTransform->x + peanutSprite->width &&
                squirrelTransform->x + 32 > peanutTransform->x &&  // assuming squirrel width
                squirrelTransform->y < peanutTransform->y + peanutSprite->height &&
                squirrelTransform->y + 32 > peanutTransform->y;   // assuming squirrel height

            if (collision) {
                // Apply powerup effect based on type
                switch (peanut->type) {
                    case PEANUT_TYPE_REGULAR:
                        squirrel->speedBoost += PEANUT_SPEED_BOOST;
                        squirrel->velocityY += PEANUT_SPEED_BOOST*6;
                        squirrel->gravity += SQUIRREL_GRAVITY/5;
                        camera->cameraKick = -150.0f;
                        break;

                    case PEANUT_TYPE_SHIELD:
                        squirrel->hasShield = true;
                        squirrel->shieldTimer = PEANUT_SHIELD_DURATION;
                        break;

                    case PEANUT_TYPE_SUPER:
                        squirrel->hasSuperMode = true;
                        squirrel->superTimer = PEANUT_SUPER_DURATION;
                        squirrel->speedBoost += PEANUT_SPEED_BOOST * 2;  // Double speed boost for super mode
                        squirrel->hasShield = true;  // Super mode includes shield
                        squirrel->shieldTimer = PEANUT_SUPER_DURATION;
                        break;
                }

                // Mark peanut as collected and hide its sprite
                peanut->wasCollected = true;
                peanutSprite->isVisible = false;  // Hide using sprite component
                
                // Play chomp sound
                Sound* chompSound = ResourceManager::GetSound(SOUND_CHOMP);
                if (chompSound) {
                    chompSound->sdlChunk->volume = 64;  // Half volume (0-128)
                    Mix_PlayChannel(-1, chompSound->sdlChunk, 0);
                }
                
                printf("peanut type %d collected\n", peanut->type);

                // Update target array
                for (int i = 0; i < g_Game.numPeanutTargets; i++) {
                    if (abs(g_Game.peanutTargets[i].x - peanutTransform->x) < 1.0f &&
                        abs(g_Game.peanutTargets[i].y - peanutTransform->y) < 1.0f) {
                        g_Game.peanutTargets[i].isCollected = true;
                        break;
                    }
                }
            }
        }
    }
}

void PeanutSystem::Destroy() {
    printf("PeanutSystem destroyed\n");
} 