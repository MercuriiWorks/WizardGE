//
// Created by mecha on 07.10.2021.
//

#pragma once

#include "ImageLayout.h"

#include <graphics/core/Renderer.h>
#include <graphics/core/frame/FrameController.h>
#include <graphics/camera/CameraController.h>
#include <graphics/core/io/ObjFile.h>

namespace engine {

    class MeshLayout : public ImageLayout, public ImageLayoutCallback {

    public:
        MeshLayout(
                const ImageLayoutProps &props,
                const Ref<Renderer> &renderer,
                const Ref<FrameController> &frameController,
                const Ref<CameraController> &cameraController
        ) : ImageLayout(props),
        _renderer(renderer), _frameController(frameController), _cameraController(cameraController) {
            create();
        }

        ~MeshLayout() override {
            destroy();
        }

    public:
        void onRender(const Time &dt) override;

    public:
        inline const Ref<CameraController>& getCameraController() {
            return _cameraController;
        }

        inline void setEntity(const Entity& entity) {
            _entity = entity;
        }

        inline void setMesh(const ObjMeshComponent &objMeshComponent) {
            _entity.set<ObjMeshComponent>(objMeshComponent);
        }

        inline void setRotateEntity(const bool &enabled) {
            _shouldRotateEntity = enabled;
        }

    public:
        void onImageResized(const uint32_t &width, const uint32_t &height) override;

        void onKeyPressed(KeyCode keyCode);
        void onKeyHold(KeyCode keyCode);
        void onKeyReleased(KeyCode keyCode);
        void onKeyTyped(KeyCode keyCode);

    private:
        void create();
        void destroy();
        void rotateEntity(const Time &dt);

    private:
        Ref<Renderer> _renderer;
        Ref<FrameController> _frameController;
        Ref<CameraController> _cameraController;
        Entity _entity;
        bool _shouldRotateEntity = true;

    };

}
