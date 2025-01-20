#include "../systems.h"

struct GravitySystem : System {
    static constexpr float GRAVITY = 980.0f;  // pixels per second squared (like 9.8 m/s^2 but scaled)
    
    void Update(float deltaTime, EntityManager* entities, ComponentArrays* components);
    void Init() override;
    void Destroy()  override;
}; 