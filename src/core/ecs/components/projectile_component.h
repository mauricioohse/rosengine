#pragma once
#include "../base_component.h"
#include "../ecs_types.h"

struct ProjectileComponent : Component {
    float damage;
    float lifetime;
    float directionX;
    float directionY;
    EntityID owner;
    
    void Init(float dmg = 5.0f, float life = 5.0f, float dx = 0.0f, float dy = 0.0f, EntityID own = 0) {
        damage = dmg;
        lifetime = life;
        directionX = dx;
        directionY = dy;
        owner = own;
    }
    
    void Destroy() override {
        damage = 0.0f;
        lifetime = 0.0f;
        directionX = 0.0f;
        directionY = 0.0f;
        owner = 0;
    }
}; 