#pragma once
#include "../core/window.h"
#include "../core/resource_manager.h"
#include "../core/ecs/systems/render_system.h"
#include "../core/ecs/systems/wasd_controller_system.h"
#include "../core/ecs/systems/collision_system.h"
#include "../core/ecs/systems/gravity_system.h"
// #include "../core/ecs/systems/squirrel_physics_system.h"
#include "../core/ecs/systems/camera_system.h"
#include "../core/ecs/systems/cloud_system.h"
#include "../core/ecs/systems/background_system.h"
#include "../core/ecs/systems/peanut_system.h"
#include "../core/ecs/systems/music_system.h"
#include "../core/ecs/systems/ice_physics_system.h"
#include "../core/ecs/systems/shooter_system.h"
#include "../core/ecs/systems/balloon_system.h"
#include "../core/ecs/systems/wave_system.h"
#include "../core/ecs/component_macros.h"

// Add new game state
enum GameState {
    GAME_STATE_START,    // New state for start screen
    GAME_STATE_PLAYING,
    GAME_STATE_GAMEOVER
};

struct PeanutTarget {
    float x;
    float y;
    bool isCollected;
};

class Game {
public:
    bool Init();
    void HandleInput();
    void Update(float deltaTime);
    void RenderUpgradeUI();
    void Render();
    void Cleanup();
    void Reset();


    
    EntityID squirrelEntity;
    EntityID helicopterEntity;
    EntityID cameraEntity;

    static const int MAX_PEANUT_TARGETS = 100;
    PeanutTarget peanutTargets[MAX_PEANUT_TARGETS];
    int numPeanutTargets;
    EntityID arrowEntity;  // To track the arrow sprite
    GameState gameState;
    bool g_Porcupine_is_hit = 0;
    float g_Porcuopine_hit_anim_timer = 0.0f;

    void AddScore(BalloonType balloonType);


    // Systems
    RenderSystem renderSystem;
    WASDControllerSystem wasdSystem;
    CollisionSystem collisionSystem;
    GravitySystem gravitySystem;
    // SquirrelPhysicsSystem squirrelSystem;
    CameraSystem cameraSystem;
    CloudSystem cloudSystem;
    BackgroundSystem backgroundSystem;
    PeanutSystem peanutSystem;
    MusicSystem musicSystem;
    WaveSystem waveSystem;
    ShooterSystem shooterSystem;
    IcePhysicsSystem icePhysicsSystem;

private:

    // Entities
    EntityID backgroundEntity;
    EntityID bottomBackgroundEntity;
    
    // Resources (using IDs instead of pointers)
    SoundID hitSoundID;
    FontID fpsFontID;
    
    float gameTimer;  // Track elapsed time in seconds
    bool isNewRecord;  // To track if current time is best time
    float bestTime;    // Store best completion time

    // Scoring system
    int currentScore;
    int bestScore;
    float comboMultiplier;
    float comboTimer;
    
    // Declare constants (but don't define them here)
    static const float COMBO_TIME_LIMIT;
    static const float COMBO_MULTIPLIER_INCREASE;
    static const float MAX_COMBO_MULTIPLIER;
    static const int SCORE_RED_BALLOON;
    static const int SCORE_GREEN_BALLOON;
    static const int SCORE_BLUE_BALLOON;

    void PrintUpgradeStats();
};

extern Game g_Game;  // Global game instance 
