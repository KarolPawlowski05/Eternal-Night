#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "Enemy.h"
#include "Boss.h"

class Player;

class WaveManager {
private:
    int currentWave;
    float waveTimer;
    float timeBetweenWaves;

    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<GameObject>>& gameObjects;

    void spawnWave();

    int bossesSpawned;
    std::shared_ptr<Boss> activeBoss;

    bool spawningEnabled = true;

public:
    WaveManager(std::shared_ptr<Player> player, std::vector<std::shared_ptr<GameObject>>& objects);

    // Wywołuj tylko w stanie PLAYING
    bool update(float deltaTime);

    void reset();

    int getCurrentWave()        const { return currentWave; }
    float getWaveTimer()        const { return waveTimer; }
    float getTimeBetweenWaves() const { return timeBetweenWaves; }
    float getSecondsToNextWave()const { return timeBetweenWaves - waveTimer; }
    // 0 = Dusiciel, 1 = Zjawa, 2 = Wampir, 3 = Golem
    void forceSpawnBoss(int bossIndex);

    std::shared_ptr<Boss> getActiveBoss() const { return activeBoss; }
    void toggleSpawning();

};

#endif