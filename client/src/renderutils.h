#pragma once

#include <SDL_ttf.h>
#include <SDL.h>

#include <string>
#include <iostream>

namespace renderutils {
inline int getTextWidth(TTF_Font* font, const std::string& text, int size) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), {255, 255, 255});
    if (textSurface == NULL) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return 0;
    }
    int width = textSurface->w * size / textSurface->h;
    SDL_FreeSurface(textSurface);
    return width;
}
inline void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int size, const SDL_Color& color) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (textSurface == NULL) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return;
    }
    int scaledWidth = textSurface->w * size / textSurface->h;
    SDL_Rect dstRect = { x, y, scaledWidth, size };
    if (SDL_RenderCopy(renderer, textTexture, NULL, &dstRect)) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
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