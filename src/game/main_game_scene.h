#pragma once
#include "../core/scene_manager.h"

class MainGameScene : public SceneBase {
public:
    void OnLoad() override;
    void OptionalUpdate(float deltaTime) override;
};

extern MainGameScene g_mainGame;
