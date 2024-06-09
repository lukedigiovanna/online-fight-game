#include "font.h"

FontManager::FontManager(const std::string& fontPath) : fontPath(fontPath) {

}

TTF_Font* FontManager::getFont(int size) const {
    // check if this size exists in the cache
    auto find = cache.find(size);
    if (find != cache.end()) {
        return find->second;
    }
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), size);
    if (font == NULL) {
        throw std::runtime_error(TTF_GetError());
    }
    cache.insert({size, font});
    return font;
}

std::map<std::string, FontManager> Fonts::registry;

void Fonts::registerFont(const std::string& fontName, const std::string& fontPath) {
    Fonts::registry.insert({fontName, FontManager(fontPath)});
}

TTF_Font* Fonts::getFont(const std::string& fontName, int size) {
    auto find = Fonts::registry.find(fontName);
    if (find == Fonts::registry.end()) {
        throw std::runtime_error("No font found with name: " + fontName);
    }
    FontManager& manager = find->second;
    return manager.getFont(size);
}