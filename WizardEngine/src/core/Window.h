//
// Created by mecha on 29.08.2021.
//
#pragma once

#include "Events.h"
#include "Memory.h"

namespace engine {

    struct WindowProps {
        std::string title;
        uint32_t width;
        uint32_t height;
        bool vSyncEnabled;

        WindowCallback* windowCallback = nullptr;
        KeyboardCallback* keyboardCallback = nullptr;
        MouseCallback* mouseCallback = nullptr;
        CursorCallback* cursorCallback = nullptr;

        explicit WindowProps(
                const std::string& title = "Wizard",
                uint32_t width = 800,
                uint32_t height = 600,
                bool vSyncEnabled = false)
        : title(title), width(width), height(height), vSyncEnabled(vSyncEnabled) {

        }

    };

    class Window {

    public:
        explicit Window(const WindowProps& windowProps) : windowProps(windowProps) {

        };
        virtual ~Window() = default;

    public:
        static Scope<Window> newInstance(const WindowProps& props = WindowProps());

    public:
        virtual void* getNativeWindow() const = 0;

        virtual void onCreate() = 0;
        virtual void onUpdate() = 0;
        virtual void onClose() = 0;

        virtual void onDestroy() {
            removeWindowCallback();
            removeKeyboardCallback();
            removeMouseCallback();
            removeCursorCallback();
        }

    public:
        virtual void enableVSync() {
            windowProps.vSyncEnabled = true;
        }
        virtual void disableVSync() {
            windowProps.vSyncEnabled = false;
        }

    public:

        inline uint32_t getWidth() const {
            return windowProps.width;
        }

        inline uint32_t getHeight() const {
            return windowProps.height;
        }

        inline bool isVSyncEnabled() const {
            return windowProps.vSyncEnabled;
        }

        inline void setWindowCallback(WindowCallback *windowCallback) {
            windowProps.windowCallback = windowCallback;
        }

        inline void removeWindowCallback() {
            windowProps.windowCallback = nullptr;
        }

        inline void setKeyboardCallback(KeyboardCallback *keyboardCallback) {
            windowProps.keyboardCallback = keyboardCallback;
        }

        inline void removeKeyboardCallback() {
            windowProps.keyboardCallback = nullptr;
        }

        inline void setMouseCallback(MouseCallback *mouseCallback) {
            windowProps.mouseCallback = mouseCallback;
        }

        inline void removeMouseCallback() {
            windowProps.mouseCallback = nullptr;
        }

        inline void setCursorCallback(CursorCallback *cursorCallback) {
            windowProps.cursorCallback = cursorCallback;
        }

        inline void removeCursorCallback() {
            windowProps.cursorCallback = nullptr;
        }

    protected:
        WindowProps windowProps;
        bool isInitialized = false;

    };

}