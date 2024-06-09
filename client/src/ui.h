#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <functional>

namespace ui {
class UIElement {
public:
    UIElement();
    virtual void render(SDL_Renderer* renderer) const = 0;
    // return true if should stop propagating this event
    virtual bool processSDLEvent(SDL_Event& event) = 0;
};

class Button: public UIElement {
private:
    SDL_Color backgroundColor = { 200, 200, 200 };
    SDL_Color hoverColor = { 150, 150, 150 };
    SDL_Color activeColor = { 230, 230, 230 };
    SDL_Color textColor = { 255, 255, 255 };

    int x, y;
    int padding = 5;
    int borderSize = 3;
    std::string text;
    std::string fontFamily;
    int size;
    mutable int textWidth = -1; // -1 for uninitialized

    bool hover = false;
    bool active = false;

    std::function<void()> onClickCallback;
public:
    Button(const std::string& fontFamily, const std::string& text, int x, int y, int size);

    void render(SDL_Renderer* renderer) const;
    bool processSDLEvent(SDL_Event& event);

    void setOnClick(std::function<void()> onClick);
};
}