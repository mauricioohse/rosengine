#pragma once
#include "../base_component.h"
#include "../ecs_types.h"

struct ShooterComponent : Component {
    float quillSpeed;        // Speed of fired quills
    float recoilForce;      // Force applied to porcupine when shooting
    float fireRate;         // Shots per  second
    float lastShotTime;     // Time tracker for fire rate limiting
    bool canShoot;          // Flag to control shooting ability
    
    void Init(float qSpeed = 500.0f, float rForce = 300.0f, float fRate = 0.2f) {
        quillSpeed = qSpeed;
        recoilForce = rForce;
        fireRate = fRate;
        lastShotTime = 0.0f;
        canShoot = true;
    }
    
    void Destroy() override {
        quillSpeed = 0.0f;
        recoilForce = 0.0f;
        fireRate = 0.0f;
        lastShotTime = 0.0f;
        canShoot = false;
    }
}; 