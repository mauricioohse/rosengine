#pragma once
#include "../core/ecs/components/peanut_components.h"

// Data structure for initializing peanuts
struct PeanutInitData {
    float x;
    float y;
    PeanutType type;
};

// Function declarations
void CreatePeanutsFromData(const PeanutInitData* peanutList, int count);
void GenerateRandomPeanuts(float spawnThreshold);
void MakeAllPeanutsVisibleAgain();

// Constants for peanut generation
#define MIN_PEANUT_SPACING 300.0f      // Minimum vertical space between peanuts
#define PEANUT_SPAWN_CHANCE 0.3f       // 30% chance to spawn a peanut at each threshold
#define SUPER_PEANUT_CHANCE 0.0f       // 0% chance for a peanut to be super
#define SHIELD_PEANUT_CHANCE 0.0f      // 0% chance for a peanut to be shield 