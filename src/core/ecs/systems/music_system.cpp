#include "music_system.h"
#include <stdio.h>
#include "../../../game/game.h"
#include "math.h"
#include <algorithm>

void MusicSystem::Init() {
    isMusicPlaying = false;
    wasKeyPressed = false;
    backgroundMusicID = SOUND_BACKGROUND_MUSIC;
    helicopterSoundID = SOUND_HELICOPTER;
    windSoundID = SOUND_WIND;
    helicopterChannel = -1;
    windChannel = -1;
    isHelicopterPlaying = false;
    isWindPlaying = false;
    
    PlayMusic();
    printf("MusicSystem initialized\n");
}

void MusicSystem::Update(float deltaTime, EntityManager* entities, ComponentArrays* components) {
    // Handle music toggle with M key
    if (Input::IsKeyDown(SDL_SCANCODE_M)) {
        if (!wasKeyPressed) {
            ToggleMusic();
            wasKeyPressed = true;
        }
    } else {
        wasKeyPressed = false;
    }

    // Update helicopter and wind sounds
    UpdateHelicopterSound(g_Game.helicopterEntity, g_Game.squirrelEntity);
    UpdateWindSound(g_Game.squirrelEntity);
}

void MusicSystem::UpdateHelicopterSound(EntityID helicopterEntity, EntityID squirrelEntity) {
    TransformComponent* heliTransform = 
        (TransformComponent*)g_Engine.componentArrays.GetComponentData(helicopterEntity, COMPONENT_TRANSFORM);
    TransformComponent* squirrelTransform = 
        (TransformComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_TRANSFORM);

    if (!heliTransform || !squirrelTransform) return;

    // Calculate distance between helicopter and squirrel
    float dx = heliTransform->x - squirrelTransform->x;
    float dy = heliTransform->y - squirrelTransform->y;
    float distance = sqrtf(dx * dx + dy * dy);

    // Calculate volume based on distance (0-128)
    int volume = 0;
    if (distance < MAX_HELICOPTER_DISTANCE) {
        float volumeRatio = 1.0f - (distance / MAX_HELICOPTER_DISTANCE);
        volume = (int)(volumeRatio * 128 / 4);
    }

    // Start playing helicopter sound if not already playing
    if (volume > 0) {
        if (!isHelicopterPlaying) {
            Sound* heliSound = ResourceManager::GetSound(helicopterSoundID);
            if (heliSound) {
                heliSound->sdlChunk->volume = volume;
                helicopterChannel = Mix_PlayChannel(-1, heliSound->sdlChunk, -1);  // Loop infinitely
                isHelicopterPlaying = true;
            }
        }
    } else if (isHelicopterPlaying) {
        // Stop helicopter sound when too far
        Mix_HaltChannel(helicopterChannel);
        isHelicopterPlaying = false;
    }
}

void MusicSystem::UpdateWindSound(EntityID squirrelEntity) {
    SquirrelComponent* squirrel = 
        (SquirrelComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_SQUIRREL);
    
    if (!squirrel) return;

    // Calculate y velocity
    float totalSpeed = squirrel->velocityY;

    // Calculate volume based on speed (0-128)
    int volume = 0;
    if (totalSpeed > MIN_SPEED_FOR_WIND) {
        float speedRatio = (totalSpeed - MIN_SPEED_FOR_WIND) / 
                          (MAX_SPEED_FOR_WIND - MIN_SPEED_FOR_WIND);
        speedRatio = std::min(1.0f, speedRatio);  // Clamp to 1.0
        volume = (int)(speedRatio * 128 / 16);  // Divide by 2 to keep wind quieter
    }

    // Start or update wind sound
    if (volume > 0) {
        if (!isWindPlaying) {
            Sound* windSound = ResourceManager::GetSound(windSoundID);
            if (windSound) {
                windSound->sdlChunk->volume = volume;
                windChannel = Mix_PlayChannel(-1, windSound->sdlChunk, -1);  // Loop infinitely
                isWindPlaying = true;
            }
        }
    } else if (isWindPlaying) {
        Mix_HaltChannel(windChannel);
        isWindPlaying = false;
    }
}

void MusicSystem::ToggleMusic() {
    if (isMusicPlaying) {
        StopMusic();
    } else {
        PlayMusic();
    }
}

void MusicSystem::PlayMusic() {
    ResourceManager::PlayMusic(backgroundMusicID, -1);  // -1 for infinite loop
    isMusicPlaying = true;
}

void MusicSystem::StopMusic() {
    ResourceManager::StopMusic();
    isMusicPlaying = false;
}

void MusicSystem::Destroy() {
    StopMusic();
    if (isHelicopterPlaying) {
        Mix_HaltChannel(helicopterChannel);
    }
    if (isWindPlaying) {
        Mix_HaltChannel(windChannel);
    }
    printf("MusicSystem destroyed\n");
} 