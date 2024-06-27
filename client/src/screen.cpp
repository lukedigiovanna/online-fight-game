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

bool Screen::processUIEvent(const SDL_Event& ev) {
    for (const auto& uiElement : uiElements) {
        if (uiElement->processSDLEvent(ev)) {
            return true;
        }
    }
    return false;
}

MainScreen::MainScreen() {
    ui::Button* playButton = new ui::Button(
        "Roboto-Bold", 
        "Play", 
        0.5f, 255.0f, 
        30, 
        ui::UIElement::PositionMode::PROPORTIONAL_X | 
        ui::UIElement::PositionMode::ABSOLUTE_Y,
        ui::UIElement::AlignmentMode::ALIGN_CENTER);
  
    playButton->setOnClick([]() {
    	ScreenManager::loadScreen("game");
    });
  
    addUIElement(playButton);
}

void MainScreen::load() {

}

void MainScreen::unload() {

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

bool MainScreen::processEvent(const SDL_Event& ev) {
    return false;
}

GameScreen::GameScreen() {

}

void GameScreen::load() {
    Client::connect();
}

void GameScreen::unload() {
    // disconnect
}

void GameScreen::render(SDL_Renderer* renderer) const {
    renderutils::drawTextWithOutline(renderer, "Roboto-Bold", "this is the game.", 200, 200, 50, colors::BLUE, colors::LIGHT_GRAY, 3);
    Client::render(renderer);
}

bool GameScreen::processEvent(const SDL_Event& ev) {
    Client::processSDLEvent(ev);
    return false;
}

std::unordered_map<std::string, Screen*> ScreenManager::screens;
Screen* ScreenManager::currentScreen;

void ScreenManager::loadScreen(const std::string& name) {
    auto sit = screens.find(name);
    if (sit == screens.end()) {
        throw std::runtime_error("No registered screen with name '" + name + "'");
    }
    currentScreen = sit->second;
    currentScreen->load();
}

void ScreenManager::registerScreen(const std::string& name, Screen* screen) {
    screens.insert({name, screen});
}

void ScreenManager::processEvent(const SDL_Event& ev) {
    if (currentScreen == nullptr) {
        return;
    }
    
    // exploits short circuiting
    // if UI event is true then don't run regular event.
    currentScreen->processUIEvent(ev) || currentScreen->processEvent(ev);
}

void ScreenManager::render(SDL_Renderer* renderer) {
    if (currentScreen == nullptr) {
        return;
    }

    currentScreen->render(renderer);
    currentScreen->renderUI(renderer);
}