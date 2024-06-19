#pragma once

#include <stdlib.h>

class RuntimeInfo {
private:
    static int d_displayWidth;
    static int d_displayHeight;
public:
    static int displayWidth();
    static int displayHeight();

    static void updateDisplayDimensions(int width, int height);
};

inline int RuntimeInfo::displayWidth() {
    return d_displayWidth;
}

inline int RuntimeInfo::displayHeight() {
    return d_displayHeight;
}

inline void RuntimeInfo::updateDisplayDimensions(int width, int height) {
    d_displayWidth = width;
    d_displayHeight = height;
}