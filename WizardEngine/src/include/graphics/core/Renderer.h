//
// Created by mecha on 06.10.2021.
//

#pragma once

#include "RenderModel.h"

#include <ecs/Components.h>

#include "geometry/MeshComponent.h"
#include "geometry/Lines.h"

#include "shader/BaseShader.h"

#include <graphics/transform/TransformComponents.h>
#include <platform/graphics/RenderCommands.h>

#define INSTANCE_COUNT_LIMIT 128

using namespace engine::shader;

namespace engine {

    class Renderer {

    public:
        Renderer(const Ref<BaseShaderProgram>& shaderProgram, const DrawType& drawType = TRIANGLE)
        : shaderProgram(shaderProgram), drawType(drawType) {
            create();
        }

        ~Renderer() {
            release();
        }

    public:
        inline void setDrawType(const DrawType& drawType) {
            this->drawType = drawType;
        }

    protected:
        void create();
        VRenderModel& createRenderModel(const uint32_t& vertexCount);
        VIRenderModel& createRenderModel(const uint32_t& vertexCount, const uint32_t& indexCount);
        void release();
        template<typename T>
        bool validate(VRenderModel &renderModel, VertexDataComponent<T>& vertexDataComponent);
        template<typename T>
        bool validate(VIRenderModel &renderModel, BaseMeshComponent<T>& meshComponent);

    protected:
        std::vector<VRenderModel> vRenderModels;
        std::vector<VIRenderModel> viRenderModels;
        Ref<BaseShaderProgram> shaderProgram;
        DrawType drawType;
    };

    class BatchRenderer : public Renderer {

    public:
        BatchRenderer(const Ref<BaseShaderProgram>& shaderProgram, const DrawType& drawType = TRIANGLE)
        : Renderer(shaderProgram, drawType) {}

    public:
        template<typename T>
        void renderV(entt::registry &registry);
        template<typename T>
        void renderVI(entt::registry &registry);
    };

    class InstanceRenderer : public Renderer {

    public:
        InstanceRenderer(const Ref<BaseShaderProgram>& shaderProgram, const DrawType& drawType = TRIANGLE)
        : Renderer(shaderProgram, drawType) {}

    public:
        template<typename T>
        void renderV(entt::registry &registry);
        template<typename T>
        void renderVI(entt::registry &registry);
    };

    class VRenderer {

    public:
        VRenderer(const Ref<BaseShaderProgram>& shaderProgram)
        : shaderProgram(shaderProgram) {
            create();
        }

        ~VRenderer() {
            destroy();
        }

    public:
        template<typename T>
        void render(const Entity& entity);

    private:
        void create();
        void destroy();

    private:
        VRenderModel vRenderModel = { 0, DEFAULT_VERTEX_COUNT };
        Ref<BaseShaderProgram> shaderProgram;

    };

    class VIRenderer {

    public:
        VIRenderer(const Ref<BaseShaderProgram>& shaderProgram)
        : shaderProgram(shaderProgram) {
            create();
        }

        ~VIRenderer() {
            destroy();
        }

    public:
        template<typename T>
        void render(const Entity& entity);

    private:
        void create();
        void destroy();

    private:
        VIRenderModel viRenderModel = { 0, DEFAULT_VERTEX_COUNT, DEFAULT_INDEX_COUNT };
        Ref<BaseShaderProgram> shaderProgram;
    };

    template<typename T>
    void BatchRenderer::renderV(entt::registry &registry) {
        if (!shaderProgram->isReady()) return;

        shaderProgram->start();
        shaderProgram->update(registry);

        auto entities = registry.view<Transform3dComponent, VertexDataComponent<InstanceVertex<T>>>();

        uint32_t nextRenderModelId = 0;
        for (auto [entity, transform, vertexDataComponent] : entities.each()) {
            if (drawType != vertexDataComponent.drawType) continue;

            vertexDataComponent.renderModelId += nextRenderModelId;
            auto& renderModel = vRenderModels[vertexDataComponent.renderModelId];
            if (!validate<InstanceVertex<T>>(renderModel, vertexDataComponent)) {
                nextRenderModelId++;
            }
        }

        for (auto& renderModel : vRenderModels) {
            uint32_t totalVertexCount = 0;
            uint32_t i = 0;
            for (auto [entity, transform, vertexDataComponent] : entities.each()) {
                if (renderModel.id != vertexDataComponent.renderModelId || drawType != vertexDataComponent.drawType) {
                    i++; // shift instance id
                    continue;
                }

                tryUploadInstance(i, vertexDataComponent, totalVertexCount, renderModel);
                shaderProgram->getVShader().setUniformArrayElement(i++, transform);
                if (i > INSTANCE_COUNT_LIMIT) {
                    renderModel.vao.bind();
                    drawV(drawType, totalVertexCount);
                    i = 0;
                    totalVertexCount = 0;
                }
            }

            if (i > 0 && totalVertexCount > 0) {
                renderModel.vao.bind();
                drawV(drawType, totalVertexCount);
            }
            resetCounts(renderModel);
        }

        shaderProgram->stop();
    }

    template<typename T>
    void BatchRenderer::renderVI(entt::registry &registry) {
        if (!shaderProgram->isReady()) return;

        shaderProgram->start();
        shaderProgram->update(registry);

        auto entities = registry.view<Transform3dComponent, BaseMeshComponent<InstanceVertex<T>>>();

        uint32_t nextRenderModelId = 0;
        for (auto [entity, transform, mesh] : entities.each()) {
            if (drawType != mesh.drawType) continue;

            mesh.renderModelId += nextRenderModelId;
            auto& renderModel = viRenderModels[mesh.renderModelId];
            if (!validate<InstanceVertex<T>>(renderModel, mesh)) {
                nextRenderModelId++;
            }
        }

        for (auto& renderModel : viRenderModels) {
            uint32_t totalVertexCount = 0;
            uint32_t totalIndexCount = 0;
            uint32_t i = 0;
            for (auto [entity, transform, mesh] : entities.each()) {
                if (renderModel.id != mesh.renderModelId || drawType != mesh.drawType) {
                    i++; // shift instance id
                    continue;
                }

                tryUploadMeshInstance(i, mesh, totalVertexCount, totalIndexCount, renderModel);
                shaderProgram->getVShader().setUniformArrayElement(i++, transform);
                if (i > INSTANCE_COUNT_LIMIT) {
                    renderModel.vao.bind();
                    drawVI(drawType, totalIndexCount);
                    i = 0;
                    totalVertexCount = 0;
                    totalIndexCount = 0;
                }
            }

            if (i > 0 && totalIndexCount > 0) {
                renderModel.vao.bind();
                drawVI(drawType, totalIndexCount);
            }
            resetCounts(renderModel);
        }

        shaderProgram->stop();
    }

    template<typename T>
    void InstanceRenderer::renderV(entt::registry& registry) {
        if (!shaderProgram->isReady()) return;

        shaderProgram->start();
        shaderProgram->update(registry);

        auto entities = registry.view<Transform3dComponent, VertexDataComponent<T>>();
        uint32_t totalVertexCount = 0;
        uint32_t i = 0;
        uint32_t renderModelId = 0;
        bool renderModelReady = false;
        for (auto [entity, transform, vertexDataComponent] : entities.each()) {
            if (drawType != vertexDataComponent.drawType) {
                i++; // shift instanceId
                continue;
            }

            if (!renderModelReady) {
                auto& oldRenderModel = vRenderModels[vertexDataComponent.renderModelId];
                resetCounts(oldRenderModel);
                validate(oldRenderModel, vertexDataComponent);
                renderModelId = vertexDataComponent.renderModelId;

                auto& renderModel = vRenderModels[vertexDataComponent.renderModelId];
                tryUpload(vertexDataComponent, totalVertexCount, renderModel);

                renderModelReady = true;
            }

            shaderProgram->getVShader().setUniformArrayElement(i++, transform);
            // if transform count is out of limit, then draw current instances and repeat iteration!
            auto& renderModel = vRenderModels[renderModelId];
            if (i > INSTANCE_COUNT_LIMIT) {
                renderModel.vao.bind();
                drawV(drawType, totalVertexCount, i);
                i = 0;
            }
        }

        auto& renderModel = viRenderModels[renderModelId];
        if (i > 0 && totalVertexCount > 0) {
            renderModel.vao.bind();
            drawV(drawType, totalVertexCount, i);
        }

        shaderProgram->stop();
    }

    template<typename T>
    void InstanceRenderer::renderVI(entt::registry& registry) {
        if (!shaderProgram->isReady()) return;

        shaderProgram->start();
        shaderProgram->update(registry);

        auto entities = registry.view<Transform3dComponent, BaseMeshComponent<T>>();
        uint32_t totalVertexCount = 0;
        uint32_t totalIndexCount = 0;
        uint32_t i = 0;
        uint32_t renderModelId = 0;
        bool renderModelReady = false;
        for (auto [entity, transform, mesh] : entities.each()) {
            if (drawType != mesh.drawType) {
                i++; // next instanceId
                continue;
            }

            if (!renderModelReady) {
                auto& oldRenderModel = viRenderModels[mesh.renderModelId];
                resetCounts(oldRenderModel);
                validate(oldRenderModel, mesh);
                renderModelId = mesh.renderModelId;

                auto& renderModel = viRenderModels[mesh.renderModelId];
                tryUpload(mesh, totalVertexCount, totalIndexCount, renderModel);

                renderModelReady = true;
            }

            shaderProgram->getVShader().setUniformArrayElement(i++, transform);
            // if transform count is out of limit, then draw current instances and repeat iteration!
            auto& renderModel = viRenderModels[renderModelId];
            if (i > INSTANCE_COUNT_LIMIT) {
                renderModel.vao.bind();
                drawVI(drawType, totalIndexCount, i);
                i = 0;
            }
        }

        auto& renderModel = viRenderModels[renderModelId];
        if (i > 0 && totalIndexCount > 0) {
            renderModel.vao.bind();
            drawVI(drawType, totalIndexCount, i);
        }

        shaderProgram->stop();
    }

    template<typename T>
    void VRenderer::render(const Entity &entity) {
        auto vertexDataComponent = entity.getPtr<VertexDataComponent<T>>();
        if (!shaderProgram->isReady() || !vertexDataComponent) return;

        shaderProgram->start();
        shaderProgram->update(entity);

        uint32_t totalVertexCount = 0;
        tryUpload(*vertexDataComponent, totalVertexCount, vRenderModel);
        // try to upload transform
        auto transform = entity.getPtr<Transform3dComponent>();
        if (transform) {
            shaderProgram->getVShader().setUniform(*transform);
        }

        vRenderModel.vao.bind();
        drawV(vertexDataComponent->drawType, totalVertexCount);

        shaderProgram->stop();
    }

    template<typename T>
    void VIRenderer::render(const Entity &entity) {
        auto mesh = entity.getPtr<BaseMeshComponent<T>>();
        if (!shaderProgram->isReady() || !mesh) return;

        shaderProgram->start();
        shaderProgram->update(entity);

        uint32_t totalIndexCount = 0;
        uint32_t totalVertexCount = 0;
        tryUpload(*mesh, totalVertexCount, totalIndexCount, viRenderModel);
        // try to upload transform
        auto transform = entity.getPtr<Transform3dComponent>();
        if (transform) {
            shaderProgram->getVShader().setUniform(*transform);
        }

        viRenderModel.vao.bind();
        drawVI(mesh->drawType, totalIndexCount);

        shaderProgram->stop();
    }

    template<typename T>
    bool Renderer::validate(VRenderModel &renderModel, VertexDataComponent<T>& vertexDataComponent) {
        if (!hasCapacity(renderModel, vertexDataComponent)) {
            auto vertexCount = vertexDataComponent.vertexData.vertexCount > DEFAULT_VERTEX_COUNT
                               ? vertexDataComponent.vertexData.vertexCount * 3 : DEFAULT_VERTEX_COUNT;
            auto& newRenderModel = createRenderModel(vertexCount);
            vertexDataComponent.renderModelId = newRenderModel.id;
            increaseCounts(newRenderModel, vertexDataComponent);
            return false;
        }
        increaseCounts(renderModel, vertexDataComponent);
        return true;
    }

    template<typename T>
    bool Renderer::validate(VIRenderModel &renderModel, BaseMeshComponent<T>& meshComponent) {
        if ((meshComponent.totalIndexCount == 0 || meshComponent.totalVertexCount == 0) && meshComponent.meshCount > 0) {
            updateCounts(meshComponent);
        }
        if (!hasCapacity(renderModel, meshComponent)) {
            auto vertexCount = meshComponent.totalVertexCount > DEFAULT_VERTEX_COUNT ? meshComponent.totalVertexCount * 3 : DEFAULT_VERTEX_COUNT;
            auto indexCount = meshComponent.totalIndexCount > DEFAULT_INDEX_COUNT ? meshComponent.totalIndexCount * 3 : DEFAULT_INDEX_COUNT;
            auto& newRenderModel = createRenderModel(vertexCount, indexCount);
            meshComponent.renderModelId = newRenderModel.id;
            increaseCounts(newRenderModel, meshComponent);
            return false;
        }
        increaseCounts(renderModel, meshComponent);
        return true;
    }

}
