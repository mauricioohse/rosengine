#pragma once
#include "../core/window.h"
#include "../core/resource_manager.h"
#include "../core/ecs/systems/render_system.h"
#include "../core/ecs/systems/wasd_controller_system.h"
#include "../core/ecs/systems/collision_system.h"
#include "../core/ecs/systems/gravity_system.h"
#include "../core/ecs/systems/squirrel_physics_system.h"
#include "../core/ecs/systems/camera_system.h"
#include "../core/ecs/systems/cloud_system.h"
#include "../core/ecs/systems/background_system.h"
#include "../core/ecs/systems/peanut_system.h"
#include "../core/ecs/systems/music_system.h"

enum GameState {
    GAME_STATE_PLAYING,
    GAME_STATE_FINISHED
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
    void Render();
    void Cleanup();
    void Reset();

    void UpdatePeanutTargets();  // Call this when peanuts are collected
    void UpdateArrowDirection();  // Call this each frame

    
    EntityID squirrelEntity;
    EntityID helicopterEntity;
    EntityID cameraEntity;

    static const int MAX_PEANUT_TARGETS = 100;
    PeanutTarget peanutTargets[MAX_PEANUT_TARGETS];
    int numPeanutTargets;
    EntityID arrowEntity;  // To track the arrow sprite


private:
    // Systems
    RenderSystem renderSystem;
    WASDControllerSystem wasdSystem;
    CollisionSystem collisionSystem;
    GravitySystem gravitySystem;
    SquirrelPhysicsSystem squirrelSystem;
    CameraSystem cameraSystem;
    CloudSystem cloudSystem;
    BackgroundSystem backgroundSystem;
    PeanutSystem peanutSystem;
    MusicSystem musicSystem;
    
    // Entities
    EntityID backgroundEntity;
    EntityID bottomBackgroundEntity;
    
    // Resources (using IDs instead of pointers)
    SoundID hitSoundID;
    FontID fpsFontID;
    
    float gameTimer;  // Track elapsed time in seconds
    GameState gameState;
    bool isNewRecord;  // To track if current time is best time
    float bestTime;    // Store best completion time

    
};

extern Game g_Game;  // Global game instance 