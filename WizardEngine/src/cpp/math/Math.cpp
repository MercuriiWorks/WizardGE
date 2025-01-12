//
// Created by mecha on 20.03.2022.
//

#include <math/Math.h>
#include <random>
#include <cmath>

namespace engine::math {

    void random(
            int minRange,
            int maxRange,
            int count,
            const std::function<void(int i, f32 random)>& callback
    ) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(minRange, maxRange);
        for (int i = 0 ; i < count ; i++) {
            auto r = (float) dist(mt);
            callback(i, r);
        }
    }

    f32 random(const s32& minRange, const s32& maxRange) {
        return random((double)minRange, (double)maxRange);
    }

    f32 random(const double& minRange, const double& maxRange) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(minRange, maxRange);
        return (f32) dist(mt);
    }
}