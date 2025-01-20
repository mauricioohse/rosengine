#pragma once
#include "../base_component.h"
#include <stdio.h>

typedef enum {
    PEANUT_TYPE_REGULAR,   // Permanent speed boost
    PEANUT_TYPE_SHIELD,    // Temporary cloud immunity
    PEANUT_TYPE_SUPER      // Both speed boost and immunity
} PeanutType;

#define PEANUT_SPEED_BOOST 50.0f        // Added to max speed
#define PEANUT_SHIELD_DURATION 5.0f      // Seconds of immunity
#define PEANUT_SUPER_DURATION 5.0f       // Seconds of super mode

struct PeanutComponent : Component {
    PeanutType type;
    bool wasCollected;

    void Init(PeanutType peanutType) {
        // printf("PeanutComponent::Init() called\n");
        type = peanutType;
        wasCollected = false;
    }

    void Destroy() override {
        wasCollected = false;
    }
}; 