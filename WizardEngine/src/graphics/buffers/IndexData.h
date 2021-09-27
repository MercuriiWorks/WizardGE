//
// Created by mecha on 24.09.2021.
//

#pragma once

#include <cstdint>

#define MIN_INDEX_COUNT 3

namespace engine {

    struct IndexData {
        uint32_t* indices;
        uint32_t indexStart;
        uint32_t indexCount;

        IndexData() = default;

        IndexData(uint32_t* indices,
                  const uint32_t &indexStart = 0,
                  const uint32_t &indexCount = MIN_INDEX_COUNT) :
                  indices(indices),
                  indexStart(indexStart),
                  indexCount(indexCount) {}
    };

}