#pragma once
#include "../components.h"
#include "../entity.h"
#include "../systems.h"
#include <array>
#include <vector>

struct WaveData {
    struct BalloonSpawn {
        BalloonType type;
        int count;
    };
    std::vector<BalloonSpawn> spawns;
};

class WaveSystem : public System {
private:
    static const int WAVES_PER_CYCLE = 5;
    int currentWave = 0;
    int currentCycle = 0;  // Increases every 5 waves
    float spawnTimer = 0.0f;
    
    // Base wave definitions
    std::array<WaveData, WAVES_PER_CYCLE> baseWaves;
    
    int GetScaledCount(int baseCount);
    bool NoBalloonsRemaining(EntityManager* entities);
    void SpawnWave(EntityManager* entities, ComponentArrays* components);
    void SpawnBalloon(BalloonType type, EntityManager* entities, ComponentArrays* components);

public:
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
    void ResetWaves() {
        currentWave = 0;
        currentCycle = 0;
        spawnTimer = 0.0f;
    }
}; 