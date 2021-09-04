//
// Created by mecha on 28.08.2021.
//

#pragma once

#include "Memory.h"
#include "Window.h"
#include "Logger.h"
#include "../events/Event.h"
#include "LayerStack.h"
#include "Assert.h"

namespace engine {

    class Application : public EventCallback {

    public:
        Application();
        virtual ~Application();

    public:
        static inline Application& getInstance() {
            return *_instance;
        }

    public:
        inline Window& getWindow() {
            return *_window;
        }

    public:
        void run();
        void onEvent(Event& event) override;

    protected:
        void pushLayer(Layer* layer);
        void pushOverlay(Layer* overlay);

    private:
        static Application* _instance;

    private:
        bool _isRunning = true;
        Scope<Window> _window;
        LayerStack _layerStack;

    };

    Application* createApplication();

}