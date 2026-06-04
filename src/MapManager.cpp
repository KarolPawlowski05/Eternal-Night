#include "MapManager.h"
#include "Obstacle.h"
#include "XpCrystal.h"
#include "Bonus.h"
#include "Enemy.h"
#include "Projectile.h"
#include <cmath>
#include <cstdlib>

MapManager::MapManager() {
    if(grassTexture.loadFromFile("assets/map/background/floor/grass.png")) {
        grassBackground.setTexture(grassTexture);
        grassBackground.setScale(2.f, 2.f);
    }
    lastObstacleSpawnPos = sf::Vector2f(0.f, 0.f);
}

void MapManager::reset(sf::Vector2f playerStartPos) {
    lastObstacleSpawnPos = playerStartPos;
}

int MapManager::getTileType(int tileX, int tileY) {
    int hash = tileX * 374761393 + tileY * 668265263;
    hash = (hash ^ (hash >> 13)) * 1274126177;
    return std::abs(hash) % 100;
}

void MapManager::update(std::shared_ptr<Player>& player, std::vector<std::shared_ptr<GameObject>>& gameObjects) {
    if (!player) return;

    sf::Vector2f playerPos = player->getPosition();

    // 1. USUWANIE STARYCH OBIEKTÓW
    for (auto& obj : gameObjects) {
        if (!obj->isActive()) continue;

        if (dynamic_cast<Obstacle*>(obj.get()) || dynamic_cast<XpCrystal*>(obj.get()) ||
            dynamic_cast<Bonus*>(obj.get()) || dynamic_cast<Enemy*>(obj.get()) ||
            dynamic_cast<Projectile*>(obj.get())) {

            float dx = obj->getPosition().x - playerPos.x;
            float dy = obj->getPosition().y - playerPos.y;
            float distanceSq = dx * dx + dy * dy;

            if (distanceSq > 2500.f * 2500.f) {
                obj->destroy();
            }
        }
    }

    // 2. GENEROWANIE NOWYCH PRZESZKÓD
    float distMoved = std::hypot(playerPos.x - lastObstacleSpawnPos.x, playerPos.y - lastObstacleSpawnPos.y);

    if (distMoved > 350.f) {
        lastObstacleSpawnPos = playerPos;

        int obstaclesToSpawn = rand() % 4 + 2;
        for (int i = 0; i < obstaclesToSpawn; ++i) {
            float angle = (rand() % 360) * 3.14159f / 180.f;
            float radius = 1300.f + (rand() % 300);

            float spawnX = playerPos.x + cos(angle) * radius;
            float spawnY = playerPos.y + sin(angle) * radius;

            ObstacleType type;
            int randType = rand() % 3;
            if (randType == 0) type = ObstacleType::TREE;
            else if (randType == 1) type = ObstacleType::BUSH;
            else type = ObstacleType::ROCK;

            gameObjects.push_back(std::make_shared<Obstacle>(spawnX, spawnY, type));
        }
    }
}

void MapManager::render(sf::RenderTarget& target, const sf::View& gameView) {
    float tileSize = 64.f;

    sf::Vector2f center = gameView.getCenter();
    sf::Vector2f size = gameView.getSize();

    int leftTile   = static_cast<int>((center.x - size.x / 2.f) / tileSize) - 1;
    int rightTile  = static_cast<int>((center.x + size.x / 2.f) / tileSize) + 1;
    int topTile    = static_cast<int>((center.y - size.y / 2.f) / tileSize) - 1;
    int bottomTile = static_cast<int>((center.y + size.y / 2.f) / tileSize) + 1;

    for (int x = leftTile; x <= rightTile; ++x) {
        for (int y = topTile; y <= bottomTile; ++y) {
            getTileType(x, y);

            grassBackground.setPosition(x * tileSize, y * tileSize);
            grassBackground.setColor(sf::Color::White);
            target.draw(grassBackground);
        }
    }
}