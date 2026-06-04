#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Player.h"

class CollisionSystem {
private:
    void spawnDamageNumber(float x, float y, int damage, bool isCrit, const sf::Font& font, std::vector<std::shared_ptr<GameObject>>& newObjects);

public:
    CollisionSystem() = default;

    // Główna funkcja sprawdzająca kolizje
    void update(std::shared_ptr<Player>& player, std::vector<std::shared_ptr<GameObject>>& gameObjects, const sf::Font& font);
};

#endif