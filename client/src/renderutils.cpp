#include "renderutils.h"

#include "font.h"

#include <sstream>

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
void setRenderColor(SDL_Renderer* renderer, const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
}
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
int drawText(SDL_Renderer* renderer, const std::string& fontFamily, const std::string& text, int x, int y, int size, const SDL_Color& color, Alignment alignment) {
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
	
	int width = textSurface->w;
	int height = textSurface->h;
	SDL_Rect dstRect;
	if (alignment == Alignment::TEXT_LEFT) {
		dstRect = {x, y, width, height};
	}
	else if (alignment == Alignment::TEXT_CENTER) {
		dstRect = {x - width / 2, y, width, height};
	}
	else if (alignment == Alignment::TEXT_RIGHT) {
		dstRect = {x + width, y, width, height};
	}

    if (SDL_RenderCopy(renderer, textTexture, NULL, &dstRect)) {
        std::cerr << "ERROR: renderutils::drawText: " << SDL_GetError() << std::endl;
        return 0;
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return width;
}
int drawTextWithOutline(SDL_Renderer* renderer, const std::string& fontFamily, const std::string& text, int x, int y, int size, const SDL_Color& color, const SDL_Color& outlineColor, int outlineWidth, Alignment alignment) {
    drawText(renderer, fontFamily, text, x - outlineWidth, y, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x + outlineWidth, y, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x, y - outlineWidth, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x, y + outlineWidth, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x - outlineWidth, y - outlineWidth, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x - outlineWidth, y + outlineWidth, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x + outlineWidth, y - outlineWidth, size, outlineColor, alignment);
    drawText(renderer, fontFamily, text, x + outlineWidth, y + outlineWidth, size, outlineColor, alignment);
    return drawText(renderer, fontFamily, text, x, y, size, color, alignment);
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

static int getIntFromHex(const std::string& s) {
    std::stringstream ss;
    ss << std::hex << s;
    int o;
    ss >> o;
    return o;
}

namespace colors {
SDL_Color getColor(const std::string& hexCode) {
    // string must be either 4 or 7 characters
    if (hexCode.size() != 4 && hexCode.size() != 7) {
        throw std::runtime_error("Hex code must be of the form #rgb or #rrggbb");
    }

    unsigned char r, g, b;
    if (hexCode.size() == 4) {
        r = getIntFromHex(hexCode.substr(1, 1));
        g = getIntFromHex(hexCode.substr(2, 1));
        b = getIntFromHex(hexCode.substr(3, 1));
    }
    else {
        r = getIntFromHex(hexCode.substr(1, 2));
        g = getIntFromHex(hexCode.substr(3, 2));
        b = getIntFromHex(hexCode.substr(5, 2));
    }

    return { r, g, b };
}

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
