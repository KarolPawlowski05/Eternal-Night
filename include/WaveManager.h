#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "Enemy.h"

class Player;

class WaveManager {
private:
    int currentWave;
    float waveTimer;
    float timeBetweenWaves;

    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<GameObject>>& gameObjects;

    void spawnWave();

public:
    WaveManager(std::shared_ptr<Player> player, std::vector<std::shared_ptr<GameObject>>& objects);

    // Wywołuj tylko w stanie PLAYING
    bool update(float deltaTime);

    void reset();

    int getCurrentWave()        const { return currentWave; }
    float getWaveTimer()        const { return waveTimer; }
    float getTimeBetweenWaves() const { return timeBetweenWaves; }
    float getSecondsToNextWave()const { return timeBetweenWaves - waveTimer; }
};

#endif