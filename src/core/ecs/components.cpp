#include "components.h"
#include <stdio.h>
#include "../engine.h"

void* ComponentArrays::GetComponentData(EntityID entity, ComponentType type) {
    if (entity >= MAX_ENTITIES) {
        printf("Warning: Entity ID %u out of bounds\n", entity);
        return nullptr;
    }

    switch (type) {
        case COMPONENT_TRANSFORM: return &transforms[entity];
        case COMPONENT_SPRITE:    return &sprites[entity];
        case COMPONENT_WASD_CONTROLLER: return &wasdControllers[entity];
        case COMPONENT_COLLIDER:  return &colliders[entity];
        case COMPONENT_ANIMATION:  return &animations[entity];
        case COMPONENT_GRAVITY:  return &gravities[entity];
        case COMPONENT_SQUIRREL:  return &squirrelComponents[entity];
        case COMPONENT_CAMERA:  return &cameras[entity];
        case COMPONENT_CLOUD: return &clouds[entity];
        case COMPONENT_BACKGROUND: return &backgrounds[entity];
        case COMPONENT_PEANUT: return &peanuts[entity];

        default:
            printf("Warning: Unknown component type %u\n", type);
            return nullptr;
    }
}


void ComponentArrays::RemoveComponent(EntityID entity, ComponentType type) {
    Component* component = (Component*)GetComponentData(entity, type);
    if (component) {
        component->Destroy();
    }
}

void InitTransform(EntityID entity, float x, float y, float rotation, float scale) {
    TransformComponent* transform = 
        (TransformComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TRANSFORM);
    if (transform) {
        transform->Init(x, y, rotation, scale);
    }
}

void InitSprite(EntityID entity, Texture* texture) {
    SpriteComponent* sprite = 
        (SpriteComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_SPRITE);
    if (sprite) {
        sprite->Init(texture);
    }
}

void InitWASDController(EntityID entity, float moveSpeed, bool canMove) {
    WASDControllerComponent* controller = 
        (WASDControllerComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_WASD_CONTROLLER);
    if (controller) {
        controller->Init(moveSpeed, canMove);
    }
}

void InitCollider(EntityID entity, float width, float height, bool isStatic, bool isTrigger) {
    ColliderComponent* collider = 
        (ColliderComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_COLLIDER);
    if (collider) {
        collider->Init(width, height, isStatic, isTrigger);
    }
} 

void InitSquirrel(EntityID entity){
    SquirrelComponent* squirrel = 
        (SquirrelComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_SQUIRREL);

    if (squirrel){
        squirrel->Init();
    }
}

void InitCamera(EntityID entity, float viewportWidth, float viewportHeight, EntityID target) {
    if (entity >= MAX_ENTITIES) return;
    
    CameraComponent* camera = (CameraComponent*) g_Engine.componentArrays.GetComponentData(entity, COMPONENT_CAMERA);
    if(camera) {
        camera->Init(viewportWidth, viewportHeight, target);
        printf("Camera component initialized for entity %d\n", entity);
    }
}

void InitCloud(EntityID entity, CloudType cloudType, CloudSize cloudSize){
    CloudComponent* cloud = (CloudComponent*) g_Engine.componentArrays.GetComponentData(entity, COMPONENT_CLOUD);

    if (cloud){
        cloud->Init(cloudType, cloudSize);
    }
} 

void InitPeanut(EntityID entity, PeanutType type) {
    PeanutComponent* peanut = 
        (PeanutComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_PEANUT);
    
    if (peanut) {
        peanut->Init(type);
    }
}

