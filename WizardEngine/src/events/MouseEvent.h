//
// Created by mecha on 29.08.2021.
//
#pragma once

#include "Event.h"
#include "../core/MouseCodes.h"

namespace engine {

    class MouseMoveEvent : public Event {

    public:
        MouseMoveEvent(const float x, const float y)
        : _xPosition(x), _yPosition(y) {

        }

    public:
        float getX() const { return _xPosition; }
        float getY() const { return _yPosition; }

    public:
        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseMoveEvent: " << _xPosition << ", " << _yPosition;
            return ss.str();
        }

    public:
        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(Mouse | Input)

    private:
        float _xPosition, _yPosition;

    };

    class MouseScrollEvent : public Event {

    public:
        MouseScrollEvent(const float xOffset, const float yOffset)
        : _xOffset(xOffset), _yOffset(yOffset) {

        }

    public:
        float getXOffset() const { return _xOffset; }
        float getYOffset() const { return _yOffset; }

    public:
        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseScrollEvent: " << getXOffset() << ", " << getYOffset();
            return ss.str();
        }

    public:
        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(Mouse | Input)

    private:
        float _xOffset, _yOffset;

    };

    class MouseButtonEvent : public Event {

    protected:
        MouseButtonEvent(const int mouseCode)
        : mMouseCode(mouseCode) {

        }

    public:
        int getMouseCode() const { return mMouseCode; }

    public:
        EVENT_CLASS_CATEGORY(Mouse | Input | MouseButton)

    protected:
        int mMouseCode;

    };

    class MouseButtonPressedEvent : public MouseButtonEvent {

    public:
        MouseButtonPressedEvent(const int mouseCode)
        : MouseButtonEvent(mouseCode) {

        }

    public:
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << mMouseCode;
            return ss.str();
        }

    public:
        EVENT_CLASS_TYPE(MouseButtonPressed)

    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {

    public:
        MouseButtonReleasedEvent(const int mouseCode)
        : MouseButtonEvent(mouseCode) {

        }

    public:
        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << mMouseCode;
            return ss.str();
        }

    public:
        EVENT_CLASS_TYPE(MouseButtonReleased)

    };

}