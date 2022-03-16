//
// Created by mecha on 01.11.2021.
//

#include <platform/graphics/RenderCommands.h>
#include <glad/glad.h>

namespace engine {

    void drawQuads(const uint32_t &indexCount) {
        ENGINE_INFO("drawQuadsIndices()");
        glDrawElements(GL_QUADS, (GLsizei) indexCount, GL_UNSIGNED_INT, nullptr);
    }

    void drawTriangles(const uint32_t &indexCount) {
        ENGINE_INFO("drawElements(indexCount : {0})", indexCount);
        glDrawElements(GL_TRIANGLES, (GLsizei) indexCount, GL_UNSIGNED_INT, nullptr);
    }

    void drawTriangles(const uint32_t &indexCount, const uint32_t &instanceCount) {
        ENGINE_INFO("drawElements(indexCount : {0}, instanceCount : {1})", indexCount, instanceCount);
        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei) indexCount, GL_UNSIGNED_INT, nullptr, (GLsizei) instanceCount);
    }

    void enableCulling() {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    void disableCulling() {
        glDisable(GL_CULL_FACE);
    }

    void setPolygonMode(const uint8_t &mode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT + mode);
    }

    const unsigned char* getAPIName() {
        return 0;
    }

    const unsigned char *getVendorName() {
        return glGetString(GL_VENDOR);
    }

    const unsigned char *getRendererName() {
        return glGetString(GL_RENDERER);
    }

    const unsigned char *getVersion() {
        return glGetString(GL_VERSION);
    }

    void enableMSAA() {
        glEnable(GL_MULTISAMPLE);
    }

    void disableMSAA() {
        glDisable(GL_MULTISAMPLE);
    }

    PolygonMode RenderCommands::activePolygonMode = FILL;
    bool RenderCommands::isCullingEnabled = false;

    void RenderCommands::toggleCulling() {
        isCullingEnabled = !isCullingEnabled;
        if (isCullingEnabled) {
            enableCulling();
        } else {
            disableCulling();
        }
    }

    void RenderCommands::setCulling(const CullingComponent &culling) {
        if (culling.enabled != isCullingEnabled) {
            toggleCulling();
        }
    }

    void RenderCommands::trySetPolygonMode(const PolygonMode &polygonMode) {
        if (polygonMode != activePolygonMode) {
            activePolygonMode = polygonMode;
            setPolygonMode(polygonMode);
        }
    }

    void RenderCommands::logApiInfo() {
        ENGINE_INFO("Graphics API : {0}", getAPIName());
        ENGINE_INFO("Version : {0}", getVersion());
        ENGINE_INFO("Vendor : {0}", getVendorName());
        ENGINE_INFO("Renderer : {0}", getRendererName());
    }

}