#pragma once

#include <SDL2/SDL.h>

#include <vector>
#include <memory>

#include "ui.h"

class Screen {
// static information about a screen
public:
    static uint32_t WIDTH;
    static uint32_t HEIGHT;
public:
    typedef ui::UIElement* UIElementPtr;
    typedef std::vector<UIElementPtr> UIElementList;
private:

    UIElementList uiElements;
public:
    Screen();

    void addUIElement(UIElementPtr element);

    // Renders the ui elements to the screen in the order they were added
    void renderUI(SDL_Renderer* renderer) const;

    // Propagates UI events to the ui elements
    // Returns true and terminates if any ui element returns true on processing
    bool processUIEvent(SDL_Event& ev);

    virtual void load() = 0;

    virtual void render(SDL_Renderer* renderer) const = 0;
    
    virtual bool processEvent(SDL_Event& ev) = 0;
};

inline void Screen::addUIElement(UIElementPtr element) {
    uiElements.push_back(element);
}

class MainScreen: public Screen {
public:
    MainScreen();

    void load();

    void render(SDL_Renderer* renderer) const;

    bool processEvent(SDL_Event& ev);
};

class GameScreen: public Screen {
public:
    GameScreen();

    void load();

    void render(SDL_Renderer* renderer) const;

    bool processEvent(SDL_Event& ev);
};