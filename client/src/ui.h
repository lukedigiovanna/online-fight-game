#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

namespace ui {
class UIElement {
public:
    UIElement();
    virtual void render(SDL_Renderer* renderer) const = 0;
};

class Button: public UIElement {
private:
    SDL_Color backgroundColor = { 200, 200, 200 };
    SDL_Color hoverColor = { 150, 150, 150 };
    SDL_Color activeColor = { 230, 230, 230 };
    SDL_Color textColor = { 255, 255, 255 };

    std::string text;
    TTF_Font* font;
    int size = 18;
public:
    Button(TTF_Font* font, std::string text);

    void render(SDL_Renderer* renderer) const;
};
}