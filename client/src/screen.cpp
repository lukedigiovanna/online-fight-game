#include "screen.h"

#include "client.h"

uint32_t Screen::WIDTH = 0;
uint32_t Screen::HEIGHT = 0;

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

void MainScreen::load() {

}

void MainScreen::render(SDL_Renderer* renderer) const {
    
}

bool MainScreen::processEvent(SDL_Event& ev) {
    return false;
}

GameScreen::GameScreen() {

}

void GameScreen::load() {
    Client::connect();
}

void GameScreen::render(SDL_Renderer* renderer) const {
    renderutils::drawTextWithOutline(renderer, "Roboto-Bold", "this is the game.", 200, 200, 50, colors::BLUE, colors::LIGHT_GRAY, 3);
    Client::render(renderer);
}

bool GameScreen::processEvent(SDL_Event& ev) {
    Client::processSDLEvent(ev);
    return false;
}