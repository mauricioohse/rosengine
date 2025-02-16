#pragma once
#include "../core/window.h"
#include "../core/resource_manager.h"
#include "../core/ecs/systems/render_system.h"
#include "../core/ecs/systems/wasd_controller_system.h"
#include "../core/ecs/systems/collision_system.h"
#include "../core/ecs/systems/camera_system.h"
#include "../core/ecs/systems/background_system.h"
#include "../core/ecs/systems/music_system.h"
#include "../core/ecs/component_macros.h"

// Add new game state
enum GameState {
    GAME_STATE_START,    // New state for start screen
    GAME_STATE_PLAYING,
    GAME_STATE_GAMEOVER,
    GAME_STATE_PAUSED
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

    // Systems
    RenderSystem renderSystem;
    WASDControllerSystem wasdSystem;
    CollisionSystem collisionSystem;
    CameraSystem cameraSystem;
    BackgroundSystem backgroundSystem;
    MusicSystem musicSystem;


};

extern Game g_Game;  // Global game instance 
