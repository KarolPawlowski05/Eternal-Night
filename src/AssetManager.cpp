#include "AssetManager.h"
#include <iostream>
#include <vector>

std::map<std::string, std::shared_ptr<sf::Texture>> AssetManager::textureCache;
std::map<std::string, std::shared_ptr<sf::SoundBuffer>> AssetManager::soundBufferCache;
static std::vector<sf::Sound> soundPool(32);
static size_t nextSoundIdx = 0;
sf::Music AssetManager::music;
static std::string currentMusicPath = "";
std::map<std::string, std::shared_ptr<sf::Font>> AssetManager::fontCache;

// Tekstury
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

// Dźwięki
std::shared_ptr<sf::SoundBuffer> AssetManager::loadSoundBuffer(const std::string& path) {
    auto it = soundBufferCache.find(path);
    if(it != soundBufferCache.end()) {
        return it->second; // Dźwięk jest już w pamięci, zwracamy wskaźnik
    }

    auto buffer = std::make_shared<sf::SoundBuffer>();
    if(!buffer->loadFromFile(path)) {
        std::cerr << "Failed to laod sound buffer: " << path << std::endl;
        return nullptr;
    }

    soundBufferCache[path] = buffer;
    return buffer;
}

// Muzyka
void AssetManager::playMusic(const std::string& path, bool loop, float volume) {
    if(currentMusicPath == path) return;
    if(!music.openFromFile(path)) {
        std::cerr << "Failed to open music: " << path << std::endl;
        return;
    }
    currentMusicPath = path;
    music.setLoop(loop);
    music.setVolume(volume);
    music.play();
}

void AssetManager::stopMusic() {
    music.stop();
    currentMusicPath = "";
}

void AssetManager::setMusicVolume(float volume) {
    music.setVolume(volume);
}

void AssetManager::playSound(const std::string& path, float volume) {
    // Poberanie buforu z cache
    auto buffer = loadSoundBuffer(path);
    if(!buffer) return;

    // Szukanie zatrzymanego slotu
    for(auto& s : soundPool) {
        if(s.getStatus() == sf::Sound::Stopped) {
            s.setBuffer(*buffer);
            s.setVolume(volume);
            s.play();
            return;
        }
    }

    // tylko gdy wszystkie 32 sloty grają jednocześnie
    soundPool[nextSoundIdx].setBuffer(*buffer);
    soundPool[nextSoundIdx].setVolume(volume);
    soundPool[nextSoundIdx].play();
    nextSoundIdx = (nextSoundIdx + 1) % soundPool.size();
}

// Czcionka
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
    soundBufferCache.clear();
    fontCache.clear();
}