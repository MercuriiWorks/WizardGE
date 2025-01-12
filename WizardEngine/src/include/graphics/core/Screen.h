//
// Created by mecha on 01.04.2022.
//

#pragma once

#include <graphics/core/Renderer.h>

namespace engine::graphics {

    class ENGINE_API ScreenRenderer : public PrimitiveRenderer {
    public:
        ScreenRenderer() = default;

    public:
        void init();
        void onWindowResized(u32 width, u32 height);
    };

}