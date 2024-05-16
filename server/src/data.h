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
    uint32_t id;

    vec2 pos;
    vec2 scale;
    color color;

    vec2 vel;
};

void updateObject(object& obj, float dt) {
    obj.pos.x += obj.vel.x * dt;
    obj.pos.y += obj.vel.y * dt;

    obj.vel.y += 200 * dt;

    if (obj.pos.y + obj.scale.y > 500) {
        obj.vel.y = 0;
        obj.pos.y = 500 - obj.scale.y;
    }
}

void writeFloat(std::byte* dataStream, float val, size_t& bytesWritten) {
    *(reinterpret_cast<float*>(dataStream + bytesWritten)) = val;
    bytesWritten += sizeof(float);
}

void writeInt(std::byte* dataStream, int val, size_t& bytesWritten) {
    *(reinterpret_cast<int*>(dataStream + bytesWritten)) = val;
    bytesWritten += sizeof(int);
}

void writeUint32(std::byte* dataStream, uint32_t val, size_t& bytesWritten) {
    *(reinterpret_cast<uint32_t*>(dataStream + bytesWritten)) = val;
    bytesWritten += sizeof(uint32_t);
}

// returns the number of bytes written
void serializeObject(const object& obj, std::byte* dataStream, size_t& bytesWritten) {
    writeUint32(dataStream, obj.id, bytesWritten);
    writeFloat(dataStream, obj.pos.x, bytesWritten);
    writeFloat(dataStream, obj.pos.y, bytesWritten);
    writeFloat(dataStream, obj.scale.x, bytesWritten);
    writeFloat(dataStream, obj.scale.y, bytesWritten);
    writeFloat(dataStream, obj.color.r, bytesWritten);
    writeFloat(dataStream, obj.color.g, bytesWritten);
    writeFloat(dataStream, obj.color.b, bytesWritten);
}