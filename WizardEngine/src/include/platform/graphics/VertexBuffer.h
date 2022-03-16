//
// Created by mecha on 07.09.2021.
//

#pragma once

#include <graphics/core/shader/VertexFormat.h>
#include <graphics/core/buffer_data/VertexData.h>
#include <graphics/core/buffer_data/Buffer.h>

#define DEFAULT_VERTEX_COUNT 1000

namespace engine {

    // VBO - Vertex Buffer Object allocated for GPU pipeline
    class VertexBuffer final : public Buffer {

    public:
        VertexBuffer() : Buffer(DEFAULT_VERTEX_COUNT) {
            create();
        }
        VertexBuffer(const uint32_t& vertexCount) : Buffer(vertexCount) {
            create();
        }
        ~VertexBuffer() = default;

    public:
        // lifetime functions
        void create();
        void destroy();
        void recreate();
        // bind/unbind
        void bind() const;
        void unbind() const;
        // GPU allocations
        void alloc();
        void alloc(const uint32_t &vertexCount);
        void malloc(const size_t &memorySize);
        void setFormat(const shader::VertexFormat &vertexFormat);
        void setFormat(const shader::VertexFormat &vertexFormat, const uint32_t& vertexCount);
        // GPU data load
        void load(const VertexData &vertexData);

    public:
        void enableAttributes();
        void disableAttributes();

    private:
        void bindAttributes();

    private:
        uint32_t id = 0;
        shader::VertexFormat vertexFormat;
    };

}