//
// Created by mecha on 17.11.2021.
//

#pragma once

#ifdef GL
#include "../../opengl/buffers/FrameBuffer.h"
#elif defined(VK)
#error "Vulkan graphics context is not supported!"
#endif