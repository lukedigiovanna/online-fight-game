#pragma once

#include <stdlib.h>

namespace math {
    float random() {
        return static_cast<float>(rand()) / RAND_MAX;
    }

    float random(float a, float b) {
        return random() * (b - a) + a;
    }
}