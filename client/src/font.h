#pragma once

#include <SDL_ttf.h>

#include <string>
#include <map>

class FontManager {
private:
    const std::string fontPath;
    mutable std::map<int, TTF_Font*> cache;
public:
    FontManager(const std::string& fontPath);

    TTF_Font* getFont(int size) const;
};

// static class to manage font families and easily retrieve fonts
class Fonts {
private:
    static std::map<std::string, FontManager> registry;
public:
    static void registerFont(const std::string& fontName, const std::string& fontPath);
    static TTF_Font* getFont(const std::string& fontName, int size);
};