//
// Created by mecha on 28.08.2021.
//

#pragma once

#include "../../WizardEngine/src/Engine.h"

#include "TestLayer.h"

namespace test {

    class TestLayout : public engine::Layout {

    public:
        TestLayout() = default;
        ~TestLayout() override = default;

    public:
        void onUpdate(engine::Time deltaTime) override {
            ImGui::Begin("Test");
            ImGui::Text("Hello World!");
            ImGui::End();
        }

    };

    class TestApplication : public engine::Application {

    protected:

        void onCreate() override {
            Application::onCreate();
            CLIENT_INFO("create()");

            pushLayer(new TestLayer());

            pushLayout(new engine::DemoLayout());
            pushLayout(new TestLayout());

            auto shape2dShader = loadShader("assets/shaders/shape2dShader.glsl");
//            shape2dShader->addAttribute(engine::Attribute("position", 0));
        }

        void onUpdate() override {
            Application::onUpdate();
            CLIENT_INFO("onUpdate()");
        }

        void onDestroy() override {
            Application::onDestroy();
            CLIENT_INFO("destroy()");
        }

        void onKeyPressed(engine::KeyCode keyCode) override {
            Application::onKeyPressed(keyCode);

            if (keyCode == engine::KeyCode::Escape) {
                engine::Application::onWindowClosed();
            }
        }

    };

}