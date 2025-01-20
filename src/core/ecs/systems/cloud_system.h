#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

#define COLLISION_GRACE_DISTANCE 15 // px

struct CloudSystem : System {
    void Init() override;
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) override;
    void Destroy() override;

private:
    SoundID cloudHitSoundID;
    SoundID cloudBounceSoundID;
    float hitSoundCooldown;
    const float HIT_SOUND_COOLDOWN_TIME = 0.5f;
}; 