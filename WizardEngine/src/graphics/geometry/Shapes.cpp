//
// Created by mecha on 24.09.2021.
//

#include "Shapes.h"

namespace engine {

    Vertex *Triangle::createVertices() {
        auto v1 = Vertex {
            {-0.5f, -0.5f, 0.0f },
            {0.5f, 0.5f}
        };

        auto v2 = Vertex {
            {0.5f, -0.5f, 0.0f},
            {0.5f, 0.5f}
        };

        auto v3 = Vertex {
            {0.0f,  0.5f, 0.0f },
            {0.5f, 0.5f}
        };

        return new Vertex[] {
            v1, v2, v3
        };
    }

    uint32_t *Triangle::createIndices() {
        return new uint32_t[] {0, 1, 2};
    }

    Vertex *Cube::createVertices() {
        auto v1 = Vertex {
            {-0.5f,0.5f,-0.5f},
        };

        auto v2 = Vertex {
            {-0.5f,-0.5f,-0.5f},
        };
        auto v3 = Vertex {
            {0.5f,-0.5f,-0.5f},
        };
        auto v4 = Vertex {
            {0.5f,0.5f,-0.5f},
        };

        auto v5 = Vertex {
            {-0.5f,0.5f,0.5f},
        };

        auto v6 = Vertex {
            {-0.5f,-0.5f,0.5f},
        };

        auto v7 = Vertex {
            {0.5f,-0.5f,0.5f},
        };

        auto v8 = Vertex {
            {0.5f,0.5f,0.5f}
        };

        auto v9 = Vertex {
            {0.5f,0.5f,-0.5f}
        };

        auto v10 = Vertex {
            {0.5f,-0.5f,-0.5f}
        };

        auto v11 = Vertex {
            {0.5f,-0.5f,0.5f}
        };

        auto v12 = Vertex {
            {0.5f,0.5f,0.5f}
        };

        auto v13 = Vertex {
            {-0.5f,0.5f,-0.5f}
        };

        auto v14 = Vertex {
            {-0.5f,-0.5f,-0.5f}
        };

        auto v15 = Vertex {
            {-0.5f,-0.5f,0.5f},
        };

        auto v16 = Vertex {
            {-0.5f,0.5f,0.5f}
        };

        auto v17 = Vertex {
            {-0.5f,0.5f,0.5f}
        };

        auto v18 = Vertex {
            {-0.5f,0.5f,-0.5f},
        };

        auto v19 = Vertex {
            { 0.5f,0.5f,-0.5f},
        };

        auto v20 = Vertex {
            {0.5f,0.5f,0.5f},
        };

        auto v21 = Vertex {
            {-0.5f,-0.5f,0.5f}
        };

        auto v22 = Vertex {
            {-0.5f,-0.5f,-0.5f}
        };

        auto v23 = Vertex {
            {0.5f,-0.5f,-0.5f}
        };

        auto v24 = Vertex {
            {0.5f,-0.5f,0.5f}
        };

        return new Vertex[] {
            v1, v2, v3, v4,
            v5, v6, v7, v8,
            v9, v10, v11, v12,
            v13, v14, v15, v16,
            v17, v18, v19, v20,
            v21, v22, v23, v24,
        };
    }

    uint32_t *Cube::createIndices() {
        return new uint32_t [] {
            0,1,3,
            3,1,2,
            4,5,7,
            7,5,6,
            8,9,11,
            11,9,10,
            12,13,15,
            15,13,14,
            16,17,19,
            19,17,18,
            20,21,23,
            23,21,22
        };
    }

}