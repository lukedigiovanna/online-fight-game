#include "renderutils.h"

#include "font.h"

void bresenham_helper(SDL_Renderer* renderer, int cx, int cy, int dx, int dy) {
    SDL_Point points[8] = {
        { cx + dx, cy + dy },
        { cx + dy, cy + dx },
        { cx - dx, cy + dy },
        { cx - dy, cy + dx },
        { cx + dx, cy - dy },
        { cx + dy, cy - dx },
        { cx - dx, cy - dy },
        { cx - dy, cy - dx },
    };
    SDL_RenderDrawPoints(renderer, points, 8);
}

namespace renderutils {
int getTextWidth(const std::string& fontFamily, const std::string& text, int size) {
    TTF_Font* font = Fonts::getFont(fontFamily, size);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), {255, 255, 255});
    if (textSurface == NULL) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return 0;
    }
    int width = textSurface->w;
    SDL_FreeSurface(textSurface);
    return width;
}
int drawText(SDL_Renderer* renderer, const std::string& fontFamily, const std::string& text, int x, int y, int size, const SDL_Color& color) {
    TTF_Font* font = Fonts::getFont(fontFamily, size);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (textSurface == NULL) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return 0;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return 0;
    }
    SDL_Rect dstRect = { x, y, textSurface->w, textSurface->h };
    if (SDL_RenderCopy(renderer, textTexture, NULL, &dstRect)) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return 0;
    }
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return textSurface->w;
}
void fillCircle(SDL_Renderer* renderer, int cx, int cy, int r) {
    int x = r;
    int y = 0;
    int err = 3 - 2 * r;
    while (x >= y) {
        for (int dx = 0; dx <= x; dx++)
            bresenham_helper(renderer, cx, cy, dx, y);
        if (err > 0) {
            err -= 4 * (--x); 
        }
        err += 4 * (++y) + 2;
    }
}
void fillRoundedRect(SDL_Renderer* renderer, int x, int y, int w, int h, int r) {
    fillCircle(renderer, x + r, y + r, r);
    fillCircle(renderer, x + w - r, y + r, r);
    fillCircle(renderer, x + r, y + h - r, r);
    fillCircle(renderer, x + w - r, y + h - r, r);
    SDL_Rect centerRect = { x + r, y + r, w - r * 2, h - r * 2};
    SDL_RenderFillRect(renderer, &centerRect);
    SDL_Rect leftRect = { x, y + r, r, h - r * 2 };
    SDL_RenderFillRect(renderer, &leftRect);
    SDL_Rect rightRect = { x + w - r, y + r, r + 1, h - r * 2 + 1};
    SDL_RenderFillRect(renderer, &rightRect);
    SDL_Rect topRect = { x + r, y, w - r * 2, r };
    SDL_RenderFillRect(renderer, &topRect);
    SDL_Rect bottomRect = { x + r, y + h - r, w - r * 2 + 1, r + 1 };
    SDL_RenderFillRect(renderer, &bottomRect);
}
}

namespace colors {
const SDL_Color RED{255, 0, 0};
const SDL_Color ORANGE{255, 128, 0};
const SDL_Color YELLOW{255, 255, 0};
const SDL_Color GREEN{0, 255, 0};
const SDL_Color BLUE{0, 0, 255};
const SDL_Color PURPLE{128, 0, 255};
const SDL_Color MAGENTA{255, 0, 255};
const SDL_Color BLACK{0, 0, 0};
const SDL_Color GRAY{128, 128, 128};
const SDL_Color LIGHT_GRAY{200, 200, 200};
const SDL_Color WHITE{255, 255, 255};
};