#pragma once

#include <chrono>

namespace utils {
inline float getTime() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000.0f;
}
}