#include "screen.h"

Screen::Screen() {

}

void Screen::renderUI(SDL_Renderer* renderer) const {
    for (const auto& uiElement : uiElements) {
        uiElement->render(renderer);
    }
}

bool Screen::processUIEvent(SDL_Event& ev) {
    for (const auto& uiElement : uiElements) {
        if (uiElement->processSDLEvent(ev)) {
            return true;
        }
    }
    return false;
}

MainScreen::MainScreen() {

}

void MainScreen::render(SDL_Renderer* renderer) const {
    
}

bool MainScreen::processEvent(SDL_Event& ev) {
    return false;
}