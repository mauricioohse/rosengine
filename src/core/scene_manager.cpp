#include "scene_manager.h"
#include "engine.h"

EntityID SceneBase::RegisterEntity(){
    EntityID newEntity = g_Engine.entityManager.CreateEntity();

    entities[entityCount++] = newEntity;

    return newEntity;
}

void SceneBase::OnUpdate(float deltaTime) {
    g_Engine.systemManager.UpdateSystems(deltaTime, &entities, &g_Engine.componentArrays);
}

void SceneManager::Init() {
    
}
void SceneManager::Update(float deltaTime) {

    for (int i = 0; sceneStack[i]!=nullptr;i++)
    {
        SceneBase* scene = sceneStack[i];
        switch (scene->state){
            case SceneState::ACTIVE:
                scene->OnUpdate(deltaTime);   
                break;

            case SceneState::PAUSED:
                // scene->OnPause(deltaTime);
                break;
            
            default:
                break;
        }

    }

}


void SceneManager::Cleanup() {
    memset(this, 0, sizeof(this));
}

// stack operations
void SceneManager::PushScene(SceneBase *scene) {
    if (stackSize < MAX_SCENE_STACK_SIZE) {
        sceneStack[stackSize++] = scene;
    } else {
        printf("ERROR: scene stack overflow - cannot push new scene\n");
    }

}    // add scene to top of stack
void SceneManager::PopScene() {
    free(sceneStack[stackSize--]);
}                     


