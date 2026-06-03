#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include <cmath>
#include <memory>

class Projectile : public GameObject {
private:
    std::shared_ptr<sf::Texture> arrowTexture;
    sf::Sprite arrowSprite;
    sf::Vector2f velocity;
    float speed;
    bool isEnemyOwned;
    bool isWand = false;
    sf::Vector2f homingTarget;
    bool hasTarget = false;
    float distanceTraveled = 0.f;

public:
Projectile(float startX, float startY, sf::Vector2f direction, float customSpeed = 1000.f, bool enemy = false, bool wand = false);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool getIsEnemyOwned() const { return isEnemyOwned; }
    void setHomingTarget(sf::Vector2f target) { homingTarget = target; hasTarget = true; }
    bool getIsWand() const { return isWand; }
};

#endif