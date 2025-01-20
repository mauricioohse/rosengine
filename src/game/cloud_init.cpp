#include "cloud_init.h"
#include "../core/engine.h"
#include "../core/resource_manager.h"
#include <stdlib.h>
#include <time.h>

void CreateCloudsFromData(const CloudInitData* cloudList, int count) {
    for (int i = 0; i < count; i++) {
        const CloudInitData& data = cloudList[i];
        
        Texture* tex = (data.type == CLOUD_WHITE) ? 
            ResourceManager::GetTexture(TEXTURE_CLOUD_1) : ResourceManager::GetTexture(TEXTURE_BLACK_CLOUD_SMALL);

        if (data.type == CLOUD_WHITE){
            switch (data.size)
            {
            case CLOUD_SIZE_SMALL:
                tex = ResourceManager::GetTexture(TEXTURE_WHITE_CLOUDE_SMALL);
                break;
            case CLOUD_SIZE_MEDIUM:
                tex = ResourceManager::GetTexture(TEXTURE_WHITE_CLOUDE_MEDIUM);
                break;
            case CLOUD_SIZE_LARGE:
                tex = ResourceManager::GetTexture(TEXTURE_WHITE_CLOUDE_LARGE);
                break;
            
            default:
                printf("invalid cloud size!\n");
                break;
            }
        }

        EntityID cloudEntity = g_Engine.entityManager.CreateEntity();
        ADD_TRANSFORM(cloudEntity, data.x, data.y, 0, 1);
        ADD_SPRITE(cloudEntity, tex);
        ADD_CLOUD(cloudEntity, data.type, data.size);
    }
}

float GetCloudDensityMultiplier(float y) {
    // Returns a value between 3.0 and 1.0 based on depth
    // Less clouds as you go deeper (higher y values)
    float depthRatio = y / GAME_HEIGHT;
    return 3.0f - (depthRatio * 2.0f); // Linear decrease down to 1x density
}

void GenerateRandomClouds(float playerStartY) {
    srand(5); // Initialize seed

    // Create an array to store cloud data
    CloudInitData clouds[MAX_CLOUDS];
    int cloudCount = 0;

    // Generate clouds for each vertical section
    for (float y = playerStartY; y < (GAME_HEIGHT- WINDOW_HEIGHT*3) && cloudCount < MAX_CLOUDS; y += WINDOW_HEIGHT) {
        // Calculate how many clouds to place in this section
        float densityMultiplier = GetCloudDensityMultiplier(y);
        int cloudsInSection = (int)(CLOUDS_PER_SECTION * densityMultiplier);

        // Generate clouds for this section
        for (int i = 0; i < cloudsInSection && cloudCount < MAX_CLOUDS; i++) {
            CloudInitData cloud;
            
            // Random position within game width and current height section
            cloud.x = (float)(rand() % GAME_WIDTH);
            cloud.y = y + (float)(rand() % WINDOW_HEIGHT);

            // Determine cloud type (20% chance for black clouds) - CHANGED - ONLY WHITE CLOUDS
            cloud.type = CLOUD_WHITE;//(rand() % 5 == 0) ? CLOUD_BLACK : CLOUD_WHITE; 
            cloud.size = (cloud.type == CLOUD_BLACK) ? CLOUD_SIZE_SMALL : (CloudSize) (rand()%3) ;

            // Check minimum spacing with previously placed clouds
            bool tooClose = false;
            for (int j = 0; j < cloudCount; j++) {
                float dx = cloud.x - clouds[j].x;
                float dy = cloud.y - clouds[j].y;
                float distSq = dx * dx + dy * dy;
                if (distSq < MIN_CLOUD_SPACING * MIN_CLOUD_SPACING) {
                    tooClose = true;
                    break;
                }
            }

            // If cloud placement is valid, add it to the array
            if (!tooClose) {
                clouds[cloudCount++] = cloud;
            }
        }
    }

    // Create all the clouds
    CreateCloudsFromData(clouds, cloudCount);
} 