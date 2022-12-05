//
// Created by mecha on 11.04.2022.
//

#include <core/test_layer.h>
#include <graphics/camera/CameraShaderScript.h>

#define ROTATE_SURVIVAL_PACK 123

namespace test {

    void TestLayer::init() {
        // setup custom scene renderer
        auto batchShader = shader::BaseShaderProgram(
                io::ShaderProps {
                        "batch",
                        "v_batch.glsl",
                        "scene_phong.glsl",
                        ENGINE_SHADERS_PATH
                },
                BaseShader(),
                BaseShader(),
                { camera3dUboScript(), phongLightScript() }
        );

        auto instanceShader = shader::BaseShaderProgram(
                io::ShaderProps {
                        "instance",
                        "v_instance.glsl",
                        "scene_phong.glsl",
                        ENGINE_SHADERS_PATH
                },
                BaseShader(),
                BaseShader(),
                { camera3dUboScript(), phongLightScript() }
        );

        Ref<Renderer> batchRenderer = createRef<BatchRenderer<Vertex3d>>(batchShader);
        Ref<Renderer> instanceRenderer = createRef<InstanceRenderer<Vertex3d>>(instanceShader);
        auto entityHandler = [](ecs::Registry& registry,
                                ecs::entity_id entityId, u32 index, BaseShaderProgram& shader) {
//            ENGINE_INFO("entityHandler: entity_id: {0}, index: {1}", entityIdInt, index);
            // update polygon mode
            auto* pmc = registry.getComponent<PolygonModeComponent>(entityId);
            PolygonModes::setPolygonMode(pmc ? *pmc : PolygonModeComponent());
            // update culling mode
            auto* culling = registry.getComponent<CullingComponent>(entityId);
            Culling::setCulling(culling ? *culling : CullingComponent());
            // pass entity id to shader
            auto* uuid = registry.getComponent<UUIDComponent>(entityId);
            IntUniform uuidUniform = { "uuids", uuid->uuid };
            shader.setUniformArrayElement(index, uuidUniform);
            ENGINE_INFO("UUID: {0}", uuid->uuid);
            // update single material
            auto material = registry.getComponent<Material>(entityId);
            if (material) {
                MaterialShader(shader).setMaterial(index, material);
            }
        };

        batchRenderer->addEntityHandler(entityHandler);
        instanceRenderer->addEntityHandler(entityHandler);

        RenderSystem::sceneRenderers.emplace_back(batchRenderer);
        RenderSystem::sceneRenderers.emplace_back(instanceRenderer);

        // setup scene, entities, components

        auto scene = createRef<Scene>("Scene1");
        auto& app = engine::core::Application::get();
        app.setActiveScene(scene);

        mainCamera = Camera3D {
                "SceneCamera",
                app.getAspectRatio(),
                scene.get()
        };
        scene->setCamera(mainCamera);

        // setup skybox
//        app.setSkyCube(scene, "Skybox", {
//                { "assets/materials/skybox/front.jpg", TextureFaceType::FRONT },
//                { "assets/materials/skybox/back.jpg", TextureFaceType::BACK },
//                { "assets/materials/skybox/left.jpg", TextureFaceType::LEFT },
//                { "assets/materials/skybox/right.jpg", TextureFaceType::RIGHT },
//                { "assets/materials/skybox/top.jpg", TextureFaceType::TOP },
//                { "assets/materials/skybox/bottom.jpg", TextureFaceType::BOTTOM }
//        });

        // spawn random objects
//        math::random(-10, 10, 1, [this, &scene](int i, f32 r) {
//            Batch3d pack = Batch3d(&"SurvivalBackPack"[i], scene.get());
//            pack.getTransform().position = { r * i, r * i, r * i };
//            pack.getTransform().rotation = { r * i, r * i, r * i };
//            pack.applyTransform();
//            vec3f velocity = { 0.0005f, 0.0005f, 0.0005f };
//            velocity *= i%2 == 0 ? 1 : -1;
//            pack.add<Velocity>(velocity);
//            pack.add<SphereCollider>(pack.getTransform().position, 3.0f);
//            packs.emplace_back(pack);
//        });

//        math::random(-10, 10, 0, [this, &scene](int i, f32 r) {
//            Instance3d pack = Instance3d(&"SurvivalBackPackInstanced"[i], scene.get());
//            pack.getTransform().position = { r * i, r * i, r * i };
//            pack.getTransform().rotation = { r * i, r * i, r * i };
//            pack.applyTransform();
//            vec3f velocity = { 0.0005f, 0.0005f, 0.0005f };
//            velocity *= i%2 == 0 ? 1 : -1;
//            pack.add<Velocity>(velocity);
//            pack.add<SphereCollider>(pack.getTransform().position, 3.0f);
//            instancedPacks.emplace_back(pack);
//        });

        RenderSystem::sceneRenderers[0]->getShader().setInstancesPerDraw(4);

        // we need to flip textures as they will be loaded vice versa
        io::TextureFile::setFlipTexture(true);
        io::ModelFile<BatchVertex<Vertex3d>>::read(
                "assets/SamuraiHelmet/source/SamuraiHelmet.fbx.fbx",
                "assets/SamuraiHelmet/textures", {
            [this, &scene](const io::Model& model) {
                RUNTIME_INFO("ModelFile read: onSuccess");
                for (int i = 0; i < model.meshes.size(); i++) {
                    auto modelMesh = model.meshes[i];
                    BaseMeshComponent<BatchVertex<Vertex3d>> meshComponent;
                    meshComponent.mesh = modelMesh.toMesh<BatchVertex<Vertex3d>>([](const io::ModelVertex& modelVertex) {
                        return BatchVertex<Vertex3d> {
                                modelVertex.position,
                                modelVertex.uv,
                                modelVertex.normal,
                                modelVertex.tangent,
                                modelVertex.bitangent,
                                0
                        };
                    });
                    std::stringstream ss;
                    ss << "Entity_" << i;
                    Batch3d newEntity = Batch3d(scene.get());
                    newEntity.get<TagComponent>()->tag = ss.str();
                    newEntity.getTransform().position = { 1, 1, 1 };
                    newEntity.applyTransform();
                    newEntity.add<BaseMeshComponent<BatchVertex<Vertex3d>>>(meshComponent);
                    newEntity.add<Material>(modelMesh.material);
                    packs.emplace_back(newEntity);
                }
                RenderSystem::sceneRenderers[0]->createVIRenderModel(packs);
            },
            [](const exception& exception) {
                RUNTIME_INFO("ModelFile read: onError");
                RUNTIME_EXCEPT(exception);
            }
        });

        RenderSystem::sceneRenderers[1]->getShader().setInstancesPerDraw(4);

//        io::ModelFile<InstanceVertex<Vertex3d>>::read("assets/SamuraiHelmet/source/SamuraiHelmet.fbx.fbx", {
//                [this, &scene](const io::Model& model) {
//                    RUNTIME_INFO("ModelFile read: onSuccess");
//                    for (int i = 0; i < model.meshes.size(); i++) {
//                        auto modelMesh = model.meshes[i];
//                        BaseMeshComponent<InstanceVertex<Vertex3d>> meshComponent;
//                        meshComponent.mesh = modelMesh.toMesh<InstanceVertex<Vertex3d>>([](const io::ModelVertex& modelVertex) {
//                            return InstanceVertex<Vertex3d> {
//                                    modelVertex.position,
//                                    modelVertex.uv,
//                                    modelVertex.normal,
//                                    modelVertex.tangent,
//                                    modelVertex.bitangent
//                            };
//                        });
//                        Instance3d newEntity = Instance3d(&"Instanced_Entity"[i], scene.get());
//                        newEntity.getTransform().position = { 1, 1, 1 };
//                        newEntity.applyTransform();
//                        newEntity.add<BaseMeshComponent<InstanceVertex<Vertex3d>>>(meshComponent);
//                        newEntity.add<Material>(modelMesh.material);
//
//                        instancedPacks.emplace_back(newEntity);
//                    }
//                    RenderSystem::sceneRenderers[1]->createVIRenderModelInstanced(instancedPacks[0], instancedPacks);
//                },
//                [](const exception& exception) {
//                    RUNTIME_INFO("ModelFile read: onError");
//                    RUNTIME_EXCEPT(exception);
//                }
//        });

//        u32 materialTextures = TextureBuffer::loadArray({
//            "assets/materials/survival_pack/1001_albedo.jpg",
//            "assets/materials/survival_pack/1001_AO.jpg",
//            "assets/materials/survival_pack/1001_metallic.jpg",
//            "assets/materials/survival_pack/1001_normal.png",
//            "assets/materials/survival_pack/1001_roughness.jpg"
//        });
//        u32 textureArrayTypeId = TextureBuffer::getTypeId(TextureType::TEXTURE_2D_ARRAY);

//        ShaderScript shaderScript;
//        shaderScript.updateRegistry = [&materialTextures, &textureArrayTypeId](const BaseShaderProgram&,ecs::Registry&) {
//            if (materialTextures != invalidTextureId) {
//                ENGINE_INFO("MaterialTexture: id={0}, typeId={1}", materialTextures, textureArrayTypeId);
//                TextureBuffer::bind(materialTextures, textureArrayTypeId);
//                TextureBuffer::activate(0);
//            }
//        };
//        shaderScript.updateEntity = [](const BaseShaderProgram& shader, const ecs::Entity& entity) {};

//        batchRenderer->getShader().addScript(shaderScript);
//        instanceRenderer->getShader().addScript(shaderScript);

        // setup HDR env
        Application::get().setHdrEnvCube(scene, "assets/hdr/ice_lake.hdr");

//        Cube<BatchVertex<Vertex3d>> cube;
//        Object<BatchVertex<Vertex3d>> cubeObj {
//            "Cube", scene.get()
//        };
//        cubeObj.add<VertexDataComponent<BatchVertex<Vertex3d>>>(cube);

//        audio::MediaPlayer::loadStream(
//                "assets/audio/forest.wav",
//                {},
//                [](const audio::Source &source) {
//                    audio::MediaPlayer::setPlayedSource(source);
//                },
//                []() {
//                    audio::MediaPlayer::playStream();
//                }
//        );

        lights.emplace_back("L_Sun_1", scene.get());
        lights.emplace_back("L_Sun_2", scene.get());
        lights.emplace_back("L_Sun_3", scene.get());
        lights.emplace_back("L_Sun_4", scene.get());
        lights[0].getPosition() = { -10, 10, -10 };
        lights[1].getPosition() = { 10, 10, 10 };
        lights[2].getPosition() = { -10, 10, 10 };
        lights[3].getPosition() = { 10, 10, -10 };

        bool tcpClientCreated = tcp::Client::init(this, this, this);
        if (tcpClientCreated) {
            tcp::Client::connect("192.168.1.101", 54000);
        }

        bool udpClientCreated = udp::Client::init(this, this, this);
        if (udpClientCreated) {
            udp::Client::connect("192.168.1.101", 54000);
        }

        Visual::openDockspace = true;
        Visual::fullScreen = true;

        // disable screen rendering as we are using ImGui for scene rendering
        RenderSystem::enableScreenRenderer = false;

        ProjectsPanel::init();
        AssetBrowser::create(app.getNativeWindow(), { "AssetBrowser" });
        MaterialPanel::create(app.getNativeWindow());

        sceneViewport.show();
    }

    TestLayer::~TestLayer() {
        AssetBrowser::destroy();
        ProjectsPanel::destroy();
        Log::clear();
        tcp::Client::close();
        udp::Client::close();
    }

    void TestLayer::bindCamera() {
        KEY_PRESSED(KeyCode::W, mainCamera.applyMove(MoveType::UP););
        KEY_PRESSED(KeyCode::A, mainCamera.applyMove(MoveType::LEFT););
        KEY_PRESSED(KeyCode::S, mainCamera.applyMove(MoveType::DOWN););
        KEY_PRESSED(KeyCode::D, mainCamera.applyMove(MoveType::RIGHT););
        KEY_PRESSED(KeyCode::Q, mainCamera.applyRotate(RotateType::LEFT_Z););
        KEY_PRESSED(KeyCode::E, mainCamera.applyRotate(RotateType::RIGHT_Z););
        KEY_PRESSED(KeyCode::Z, mainCamera.applyZoom(ZoomType::ZOOM_IN););
        KEY_PRESSED(KeyCode::X, mainCamera.applyZoom(ZoomType::ZOOM_OUT););

        mainCamera.zoomSpeed = 10.0f;
        mainCamera.rotateSpeed = 0.5f;
        mainCamera.moveSpeed = 0.5f;
        mainCamera.distance = 10.0f;
    }

    void TestLayer::onPrepare() {
        bindCamera();

        KEY_PRESSED(D1, switchHDR(););
        KEY_PRESSED(D2, switchBlur(););
        KEY_PRESSED(D3, switchSharpen(););
        KEY_PRESSED(D4, switchEdgeDetection(););
        KEY_PRESSED(D5, switchBloom(););

        KEY_PRESSED(M, switchMSAA(););

        KEY_PRESSED(D9, Application::get().activeSceneFrame->setRenderTargetIndex(0););
        KEY_PRESSED(D0, Application::get().activeSceneFrame->setRenderTargetIndex(1););

        GAMEPAD_PRESSED(GamepadButtonCode::PAD_BTN_A, onPadA(););
        GAMEPAD_PRESSED(GamepadButtonCode::PAD_BTN_B, onPadB(););
        GAMEPAD_PRESSED(GamepadButtonCode::PAD_BTN_X, onPadX(););
        GAMEPAD_PRESSED(GamepadButtonCode::PAD_BTN_Y, onPadY(););

        GAMEPAD_ROLL_LEFT(onGamepadRollLeft(roll););
        GAMEPAD_ROLL_RIGHT(onGamepadRollRight(roll););

//        KEY_PRESSED(KeyCode::D1, audio::MediaPlayer::pause(););
//        KEY_PRESSED(KeyCode::D2, audio::MediaPlayer::stop(););
//        KEY_PRESSED(KeyCode::D3, audio::MediaPlayer::playStream(););

//        KEY_PRESSED(
//                KeyCode::R,
//                // notify all players to rotate a survival pack
//                GDHeader header(CLIENT_TO_CLIENT, ROTATE_SURVIVAL_PACK);
//                GDFloat rotation(0.5f);
//                udp::Client::getRequestQueue().push(header, rotation);
//        );

//        KEY_PRESSED(
//                KeyCode::K,
//                io::RemoteAssetManager::saveScene(Application::get().activeScene);
//        );
//
//        KEY_PRESSED(
//                KeyCode::L,
//                io::RemoteAssetManager::loadScene(Application::get().activeScene->getName().c_str());
//        );

        Application::get().enableMouseHovering = true;
        Application::get().enableMouseCursor = true;

        for (auto& pack : packs) {
            pack.applyTransform();
        }

        for (auto& pack : instancedPacks) {
            pack.applyTransform();
        }
    }

    void TestLayer::onUpdate(time::Time dt) {
        mainCamera.onUpdate(dt);
        // notify all players to rotate a survival pack
        GDHeader header(CLIENT_TO_CLIENT, ROTATE_SURVIVAL_PACK);
        GDFloat rotation(0.005f);
        udp::Client::getRequestQueue().push(header, rotation);
        tcp::Client::getRequestQueue().push(header, rotation);

//        Application::get().activeScene->getSkybox().get<Skybox>()->rotate({0, 0.001f, 0});

        auto hoveredTransform = Application::get().hoveredEntity.get<Transform3dComponent>();
        if (hoveredTransform) {
            ENGINE_INFO("hoveredEntity: transform");
//            hoveredTransform->modelMatrix.rotation[1] += 0.1f;
//            hoveredTransform->modelMatrix.apply();
        }

        if (EventRegistry::mouseHold(ButtonLeft)) {
            RUNTIME_INFO("mouseHold: button left");
            mainCamera.applyMouseRotate();
        }

        if (EventRegistry::mouseHold(ButtonRight)) {
            RUNTIME_INFO("mouseHold: button right");
            mainCamera.applyMouseMove();
        }

        if (EventRegistry::mouseHold(ButtonMiddle)) {
            RUNTIME_INFO("mouseHold: button middle");
            mainCamera.applyMouseZoom();
        }
    }

    void TestLayer::onWindowResized(const uint32_t &width, const uint32_t &height) {
        RUNTIME_INFO("TestLayer::onWindowResized(w: {0}, h: {1})", width, height);
        mainCamera.setAspectRatio(width, height);
    }

    void TestLayer::onKeyPressed(event::KeyCode keyCode) {
    }

    void TestLayer::onKeyHold(event::KeyCode keyCode) {
        switch (keyCode) {
            case W:
                mainCamera.applyMove(UP);
                break;
            case A:
                mainCamera.applyMove(LEFT);
                break;
            case S:
                mainCamera.applyMove(DOWN);
                break;
            case D:
                mainCamera.applyMove(RIGHT);
                break;
            case Q:
                mainCamera.applyRotate(LEFT_Z);
                break;
            case E:
                mainCamera.applyRotate(RIGHT_Z);
                break;
            case Z:
                mainCamera.applyZoom(ZOOM_IN);
                break;
            case X:
                mainCamera.applyZoom(ZOOM_OUT);
                break;
            case LeftAlt:
                dragLight();
                break;
            default: break;
        }
    }

    void TestLayer::onKeyReleased(event::KeyCode keyCode) {
    }

    void TestLayer::onKeyTyped(event::KeyCode keyCode) {
    }

    void TestLayer::onPadA() {
        RUNTIME_INFO("Gamepad button A pressed!");
        mainCamera.move(DOWN);
        mainCamera.applyView();
    }

    void TestLayer::onPadB() {
        RUNTIME_INFO("Gamepad button B pressed!");
        mainCamera.move(RIGHT);
        mainCamera.applyView();
    }

    void TestLayer::onPadX() {
        RUNTIME_INFO("Gamepad button X pressed!");
        mainCamera.move(LEFT);
        mainCamera.applyView();
    }

    void TestLayer::onPadY() {
        RUNTIME_INFO("Gamepad button Y pressed!");
        mainCamera.move(UP);
        mainCamera.applyView();
    }

    void TestLayer::onGamepadRollLeft(const GamepadRoll& roll) {
        RUNTIME_INFO("onGamepadRollLeft: (x: {0}, y: {1}, trigger: {2})", roll.x, roll.y, roll.triggered);
        mainCamera.move({ roll.x, roll.y, 1 });
        mainCamera.applyView();
    }

    void TestLayer::onGamepadRollRight(const GamepadRoll& roll) {
        RUNTIME_INFO("onGamepadRollRight: (x: {0}, y: {1}, trigger: {2})", roll.x, roll.y, roll.triggered);
    }

    void TestLayer::onWindowClosed() {
    }

    void TestLayer::onMousePressed(event::MouseCode mouseCode) {

    }

    void TestLayer::onMouseRelease(event::MouseCode mouseCode) {

    }

    void TestLayer::onMouseScrolled(double xOffset, double yOffset) {

    }

    void TestLayer::onCursorMoved(double xPos, double yPos) {

    }

    void TestLayer::onTCPSocketCreated() {

    }

    void TestLayer::onTCPConnectionFailed() {

    }

    void TestLayer::onTCPConnected() {

    }

    void TestLayer::onTCPSocketClosed() {

    }

    void TestLayer::onTCPSenderFailed(char *data, size_t size) {

    }

    void TestLayer::onTCPSenderSuccess() {

    }

    void TestLayer::onTCPReceiverFailed(char *data, size_t size) {

    }

    void TestLayer::onTCPReceiverSuccess(const YAML::Node &gdNode, const GDHeader &header) {
        RUNTIME_INFO("onTCPReceiverSuccess()");
        RUNTIME_INFO("GDHeader[address: {0}, type: {1}]", header.address, header.type);
        io::RemoteAssetManager::dispatchScene(Application::get().activeScene, gdNode, header.type);
        // dispatch header type
        if (header.type == ROTATE_SURVIVAL_PACK) {
            // unpack data
            GDPrimitive<f32> gdPrimitive;
            gdPrimitive.deserialize(gdNode);
            auto rotationY = gdPrimitive.value;
            RUNTIME_INFO("Rotating SurvivalPack with value: {0}", rotationY);
            // handle data
//            auto& modelMatrix = survivalBackPack.get<Transform3dComponent>()->modelMatrix;
//            modelMatrix.rotation.v[1] += rotationY;
//            updateModel3d(modelMatrix);
        }
    }

    void TestLayer::onUDPSocketCreated() {

    }

    void TestLayer::onUDPSocketClosed() {

    }

    void TestLayer::onUDPSenderFailed(char *data, size_t size) {

    }

    void TestLayer::onUDPSenderSuccess() {

    }

    void TestLayer::onUDPReceiverFailed(char *data, size_t size) {

    }

    void TestLayer::onUDPReceiverSuccess(const YAML::Node &gdNode, const GDHeader &header) {
        RUNTIME_INFO("onUDPReceiverSuccess()");
        RUNTIME_INFO("GDHeader[address: {0}, type: {1}]", header.address, header.type);
        io::RemoteAssetManager::dispatchScene(Application::get().activeScene, gdNode, header.type);
        // dispatch header type
        if (header.type == ROTATE_SURVIVAL_PACK) {
            // unpack data
            GDFloat rotation;
            rotation.deserialize(gdNode);
            auto rotationY = rotation.value;
            RUNTIME_INFO("Rotating SurvivalPack with value: {0}", rotationY);
            // handle data
//            auto& modelMatrix = survivalBackPack.get<Transform3dComponent>()->modelMatrix;
//            modelMatrix.rotation.v[1] += rotationY;
//            updateModel3d(modelMatrix);
        }
    }

    void TestLayer::dragLight() {
        RUNTIME_INFO("onLeftAltHold()");
        auto mouseCoords = Input::getMousePosition();
        u32 w = Application::get().getWindowWidth();
        u32 h = Application::get().getWindowHeight();
        vec3f mouseWorldPos = RayCast::toWorldSpace(
                { mouseCoords.x, mouseCoords.y }, w, h,
                mainCamera.getViewProjection()
        );

//        auto distanceToCar = math::distance(mainCamera.getPosition(), car.getTransform().position);
//        mouseWorldPos[0] *= distanceToCar;
//        mouseWorldPos[1] *= distanceToCar;
//        mouseWorldPos[2] *= distanceToCar;
//        RUNTIME_INFO("distanceToCar: {0}", distanceToCar);

        RUNTIME_INFO("mouseWorldPos: {0}, {1}, {2}", mouseWorldPos.x(), mouseWorldPos.y(), mouseWorldPos.z());
//        car.getTransform().position = mouseWorldPos;
//        car.applyTransform();

        mouseWorldPos[0] *= 10;
        mouseWorldPos[1] *= -10;
        mouseWorldPos[2] *= 10;

        lights[0].getPosition() = mouseWorldPos;
    }

    void TestLayer::switchMSAA() {
        msaaEnabled = !msaaEnabled;
        Application::get().setSampleSize(msaaEnabled ? 8 : 1);
    }

    void TestLayer::switchHDR() {
        Application::get().hdrEffect->enabled = !Application::get().hdrEffect->enabled;
    }

    void TestLayer::switchBlur() {
        Application::get().blurEffect->enabled = !Application::get().blurEffect->enabled;
    }

    void TestLayer::switchSharpen() {
        Application::get().sharpenEffect->enabled = !Application::get().sharpenEffect->enabled;
    }

    void TestLayer::switchEdgeDetection() {
        Application::get().edgeDetectionEffect->enabled = !Application::get().edgeDetectionEffect->enabled;
    }

    void TestLayer::switchGaussianBlur() {
        Application::get().gaussianBlurEffect->enabled = !Application::get().gaussianBlurEffect->enabled;
    }

    void TestLayer::switchBloom() {
        switchHDR();
        switchGaussianBlur();
    }

    void TestLayer::onVisualDraw(time::Time dt) {
        // dock space
        Visual::beginDockspace("Test");
        Visual::endDockspace();
        // scene panel
//        Panel::begin("Scene", { 800, 600 });
        // physics
//        Checkbox::draw("enablePhysics", Physics::isEnabled);
//        // entity hovering
//        // draw tag of hovered entity
//        auto* tag = Application::get().hoveredEntity.get<TagComponent>();
//        if (tag) {
//            Text::draw(tag->tag.c_str());
//        }
//        // checkbox to switch mouse hovering
//        Checkbox::draw("enableMouseHovering", Application::get().enableMouseHovering);
//        // post effects
//        Checkbox::draw("enabled", Application::get().gaussianBlurEffect->enabled);
//        Slider::draw("amount", Application::get().gaussianBlurEffect->amount, { 1, 20 });
//        // objects
//        for (auto& pack : packs) {
//            Slider::draw("brightness", pack.get<Phong>()->brightness(), { 0, 100 });
//            auto& transform = pack.getTransform();
//            auto v = pack.get<Velocity>();
//            Vec3fUniform position = { "position", transform.position };
//            Vec3fUniform rotation = { "rotation", transform.rotation };
//            Vec3fUniform scale = { "scale", transform.scale };
//            Vec3fUniform velocity = { "velocity", v->velocity };
//            Controller::draw(position, 0, 10);
//            Controller::draw(scale, 0, 10);
//            Controller::draw(rotation, 0, 10);
//            Controller::draw(velocity, 0, 10);
//            transform.position = position.value;
//            transform.rotation = rotation.value;
//            transform.scale = scale.value;
//            v->velocity = velocity.value;
//        }

//        Panel::end();
        // Console
//        Console::draw("Console", { 800, 600 });
        // Troubleshoot
//        ProfilerMenu::draw("Profiler Menu", { 800, 600 });

        if (EventRegistry::keyHold(KeyCode::LeftControl) && Input::isMousePressed(MouseCode::ButtonLeft)) {
            Entity selectedEntity = Application::get().hoveredEntity;
            auto* selectedTransform = selectedEntity.get<Transform3dComponent>();
            if (selectedTransform) {
                Application::get().selectedEntity = selectedEntity;
                showGizmo = !showGizmo;
            }
        }

        if (showGizmo) {
            // get selected entity transform
            auto& app = Application::get();
            Entity selectedEntity = app.selectedEntity;
            auto* selectedTransform = selectedEntity.get<Transform3dComponent>();
            // get window position and size
            vec2f windowSize = {
                    static_cast<float>(app.getWindowWidth()),
                    static_cast<float>(app.getWindowHeight())
            };
            int xPos = 0, yPos = 0;
            app.getWindow()->getPosition(xPos, yPos);
            // draw translation gizmo
            Gizmo::drawTranslate(mainCamera, *selectedTransform,
                                 { static_cast<float>(xPos), static_cast<float>(yPos) },
                                 windowSize);
        }

//        ProjectsPanel::draw("Project Manager", { 800, 600 }, [this](const Project& openedProject) {});

        MaterialPanel::draw(packs[2]);
//        LightsPanel::draw(lights);
        AssetBrowser::draw(dt);
//        Log::draw("Log");
        sceneViewport.setId(RenderSystem::finalRenderTargetId);
        sceneViewport.onUpdate(dt);
     }
}