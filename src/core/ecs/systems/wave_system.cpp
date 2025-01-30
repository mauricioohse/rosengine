#include "wave_system.h"
#include "../../../game/game.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>


void WaveSystem::Init() {
    printf("WaveSystem initialized\n");
    
    // Initialize base waves
    baseWaves = {{
        // Wave 1
        {{{BALLOON_RED, 2}}},
        
        // Wave 2
        {{{BALLOON_RED, 2}, 
          {BALLOON_GREEN, 1}}},
        
        // Wave 3
        {{{BALLOON_BLUE, 1}, 
          {BALLOON_GREEN, 2}}},
        
        // Wave 4
        {{{BALLOON_RED, 3}, 
          {BALLOON_BLUE, 1}}},
        
        // Wave 5
        {{{BALLOON_RED, 1}, 
          {BALLOON_GREEN, 1}, 
          {BALLOON_BLUE, 1}}}
    }};
}

void WaveSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    // Only update if not waiting for upgrade choice
    if (awaitingUpgradeChoice || g_Game.gameState != GAME_STATE_PLAYING) {
        return;
    }
    static bool once = false;

    if (once)
    {
        SpawnWave(entities, components);
        once = false;
    }

    // Check if wave is complete
    if (NoBalloonsRemaining(entities)) {
        currentWave++;
        GenerateUpgradeChoices();
        awaitingUpgradeChoice = true;
        once = true;

    }
}

bool WaveSystem::NoBalloonsRemaining(EntityManager* entities) {
    // Check if any balloons exist
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_BALLOON)) {
            return false;
        }
    }
    return true;
}

int WaveSystem::GetScaledCount(int baseCount, BalloonType type) {
    // Base cycle scaling (50% more per cycle)
    float multiplier = 1.0f + (currentCycle * 0.5f);
    
    // Apply balloon-specific multipliers
    switch (type) {
        case BALLOON_RED:
            multiplier *= redBalloonMultiplier;
            break;
        case BALLOON_BLUE:
            multiplier *= blueBalloonMultiplier;
            break;
        case BALLOON_GREEN:
            multiplier *= greenBalloonMultiplier;
            break;
    }
    
    return (int)ceil(baseCount * multiplier);
}

void WaveSystem::SpawnWave(EntityManager* entities, ComponentArrays* components) {
    WaveData& baseWave = baseWaves[currentWave%5];
    
    for (const auto& spawn : baseWave.spawns) {
        int scaledCount = GetScaledCount(spawn.count, spawn.type);  // Pass balloon type
        for (int i = 0; i < scaledCount; i++) {
            SpawnBalloon(spawn.type, entities, components);
        }
    }
}

void WaveSystem::SpawnBalloon(BalloonType type, EntityManager* entities, ComponentArrays* components) {
    EntityID balloon = entities->CreateEntity();
    
    // Calculate random position around arena (outside player view)
    float angle = (float)(rand()) / RAND_MAX * 2 * 3.14159f;
    float distance = 1000.0f;  // Spawn outside arena
    float spawnX = cos(angle) * distance;
    float spawnY = sin(angle) * distance;
    
    // Add components based on balloon type
    ADD_TRANSFORM(balloon, spawnX, spawnY, 0, 1);
    ADD_PHYSICS(balloon, 50, 15, 1);
    ADD_COLLIDER(balloon, 32, 32, 0, 1);
    ADD_BALLOON(balloon, type, g_Game.squirrelEntity, 50, 100);
    
    // Set sprite based on type
    Texture* texture;
    switch (type) {
        case BALLOON_RED:
            texture = ResourceManager::GetTexture(TEXTURE_BALLOON_RED);
            break;
        case BALLOON_GREEN:
            texture = ResourceManager::GetTexture(TEXTURE_BALLOON_GREEN);
            break;
        case BALLOON_BLUE:
            texture = ResourceManager::GetTexture(TEXTURE_BALLOON_BLUE);
            break;
    }
    ADD_SPRITE(balloon, texture);
}

void WaveSystem::Destroy() {
    printf("WaveSystem destroyed\n");
}

bool WaveSystem::IsUpgradeValid(UpgradeType type, const std::vector<UpgradeType>& usedTypes) {
    return std::find(usedTypes.begin(), usedTypes.end(), type) == usedTypes.end();
}

bool WaveSystem::IsDifficultyValid(DifficultyType type, const std::vector<DifficultyType>& usedTypes) {
    return std::find(usedTypes.begin(), usedTypes.end(), type) == usedTypes.end();
}

UpgradeType WaveSystem::GetRandomUnusedUpgrade(const std::vector<UpgradeType>& usedTypes) {
    std::vector<UpgradeType> availableTypes;
    
    // Collect all unused upgrade types
    for (int i = 1; i < UPGRADE_MAX; i++) {
        UpgradeType type = static_cast<UpgradeType>(i);
        if (IsUpgradeValid(type, usedTypes)) {
            availableTypes.push_back(type);
        }
    }
    
    // Return random unused type
    if (!availableTypes.empty()) {
        return availableTypes[rand() % availableTypes.size()];
    }
    return UPGRADE_NONE;
}

DifficultyType WaveSystem::GetRandomUnusedDifficulty(const std::vector<DifficultyType>& usedTypes) {
    std::vector<DifficultyType> availableTypes;
    
    // Collect all unused difficulty types
    for (int i = 1; i < DIFFICULTY_MAX; i++) {
        DifficultyType type = static_cast<DifficultyType>(i);
        if (IsDifficultyValid(type, usedTypes)) {
            availableTypes.push_back(type);
        }
    }
    
    // Return random unused type
    if (!availableTypes.empty()) {
        return availableTypes[rand() % availableTypes.size()];
    }
    return DIFFICULTY_NONE;
}

void WaveSystem::GenerateUpgradeChoices() {
    std::vector<UpgradeType> usedUpgrades;
    std::vector<DifficultyType> usedDifficulties;
    
    // Generate 3 unique upgrade-difficulty pairs
    for (int i = 0; i < 3; i++) {
        // Get random unused upgrade
        UpgradeType upgradeType = GetRandomUnusedUpgrade(usedUpgrades);
        usedUpgrades.push_back(upgradeType);
        currentUpgradeChoices[i].upgrade = GetUpgradeData(upgradeType);
        
        // Get random unused difficulty
        DifficultyType difficultyType = GetRandomUnusedDifficulty(usedDifficulties);
        usedDifficulties.push_back(difficultyType);
        currentUpgradeChoices[i].difficulty = GenerateRandomDifficulty();
        currentUpgradeChoices[i].difficulty.type = difficultyType;
    }
}

Difficulty WaveSystem::GenerateRandomDifficulty() {
    Difficulty diff;
    diff.type = (DifficultyType)(1 + (rand() % (DIFFICULTY_MAX - 1)));
    
    // Generate appropriate value based on type
    switch (diff.type) {
        case DIFFICULTY_RED_QUANTITY:
        case DIFFICULTY_BLUE_QUANTITY:
        case DIFFICULTY_GREEN_QUANTITY:
            diff.value = 1.0f + (rand() % 2);  // Add 1-2 more balloons
            break;
            
        case DIFFICULTY_BALLOON_SPEED:
        case DIFFICULTY_BLUE_PROJECTILE_SPEED:
        case DIFFICULTY_GREEN_PROJECTILE_SPEED:
            diff.value = 1.15f + (rand() % 20) / 100.0f;  // 15-35% faster
            break;
            
        case DIFFICULTY_DAMAGE_TAKEN:
            diff.value = 1.2f + (rand() % 30) / 100.0f;  // 20-50% more damage
            break;
            
        default:
            diff.value = 1.0f;
    }
    
    diff.name = GetDifficultyName(diff.type);
    diff.description = GetDifficultyDescription(diff.type, diff.value);
    return diff;
}

void WaveSystem::SelectUpgrade(int choice) {
    if (choice >= 0 && choice < 3) {
        ApplyUpgrade(currentUpgradeChoices[choice]);
        awaitingUpgradeChoice = false;
    }
}

void WaveSystem::ApplyUpgrade(const UpgradeChoice& choice) {
    // Apply the upgrade based on type
    switch (choice.upgrade.type) {
        case UPGRADE_FIRE_RATE:
        case UPGRADE_MULTI_SHOT:

            // Shooter upgrades
            g_Game.shooterSystem.ApplyUpgrade(
                choice.upgrade.type, choice.upgrade.value);
            break;
            
        case UPGRADE_SPEED:
            // Movement speed upgrade
            g_Game.wasdSystem.ApplyUpgrade(
                choice.upgrade.type, choice.upgrade.value);
            break;
            
        case UPGRADE_GRIP:
        case UPGRADE_RECOIL_RES:
            // Physics-related upgrades
             g_Game.icePhysicsSystem.ApplyUpgrade(
                choice.upgrade.type, choice.upgrade.value);
            break;
    }
    
    // Track active upgrades
    activeUpgrades.push_back(choice.upgrade.type);
    
    // Apply the difficulty increase
    activeDifficulties.push_back(choice.difficulty.type);
    
    // Apply difficulty changes in wave system
    switch (choice.difficulty.type) {
        case DIFFICULTY_RED_QUANTITY:
            redBalloonMultiplier *= choice.difficulty.value;
            break;
        case DIFFICULTY_BLUE_QUANTITY:
            blueBalloonMultiplier *= choice.difficulty.value;
            break;
        case DIFFICULTY_GREEN_QUANTITY:
            greenBalloonMultiplier *= choice.difficulty.value;
            break;
        case DIFFICULTY_BALLOON_SPEED:
            balloonSpeedMultiplier *= choice.difficulty.value;
            break;
        case DIFFICULTY_BLUE_PROJECTILE_SPEED:
            blueProjectileSpeedMultiplier *= choice.difficulty.value;
            break;
        case DIFFICULTY_GREEN_PROJECTILE_SPEED:
            greenProjectileSpeedMultiplier *= choice.difficulty.value;
            break;
        case DIFFICULTY_DAMAGE_TAKEN:
            damageMultiplier *= choice.difficulty.value;
            break;
    }
}

Upgrade WaveSystem::GetUpgradeData(UpgradeType type) {
    Upgrade upgrade;
    upgrade.type = type;
    
    switch (type) {
        case UPGRADE_FIRE_RATE:
            upgrade.name = "Rapid Fire";
            upgrade.value = 0.75f;  // 10% faster firing
            upgrade.description = "Shoot 25% faster";
            break;
            
        case UPGRADE_MULTI_SHOT:
            upgrade.name = "Multi Shot";
            upgrade.value = 1.0f;   // Add 1 additional projectile
            upgrade.description = "Shoot an additional quill";
            break;
            
        case UPGRADE_SPEED:
            upgrade.name = "Swift Feet";
            upgrade.value = 1.3f;   // 10% faster movement
            upgrade.description = "Move 30% faster";
            break;
            
        case UPGRADE_GRIP:
            upgrade.name = "Ice Grip";
            upgrade.value = 1.30f;   // 30% better grip
            upgrade.description = "30% better control on ice";
            break;
            
        case UPGRADE_RECOIL_RES:
            upgrade.name = "Steady Aim";
            upgrade.value = 0.8f;   // 20% less recoil
            upgrade.description = "Reduce quill recoil by 20%";
            break;
            
        default:
            upgrade.name = "Unknown Upgrade";
            upgrade.value = 1.0f;
            upgrade.description = "Error: Invalid upgrade type";
            break;
    }
    
    return upgrade;
}

const char* WaveSystem::GetDifficultyName(DifficultyType type) {
    switch (type) {
        case DIFFICULTY_RED_QUANTITY:
            return "Red Balloon Swarm";
            
        case DIFFICULTY_BLUE_QUANTITY:
            return "Blue Balloon Swarm";
            
        case DIFFICULTY_GREEN_QUANTITY:
            return "Green Balloon Swarm";
            
        case DIFFICULTY_BALLOON_SPEED:
            return "Swift Balloons";
            
        case DIFFICULTY_BLUE_PROJECTILE_SPEED:
            return "Rapid Blue Fire";
            
        case DIFFICULTY_GREEN_PROJECTILE_SPEED:
            return "Rapid Green Fire";
            
        case DIFFICULTY_DAMAGE_TAKEN:
            return "Fragile Form";
            
        default:
            return "Unknown Difficulty";
    }
}

const char* WaveSystem::GetDifficultyDescription(DifficultyType type, float value) {
    static char description[128];  // Static buffer for the description
    
    switch (type) {
        case DIFFICULTY_RED_QUANTITY:
            snprintf(description, sizeof(description), 
                    "%.0f more red balloon(s) per wave", value);
            break;
            
        case DIFFICULTY_BLUE_QUANTITY:
            snprintf(description, sizeof(description), 
                    "%.0f more blue balloon(s) per wave", value);
            break;
            
        case DIFFICULTY_GREEN_QUANTITY:
            snprintf(description, sizeof(description), 
                    "%.0f more green balloon(s) per wave", value);
            break;
            
        case DIFFICULTY_BALLOON_SPEED:
            snprintf(description, sizeof(description), 
                    "Balloons move %.0f%% faster", (value - 1.0f) * 100);
            break;
            
        case DIFFICULTY_BLUE_PROJECTILE_SPEED:
            snprintf(description, sizeof(description), 
                    "Blue balloon projectiles %.0f%% faster", (value - 1.0f) * 100);
            break;
            
        case DIFFICULTY_GREEN_PROJECTILE_SPEED:
            snprintf(description, sizeof(description), 
                    "Green balloon projectiles %.0f%% faster", (value - 1.0f) * 100);
            break;
            
        case DIFFICULTY_DAMAGE_TAKEN:
            snprintf(description, sizeof(description), 
                    "Take %.0f%% more damage", (value - 1.0f) * 100);
            break;
            
        default:
            snprintf(description, sizeof(description), 
                    "Unknown difficulty effect");
            break;
    }
    
    return description;
} 