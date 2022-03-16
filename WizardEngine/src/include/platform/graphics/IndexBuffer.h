//
// Created by mecha on 07.09.2021.
//

#pragma once

#include <graphics/core/buffer_data/IndexData.h>
#include <graphics/core/buffer_data/Buffer.h>

#define DEFAULT_INDEX_COUNT 5000

namespace engine {

    // Single IBO - Index Buffer Object.
    // Can be used as graphics API interface
    class IndexBuffer final : public Buffer {

    public:
        IndexBuffer() : Buffer(DEFAULT_INDEX_COUNT) {
            create();
        }
        IndexBuffer(const uint32_t& indexCount) : Buffer(indexCount) {
            create();
        }
        ~IndexBuffer() = default;

    public:
        // lifetime functions
        void create();
        void destroy();
        void recreate();
        // bind/unbind
        void bind();
        void unbind();
        // GPU allocations
        void alloc();
        void alloc(const uint32_t &indexCount);
        void malloc(const size_t &memorySize);
        // GPU data load
        void load(const IndexData &indexData);

    private:
        uint32_t id = 0;

    };

}