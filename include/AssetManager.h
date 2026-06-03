#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

/*
 *  Singleton do cache'owania tekstur i czcionek
 *  Zapobiega kilkukrotnego ładowania tekstur
 *  Zmniejsza użycie pamięci
 */

class AssetManager {
private:
    static std::map<std::string, std::shared_ptr<sf::Texture>> textureCache;
    static std::map<std::string, std::shared_ptr<sf::Font>> fontCache;

    AssetManager() = default;

public:
    // Tekstury
    static std::shared_ptr<sf::Texture> loadTexture(const std::string& path);
    static sf::Texture* getTexture(const std::string& path);
    static void unloadTexture(const std::string& path);

    // Czcionka
    static std::shared_ptr<sf::Font> loadFont(const std::string& path);
    static sf::Font* getFont(const std::string& path);
    static void unloadFont(const std::string& path);

    static void clearAll();
};

#endif
