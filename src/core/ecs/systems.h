#pragma once
#include "components.h"
#include "entity.h"

struct System {
    virtual void Init() = 0;
    virtual void Update(float deltaTime, EntityManager* entities, ComponentArrays* components) = 0;
    virtual void Destroy() = 0;
};

struct SystemManager {
    static const int MAX_SYSTEMS = 32;
    System* systems[MAX_SYSTEMS];
    int systemCount;
    
    void Init();
    void RegisterSystem(System* system);
    void UnregisterSystem(System* system);
    void UpdateSystems(float deltaTime, EntityManager* entities, ComponentArrays* components);
    void Destroy();
}; 