#pragma once
#include "../base_component.h"
#include "../ecs_types.h"

enum BalloonType {
    BALLOON_RED,    // Direct chase
    BALLOON_GREEN,  // Laser patterns (for later)
    BALLOON_BLUE    // Aimed shots (for later)
};

struct BalloonComponent : Component {
    BalloonType type;
    EntityID targetEntity;  // Usually the porcupine
    float moveSpeed;        // Base movement speed
    float health;          // Current health
    float shootTimer;    // Timer for shooting lasers
    float orbitAngle;    // Angle for circular movement
    float explosionTimer;
    bool isExploding;

    void Init(BalloonType balloonType = BALLOON_RED, 
              EntityID target = 0, 
              float speed = 200.0f, 
              float initialHealth = 100.0f) 
    {
        type = balloonType;
        targetEntity = target;
        moveSpeed = speed;
        health = initialHealth;
        shootTimer = 0.0f;
        orbitAngle = 0.0f;
        explosionTimer = 0.0f;
        isExploding = false;
    }
    
    void Destroy() override {
        targetEntity = 0;
        moveSpeed = 0.0f;
        health = 0.0f;
        shootTimer = 0.0f;
        orbitAngle = 0.0f;
        explosionTimer = 0.0f;
        isExploding = false;
    }
}; 