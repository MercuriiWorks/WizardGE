//
// Created by mecha on 04.10.2021.
//

#include "FLLayer.h"

namespace fairy {

    void FLLayer::create() {
        auto graphicsFactory = app->getGraphicsFactory();
        auto objPreviewShader = graphicsFactory->newShader(engine::ShaderProps {
            "obj",
            "v_obj",
            "f_obj",
            EDITOR_RES_PATH
        });
        auto objRenderer = engine::createRef<engine::Renderer>(app->getGraphicsFactory());

        engine::FramebufferSpecification framebufferSpecification;
        framebufferSpecification.attachmentSpecification = {
                engine::FramebufferTextureFormat::RGBA8,
                engine::FramebufferTextureFormat::RED_INTEGER,
                engine::FramebufferTextureFormat::Depth
        };
        framebufferSpecification.width = 1920;
        framebufferSpecification.height = 1080;
        objRenderer->updateFboSpecs(framebufferSpecification);

        objRenderer->prepare(objPreviewShader);

        _objPreview = engine::createRef<engine::MeshLayout>(
                engine::ImageLayoutProps {"Object Preview"},
                objRenderer
        );

        auto objTransformMat = engine::TransformMatrix3d {
            "transform",
            { 2.5, 0, 12 },
            {135, 0, 0},
            {0.5, 0.5, 0.5}
        };
        auto objTransform = engine::createRef<engine::TransformComponent3d>(objTransformMat);
        objTransform->applyChanges();

        _objPreview->setTransform(objTransform);

        auto objCamera = new engine::Camera3d {
            "camera",
            engine::ViewMatrix3d {
                "view3d"
                },
                engine::PerspectiveMatrix {
                "projection3d",
                app->getAspectRatio()
            }
        };
        auto objCameraController = engine::createRef<engine::Camera3dController>(objCamera);
        objCameraController->applyChanges();
        _objPreview->setCameraController(objCameraController);

        _scenePreviewCallback = new FLLayer::ScenePreviewCallback(*this);
        _imagePreviewCallback = new FLLayer::ImagePreviewCallback(*this);

        _scenePreview.setCallback(_scenePreviewCallback);
        _imagePreview.setCallback(_imagePreviewCallback);
        _assetBrowser->setCallback(this);

        auto humanShaderProps = engine::ShaderProps {
            "shape2d",
            "v_shape2d",
            "f_shape2d"
        };

        app->loadShader(humanShaderProps);
        app->loadTexture("demo.png");

        auto humanMesh = app->loadObj("human");
        humanMesh->applyChanges();

        auto humanTransform = engine::TransformComponent3d {
            "transform",
            { 2.5, 0, 12 },
            {135, 0, 0},
            {0.5, 0.5, 0.5}
        };
        humanTransform.applyChanges();

        auto humanTexture = engine::TextureComponent {
            "diffuseSampler",
            0
        };
        humanTexture.applyChanges();

        _humanEntity = app->activeScene->createEntity("Human");
        _humanEntity.addComponent<engine::MeshComponent>(*humanMesh);
        _humanEntity.addComponent<engine::TransformComponent3d>(humanTransform);
        _humanEntity.addComponent<engine::TextureComponent>(humanTexture);
    }

    void FLLayer::destroy() {
        _scenePreview.removeCallback();
        _imagePreview.removeCallback();
        delete _imagePreviewCallback;
        delete _scenePreviewCallback;
    }

    void FLLayer::onPrepare() {
        CLIENT_INFO("onPrepare()");
        ImGuiLayer::onPrepare();
        app->closeKeyPressed = engine::KeyCode::Escape;
        app->sceneCameraController->bind(engine::KeyCode::W, engine::MoveType::UP);
        app->sceneCameraController->bind(engine::KeyCode::A, engine::MoveType::LEFT);
        app->sceneCameraController->bind(engine::KeyCode::S, engine::MoveType::DOWN);
        app->sceneCameraController->bind(engine::KeyCode::D, engine::MoveType::RIGHT);
        app->sceneCameraController->bind(engine::KeyCode::Q, engine::RotateType::LEFT_Z);
        app->sceneCameraController->bind(engine::KeyCode::E, engine::RotateType::RIGHT_Z);
        app->sceneCameraController->bind(engine::KeyCode::Z, engine::ZoomType::IN);
        app->sceneCameraController->bind(engine::KeyCode::X, engine::ZoomType::OUT);
        app->sceneCameraController->setPosition({0, 0, -1});
        app->sceneCameraController->applyChanges();

        _scenePreview.setTextureId(app->activeScene->getTextureId());
        _scenePreview.setClosable(false);

        _imagePreview.hide();
        _objPreview->hide();

        const auto& objCameraController = _objPreview->getCameraController();
        objCameraController->bind(engine::KeyCode::W, engine::MoveType::UP);
        objCameraController->bind(engine::KeyCode::A, engine::MoveType::LEFT);
        objCameraController->bind(engine::KeyCode::S, engine::MoveType::DOWN);
        objCameraController->bind(engine::KeyCode::D, engine::MoveType::RIGHT);
        objCameraController->bind(engine::KeyCode::Q, engine::RotateType::LEFT_Z);
        objCameraController->bind(engine::KeyCode::E, engine::RotateType::RIGHT_Z);
        objCameraController->bind(engine::KeyCode::Z, engine::ZoomType::IN);
        objCameraController->bind(engine::KeyCode::X, engine::ZoomType::OUT);
        objCameraController->setPosition({0, 0, -1});
        objCameraController->applyChanges();
    }

    void FLLayer::onRender(engine::Time dt) {
        _scenePreview.onUpdate(dt);
        _sceneHierarchy.onUpdate(dt);
        _assetBrowser->onUpdate(dt);
        _imagePreview.onUpdate(dt);
        _objPreview->onUpdate(dt);
    }

    void FLLayer::onUpdate(engine::Time dt) {
        ImGuiLayer::onUpdate(dt);

//        if (_humanEntity.hasComponent<engine::TransformComponent3d>()) {
//            auto& transform = _humanEntity.getComponent<engine::TransformComponent3d>().transformMatrix;
//
//            transform.rotation.y += 0.0001f / dt;
//            transform.applyChanges();
//        }
    }

    void FLLayer::onKeyPressed(engine::KeyCode keyCode) {
        Layer::onKeyPressed(keyCode);

        _objPreview->onKeyPressed(keyCode);

        if (_scenePreview.isFocused()) {
            app->sceneCameraController->onKeyPressed(keyCode);
        }

        if (keyCode == engine::KeyCode::D1) {
            app->setPolygonMode(engine::PolygonMode::POINT);
        }

        if (keyCode == engine::KeyCode::D2) {
            app->setPolygonMode(engine::PolygonMode::LINE);
        }

        if (keyCode == engine::KeyCode::D3) {
            app->setPolygonMode(engine::PolygonMode::FILL);
        }

        if (keyCode == engine::KeyCode::D0) {
            app->fpsTimer.setMaxFps(15);
        }

        if (keyCode == engine::KeyCode::D9) {
            app->fpsTimer.setMaxFps(30);
        }

        if (keyCode == engine::KeyCode::D8) {
            app->fpsTimer.setMaxFps(60);
        }

        if (keyCode == engine::KeyCode::D7) {
            app->fpsTimer.setMaxFps(app->getRefreshRate());
        }
    }

    void FLLayer::onKeyHold(engine::KeyCode keyCode) {
        Layer::onKeyHold(keyCode);
        _objPreview->onKeyHold(keyCode);

        if (_scenePreview.isFocused()) {
            app->sceneCameraController->onKeyHold(keyCode);
        }
    }

    void FLLayer::onKeyReleased(engine::KeyCode keyCode) {
        Layer::onKeyReleased(keyCode);
        _objPreview->onKeyReleased(keyCode);

        if (_scenePreview.isFocused()) {
            app->sceneCameraController->onKeyReleased(keyCode);
        }
    }

    void FLLayer::onKeyTyped(engine::KeyCode keyCode) {
        Layer::onKeyTyped(keyCode);
        _objPreview->onKeyTyped(keyCode);

        if (_scenePreview.isFocused()) {
            app->sceneCameraController->onKeyTyped(keyCode);
        }
    }

    void FLLayer::onMousePressed(engine::MouseCode mouseCode) {
        _scenePreview.onMousePressed(mouseCode);
    }

    void FLLayer::onMouseRelease(engine::MouseCode mouseCode) {
        _scenePreview.onMouseRelease(mouseCode);
    }

    void FLLayer::onPngOpen(const std::string &fileName) {
        ENGINE_INFO("onPngOpen({0})", fileName);
        auto imageName = fileName + ".png";
        _imagePreview.load(imageName);
        _imagePreview.show();
    }

    void FLLayer::onJpgOpen(const std::string &fileName) {
        ENGINE_INFO("onJpgOpen({0})", fileName);
        auto imageName = fileName + ".jpg";
        _imagePreview.load(imageName);
        _imagePreview.show();
    }

    void FLLayer::onObjOpen(const std::string &fileName) {
        ENGINE_INFO("onObjOpen({0})", fileName);
        auto objMesh = app->loadObj(fileName);
        objMesh->applyChanges();
        objMesh->updateCounts();
        _objPreview->setMesh(objMesh);
        _objPreview->show();
    }

    void FLLayer::onGlslOpen(const std::string &fileName) {
        ENGINE_INFO("onGlslOpen({0})", fileName);
    }

    void FLLayer::ImagePreviewCallback::onImageResized(const uint32_t &width, const uint32_t &height) {
        // do nothing.
    }

    void FLLayer::ScenePreviewCallback::onImageResized(const uint32_t &width, const uint32_t &height) {
        _parent.app->onWindowResized(width, height);
    }

    void FLLayer::onImport(const char *filter) {
        auto importPath = app->fileDialog->getImportPath(filter);
        ENGINE_INFO("Import path : {0}", importPath);
    }

    void FLLayer::onExport(const char *filter) {
        auto exportPath = app->fileDialog->getExportPath(filter);
        ENGINE_INFO("Export path : {0}", exportPath);
    }

}