//
// Created by mecha on 03.10.2021.
//

#pragma once

#include "Thread.h"

#define DEFAULT_FPS 15
#define CURRENT_TIME_MS std::chrono::duration_cast<ms>(clock::now().time_since_epoch())
#define CURRENT_TIME_NANO std::chrono::duration_cast<nano>(clock::now().time_since_epoch())

namespace engine {

    typedef std::chrono::high_resolution_clock clock;

    class Timer {

    public:
        Timer(const uint32_t &maxFps = DEFAULT_FPS) :
        _maxFps(maxFps) {}

        ~Timer() = default;

    public:
        inline void setMaxFps(const uint32_t &maxFps) {
            _maxFps = maxFps;
        }

        inline const uint32_t& getMaxFps() const {
            return _maxFps;
        }

        inline const Time& getDeltaTime() {
            return _deltaTime;
        }

        inline Time getMinDeltaTime() const {
            return { 1.0f / (float) _maxFps };
        }

    public:
        void begin();
        void end();

    private:
        float _beginTime;
        Time _deltaTime;
        uint32_t _maxFps;

    };

}