#pragma once
#include "../core/scene_manager.h"

class MainGameScene : public SceneBase {
public:
    void OnLoad() override;
};

extern MainGameScene g_mainGame;
