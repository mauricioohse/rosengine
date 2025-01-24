#pragma once
#include "../base_component.h"
#include "../ecs_types.h"

struct QuillComponent : Component {
    float damage;           // Damage dealt on hit
    float lifetime;         // How long the quill exists
    float currentTime;      // Current lifetime tracker
    EntityID owner;         // Entity that fired this quill
    
    void Init(float dmg = 10.0f, float life = 2.0f, EntityID own = 0) {
        damage = dmg;
        lifetime = life;
        currentTime = 0.0f;
        owner = own;
    }
    
    void Destroy() override {
        damage = 0.0f;
        lifetime = 0.0f;
        currentTime = 0.0f;
        owner = 0;
    }
}; 