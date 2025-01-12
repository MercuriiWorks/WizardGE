//
// Created by mecha on 02.10.2021.
//

#pragma once

#include <core/vector.h>
#include <graphics/core/shader/VertexAttribute.h>

namespace engine::shader {

    struct ENGINE_API UniformAttribute {
        std::string name;
        ElementCount elementCount = SINGLE;
        u16 count = 1;
        size_t offset = 0;
    };

    ENGINE_API inline size_t elementSize(const UniformAttribute &uniformAttribute) {
        return uniformAttribute.elementCount * sizeof(float);
    }

    ENGINE_API inline size_t size_of(const UniformAttribute &uniformAttribute) {
        return uniformAttribute.count * elementSize(uniformAttribute);
    }

    struct ENGINE_API UniformStructAttribute {
        std::string name;
        vector<UniformAttribute> uniformAttributes;

        void add(const UniformAttribute &uniformAttribute) {
            uniformAttributes.emplace_back(uniformAttribute);
        }

        void add(const std::vector<UniformAttribute> &uniformAttributes) {
            for (const auto &uniformAttribute : uniformAttributes) {
                add(uniformAttribute);
            }
        }

        void clear() {
            uniformAttributes.clear();
        }
    };

}