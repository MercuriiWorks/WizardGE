//
// Created by mecha on 12.09.2021.
//

#pragma once

#include <core/primitives.h>

namespace engine::graphics {

    using namespace core;

    // VAO - Vertex Array Object in Graphics pipeline
    class VertexArray final {

    public:
        VertexArray() {
            create();
        }
        ~VertexArray() = default;

    public:
        void create();
        void destroy();
        void recreate();
        // bind/unbind
        void bind() const;
        void unbind() const;

    private:
        u32 id = 0;
    };

}