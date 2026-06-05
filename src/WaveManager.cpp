#include "WaveManager.h"
#include  "Player.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

WaveManager::WaveManager(std::shared_ptr<Player> p, std::vector<std::shared_ptr<GameObject>>& objects)
    : currentWave(1), waveTimer(20.f), timeBetweenWaves(20.f), player(p), gameObjects(objects), bossesSpawned(0), activeBoss(nullptr) {}

void WaveManager::reset() {
    currentWave         = 1;
    timeBetweenWaves    = 20.f;
    waveTimer           = 20.f;

    //RESETOWANIE BOSSA
    bossesSpawned = 0;
    activeBoss = nullptr;
}

bool WaveManager::update(float deltaTime) {
    waveTimer += deltaTime;
    bool waveSpawned = false;
    if (waveTimer >= timeBetweenWaves) {
        spawnWave();
        //SYSTEM SPAWNOWANIA BOSSÓW NA KONKRETNYCH FALACH
        bool spawnBoss = false;
        BossType bType = BossType::ROOT_STRANGLER;
        float scaling = 1.0f;

        // Bossowie przypisani do konkretnych etapów rozgrywki
        if (currentWave == 3) {
            spawnBoss = true; bType = BossType::ROOT_STRANGLER;
        } else if (currentWave == 6) {
            spawnBoss = true; bType = BossType::STORM_WRAITH;
        } else if (currentWave == 10) {
            spawnBoss = true; bType = BossType::VAMPIRE_PRINCE;
        } else if (currentWave == 15) {
            spawnBoss = true; bType = BossType::CHAOS_GOLEM;
        }
        // Nieskończona rozgrywka po fali 15 - losowy boss co 5 fal, coraz silniejszy
        else if (currentWave > 15 && currentWave % 5 == 0) {
            spawnBoss = true;
            bType = static_cast<BossType>(rand() % 4);
            scaling = 1.0f + ((currentWave - 15) * 0.10f); // +10% statystyk z każdym loopem
        }

        // Jeśli to odpowiednia fala i nie ma aktualnie aktywnego bossa na mapie
        if (spawnBoss && activeBoss == nullptr) {
            bossesSpawned++;
            // Spawnuje się z dala od gracza
            float spawnX = player->getPosition().x + 1000.f;
            float spawnY = player->getPosition().y + 1000.f;

            activeBoss = std::make_shared<Boss>(spawnX, spawnY, bType, player, scaling);
            gameObjects.push_back(activeBoss);
        }
        currentWave++;
        waveTimer = 0.f;
        waveSpawned = true;
    }

    // Czyszczenie wskaźnika, jeśli gracz pokonał bossa
    if (activeBoss && !activeBoss->isActive()) {
        activeBoss = nullptr;
    }

    // Odbieranie ataków i sługusów stworzonych przez umiejętności Bossa
    if (activeBoss) {
        auto bossSpawns = activeBoss->extractSpawns();
        for (auto& spawnedObject : bossSpawns) {
            gameObjects.push_back(spawnedObject);
        }
    }
    return waveSpawned;
}

void WaveManager::spawnWave() {
    if (!spawningEnabled) return;
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
void WaveManager::toggleSpawning() {
    spawningEnabled = !spawningEnabled;

    if (!spawningEnabled) {
        for (auto& obj : gameObjects) {
            // dynamic_cast<Enemy*> znajdzie tylko zwykłych przeciwników (ominie Bossa, pociski i kryształy)
            if (auto enemy = dynamic_cast<Enemy*>(obj.get())) {
                enemy->destroy();
            }
        }
    }
}
void WaveManager::forceSpawnBoss(int bossIndex) {
    // Spawnuje bossa tylko jeśli żadnego obecnie nie ma na mapie
    if (activeBoss == nullptr) {
        bossesSpawned++;

        // Spawnuje się z dala od gracza (1000px)
        float spawnX = player->getPosition().x + 1000.f;
        float spawnY = player->getPosition().y + 1000.f;

        // Zabezpieczenie, by indeks na pewno był w przedziale od 0 do 3
        if (bossIndex < 0 || bossIndex > 3) bossIndex = 0;

        BossType bType = static_cast<BossType>(bossIndex);
        float scaling = 1.0f + (bossesSpawned * 0.15f);

        activeBoss = std::make_shared<Boss>(spawnX, spawnY, bType, player, scaling);
        gameObjects.push_back(activeBoss);
    }
}