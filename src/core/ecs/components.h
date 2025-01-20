#pragma once
#include "ecs_types.h"
#include "../resource_manager.h"
#include "components/squirrel_components.h"
#include "string.h"
#include "stdio.h"
#include "base_component.h"
#include <float.h>
#include "components/cloud_components.h"
#include "components/background_component.h"
#include "components/peanut_components.h"

// Add camera constants
#define CAMERA_FOLLOW_SPEED 15.0f     // How fast camera catches up to target
#define CAMERA_DEADZONE_X 100.0f     // Horizontal deadzone before camera starts moving
#define CAMERA_DEADZONE_Y 100.0f     // Vertical deadzone before camera starts moving

struct TransformComponent : Component {
    float x, y;
    float rotation;
    float scale;

    void Init(float posX, float posY, float rot = 0.0f, float scl = 1.0f) {
        x = posX;
        y = posY;
        rotation = rot;
        scale = scl;
    }

    void Destroy() override {
        x = 0.0f;
        y = 0.0f;
        rotation = 0.0f;
        scale = 1.0f;
    }
};

struct SpriteComponent : Component {
    Texture* texture;
    int width, height;
    SDL_Rect srcRect;
    bool isVisible;

    void Init(Texture* tex) {
        texture = tex;
        if (texture) {
            width = texture->width;
            height = texture->height;
            srcRect = {0, 0, width, height};
            isVisible = true;
        } else {
            texture = nullptr;
            width = 0;
            height = 0;
            srcRect = {0, 0, 0, 0};
            isVisible = false;
        }
    }

    void ChangeTexture(Texture* newTexture) {
        texture = newTexture;
        if (texture) {
            width = texture->width;
            height = texture->height;
            srcRect = {0, 0, width, height};
        }
    }

    void Destroy() override {
        // Note: We don't destroy the texture here as it's managed by ResourceManager
        texture = nullptr;
        width = 0;
        height = 0;
        srcRect = {0, 0, 0, 0};
    }
};

struct WASDControllerComponent : Component {
    float moveSpeed;
    bool canMove;

    void Init(float speed = 200.0f, bool enabled = true) {
        moveSpeed = speed;
        canMove = enabled;
    }

    void Destroy() override {
        moveSpeed = 0.0f;
        canMove = false;
    }
};

struct ColliderComponent : Component {
    float width;
    float height;
    bool isTrigger;  // If true, detects collision but doesn't prevent movement
    bool isStatic;   // If true, this object won't be moved during collision resolution
    
    void Init(float w, float h, bool staticCollider = false, bool triggerCollider = false) {
        width = w;
        height = h;
        isStatic = staticCollider;
        isTrigger = triggerCollider;
    }
    
    void Destroy() override {
        width = 0.0f;
        height = 0.0f;
        isTrigger = false;
        isStatic = false;
    }
};

struct AnimationComponent : Component {
    Texture* spriteSheet;            // The sprite sheet texture
    SDL_Rect frameRect;             // Current frame rectangle
    int frameWidth;                 // Width of each frame
    int frameHeight;                // Height of each frame
    int columns;                    // Number of columns in sheet
    int totalFrames;                // Total number of frames
    
    int currentFrame;               // Current frame index
    float frameTime;                // Time per frame (in seconds)
    float accumulator;              // Time accumulator for animation
    bool playing;                   // Is animation playing?
    bool loop;                      // Should animation loop?

    void Init(Texture* sheet, int frameW, int frameH, int cols, int frames, 
              float time = 0.1f, bool shouldLoop = true) {
        spriteSheet = sheet;
        frameWidth = frameW;
        frameHeight = frameH;
        columns = cols;
        totalFrames = frames;
        
        currentFrame = 0;
        frameTime = time;
        accumulator = 0.0f;
        playing = true;
        loop = shouldLoop;

        // Initialize first frame rectangle
        UpdateFrameRect();
    }

    void Destroy() override {
        spriteSheet = nullptr;
        frameRect = {0, 0, 0, 0};
        frameWidth = 0;
        frameHeight = 0;
        columns = 0;
        totalFrames = 0;
        currentFrame = 0;
        frameTime = 0.0f;
        accumulator = 0.0f;
        playing = false;
        loop = false;
    }

    void UpdateFrameRect() {
        frameRect.x = currentFrame % columns * frameWidth;
        frameRect.y = currentFrame / columns * frameHeight;
        frameRect.w = frameWidth;
        frameRect.h = frameHeight;
    }
};

struct GravityComponent : Component {
    float velocityY;       // Current vertical velocity
    float gravityScale;    // Multiplier for gravity (1.0 = normal, 0.5 = half gravity, etc.)
    bool isGrounded;       // Is the entity touching the ground?
    
    void Init(float scale = 1.0f) {
        velocityY = 0.0f;
        gravityScale = scale;
        isGrounded = false;
    }
    
    void Destroy() override {
        velocityY = 0.0f;
        gravityScale = 1.0f;
        isGrounded = false;
    }
};

struct CameraComponent : Component {
    float x, y;              // Camera position (top-left corner)
    float targetX, targetY;  // Position camera is trying to reach
    float viewportWidth;     // Width of the camera view
    float viewportHeight;    // Height of the camera view
    EntityID targetEntity;   // Entity the camera should follow
    float cameraKick;  
    
    // Bounds for camera movement
    float minX, maxX;        // Horizontal bounds
    float minY, maxY;        // Vertical bounds
    
    void Init(float width, float height, EntityID target = 0) {
        x = y = 0.0f;
        targetX = targetY = 0.0f;
        viewportWidth = width;
        viewportHeight = height;
        targetEntity = target;
        cameraKick = 0;
        
        // Set reasonable bounds for our game world (3 windows wide, 50 windows tall)
        minX = 0.0f;
        maxX = width * 3.0f;
        minY = 0.0f;
        maxY = height * 50.0f;
    }
    
    void Destroy() override {
        x = y = 0.0f;
        targetX = targetY = 0.0f;
        viewportWidth = viewportHeight = 0.0f;
        targetEntity = 0;
        minX = minY = 0.0f;
        maxX = maxY = 0.0f;
    }
};

// Component initialization functions
void InitTransform(EntityID entity, float x, float y, float rotation = 0.0f, float scale = 1.0f);
void InitSprite(EntityID entity, Texture* texture);
void InitWASDController(EntityID entity, float moveSpeed = 200.0f, bool canMove = true);
void InitCollider(EntityID entity, float width, float height, bool isStatic = false, bool isTrigger = false);
void InitAnimation(EntityID entity, Texture* sheet, int frameW, int frameH, int cols, int frames, 
                   float time = 0.1f, bool shouldLoop = true);
void InitGravity(EntityID entity, float scale = 1.0f);
void InitSquirrel(EntityID entity);
void InitSquirrelPhysics(EntityID entity);
void InitCamera(EntityID entity, float width, float height, EntityID target = 0);
void InitCloud(EntityID entity, CloudType type, CloudSize cloudSize);
void InitPeanut(EntityID entity, PeanutType type);

struct ComponentArrays {
    // Component data pools
    TransformComponent transforms[MAX_ENTITIES];
    SpriteComponent sprites[MAX_ENTITIES];
    WASDControllerComponent wasdControllers[MAX_ENTITIES];
    ColliderComponent colliders[MAX_ENTITIES];
    AnimationComponent animations[MAX_ENTITIES];
    GravityComponent gravities[MAX_ENTITIES];
    SquirrelComponent squirrelComponents[MAX_ENTITIES];
    CameraComponent cameras[MAX_ENTITIES];
    CloudComponent clouds[MAX_ENTITIES];
    BackgroundComponent backgrounds[MAX_ENTITIES];
    PeanutComponent peanuts[MAX_ENTITIES];

    // Core functions
    void* GetComponentData(EntityID entity, ComponentType type);
    void RemoveComponent(EntityID entity, ComponentType type);
    
    // Add this to ComponentArrays struct
    void Init() {
        // Zero out all component arrays
        memset(this, 0, sizeof(ComponentArrays));
        
        printf("ComponentArrays initialized\n");
    }
}; 