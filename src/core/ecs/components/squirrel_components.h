#ifndef SQUIRREL_COMPONENTS_H
#define SQUIRREL_COMPONENTS_H

#include "../base_component.h"
#include <stdio.h>
 
typedef enum {
    SQUIRREL_STATE_DROPPING,
    SQUIRREL_STATE_OPEN_ARMS,
    SQUIRREL_STATE_CLOSED_ARMS,
    SQUIRREL_STATE_WIGGLING
} SquirrelState;

// Physics constants
#define SQUIRREL_GRAVITY 150.0f          // Increased for more noticeable falling
#define SQUIRREL_OPEN_ARMS_MAX_SPEED 200.0f
#define SQUIRREL_CLOSED_ARMS_MAX_SPEED 400.0f
#define SQUIRREL_WIGGLE_MAX_SPEED 100.0f
#define SQUIRREL_ROTATION_SPEED 90.0f  // Degrees per second
#define SQUIRREL_TAP_ROTATION 15.0f     // Degrees per tap
#define SQUIRREL_SPRITE_ROTATION_OFFSET -90.0f  // Rotate sprite 90 degrees to point feet down
#define SQUIRREL_WIGGLE_DURATION 1.0f // in seconds
#define SQUIRREL_GRACE_PERIOD 3.0f // in seconds
#define SQUIRREL_DROP_DELAY 1.0f  // Time before squirrel starts falling
#define SMOOTHING_FACTOR 0.05f;  // Adjust this value to control smoothing (0.05-0.2 works well)
struct SquirrelComponent : Component {
    // Gameplay state
    SquirrelState state;
    float wiggleTimer;      // For wiggle state duration
    float graceTimer;       // For post-wiggle grace period
    float maxSpeed;         // Current max speed (changes with state)
    float rotation;         // Current rotation in degrees
    float targetRotation;   // Target rotation for smooth interpolation
    float dropTimer;

    // Physics properties 
    float velocityX;
    float velocityY;
    float gravity;          // Gravity acceleration
    float currentGravity;
    float acceleration;     // For closed arms state
    float rotationSpeed;    // For open arms state rotation

// Powerup properties
    float baseMaxSpeed;        // Store original max speed
    float speedBoost;         // Additional speed from powerups
    bool hasShield;           // Immunity to clouds
    float shieldTimer;        // Duration of shield powerup
    bool hasSuperMode;        // Super peanut active
    float superTimer;         // Duration of super mode

    void Init() {
        printf("SquirrelComponent::Init() called\n");
        // Gameplay state init
        state = SQUIRREL_STATE_DROPPING;  // Start in dropping state
        dropTimer = SQUIRREL_DROP_DELAY;
        wiggleTimer = 0;
        graceTimer = 0;
        maxSpeed = SQUIRREL_OPEN_ARMS_MAX_SPEED;  // Start with open arms speed
        rotation = 0.0f;        // Start pointing straight down
        targetRotation = 0.0f;

        // Physics init
        velocityX = 0;
        velocityY = 0;
        gravity = SQUIRREL_GRAVITY;
        currentGravity = SQUIRREL_GRAVITY;
        acceleration = 0;
        rotationSpeed = 0;

        // Initialize powerup properties
        baseMaxSpeed = SQUIRREL_OPEN_ARMS_MAX_SPEED;
        speedBoost = 0.0f;
        hasShield = false;
        shieldTimer = 0.0f;
        hasSuperMode = false;
        superTimer = 0.0f;
    }

    void Destroy() override {
        Init();
    }
};

#endif 