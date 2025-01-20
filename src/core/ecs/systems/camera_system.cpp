#include "camera_system.h"
#include <stdio.h>
#include <algorithm>
#include <math.h>

void CameraSystem::Init() {
    printf("CameraSystem initialized\n");
}

void CameraSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (entities->HasComponent(entity, COMPONENT_CAMERA)) {
            CameraComponent* camera = &components->cameras[entity];
            
            if (camera->targetEntity == 0) continue;
            
            // Get target's transform
            TransformComponent* targetTransform = 
                (TransformComponent*)components->GetComponentData(camera->targetEntity, COMPONENT_TRANSFORM);
            
            if (!targetTransform) continue;

            // Gradually reduce camera kick
            if (camera->cameraKick != 0) {
                camera->cameraKick *= 0.95f;  // Reduce kick by 5% each frame
                if (fabs(camera->cameraKick) < 0.1f) {
                    camera->cameraKick = 0;
                }
            }

            // Calculate target position (center of screen)
            camera->targetX = targetTransform->x - camera->viewportWidth/2;
            camera->targetY = targetTransform->y - camera->viewportHeight/2 + 325.0f + camera->cameraKick;

            // Smooth follow
            camera->x += (camera->targetX - camera->x) * CAMERA_FOLLOW_SPEED*4 * deltaTime;
            camera->y += (camera->targetY - camera->y) * CAMERA_FOLLOW_SPEED * deltaTime;

            // clamp at the bottom
            camera->y = std::min((float)GAME_HEIGHT - 200, camera->y);
        }
    }
}

void CameraSystem::Destroy() {
    printf("CameraSystem destroyed\n");
} 