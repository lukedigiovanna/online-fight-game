#include "objects.h"
#include "../../common/serialization.h"

#include <math.h>

uint32_t Object::lastUsedID = 0;

Object::Object(float x, float y) {
    this->id = ++Object::lastUsedID;
    this->pos = { x, y };
    this->scale = { 50, 50 };
    this->color = { 255, 255, 255 };
    this->vel = { 0, 0 };
    this->age = 0;
}

void Object::updatePosition(float dt) {
    pos.x += vel.x * dt;
    pos.y += vel.y * dt;

    vel.y += 200 * dt;

    if (pos.y + scale.y > 500) {
        vel.y = 0;
        pos.y = 500 - scale.y;
    }
}

void Object::generalUpdate(float dt) {
    age += dt;
}

void Object::update(float dt) {

}

void Object::serialize(std::byte* dataStream, size_t& bytesWritten) {
    serializer::write<uint32_t>(dataStream, id, bytesWritten);
    serializer::write<float>(dataStream, pos.x, bytesWritten);
    serializer::write<float>(dataStream, pos.y, bytesWritten);
    serializer::write<float>(dataStream, scale.x, bytesWritten);
    serializer::write<float>(dataStream, scale.y, bytesWritten);
    serializer::write<float>(dataStream, color.r, bytesWritten);
    serializer::write<float>(dataStream, color.g, bytesWritten);
    serializer::write<float>(dataStream, color.b, bytesWritten);
}

// player stuff

Player::Player(float x, float y) : Object(x, y) {
    
}

void Player::update(float dt) {
    this->vel.x = 0;
    if (this->moveLeft) {
        this->vel.x -= 50;
    }
    if (this->moveRight) {
        this->vel.x += 50;
    }
}