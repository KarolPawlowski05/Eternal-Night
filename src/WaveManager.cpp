#include "WaveManager.h"
#include  "Player.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

WaveManager::WaveManager(std::shared_ptr<Player> p, std::vector<std::shared_ptr<GameObject>>& objects)
    : currentWave(1), waveTimer(20.f), timeBetweenWaves(20.f), player(p), gameObjects(objects) {}

void WaveManager::reset() {
    currentWave         = 1;
    timeBetweenWaves    = 20.f;
    waveTimer           = 20.f;
}

bool WaveManager::update(float deltaTime) {
    waveTimer += deltaTime;

    if (waveTimer >= timeBetweenWaves) {
        spawnWave();
        currentWave++;
        waveTimer = 0.f;
        return true;
    }
    return false;
}

void WaveManager::spawnWave() {
    int enemiesToSpawn = 8 + (currentWave * 2);
    float hpM    = 1.0f + (currentWave * 0.15f);
    float speedM = 1.0f + (currentWave * 0.04f);

    sf::Vector2f center = player->getPosition();
    constexpr float SPAWN_RADIUS = 700.f;

    for (int i = 0; i < enemiesToSpawn; ++i) {
        float angle  = (rand() % 360) * 3.14159f / 180.f;
        float spawnX = center.x + std::cos(angle) * SPAWN_RADIUS;
        float spawnY = center.y + std::sin(angle) * SPAWN_RADIUS;

        EnemyType typ;
        int typeChance = rand() % 100;

        if (typeChance < 40) {
            typ = EnemyType::TRUPOJADY;
        } else {
            int unlockedTypes = std::min(currentWave, 6);
            int roll = rand() % unlockedTypes;

            switch (roll) {
            case 0:  typ = EnemyType::UPIOR;    break;
            case 1:  typ = EnemyType::OGROWATE;  break;
            case 2:  typ = EnemyType::KAMIKAZE;  break;
            case 3:  typ = EnemyType::CIEN;      break;
            case 4:  typ = EnemyType::WAMPIR;    break;
            default: typ = EnemyType::ZJAWA;     break;
            }
        }

        gameObjects.push_back(
            std::make_shared<Enemy>(spawnX, spawnY, typ, player, hpM, speedM));
    }
}