#pragma once

#include <stdlib.h>

namespace math {
    struct vec2 {
        float x;
        float y;
    };

    inline vec2 operator+(const vec2& lhs, const vec2& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    inline vec2 operator-(const vec2& lhs, const vec2& rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    inline float random() {
        return static_cast<float>(rand()) / RAND_MAX;
    }

    inline float random(float a, float b) {
        return random() * (b - a) + a;
    }
}