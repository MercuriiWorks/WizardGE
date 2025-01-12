//
// Created by mecha on 04.09.2021.
//

#pragma once

#include <core/primitives.h>
#include <core/core.h>
#include <event/KeyCodes.h>
#include <event/MouseCodes.h>

namespace engine::event {

    using namespace core;

    struct ENGINE_API MousePosition {
        f32 x = 0;
        f32 y = 0;

        MousePosition operator -(const MousePosition& mousePosition) const;

        template<typename T>
        friend MousePosition operator *(const MousePosition& mp, T scalar) {
            return { mp.x * scalar, mp.y * scalar };
        }
    };

    struct ENGINE_API JoystickAxisStates {
        const f32* axes;
        s32 count;
    };

    struct ENGINE_API JoystickButtonStates {
        const u8* buttons;
        s32 count;
    };

    struct ENGINE_API JoystickHatStates {
        const u8* hats;
        s32 count;
    };

    struct ENGINE_API GamepadState {
        u8 buttons[15];
        f32 axes[6];
    };

    class ENGINE_API Input final {

    public:
        static void create(void* nativeWindow);
        static void destroy();

    public:
        static bool isKeyPressed(KeyCode keyCode);
        static bool isKeyHold(KeyCode keyCode);
        static bool isMousePressed(MouseCode mouseCode);
        static MousePosition getMousePosition();
        static MousePosition getMouseDelta();
        static void updateMousePosition();
        [[nodiscard]] static bool isJoystickConnected();
        [[nodiscard]] static bool isJoystickDisconnected();
        [[nodiscard]] static JoystickAxisStates getJoystickAxisStates();
        [[nodiscard]] static JoystickButtonStates getJoystickButtonStates();
        [[nodiscard]] static JoystickHatStates getJoystickHatStates();
        [[nodiscard]] static const char* getJoystickName();
        [[nodiscard]] static bool isGamepadPresent();
        [[nodiscard]] static const char* getGamepadName();
        [[nodiscard]] static GamepadState getGamepadState();

    public:
        static inline f32 getMousePosX() {
            return getMousePosition().x;
        }

        static inline f32 getMousePosY() {
            return getMousePosition().y;
        }

        static void setJoystickId(const s32& joystickId) {
            Input::joystickId = joystickId;
        }

    private:
        static void* nativeWindow;
        static s32 joystickId;
        static MousePosition previousMousePosition;
    };
}
