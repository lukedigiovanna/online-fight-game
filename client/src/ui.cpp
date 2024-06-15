#include "ui.h"

#include "renderutils.h"
#include "font.h"
#include "utils.h"

ui::UIElement::UIElement() {

}

ui::Button::Button(const std::string& fontFamily, const std::string& text, int x, int y, int size) : fontFamily(fontFamily), text(text), x(x), y(y), size(size) {

} 

void ui::Button::render(SDL_Renderer* renderer) const {
    if (textWidth < 0) { // calculate the textWidth once
        textWidth = renderutils::getTextWidth(fontFamily, text, size);
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
    renderutils::drawText(renderer, fontFamily, text, x, y, size, textColor);
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

ui::TextInput::TextInput(const std::string& fontFamily, int x, int y, int width, int size) :
    fontFamily(fontFamily), x(x), y(y), width(width), size(size) {

}

void ui::TextInput::render(SDL_Renderer* renderer) const {
    if (selected) {
        SDL_SetRenderDrawColor(renderer, selectedBorderColor.r, selectedBorderColor.g, selectedBorderColor.b, 255);
    }
    else if (active) {
        SDL_SetRenderDrawColor(renderer, activeBorderColor.r, activeBorderColor.g, activeBorderColor.b, 255);
    }
    else if (hover) {
        SDL_SetRenderDrawColor(renderer, hoverBorderColor.r, hoverBorderColor.g, hoverBorderColor.b, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
    }
    renderutils::fillRoundedRect(renderer, x - padding - borderSize, y - padding - borderSize, width + padding * 2 + borderSize * 2, size + padding * 2 + borderSize * 2, padding);
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
    renderutils::fillRoundedRect(renderer, x - padding, y - padding, width + padding * 2, size + padding * 2, padding);
    int textWidth = 0;
    if (text.size() > 0) {
        textWidth = renderutils::drawText(renderer, fontFamily, text, x, y, size, textColor);
    }
    if (selected) {
        float now = utils::getTime();
        float m = std::fmod(now, 0.5f);
        if (m < 0.25f) {
            SDL_Rect rect = { x + textWidth + 3, y, 2, size };
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

bool ui::TextInput::processSDLEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mx = event.motion.x;
        int my = event.motion.y;
        int l = x - padding;
        int r = x + width + padding;
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
        else if (!hover) {
            selected = false;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (active) {
            active = false;
            selected = true;
            return true;
        }
    }
    else if (event.type == SDL_KEYDOWN) {
        if (selected) {
            if (event.key.keysym.sym == SDLK_BACKSPACE) {
                if (text.size() > 0) {
                    text = text.substr(0, text.size() - 1);
                }
            }
            else {
                text += event.key.keysym.sym;
            }
        }
    }
    return false;
}