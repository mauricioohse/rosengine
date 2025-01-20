#include "peanut_init.h"
#include "../core/engine.h"
#include "../core/resource_manager.h"
#include <stdlib.h>
#include <time.h>
#include "game.h"

void CreatePeanutsFromData(const PeanutInitData* peanutList, int count) {
    for (int i = 0; i < count; i++) {
        EntityID peanut = g_Engine.entityManager.CreateEntity();
        
        // Select texture based on peanut type
        Texture* texture;
        switch (peanutList[i].type) {
            case PEANUT_TYPE_SHIELD:
                texture = ResourceManager::GetTexture(TEXTURE_SHIELD_PEANUT);
                break;
            case PEANUT_TYPE_SUPER:
                texture = ResourceManager::GetTexture(TEXTURE_SUPER_PEANUT);
                break;
            default:
                texture = ResourceManager::GetTexture(TEXTURE_PEANUT);
                break;
        }
        
        ADD_TRANSFORM(peanut, peanutList[i].x, peanutList[i].y, 0.0f, 1.0f);
        ADD_SPRITE(peanut, texture);
        ADD_PEANUT(peanut, peanutList[i].type);
    }
}

void GenerateRandomPeanuts(float spawnThreshold) {
    g_Game.numPeanutTargets = 0;
    float currentHeight = spawnThreshold;
    
    while (currentHeight < GAME_HEIGHT - spawnThreshold) {  // Stop before bottom
        // Decide if we spawn a peanut at this height
        if ((float)rand() / RAND_MAX < PEANUT_SPAWN_CHANCE) {
            EntityID peanut = g_Engine.entityManager.CreateEntity();
            
            // Random x position within reasonable bounds
            float x = 800.0f + (float)(rand() % 800);  // Between 800 and 1600
            
            if (g_Game.numPeanutTargets < Game::MAX_PEANUT_TARGETS) {
                g_Game.peanutTargets[g_Game.numPeanutTargets].x = x;
                g_Game.peanutTargets[g_Game.numPeanutTargets].y = currentHeight;
                g_Game.peanutTargets[g_Game.numPeanutTargets].isCollected = false;
                g_Game.numPeanutTargets++;
            }

            // Determine peanut type
            PeanutType type;
            float typeRoll = (float)rand() / RAND_MAX;
            
            if (typeRoll < SUPER_PEANUT_CHANCE) {
                type = PEANUT_TYPE_SUPER;
            } else if (typeRoll < SUPER_PEANUT_CHANCE + SHIELD_PEANUT_CHANCE) {
                type = PEANUT_TYPE_SHIELD;
            } else {
                type = PEANUT_TYPE_REGULAR;
            }
            
            // Select texture based on type
            Texture* texture;
            switch (type) {
                case PEANUT_TYPE_SHIELD:
                    texture = ResourceManager::GetTexture(TEXTURE_SHIELD_PEANUT);
                    break;
                case PEANUT_TYPE_SUPER:
                    texture = ResourceManager::GetTexture(TEXTURE_SUPER_PEANUT);
                    break;
                default:
                    texture = ResourceManager::GetTexture(TEXTURE_PEANUT);
                    break;
            }
            
            ADD_TRANSFORM(peanut, x, currentHeight, 0.0f, 1.0f);
            ADD_SPRITE(peanut, texture);
            ADD_PEANUT(peanut, type);
            
            // printf("Generated %s peanut at (%.1f, %.1f)\n", 
            //     type == PEANUT_TYPE_SUPER ? "super" : 
            //     type == PEANUT_TYPE_SHIELD ? "shield" : "regular",
            //     x, currentHeight);
        }
        
        currentHeight += MIN_PEANUT_SPACING;
    }
} 

void MakeAllPeanutsVisibleAgain() {
    // Iterate through all entities
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        // Check if entity has peanut and sprite components
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_PEANUT | COMPONENT_SPRITE)) {
            // Get components
            PeanutComponent* peanut = &g_Engine.componentArrays.peanuts[entity];
            SpriteComponent* sprite = &g_Engine.componentArrays.sprites[entity];
            
            // Reset peanut state
            peanut->wasCollected = false;
            sprite->isVisible = true;
            
            printf("Reset peanut entity %d\n", entity);
        }
    }
}
