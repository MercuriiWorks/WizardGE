//
// Created by mecha on 04.10.2021.
//

#include <Activity.h>

#include <core/FileExtensions.h>
#include <graphics/light/Light.h>
#include <graphics/camera/CameraShaderScript.h>
#include <graphics/light/LightShaderScript.h>
#include <graphics/GraphicsObject.h>
#include <scripting/ScriptBuilder.h>

#include <graphics/skybox/Skybox.h>
#include <graphics/core/geometry/Quad.h>
#include <graphics/core/geometry/Point.h>

#include <imgui.h>

#define BIND_KEY_PRESS(keycode, action) engine::core::Application::get().eventRegistry.onKeyPressedMap[keycode] = { [this](KeyCode keyCode) { action }}

namespace studio {

    using namespace engine::shader;
    using namespace engine::io;

    void Activity::create() {
        createAssetBrowser();

        auto vObjShader = BaseShader({ camera3dScript() });
        auto fObjShader = BaseShader({ phongLightScript() });
        auto objShader = BaseShaderProgram(
                ShaderProps {
                        "obj",
                        "v_obj.glsl",
                        "f_obj.glsl",
                        EDITOR_SHADERS_PATH
                },
                vObjShader,
                fObjShader
        );
        auto objRenderer = createRef<VIRenderer>(objShader);

        auto objCamera = Camera3D {
                "obj",
                app->getAspectRatio(),
                editorScene.get()
        };
        auto objCameraController = createRef<Camera3dController>(
                "ObjCameraController",
                objCamera
        );

        auto objTransform = Transform3dComponent(
                { 2.5, 0, 12 },
                { 135, 0, 0 },
                { 0.5, 0.5, 0.5 }
        );
        objCamera.add<Transform3dComponent>(objTransform);

        auto objPhongLight = PhongLightComponent();
        objPhongLight.position.value = { 25, 25, -25, 0 };
        objPhongLight.ambient.value = { 0.05, 0.05, 0.05, 0 };
        objPhongLight.diffuse.value = { 0.4, 0.4, 0.4, 0 };
        objPhongLight.specular.value = { 0.4, 0.4, 0.4, 0 };
        objCamera.add<PhongLightComponent>(objPhongLight);

        auto objFrame = createRef<FrameBuffer>();
        FrameBufferFormat objFrameFormat;
        objFrameFormat.colorAttachments = {
                { ColorFormat::RGBA8 }
        };
        objFrameFormat.depthStencilAttachment = { DepthStencilFormat::DEPTH24STENCIL8 };
        objFrameFormat.width = app->getWindow()->getWidth();
        objFrameFormat.height = app->getWindow()->getHeight();
        objFrameFormat.samples = 1;
        objFrame->updateFormat(objFrameFormat);

        _objPreview = createRef<MeshLayout>(
                ImageLayoutProps {"Object Preview"},
                objRenderer,
                objFrame,
                objCameraController
        );

        _objPreview->setEntity(objCamera);

        _scenePreviewCallback = new Activity::ScenePreviewCallback(*this);
        _imagePreviewCallback = new Activity::ImagePreviewCallback(*this);

        sceneViewport.setCallback(_scenePreviewCallback);
        sceneViewport.setDragDropCallback(this);
        screenViewport.setCallback(_scenePreviewCallback);
        screenViewport.setDragDropCallback(this);

        _texturePreview.setCallback(_imagePreviewCallback);
        _assetBrowser->setCallback(this);

        createTest();
    }

    void Activity::createTest() {
        auto scene1 = createRef<Scene>();
        app->setActiveScene(scene1);

        auto activeSceneCamera = Camera3D {
                "SceneCamera",
                app->getAspectRatio(),
                scene1.get()
        };

        activeSceneCameraController = createRef<Camera3dController>(
                "ActiveSceneCameraController",
                activeSceneCamera
        );

        u32 skyboxId = TextureBuffer::load(
                {
                        { "skybox/front.jpg", TextureFaceType::FRONT },
                        { "skybox/back.jpg", TextureFaceType::BACK },
                        { "skybox/left.jpg", TextureFaceType::LEFT },
                        { "skybox/right.jpg", TextureFaceType::RIGHT },
                        { "skybox/top.jpg", TextureFaceType::TOP },
                        { "skybox/bottom.jpg", TextureFaceType::BOTTOM },
                }
        );

        scene1->setSkybox(SkyboxCube(
                "Skybox",
                scene1.get(),
                CubeMapTextureComponent(skyboxId, TextureBuffer::getTypeId(TextureType::CUBE_MAP))
        ));

        auto points = Entity("Points", scene1.get());
        auto pointsComponent = Points {
            new PointVertex[4] {
                { { -0.5, 0.5 }, { 1, 0, 0 } },
                { { 0.5, 0.5 }, { 0, 1, 0 } },
                { { 0.5, -0.5 }, { 0, 0, 1 }},
                { { -0.5, -0.5 }, { 1, 1, 0 }}
            },
            4
        };
        points.add<Points>(pointsComponent);

//        Object3d(
//                scene1.get(),
//                "Sponza",
//                transform3d(),
//                GET_MESH_COMPONENT(BatchVertex<Vertex3d>, "assets/obj/sponza.obj")
//        );

        Object3d(
                scene1.get(),
                "Quad",
                Transform3dComponent(),
                BatchQuad()
        );
    }

    void Activity::destroy() {
        delete _imagePreviewCallback;
        delete _scenePreviewCallback;
    }

    void Activity::onPrepare() {
        EDITOR_INFO("onPrepare()");
        ImGuiLayer::onPrepare();

        activeSceneCameraController->bind(KeyCode::W, MoveType::UP);
        activeSceneCameraController->bind(KeyCode::A, MoveType::LEFT);
        activeSceneCameraController->bind(KeyCode::S, MoveType::DOWN);
        activeSceneCameraController->bind(KeyCode::D, MoveType::RIGHT);
        activeSceneCameraController->bind(KeyCode::Q, RotateType::LEFT_Z);
        activeSceneCameraController->bind(KeyCode::E, RotateType::RIGHT_Z);
        activeSceneCameraController->bind(KeyCode::Z, ZoomType::ZOOM_IN);
        activeSceneCameraController->bind(KeyCode::X, ZoomType::ZOOM_OUT);

        _texturePreview.hide();
        _objPreview->hide();

        const auto& objCameraController = _objPreview->getCameraController();
        objCameraController->bind(KeyCode::W, MoveType::UP);
        objCameraController->bind(KeyCode::A, MoveType::LEFT);
        objCameraController->bind(KeyCode::S, MoveType::DOWN);
        objCameraController->bind(KeyCode::D, MoveType::RIGHT);
        objCameraController->bind(KeyCode::Q, RotateType::LEFT_Z);
        objCameraController->bind(KeyCode::E, RotateType::RIGHT_Z);
        objCameraController->bind(KeyCode::Z, ZoomType::ZOOM_IN);
        objCameraController->bind(KeyCode::X, ZoomType::ZOOM_OUT);

        _sceneHierarchy.setCallback(this);

        // switch between scenes : Scene0, Scene1
        BIND_KEY_PRESS(KeyCode::D0,
                       app->setActiveScene(0);
                        _sceneHierarchy.setScene(app->scenes[0]);
        );
        BIND_KEY_PRESS(KeyCode::D1,
                       app->setActiveScene(1);
                        _sceneHierarchy.setScene(app->scenes[1]);
        );
        // switch between fullscreen/windowed modes
        BIND_KEY_PRESS(KeyCode::F, if (app->input->isKeyPressed(KeyCode::LeftControl)) {
            app->getWindow()->enableFullScreen();
        });
        BIND_KEY_PRESS(KeyCode::Escape, app->getWindow()->disableFullScreen(););
        BIND_KEY_PRESS(KeyCode::M, app->setSampleSize(8););
        BIND_KEY_PRESS(KeyCode::N, app->setSampleSize(1););

        setMSAA(true);
    }

    void Activity::onRender(engine::time::Time dt) {
        sceneViewport.onUpdate(dt);
        screenViewport.onUpdate(dt);
        _sceneHierarchy.onUpdate(dt);
        _assetBrowser->onUpdate(dt);
        _texturePreview.onUpdate(dt);
        _objPreview->onUpdate(dt);
    }

    void Activity::onUpdate(engine::time::Time dt) {
        ImGuiLayer::onUpdate(dt);
        activeSceneCameraController->setDeltaTime(dt);
        sceneViewport.setId(app->screenFrame->getColorAttachment(0).id);
        screenViewport.setId(app->screenFrame->getColorAttachment(0).id);
    }

    void Activity::onKeyPressed(KeyCode keyCode) {
        ImGuiLayer::onKeyPressed(keyCode);
        _objPreview->onKeyPressed(keyCode);
        if (sceneViewport.isFocused() || screenViewport.isFocused()) {
            activeSceneCameraController->onKeyPressed(keyCode);
        }
    }

    void Activity::onKeyHold(KeyCode keyCode) {
        ImGuiLayer::onKeyHold(keyCode);
        _objPreview->onKeyHold(keyCode);
        if (sceneViewport.isFocused() || screenViewport.isFocused()) {
            activeSceneCameraController->onKeyHold(keyCode);
        }
    }

    void Activity::onKeyReleased(KeyCode keyCode) {
        ImGuiLayer::onKeyReleased(keyCode);
        _objPreview->onKeyReleased(keyCode);
        if (sceneViewport.isFocused() || screenViewport.isFocused()) {
            activeSceneCameraController->onKeyReleased(keyCode);
        }
    }

    void Activity::onKeyTyped(KeyCode keyCode) {
        ImGuiLayer::onKeyTyped(keyCode);
        _objPreview->onKeyTyped(keyCode);
        if (sceneViewport.isFocused() || screenViewport.isFocused()) {
            activeSceneCameraController->onKeyTyped(keyCode);
        }
    }

    void Activity::onMousePressed(MouseCode mouseCode) {
        ImGuiLayer::onMousePressed(mouseCode);
        sceneViewport.onMousePressed(mouseCode);
        screenViewport.onMousePressed(mouseCode);
    }

    void Activity::onMouseRelease(MouseCode mouseCode) {
        ImGuiLayer::onMouseRelease(mouseCode);
        sceneViewport.onMouseRelease(mouseCode);
        screenViewport.onMouseRelease(mouseCode);
    }

    void Activity::onImageOpen(const std::string &fileName) {
        EDITOR_INFO("onImageOpen({0})", fileName);
        _texturePreview.setId(TextureBuffer::load(("assets/textures/" + fileName).c_str()));
        _texturePreview.show();
    }

    void Activity::onObjOpen(const std::string &fileName) {
        EDITOR_INFO("onObjOpen({0})", fileName);
        MeshSource<ModelVertex>::getMesh("assets/obj/" + fileName, {
            [this](const io::ModelMeshComponent& mesh) {
                _objPreview->setMesh(mesh);
                _objPreview->show();
            },
            [](const exception& exception) {
                EDITOR_EXCEPT(exception);
            },
            [](const ModelVertex& vertex) {
                return vertex;
            }
        });
    }

    void Activity::ImagePreviewCallback::onImageResized(const uint32_t &width, const uint32_t &height) {
        // do nothing.
    }

    void Activity::ScenePreviewCallback::onImageResized(const uint32_t &width, const uint32_t &height) {
        if (width == 0 || height == 0) return;

        _parent.app->activeSceneFrame->resize(width, height);
        _parent.app->screenFrame->resize(width, height);
        _parent.activeSceneCameraController->onWindowResized(width, height);
    }

    void Activity::onAssetImported(const std::string &assetPath) {
        EDITOR_INFO("onAssetImported() - {0}", assetPath);
    }

    void Activity::onAssetExported(const std::string &assetPath) {
        EDITOR_INFO("onAssetExported() - {0}", assetPath);
    }

    void Activity::onAssetRemoved(const std::string &assetPath) {
        EDITOR_INFO("onAssetRemoved() - {0}", assetPath);
    }

    void Activity::onEntityRemoved(const Entity &entity) {
        EDITOR_INFO("onEntityRemoved({0})", entity.getId());
        app->activeSceneFrame->bind();
        setDepthTest(true);
        setClearColor({0.2, 0.2, 0.2, 1});
        clearDepthBuffer();

        FrameBuffer::bindDefault();
        setDepthTest(false);
        clearColorBuffer();
    }

    void Activity::createAssetBrowser() {
        auto props = AssetBrowserProps {
                "Asset Browser",
                RUNTIME_ASSET_PATH
        };

        auto dirItem = AssetBrowserItem {
                "",
                TextureBuffer::load("editor/textures/dir_icon.png")
        };

        auto pngItem = AssetBrowserItem {
                PNG_EXT,
                TextureBuffer::load("editor/textures/png_icon.png")
        };

        auto jpgItem = AssetBrowserItem {
                JPG_EXT,
                TextureBuffer::load("editor/textures/jpg_icon.png")
        };

        auto glslItem = AssetBrowserItem {
                GLSL_EXT,
                TextureBuffer::load("editor/textures/glsl_icon.png")
        };

        auto objItem = AssetBrowserItem {
                OBJ_EXT,
                TextureBuffer::load("editor/textures/obj_icon.png")
        };

        auto ttfItem = AssetBrowserItem {
                TTF_EXT,
                TextureBuffer::load("editor/textures/ttf_icon.png")
        };

        auto cppItem = AssetBrowserItem {
                CPP_EXT,
                TextureBuffer::load("editor/textures/cpp_icon.png")
        };

        auto items = AssetBrowserItems<6> {
                dirItem,
                { pngItem, jpgItem, glslItem, objItem, ttfItem, cppItem }
        };

        auto fileDialog = app->createFileDialog();

        _assetBrowser = createRef<AssetBrowser>(props, items, fileDialog);
    }

    void Activity::onWindowClosed() {
        ImGuiLayer::onWindowClosed();
    }

    void Activity::onObjDragged(const std::string &fileName) {
        EDITOR_INFO("onObjDragged({0})", fileName);
        MeshSource<BatchVertex<Vertex3d>>::getMesh("assets/obj/" + fileName, {
                [this, &fileName](const BaseMeshComponent<BatchVertex<Vertex3d>>& mesh) {
                    Object3d {
                            app->activeScene.get(),
                            engine::filesystem::getFileName(fileName),
                            Transform3dComponent(),
                            mesh
                    };
                },
                [](const exception& exception) {
                    EDITOR_EXCEPT(exception);
                },
                [](const ModelVertex& vertex) {
                    return BatchVertex<Vertex3d> { vertex.position, vertex.uv, vertex.normal, 0 };
                }
        });

    }

    void Activity::onImageDragged(const std::string &fileName) {
        EDITOR_INFO("onImageDragged({0})", fileName);
    }
}