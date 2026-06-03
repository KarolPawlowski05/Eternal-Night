#include "AssetManager.h"
#include <iostream>

std::map<std::string, std::shared_ptr<sf::Texture>> AssetManager::textureCache;
std::map<std::string, std::shared_ptr<sf::Font>> AssetManager::fontCache;

std::shared_ptr<sf::Texture> AssetManager::loadTexture(const std::string& path) {
    // Sprawdzenie czy już załadowana
    auto it = textureCache.find(path);
    if(it != textureCache.end()) {
        return it->second;
    }

    // Ładowanie nowej tekstury
    auto texture = std::make_shared<sf::Texture>();
    if(!texture->loadFromFile(path)) {
        std::cerr << "Failed to load a texture: " << path << std::endl;
        return nullptr;
    }

    textureCache[path] = texture;
    return texture;
}

sf::Texture* AssetManager::getTexture(const std::string& path) {
    auto it = textureCache.find(path);
    if(it != textureCache.end()) {
        return it->second.get();
    }
    return nullptr;
}

void AssetManager::unloadTexture(const std::string &path) {
    textureCache.erase(path);
}

std::shared_ptr<sf::Font> AssetManager::loadFont(const std::string &path) {
    auto it = fontCache.find(path);
    if(it != fontCache.end()) {
        return it->second;
    }

    auto font = std::make_shared<sf::Font>();
    if(!font->loadFromFile(path)) {
        std::cerr << "Failed to load font: " << path << std::endl;
        return nullptr;
    }

    fontCache[path] = font;
    return font;
}

sf::Font* AssetManager::getFont(const std::string &path) {
    auto it = fontCache.find(path);
    if(it != fontCache.end()) {
        return it->second.get();
    }
    return nullptr;
}

void AssetManager::unloadFont(const std::string &path) {
    fontCache.erase(path);
}

void AssetManager::clearAll() {
    textureCache.clear();
    fontCache.clear();
}