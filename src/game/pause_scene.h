#pragma once
#include "../core/scene_manager.h"

class PauseScene : public SceneBase {
public:
    void OnLoad() override;
};

extern PauseScene g_pauseScene; 