#pragma once

#include <SDL_ttf.h>
#include <SDL.h>

#include <string>
#include <iostream>

namespace renderutils {
    extern int getTextWidth(TTF_Font* font, const std::string& text, int size);
    extern void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int size, const SDL_Color& color);
    extern void fillCircle(SDL_Renderer* renderer, int x, int y, int r);
    extern void fillRoundedRect(SDL_Renderer* renderer, int x, int y, int w, int h, int r);
}

namespace colors {
    extern const SDL_Color RED;
    extern const SDL_Color ORANGE;
    extern const SDL_Color YELLOW;
    extern const SDL_Color GREEN;
    extern const SDL_Color BLUE;
    extern const SDL_Color PURPLE;
    extern const SDL_Color MAGENTA;
    extern const SDL_Color BLACK;
    extern const SDL_Color GRAY;
    extern const SDL_Color LIGHT_GRAY;
    extern const SDL_Color WHITE;
};