#pragma once

#include <visual/Visual.h>
#include <graphics/core/texture/Texture.h>
#include <ecs/Scene.h>

namespace engine::visual {

    using namespace ecs;

    class ENGINE_API Toolbar final {

    public:
        Toolbar();
        ~Toolbar();

    public:
        void draw();

    public:
        bool enabled = false;

    private:
        void onScenePlay();
        void onSceneStop();
        void onSceneStep();
        void onSceneSimulate();

        u32 playIcon = invalidTextureId;
        u32 stopIcon = invalidTextureId;
        u32 pauseIcon = invalidTextureId;
        u32 simulateIcon = invalidTextureId;
        u32 stepIcon = invalidTextureId;
        bool isPaused = false;
        SceneState sceneState = SceneState::EDIT;
    };

}
