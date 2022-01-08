//
// Created by mecha on 27.10.2021.
//

#include "TransformComponents.h"

namespace engine {

    Transform2dComponent transform2d() {
        ModelMatrix2d transform = {};
        transform.name = DEFAULT_TRANSFORM_NAME;
        ModelMatrices::update(transform);
        return transform;
    }

    Transform3dComponent transform3d() {
        ModelMatrix3d transform = {};
        transform.name = DEFAULT_TRANSFORM_NAME;
        ModelMatrices::update(transform);
        return transform;
    }

    Transform3dComponent transform3d(const glm::vec3 &pos,
                                     const glm::vec3 &rot,
                                     const glm::vec3 &scale) {
        ModelMatrix3d transform = {pos, rot, scale};
        transform.name = DEFAULT_TRANSFORM_NAME;
        ModelMatrices::update(transform);
        return transform;
    }

}