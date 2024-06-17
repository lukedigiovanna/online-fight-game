#pragma once

#include <stdlib.h>
#include <map>

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
};

struct snapshot {
  float timestamp;
  std::map<uint32_t, object> objects;
};
