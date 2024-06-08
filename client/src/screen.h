#pragma once

#include <SDL2/SDL.h>

class Screen {
public:
    Screen();
    virtual void render(SDL_Renderer* renderer);
};