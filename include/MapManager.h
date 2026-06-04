#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "Player.h"

class MapManager {
private:
    sf::Texture grassTexture;
    sf::Sprite grassBackground;
    sf::Vector2f lastObstacleSpawnPos;

    int getTileType(int tileX, int tileY);
public:
    MapManager();

    // Resetuje punkt odniesienia przy restarcie gry
    void reset(sf::Vector2f playerStartPos);

    // Czyści stare obiekty i generuje nowe
    void update(std::shared_ptr<Player>& player, std::vector<std::shared_ptr<GameObject>>& gameObjects);

    // Rysuje trawę (wymaga podania okna/celu oraz kamery)
    void render(sf::RenderTarget& target, const sf::View& gameView);
};

#endif