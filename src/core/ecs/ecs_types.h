#pragma once
#include <SDL.h>

// Common type definitions
typedef uint32_t EntityID;
typedef uint32_t ComponentType;

// Constants
#define MAX_ENTITIES 1000
#define INVALID_ENTITY 0

// Component type identifiers
enum ComponentTypes {
    COMPONENT_NONE = 0,
    COMPONENT_TRANSFORM = 1 << 0,
    COMPONENT_SPRITE = 1 << 1,
    COMPONENT_WASD_CONTROLLER = 1 << 2,
    COMPONENT_COLLIDER = 1 << 3,
    COMPONENT_ANIMATION = 1 << 4,
    COMPONENT_GRAVITY = 1 << 5,
    COMPONENT_SQUIRREL = 1 << 6,
    COMPONENT_CAMERA = 1 << 7,
    COMPONENT_CLOUD = 1 << 8,
    COMPONENT_BACKGROUND = 1 << 9,
    COMPONENT_PEANUT = 1 << 10,
    // Add more component types here
}; 

#define ADD_TRANSFORM(entity, x, y, rot, scale) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_TRANSFORM); \
        InitTransform(entity, x, y, rot, scale); \
    } while(0)

#define ADD_SPRITE(entity, texture) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_SPRITE); \
        InitSprite(entity, texture); \
    } while(0)

#define ADD_WASD_CONTROLLER(entity, speed, enabled) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_WASD_CONTROLLER); \
        InitWASDController(entity, speed, enabled); \
    } while(0)

#define ADD_COLLIDER(entity, width, height, isStatic, isTrigger) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_COLLIDER); \
        InitCollider(entity, width, height, isStatic, isTrigger); \
    } while(0)

#define ADD_ANIMATION(entity, sheet, frameW, frameH, cols, frames, time, shouldLoop) \
    do { \
        AnimationComponent* anim = (AnimationComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_ANIMATION); \
        if (anim) { \
            anim->Init(sheet, frameW, frameH, cols, frames, time, shouldLoop); \
        } \
    } while(0)

#define ADD_GRAVITY(entity, scale) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_GRAVITY); \
        GravityComponent* gravity = (GravityComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_GRAVITY); \
        if (gravity) { \
            gravity->Init(scale); \
        } \
    } while(0)

#define ADD_SQUIRREL(entity) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_SQUIRREL); \
        InitSquirrel(entity); \
    } while(0)

#define ADD_CAMERA(entity, viewportWidth, viewportHeight, targetEntity) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_CAMERA); \
        InitCamera(entity, viewportWidth, viewportHeight, targetEntity); \
    } while(0)


#define ADD_CLOUD(entity, cloudType, size) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_CLOUD); \
        InitCloud(entity, cloudType, size); \
    } while(0)

#define ADD_BACKGROUND(entity, parallax) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_BACKGROUND); \
        BackgroundComponent* background = (BackgroundComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_BACKGROUND); \
        if (background) { \
            background->Init(parallax); \
        } \
    } while(0)

#define ADD_PEANUT(entity, peanutType) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_PEANUT); \
        InitPeanut(entity, peanutType); \
    } while(0)




