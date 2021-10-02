//
// Created by mecha on 02.10.2021.
//

#pragma once

#include <cstdint>

#include "../shader/Uniform.h"

namespace engine {

    struct UniformData {
        float* uniforms;
        uint32_t uniformStart;
        uint32_t uniformCount;
    };

}