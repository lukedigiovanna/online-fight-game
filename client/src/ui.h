#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <functional>

#include "renderutils.h"

namespace ui {
class UIElement {
public:
    enum PositionMode {
        ABSOLUTE_X = 0b0, // treat the coordinates as screen coordinates
        PROPORTIONAL_X = 0b1, // treat the coordinates as a proportion 
        ABSOLUTE_Y = 0b00,
        PROPORTIONAL_Y = 0b10,

        // 0 = absolute both
        // 1 = proportional x, absolute y
        // 2 = absolute x, proportional y
        // 3 = proportional both
    };

    enum AlignmentMode {
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT
    };
private:
    float d_x;
    float d_y;
    int d_positionMode;
    AlignmentMode d_alignmentMode;
public:
    UIElement(float x, float y, int positionMode, AlignmentMode alignmentMode);
    ~UIElement();

    virtual void render(SDL_Renderer* renderer) const = 0;
    // return true if should stop propagating this event
    virtual bool processSDLEvent(SDL_Event& event) = 0;

    int x() const;
    int y() const;

    AlignmentMode alignmentMode() const;
};

inline UIElement::AlignmentMode UIElement::alignmentMode() const {
    return d_alignmentMode;
}

class Button: public UIElement {
private:
    SDL_Color backgroundColor = colors::getColor("#32d53e");
    SDL_Color hoverColor = { 150, 150, 150 };
    SDL_Color activeColor = { 230, 230, 230 };
    SDL_Color textColor = { 255, 255, 255 };
    SDL_Color borderColor = colors::getColor("#19a921");

    int padding = 5;
    int borderSize = 3;
    std::string text;
    std::string fontFamily;
    int size;
    mutable int textWidth = -1; // -1 for uninitialized

    bool hover = false;
    bool active = false;

    std::function<void()> onClickCallback;

    int positionMode;
public:
    Button(
        const std::string& fontFamily, 
        const std::string& text, 
        float x, float y, 
        int size,
        int positionMode = ABSOLUTE_X | ABSOLUTE_Y,
        AlignmentMode alignmentMode = ALIGN_LEFT);

    void render(SDL_Renderer* renderer) const;
    bool processSDLEvent(SDL_Event& event);

    void setOnClick(std::function<void()> onClick);
};
class TextInput: public UIElement {
private:
    SDL_Color borderColor = { 128, 128, 128 };
    SDL_Color hoverBorderColor = { 80, 80, 80 };
    SDL_Color activeBorderColor = { 20, 20, 20 };
    SDL_Color selectedBorderColor = { 20, 20, 255 };
    SDL_Color backgroundColor = { 200, 200, 200 };
    SDL_Color textColor = colors::BLACK;

    std::string text;
    std::string placeholder;
    std::string fontFamily;

    int width;
    int size;
    int padding = 5;
    int borderSize = 3;

    bool hover = false;
    bool active = false;
    bool selected = false;
public:
    TextInput(
        const std::string& fontFamily, 
        float x, float y, 
        int width, 
        int size,
        int positionMode = ABSOLUTE_X | ABSOLUTE_Y,
        AlignmentMode alignmentMode = ALIGN_LEFT);

    void render(SDL_Renderer* renderer) const;
    bool processSDLEvent(SDL_Event& event);
};
}
