#pragma once
#include "../core/ecs/components/cloud_components.h"

struct CloudInitData {
    float x;
    float y;
    CloudType type;
    CloudSize size;
};

// Define cloud layout
const CloudInitData cloudList[] = {
    {600, 600, CLOUD_WHITE, CLOUD_SIZE_MEDIUM},
    {200, 600, CLOUD_BLACK, CLOUD_SIZE_SMALL},
    {400, 400, CLOUD_WHITE, CLOUD_SIZE_LARGE},
    {800, 300, CLOUD_BLACK, CLOUD_SIZE_SMALL},
    // Add more clouds as needed
};
// Constants for cloud generation
#define MIN_CLOUD_SPACING 100.0f         // Minimum distance between clouds
#define CLOUDS_PER_SECTION 5             // Base number of clouds per window height
#define MAX_CLOUDS 1000                  // Maximum number of clouds to generate


// Helper functions
void CreateCloudsFromData(const CloudInitData* cloudList, int count);
void GenerateRandomClouds(float playerStartY);
float GetCloudDensityMultiplier(float y); // Returns higher values as y increases