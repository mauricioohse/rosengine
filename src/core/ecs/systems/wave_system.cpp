#include "wave_system.h"
#include "../../../game/game.h"
#include <stdio.h>
#include <math.h>


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
    // Only update waves if game is in playing state
    if (g_Game.gameState != GAME_STATE_PLAYING) {
        return;
    }

    // Check if wave is complete
    if (NoBalloonsRemaining(entities)) {
        currentWave++;
        
        // Check if cycle is complete
        if (currentWave >= WAVES_PER_CYCLE) {
            currentWave = 0;
            currentCycle++;
        }
        
        SpawnWave(entities, components);
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

int WaveSystem::GetScaledCount(int baseCount) {
    // Increase by 50% for each cycle completed
    float multiplier = 1.0f + (currentCycle * 0.5f);
    return (int)ceil(baseCount * multiplier);
}

void WaveSystem::SpawnWave(EntityManager* entities, ComponentArrays* components) {
    WaveData& baseWave = baseWaves[currentWave];
    
    for (const auto& spawn : baseWave.spawns) {
        int scaledCount = GetScaledCount(spawn.count);
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
    ADD_PHYSICS(balloon, 10, 15);
    ADD_COLLIDER(balloon, 64, 64, 0, 1);
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