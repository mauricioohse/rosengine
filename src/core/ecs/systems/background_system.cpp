#include "background_system.h"
#include "../components.h"
#include "../../engine.h"
#include "../../window.h"
#include "../../../game/game.h"

float animationTimer;
const float ANIMATION_FRAME_TIME = 0.25f;  // 250ms per frame
int currentFrame;
const int NUM_FRAMES = 2;  // Number of bottom background frames

void BackgroundSystem::Init() {
    printf("BackgroundSystem initialized\n");
    animationTimer = 0.0f;
    currentFrame = 0;
}

void BackgroundSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    // Get camera position first
    CameraComponent* camera = nullptr;
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_CAMERA)) {
            camera = &components->cameras[entity];
            break;
        }
    }
    
    if (!camera) return;

    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_BACKGROUND | COMPONENT_TRANSFORM | COMPONENT_SPRITE)) {
            TransformComponent* transform = &components->Transforms[entity];
            SpriteComponent* sprite = &components->Sprites[entity];
            
            // Calculate the position to draw the background
            // Offset by 1500 to align background center with world (0,0)
            SDL_Rect destRect = {
                0,
                0,
                sprite->width,
                sprite->height
            };
            
            SDL_RenderCopy(g_Engine.window->renderer, sprite->texture->sdlTexture, NULL, &destRect);
        }
    }
}

void BackgroundSystem::Destroy() {
    printf("BackgroundSystem destroyed\n");
} 