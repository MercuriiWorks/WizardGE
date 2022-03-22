//
// Created by mecha on 18.03.2022.
//

#pragma once

#include <graphics/core/buffer_data/VertexData.h>
#include <glm/glm.hpp>

namespace engine {

    struct LineVertex {
        glm::vec3 position = { 0.5, 0.5, 0.5 };
        glm::vec4 color = { 0, 1, 0, 1 };
    };

    struct InstanceLine : VertexDataComponent<InstanceVertex<LineVertex>> {
        InstanceLine(const std::vector<LineVertex>& linesVertices)
        : VertexDataComponent<InstanceVertex<LineVertex>>() {
            vertexData = toVertexData<LineVertex, InstanceVertex<LineVertex>>(linesVertices);
            this->drawType = LINE;
        }
    };

    struct BatchLine : VertexDataComponent<BatchVertex<LineVertex>> {
        BatchLine(const std::vector<LineVertex>& linesVertices)
        : VertexDataComponent<BatchVertex<LineVertex>>() {
            vertexData = toVertexData<LineVertex, BatchVertex<LineVertex>>(linesVertices);
            this->drawType = LINE;
        }
    };
}
