#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>

/*
 *  Singleton do cache'owania tekstur, dźwięków i czcionek
 *  Zapobiega kilkukrotnego ładowania tekstur i dźwięków
 *  Zmniejsza użycie pamięci
 */

class AssetManager {
private:
    static std::map<std::string, std::shared_ptr<sf::Texture>> textureCache;
    static std::map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBufferCache;
    static sf::Music music;
    static std::map<std::string, std::shared_ptr<sf::Font>> fontCache;

    AssetManager() = default;

public:
    // Tekstury
    static std::shared_ptr<sf::Texture> loadTexture(const std::string& path);
    static sf::Texture* getTexture(const std::string& path);
    static void unloadTexture(const std::string& path);

    // Dźwięk
    static std::shared_ptr<sf::SoundBuffer> loadSoundBuffer(const std::string& path);
    static void playSound(const std::string& path, float volume = 20.f);

    // Muzyka
    static void playMusic(const std::string& path, bool loop = true, float volume = 5.f);
    static void stopMusic();
    static void setMusicVolume(float volume);
    static bool isMusicPlaying(const std::string& path);

    // Czcionka
    static std::shared_ptr<sf::Font> loadFont(const std::string& path);
    static sf::Font* getFont(const std::string& path);
    static void unloadFont(const std::string& path);

    static void clearAll();
};

#endif
