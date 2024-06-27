#pragma once

#include <SDL2/SDL.h>

#include <vector>
#include <unordered_map>
#include <memory>

#include "ui.h"

class Screen {
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
    bool processUIEvent(const SDL_Event& ev);

    virtual void load() = 0;
    virtual void unload() = 0;

    virtual void render(SDL_Renderer* renderer) const = 0;
    
    virtual bool processEvent(const SDL_Event& ev) = 0;
};

inline void Screen::addUIElement(UIElementPtr element) {
    uiElements.push_back(element);
}

class MainScreen: public Screen {
public:
    MainScreen();

    void load();

    void unload();

    void render(SDL_Renderer* renderer) const;

    bool processEvent(const SDL_Event& ev);
};

class GameScreen: public Screen {
public:
    GameScreen();

    void load();

    void unload();

    void render(SDL_Renderer* renderer) const;

    bool processEvent(const SDL_Event& ev);
};

class ScreenManager {
private:
    static std::unordered_map<std::string, Screen*> screens;
    static Screen* currentScreen;
public:
    static void loadScreen(const std::string& name);
    static void registerScreen(const std::string& name, Screen* screen);
    
    static void processEvent(const SDL_Event& ev);
    static void render(SDL_Renderer* renderer);
};