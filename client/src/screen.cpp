#include "screen.h"

#include "runtimeinfo.h"
#include "client.h"

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
	renderutils::drawTextWithOutline(
			renderer,
			"Roboto-Bold",
			"Fight Game",
			RuntimeInfo::displayWidth() / 2,
			100,
			30,
			colors::BLUE,
			colors::WHITE,
			2,
			renderutils::Alignment::TEXT_CENTER);  
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
