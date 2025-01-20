#pragma once
#include "../base_component.h"

enum CloudType {
    CLOUD_WHITE,
    CLOUD_BLACK
};

enum CloudSize {
    CLOUD_SIZE_SMALL,  
    CLOUD_SIZE_MEDIUM, 
    CLOUD_SIZE_LARGE   
};

struct CloudComponent : Component {

#define CLOUD_BOUNCE_FORCE 300.0f

    CloudType type;
    CloudSize size;
    bool isBouncy;     // Only used for black clouds
    float bounceForce; // Strength of the bounce for black clouds
    
    void Init(CloudType cloudType, CloudSize cloudSize) {
        type = cloudType;
        isBouncy = (type == CLOUD_BLACK);
        bounceForce = (type == CLOUD_BLACK) ? CLOUD_BOUNCE_FORCE : 0.0f; // Default bounce force
        size = cloudSize;
    }
    
    void Destroy() override {
        type = CLOUD_WHITE;
        isBouncy = false;
        bounceForce = 0.0f;
    }
}; 