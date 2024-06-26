#include "ui.h"

#include "renderutils.h"
#include "font.h"
#include "utils.h"
#include "runtimeinfo.h"

ui::UIElement::UIElement(
    float x, float y, 
    int positionMode, 
    AlignmentMode alignmentMode) :
    d_x(x), d_y(y), 
    d_positionMode(positionMode),
    d_alignmentMode(alignmentMode) 
{

}

ui::UIElement::~UIElement() {

}

int ui::UIElement::x() const {
    if ((d_positionMode & 0b1) == ABSOLUTE_X) {
        return (int)d_x;
    }
    else {
        return (int)(d_x * RuntimeInfo::displayWidth());
    }
}

int ui::UIElement::y() const {
    if ((d_positionMode & 0b10) == ABSOLUTE_Y) {
        return (int)d_y;
    }
    else {
        return (int)(d_y * RuntimeInfo::displayHeight());
    }
}

ui::Button::Button(
        const std::string& fontFamily, 
        const std::string& text, 
        float x, float y, 
        int size,
        int positionMode,
        AlignmentMode alignmentMode) 
    : fontFamily(fontFamily), 
    text(text), 
    size(size),
    UIElement(x, y, positionMode, alignmentMode)
{

} 

void ui::Button::render(SDL_Renderer* renderer) const {
    if (textWidth < 0) { // calculate the textWidth once
        textWidth = renderutils::getTextWidth(fontFamily, text, size);
    }

    int buttonWidth = textWidth + padding * 2;
    int alignmentOffset = alignmentMode() == ALIGN_LEFT ? 0 
                        : alignmentMode() == ALIGN_CENTER ? buttonWidth / 2
                        : buttonWidth;

    renderutils::setRenderColor(renderer, borderColor);
    renderutils::fillRoundedRect(
        renderer, 
        x() - padding - borderSize - alignmentOffset, 
        y() - padding - borderSize, 
        buttonWidth + borderSize * 2, 
        size + padding * 2 + borderSize * 2, 
        padding);

    // determine what the background color should be
    if (active) {
        renderutils::setRenderColor(renderer, activeColor);
    }
    else if (hover) {
        renderutils::setRenderColor(renderer, hoverColor);
    }
    else {
        renderutils::setRenderColor(renderer, backgroundColor);
    }

    renderutils::fillRoundedRect(
        renderer,
        x() - padding - alignmentOffset,
        y() - padding,
        textWidth + padding * 2,
        size + padding * 2,
        padding);
    renderutils::drawTextWithOutline(
        renderer, 
        fontFamily, 
        text, 
        x() - alignmentOffset, y(), 
        size, 
        textColor, 
        colors::BLACK, 
        1);
}

bool ui::Button::processSDLEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mx = event.motion.x;
        int my = event.motion.y;
        int width = textWidth + padding * 2;
        int alignmentOffset = alignmentMode() == ALIGN_LEFT ? 0 
                        : alignmentMode() == ALIGN_CENTER ? width / 2
                        : width;
        int l = x() - alignmentOffset;
        int r = x() + width - alignmentOffset;
        int t = y() - padding;
        int b = y() + size + padding;
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
            if (onClickCallback) {
                onClickCallback();
            }
            active = false;
            return true;
        }
    }
    return false;
}

void ui::Button::setOnClick(std::function<void()> onClick) {
    onClickCallback = onClick;
}

ui::TextInput::TextInput(
        const std::string& fontFamily, 
        float x, float y, 
        int width, 
        int size,
        int positionMode,
        AlignmentMode alignmentMode) :
    fontFamily(fontFamily), 
    width(width), 
    size(size), 
    UIElement(x, y, positionMode, alignmentMode)
{
    
}

void ui::TextInput::render(SDL_Renderer* renderer) const {
    // determine border color
    if (selected) {
        renderutils::setRenderColor(renderer, selectedBorderColor);
    }
    else if (active) {
        renderutils::setRenderColor(renderer, activeBorderColor);
    }
    else if (hover) {
        renderutils::setRenderColor(renderer, hoverBorderColor);
    }
    else {
        renderutils::setRenderColor(renderer, borderColor);
    }

    renderutils::fillRoundedRect(
        renderer, 
        x() - padding - borderSize, 
        y() - padding - borderSize, 
        width + padding * 2 + borderSize * 2, 
        size + padding * 2 + borderSize * 2, 
        padding);
    renderutils::setRenderColor(renderer, backgroundColor);
    renderutils::fillRoundedRect(
        renderer, 
        x() - padding, 
        y() - padding, 
        width + padding * 2, 
        size + padding * 2, 
        padding);
    int textWidth = 0;
    if (text.size() > 0) {
        textWidth = renderutils::drawText(
            renderer, 
            fontFamily, 
            text, 
            x(), y(), 
            size, 
            textColor);
    }
    if (selected) {
        float now = utils::getTime();
        float m = std::fmod(now, 0.5f);
        if (m < 0.25f) {
            SDL_Rect rect = { x() + textWidth + 3, y(), 2, size };
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

bool ui::TextInput::processSDLEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mx = event.motion.x;
        int my = event.motion.y;
        int l = x() - padding;
        int r = x() + width + padding;
        int t = y() - padding;
        int b = y() + size + padding;
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
