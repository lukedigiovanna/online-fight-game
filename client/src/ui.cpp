#include "ui.h"

#include "renderutils.h"
#include "font.h"

ui::UIElement::UIElement() {

}

ui::Button::Button(const std::string& fontFamily, const std::string& text, int x, int y, int size) : fontFamily(fontFamily), text(text), x(x), y(y), size(size) {

} 

void ui::Button::render(SDL_Renderer* renderer) const {
    TTF_Font* font = Fonts::getFont(fontFamily, 18);
    int textWidth = renderutils::getTextWidth(font, text, size);
    SDL_Rect rect = { x, y, textWidth, size };
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
    SDL_RenderDrawRect(renderer, &rect);
    renderutils::drawText(renderer, font, text, 50, 50, size, textColor);
}