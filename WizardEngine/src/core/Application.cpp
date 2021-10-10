//
// Created by mecha on 28.08.2021.
//

#include "Application.h"

#include "../platform/Platform.h"

namespace engine {

    void Application::run() {
        onCreate();
        onPrepare();
        while (_isRunning) {
            onUpdate();
        }
        onDestroy();
    }

    void Application::onCreate() {
        ENGINE_INFO("create()");

        _window = INIT_WINDOW(createWindowProps());

        fpsTimer.setMaxFps(getRefreshRate());

        _graphicsContext = INIT_GRAPHICS_CONTEXT(_window->getNativeWindow());
        _graphicsContext->onCreate();
        _graphicsContext->printInfo();

        _window->setWindowCallback(this);
        _window->setMouseCallback(this);
        _window->setKeyboardCallback(this);
        _window->setCursorCallback(this);

        input = INIT_INPUT(_window->getNativeWindow());

        createRenderSystem();
        createActiveScene();
        createCamera();
    }

    void Application::onPrepare() {
        _window->onPrepare();
        updateFboSpecification();
        _layerStack.onPrepare();
    }

    void Application::onDestroy() {
        ENGINE_INFO("onDestroy()");
    }

    void Application::onUpdate() {
        auto dt = fpsTimer.getDeltaTime();
        fpsTimer.begin();

        sceneCameraController->setDeltaTime(dt);
        _renderSystem->onUpdate();

        _layerStack.onUpdate(dt);
        _window->onUpdate();
        _graphicsContext->swapBuffers();

        fpsTimer.end();
    }

    void Application::pushFront(Layer *layer) {
        ENGINE_INFO("Pushing layer : {0}", layer->getTag());
        _layerStack.pushFront(layer);
    }

    void Application::pushBack(Layer *layer) {
        ENGINE_INFO("Pushing overlay : {0}", layer->getTag());
        _layerStack.pushBack(layer);
    }

    void Application::onWindowClosed() {
        ENGINE_INFO("Application : onWindowClosed()");
        _layerStack.onWindowClosed();
        _renderSystem->onWindowClosed();
        _isRunning = false;
    }

    void Application::onWindowResized(const uint32_t &width , const uint32_t &height) {
        ENGINE_INFO("Application : onWindowResized({0}, {1})", width, height);
        if (width == 0 || height == 0) return;

        _layerStack.onWindowResized(width, height);
        _renderSystem->onWindowResized(width, height);
        sceneCameraController->onWindowResized(width, height);
    }

    void Application::onKeyPressed(KeyCode keyCode) {
        if (closeKeyPressed == keyCode) {
            onWindowClosed();
        }
        _layerStack.onKeyPressed(keyCode);
    }

    void Application::onKeyHold(KeyCode keyCode) {
        _layerStack.onKeyHold(keyCode);
    }

    void Application::onKeyReleased(KeyCode keyCode) {
        _layerStack.onKeyReleased(keyCode);
    }

    void Application::onMousePressed(MouseCode mouseCode) {
        _layerStack.onMousePressed(mouseCode);
    }

    void Application::onMouseRelease(MouseCode mouseCode) {
        _layerStack.onMouseRelease(mouseCode);
    }

    void Application::onMouseScrolled(double xOffset, double yOffset) {
        _layerStack.onMouseScrolled(xOffset, yOffset);
    }

    void Application::onCursorMoved(double xPos, double yPos) {
        _layerStack.onCursorMoved(xPos, yPos);
    }

    void Application::onKeyTyped(KeyCode keyCode) {
        _layerStack.onKeyTyped(keyCode);
    }

    void Application::addShader(const std::string &name, const Ref<Shader> &shader) {
        _renderSystem->addShader(name, shader);
    }

    void Application::addShader(const Ref<Shader> &shader) {
        _renderSystem->addShader(shader);
    }

    ShaderError Application::loadShader(const ShaderProps &shaderProps, VertexFormat* vertexFormat) {
        return _renderSystem->loadShader(shaderProps, vertexFormat);
    }

    ShaderError Application::loadShader(const ShaderProps &shaderProps) {
        return _renderSystem->loadShader(shaderProps);
    }

    Ref<Shader> Application::getShader(const std::string &name) {
        return _renderSystem->getShader(name);
    }

    bool Application::shaderExists(const std::string &name) const {
        return _renderSystem->shaderExists(name);
    }

    float Application::getAspectRatio() const {
        return _window->getAspectRatio();
    }

    void Application::enableDepthRendering() {
        _renderSystem->enableDepth();
    }

    void Application::createCamera3D(const char *name) {
        createCamera3D(name, {0.5, 0.5, 1});
    }

    void Application::createCamera3D(const char *name, const glm::vec3 &position) {
        auto camera3D = new engine::Camera3d {
            name,
            engine::ViewMatrix3d {
                "view3d",
                position
                },
                engine::PerspectiveMatrix {
                "projection3d",
                getAspectRatio()
            }
        };
        createCamera3D(camera3D);
    }

    void Application::createCamera3D(Camera3d* camera3D) {
        sceneCameraController = createRef<Camera3dController>(camera3D);
        _renderSystem->setSceneCameraController(sceneCameraController);
    }

    void Application::createActiveScene() {
        activeScene = createRef<Scene>();
        _renderSystem->setActiveScene(activeScene.get());
    }

    void Application::createCamera2D(const char *name) {
        createCamera2D(name, {0, 0, 1});
    }

    void Application::createCamera2D(const char *name, const glm::vec3 &position) {
        auto camera2D = new engine::Camera2d {
            name,
            engine::ViewMatrix2d {
                "view2d",
                position
                },
                engine::OrthographicMatrix {
                "projection2d",
                getAspectRatio()
            }
        };
        createCamera2D(camera2D);
    }

    void Application::createCamera2D(Camera2d *camera2D) {
        sceneCameraController = createRef<Camera2dController>(camera2D);
        _renderSystem->setSceneCameraController(sceneCameraController);
    }

    void Application::restart() {
        onDestroy();
        onCreate();
    }

    void Application::createCamera(const char *name) {
        if (_engineType == ENGINE_2D) {
            createCamera2D(name);
        } else {
            createCamera3D(name);
        }
    }

    void Application::createRenderSystem() {
        _graphicsFactory = _graphicsContext->newGraphicsFactory();
        if (_engineType == ENGINE_2D) {
            _renderSystem = createRef<RenderSystem2d>(_graphicsFactory);
        } else {
            _renderSystem = createRef<RenderSystem3d>(_graphicsFactory);
        }
    }

    Ref<MeshComponent> Application::loadObj(const std::string &objName) {
        return objFile.readObj(objName);
    }

    const uint32_t &Application::getWindowWidth() {
        return _window->getWidth();
    }

    const uint32_t &Application::getWindowHeight() {
        return _window->getHeight();
    }

    void Application::updateFboSpecification() {
        FramebufferSpecification framebufferSpecification;
        framebufferSpecification.attachmentSpecification = {
                FramebufferTextureFormat::RGBA8,
                FramebufferTextureFormat::RED_INTEGER,
                FramebufferTextureFormat::Depth
        };
        framebufferSpecification.width = 1920;
        framebufferSpecification.height = 1080;

        updateFboSpecification(framebufferSpecification);
    }

    void Application::updateFboSpecification(const FramebufferSpecification &framebufferSpecification) {
        _renderSystem->updateFboSpecification(framebufferSpecification);
    }

    void Application::setPolygonMode(const PolygonMode &polygonMode) {
        _renderSystem->setPolygonMode(polygonMode);
    }

    uint32_t Application::getRefreshRate() {
        return _window->getRefreshRate();
    }

    void Application::enableDisplay() {
        _isDisplayEnabled = true;
    }

    void Application::disableDisplay() {
        _isDisplayEnabled = false;
    }

    void *Application::getNativeWindow() {
        return _window->getNativeWindow();
    }

    WindowProps Application::createWindowProps() {
        return WindowProps();
    }

    void Application::loadTexture(const std::string &fileName) {
        _renderSystem->loadTexture(fileName);
    }

    void Application::setWindowIcon(const std::string &filePath) {
        _window->setWindowIcon(filePath);
    }

    Ref<FileDialog> Application::createFileDialog() {
        return INIT_FILE_DIALOG(_window->getNativeWindow());
    }

}