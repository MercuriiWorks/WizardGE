//
// Created by mecha on 26.09.2021.
//

#pragma once

#include <platform/graphics/RenderCommands.h>
#include <ecs/ecs.h>

#include <algorithm>
#include <functional>

#define MIN_VERTEX_COUNT 3

namespace engine::graphics {

    using namespace core;

    template<typename V>
    struct BatchVertex {
        V vertex;
        f32 id = 0;
    };

    template<typename V>
    struct InstanceVertex {
        V vertex;
    };

    template_component(VertexDataComponent, T) {
        array<T> vertexData;
        bool isUpdated = true;
        u8 renderModelId = 0;
        u32 drawType = DrawType::QUAD;

        VertexDataComponent<T> copy();

        void setBatchId(u32 batchId);

        void updateStart(u32 prevVertexCount);

        void invalidate();

        template<typename TO>
        VertexDataComponent<TO> toVertexDataComponent(const std::function<TO(const T&)>& vertexMapper);
    };

    template<typename T>
    VertexDataComponent<T> VertexDataComponent<T>::copy() {
        VertexDataComponent<T> copyVertexDataComponent;
        copyVertexDataComponent.drawType = drawType;
        copyVertexDataComponent.renderModelId = renderModelId;
        copyVertexDataComponent.isUpdated = isUpdated;
        copyVertexDataComponent.vertexData = vertexData.copy();
        return copyVertexDataComponent;
    }

    template<typename T>
    void VertexDataComponent<T>::setBatchId(u32 batchId) {
        for (auto i = 0; i < vertexData.size; i++) {
            vertexData.values[i].id = (float) batchId;
        }
    }

    template<typename T>
    void VertexDataComponent<T>::updateStart(u32 prevVertexCount) {
        vertexData.offset = prevVertexCount;
    }

    template<typename T>
    void VertexDataComponent<T>::invalidate() {
        vertexData.offset = 0;
    }

    template<typename T>
    template<typename TO>
    VertexDataComponent<TO>
    VertexDataComponent<T>::toVertexDataComponent(const std::function<TO(const T &)>& vertexMapper) {
        return VertexDataComponent<TO>{ vertexData.toVertexData(vertexMapper) };
    }
}

namespace YAML {

    using namespace engine::graphics;

    template<typename V>
    struct convert<InstanceVertex<V>> {

        static Node encode(const InstanceVertex<V>& instanceVertex) {
            Node node;
            instanceVertex.vertex.encode(node);
            return node;
        }

        static bool decode(const Node& node, InstanceVertex<V>& instanceVertex) {
            instanceVertex.vertex.decode(node, 0);
            return true;
        }

    };

    template<typename V>
    struct convert<BatchVertex<V>> {

        static Node encode(const BatchVertex<V>& batchVertex) {
            Node node;
            node.push_back(batchVertex.id);
            batchVertex.vertex.encode(node);
            return node;
        }

        static bool decode(const Node& node, BatchVertex<V>& batchVertex) {
            batchVertex.id = node[0].as<f32>();
            batchVertex.vertex.decode(node, 1);
            return true;
        }

    };
}