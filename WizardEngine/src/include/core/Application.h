//
// Created by mecha on 28.08.2021.
//

#pragma once

#include <core/String.h>
#include <core/LayerStack.h>
#include <core/Memory.h>
#include <event/Events.h>
#include <event/GamepadCodes.h>
#include <time/FpsController.h>

#include <platform/core/Window.h>
#include <platform/core/Input.h>
#include <platform/tools/FileDialog.h>
#include <platform/graphics/Context.h>

#include <graphics/core/RenderSystem.h>
#include <graphics/core/sources/MeshSource.h>
#include <graphics/core/sources/ShaderSource.h>

#include <scripting/ScriptSystem.h>

#define BIND_GAMEPAD_BUTTON_PRESSED(gamepadBtn, action) engine::core::Application::get().eventRegistry.onGamepadButtonPressedMap[gamepadBtn] = { \
[this](GamepadButtonCode gamepadBtnCode) { action }                                                                                 \
}
#define BIND_KEY_PRESSED(key, action) engine::core::Application::get().eventRegistry.onKeyPressedMap[key] = { [this](KeyCode keyCode) { action } }

namespace engine::core {

    // The entry point in core hierarchy, behaves as singleton.
    // It's standalone class, which can NOT be created several times and should be created only once!
    class Application {

    public:
        Application();
        virtual ~Application();

    public:
        void run();

    public:
        static inline Application& get() {
            return *instance;
        }

        inline const Scope<Window>& getWindow() {
            return _window;
        }

    public:
        // window events
        void onWindowClosed();
        void onWindowResized(const u32 &width, const u32 &height);
        // input keyboard events
        void onKeyPressed(event::KeyCode keyCode);
        void onKeyHold(event::KeyCode keyCode);
        void onKeyReleased(event::KeyCode keyCode);
        void onKeyTyped(event::KeyCode keyCode);
        // input mouse events
        void onMousePressed(event::MouseCode mouseCode);
        void onMouseRelease(event::MouseCode mouseCode);
        void onMouseScrolled(double xOffset, double yOffset);
        // input mouse cursor events
        void onCursorMoved(double xPos, double yPos);
        // input gamepad events
        void onGamepadConnected(s32 joystickId);
        void onGamepadDisconnected(s32 joystickId);

    protected:
        virtual void onCreate();
        virtual void onPrepare();
        virtual void onDestroy();
        virtual WindowProps createWindowProps();

    public:
        [[nodiscard]] float getAspectRatio() const;
        const uint32_t& getWindowWidth();
        const uint32_t& getWindowHeight();
        uint32_t getRefreshRate();
        void* getNativeWindow();
        void setWindowIcon(const std::string &filePath);
        Ref<tools::FileDialog> createFileDialog();
        void setSampleSize(const uint32_t& samples);
        void setActiveScene(const Ref<Scene>& activeScene);
        void setActiveScene(const uint32_t& activeSceneId);
        void loadGamepadMappings(const char* mappingsFilePath);

    protected:
        void pushFront(Layer* layer);
        void pushBack(Layer* overlay);

    private:
        void shutdown();
        void restart();
        void onUpdate();
        void updateRuntime(time::Time dt);
        void updateEventRegistry();
        void createGraphics();
        void createScripting();

    public:
        Scope<event::Input> input;
        std::vector<Ref<Scene>> scenes;
        Ref<Scene> activeScene;
        Ref<FrameBuffer> activeSceneFrame;
        Ref<FrameBuffer> screenFrame;
        time::FpsController fpsController;
        event::EventRegistry eventRegistry;
        bool isJoystickConnected = false;

    private:
        static Application* instance;

    private:
        bool _isRunning = true;
        // core systems
        LayerStack _layerStack; // todo consider remove LayerStack. Instead replace with Activity class.
        Scope<Window> _window;
        // graphics system
        Scope<RenderSystem> _renderSystem;
        // scripting system
        Scope<scripting::ScriptSystem> _scriptSystem;
    };

    Application* createApplication();

}