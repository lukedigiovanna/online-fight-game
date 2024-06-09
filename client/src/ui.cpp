#include "ui.h"

#include "renderutils.h"
#include "font.h"

ui::UIElement::UIElement() {

}

ui::Button::Button(const std::string& fontFamily, const std::string& text, int x, int y, int size) : fontFamily(fontFamily), text(text), x(x), y(y), size(size) {

} 

void ui::Button::render(SDL_Renderer* renderer) const {
    TTF_Font* font = Fonts::getFont(fontFamily, size);
    if (textWidth < 0) { // calculate the textWidth once
        textWidth = renderutils::getTextWidth(font, text, size);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    renderutils::fillRoundedRect(renderer, x - padding - borderSize, y - padding - borderSize, textWidth + padding * 2 + borderSize * 2, size + padding * 2 + borderSize * 2, padding);
    if (active) {
        SDL_SetRenderDrawColor(renderer, activeColor.r, activeColor.g, activeColor.b, 255);
    }
    else if (hover) {
        SDL_SetRenderDrawColor(renderer, hoverColor.r, hoverColor.g, hoverColor.b, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
    }
    renderutils::fillRoundedRect(renderer, x - padding, y - padding, textWidth + padding * 2, size + padding * 2, padding);
    renderutils::drawText(renderer, font, text, x, y, size, textColor);
}

bool ui::Button::processSDLEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mx = event.motion.x;
        int my = event.motion.y;
        int l = x - padding;
        int r = x + textWidth + padding;
        int t = y - padding;
        int b = y + size + padding;
        if (mx >= l && mx <= r && my >= t && my <= b) {
            hover = true;
            return true;
        }
        else {
            hover = false;
            active = false;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (hover && event.button.button == 1) {
            active = true;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (active) {
            // button clicked!
            onClickCallback();
            active = false;
            return true;
        }
    }
    return false;
}

void ui::Button::setOnClick(std::function<void()> onClick) {
    onClickCallback = onClick;
}