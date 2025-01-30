#pragma once
#include "../base_component.h"

struct PhysicsComponent : Component {
    // Velocity
    float velocityX;
    float velocityY;
    
    // Mass affects knockback force
    float mass;
    
    // Ice physics properties
    float friction;          // How quickly velocity decreases (lower = more slippery)
    float maxSpeed;         // Maximum speed cap
    bool  hasSpeedCap;
    
    // Knockback properties
    float knockbackMultiplier;  // Increases with damage (like Smash Bros)
    float damagePercent;       // Current damage percentage
    
    void Init(float initialMass = 1.0f, float iceFriction = 0.1f, bool speedCap = 0, float maxSpeed_ = 500.0f) {
        velocityX = 0.0f;          velocityY = 0.0f;
        mass = initialMass;
        friction = iceFriction;
        maxSpeed = maxSpeed_;  // Adjust based on testing
        knockbackMultiplier = 1.0f;
        damagePercent = 0.0f;
        hasSpeedCap = speedCap;
    }
    
    void Destroy() override {
        velocityX = 0.0f;
        velocityY = 0.0f;
        mass = 1.0f;
        friction = 0.1f;
        maxSpeed = 500.0f;
        knockbackMultiplier = 1.0f;
        damagePercent = 0.0f;
    }
}; 