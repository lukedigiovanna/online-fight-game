#include "ui.h"

#include "renderutils.h"

ui::UIElement::UIElement() {
    
}

ui::Button::Button(TTF_Font* font, std::string text) : font(font), text(text) {

} 

void ui::Button::render(SDL_Renderer* renderer) const {
    int textWidth = renderutils::getTextWidth(font, text, size);
    SDL_Rect rect = { 50, 50, textWidth, size };
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
    SDL_RenderDrawRect(renderer, &rect);
    renderutils::drawText(renderer, font, text, 50, 50, size, textColor);
}