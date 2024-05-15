#pragma once

#include <cstdlib>
#include <cstddef>

struct vec2 {
    float x;
    float y;
};

struct color {
    float r;
    float g;
    float b;
};

struct object {
    vec2 pos;
    vec2 scale;
    color color;
};

static void writeFloat(std::byte* dataStream, float val, size_t& bytesWritten) {
    *(reinterpret_cast<float*>(dataStream + bytesWritten)) = val;
    bytesWritten += sizeof(float);
}

// returns the number of bytes written
void serializeObject(const object& obj, std::byte* dataStream, size_t& bytesWritten) {
    writeFloat(dataStream, obj.pos.x, bytesWritten);
    writeFloat(dataStream, obj.pos.y, bytesWritten);
    writeFloat(dataStream, obj.scale.x, bytesWritten);
    writeFloat(dataStream, obj.scale.y, bytesWritten);
    writeFloat(dataStream, obj.color.r, bytesWritten);
    writeFloat(dataStream, obj.color.g, bytesWritten);
    writeFloat(dataStream, obj.color.b, bytesWritten);
}