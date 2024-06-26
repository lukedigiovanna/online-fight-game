#pragma once

#include <SDL_ttf.h>
#include <SDL.h>

#include <string>
#include <iostream>

namespace renderutils {
	enum Alignment {
		TEXT_LEFT,
		TEXT_CENTER,
		TEXT_RIGHT
	};

    extern void setRenderColor(
                SDL_Renderer* renderer, 
                const SDL_Color& color);
    extern int getTextWidth(
                const std::string& fontFamily, 
                const std::string& text, 
                int size);
    extern int drawText(
                SDL_Renderer* renderer, 
                const std::string& fontFamily, 
                const std::string& text, 
                int x, int y, int size, 
                const SDL_Color& color,
				Alignment alignment=Alignment::TEXT_LEFT);
    extern int drawTextWithOutline(
                SDL_Renderer* renderer, 
                const std::string& fontFamily, 
                const std::string& text, 
                int x, int y, int size, 
                const SDL_Color& color, 
                const SDL_Color& outlineColor, 
                int outlineWidth,
				Alignment alignment=Alignment::TEXT_LEFT);
    extern void fillCircle(
                SDL_Renderer* renderer, 
                int x, int y, int r);
    extern void fillRoundedRect(
                SDL_Renderer* renderer, 
                int x, int y, int w, int h, int r);
}

namespace colors {
    extern SDL_Color getColor(const std::string& hexCode);

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
