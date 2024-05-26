#pragma once

#include "mathutils.h"

#include <cstddef>  

struct color {
    float r;
    float g;
    float b;
};

class Object {
private:
    static uint32_t lastUsedID;
    uint32_t id;
    float age;

public:
    math::vec2 pos;
    math::vec2 scale;
    color color;

    math::vec2 vel;
    Object(float x, float y);

    void updatePosition(float dt);
    void generalUpdate(float dt);

    void serialize(std::byte* dataStream, size_t& bytesWritten);

    inline uint32_t getID() const { return this->id; }
    inline float getAge() const { return this->age; }

    virtual void update(float dt);
};

class Player : public Object {
public:
    bool moveLeft = false;
    bool moveRight = false;

    Player(float x, float y);

    void update(float dt);
};