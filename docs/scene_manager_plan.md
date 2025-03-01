# scene manager requirements

## core functionality
- handle scene transitions (load/unload/pause)
- manage scene-specific entities and systems
- support additive and single-scene loading
- implement scene stacking for layered ui
enum class SceneState {
    ACTIVE,    // fully running, visible
    PAUSED,    // visible but not updating (for UI overlays)
    INACTIVE,  // not visible, not updating, but preserved
    DESTROYED  // fully removed from stack
};

## entity management
- scenes own and track their entities
- entities belong to exactly one scene
- scene maintains array of owned EntityIDs
- clear scene-specific entities on unload
- integrate with existing entitymanager (`entity.h`)

## system management
- systems remain singleton instances in SystemManager
- systems receive scene context for updates
- systems filter entities based on scene ownership
- coordinate with systemmanager lifecycle (`systems.h`)
- support updating multiple active scenes per frame

## resource management
- resource management (loading/unloading files) will be still all in resource management
- no change here

## transition features
- no transition animation nor feature

## serialization
- wont implement serialization at this moment

## architecture requirements
- maintain ecs pattern (`ecs_types.h`)
- compatible with current render system (`render_system.cpp`)
- work with input system (`input.h`)
- support existing component macros (`component_macros.h`)

## scene stack behavior
- multiple scenes can be active simultaneously
- stack order determines update/render order (bottom-up)
- states control scene behavior:
  - ACTIVE: full update and render
  - PAUSED: render only, no updates
  - INACTIVE: neither update nor render
  - DESTROYED: remove from stack
- example stack:
  - PauseMenu (ACTIVE) - top
  - GameScene (PAUSED) - middle
  - MainMenu (INACTIVE) - bottom

## error handling
- basic error handling, just print error and exit

## debug tools
- very simple printing, below points are future improvement
    - scene hierarchy visualization
    - entity count monitoring
    - system activity tracking
    - transition profiling
