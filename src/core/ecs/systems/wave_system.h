#pragma once
#include "../components.h"
#include "../entity.h"
#include "../systems.h"
#include <array>
#include <vector>

enum UpgradeType {
    UPGRADE_NONE = 0,
    UPGRADE_FIRE_RATE,      // Faster shooting
    UPGRADE_MULTI_SHOT,     // Shoot multiple quills
    UPGRADE_SPEED,          // Move faster
    UPGRADE_GRIP,           // Better ice control
    UPGRADE_RECOIL_RES,     // Take less recoil from shooting
    UPGRADE_MAX
};

enum DifficultyType {
    DIFFICULTY_NONE = 0,
    DIFFICULTY_RED_QUANTITY,         // More red balloons per wave
    DIFFICULTY_BLUE_QUANTITY,        // More blue balloons per wave
    DIFFICULTY_GREEN_QUANTITY,       // More green balloons per wave
    DIFFICULTY_BALLOON_SPEED,        // All balloons move faster
    DIFFICULTY_BLUE_PROJECTILE_SPEED,// Blue balloon projectiles faster
    DIFFICULTY_GREEN_PROJECTILE_SPEED,// Green balloon projectiles faster
    DIFFICULTY_DAMAGE_TAKEN,         // Player takes more damage/knockback
    DIFFICULTY_MAX
};

struct Upgrade {
    UpgradeType type;
    float value;          // Amount of improvement
    const char* name;     // Display name
    const char* description;
};

struct Difficulty {
    DifficultyType type;
    float value;          // Amount of increase
    const char* name;     // Display name for UI
    const char* description;
};

struct UpgradeChoice {
    Upgrade upgrade;
    Difficulty difficulty;  // The trade-off
};

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
    float spawnTimer = 0.0f;
    
    // Base wave definitions
    std::array<WaveData, WAVES_PER_CYCLE> baseWaves;
    
    // Upgrade and difficulty tracking
    std::vector<UpgradeType> activeUpgrades;
    std::vector<DifficultyType> activeDifficulties;
    bool awaitingUpgradeChoice;
    UpgradeChoice currentUpgradeChoices[3];  // 3 upgrade-difficulty pairs
    
    int GetScaledCount(int baseCount);
    bool NoBalloonsRemaining(EntityManager* entities);
    int GetScaledCount(int baseCount, BalloonType type);
    void SpawnWave(EntityManager *entities, ComponentArrays *components);
    void SpawnBalloon(BalloonType type, EntityManager* entities, ComponentArrays* components);
    void ApplyUpgrade(const UpgradeChoice& choice);
    const char* GetDifficultyDescription(DifficultyType type, float value);
    const char* GetDifficultyName(DifficultyType type);

    // Helper functions for upgrade generation
    UpgradeType GetRandomUnusedUpgrade(const std::vector<UpgradeType>& usedTypes);
    DifficultyType GetRandomUnusedDifficulty(const std::vector<DifficultyType>& usedTypes);
    bool IsUpgradeValid(UpgradeType type, const std::vector<UpgradeType>& usedTypes);
    bool IsDifficultyValid(DifficultyType type, const std::vector<DifficultyType>& usedTypes);

public:
    // Difficulty values
    float redBalloonMultiplier = 1.0f;
    float blueBalloonMultiplier = 1.0f;
    float greenBalloonMultiplier = 1.0f;
    float balloonSpeedMultiplier = 1.0f;
    float blueProjectileSpeedMultiplier = 1.0f;
    float greenProjectileSpeedMultiplier = 1.0f;
    float damageMultiplier = 1.0f;
    
    
    int currentWave = 0;
    int currentCycle = 0;  // Increases every 5 waves
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;
    void ResetWaves() {
        currentWave = 0;
        currentCycle = 0;
        spawnTimer = 0.0f;
        redBalloonMultiplier = 1.0f;
        blueBalloonMultiplier = 1.0f;
        greenBalloonMultiplier = 1.0f;
        balloonSpeedMultiplier = 1.0f;
        blueProjectileSpeedMultiplier = 1.0f;
        greenProjectileSpeedMultiplier = 1.0f;
        damageMultiplier = 1.0f;
    }
    bool IsAwaitingUpgradeChoice() const { return awaitingUpgradeChoice; }
    const UpgradeChoice* GetCurrentUpgradeChoices() const { return currentUpgradeChoices; }
    void SelectUpgrade(int choice);  // 0-2 for the three choices

    Upgrade GetUpgradeData(UpgradeType type);

    void GenerateUpgradeChoices();
    Difficulty GenerateRandomDifficulty();

    // Getters for difficulty stats
    float GetRedBalloonMultiplier() const { return redBalloonMultiplier; }
    float GetBlueBalloonMultiplier() const { return blueBalloonMultiplier; }
    float GetGreenBalloonMultiplier() const { return greenBalloonMultiplier; }
    float GetBalloonSpeedMultiplier() const { return balloonSpeedMultiplier; }
    float GetBlueProjectileSpeedMultiplier() const { return blueProjectileSpeedMultiplier; }
    float GetGreenProjectileSpeedMultiplier() const { return greenProjectileSpeedMultiplier; }
    float GetDamageMultiplier() const { return damageMultiplier; }

    void SetAwaitingUpgradeChoice(bool waiting) { awaitingUpgradeChoice = waiting; }
}; 