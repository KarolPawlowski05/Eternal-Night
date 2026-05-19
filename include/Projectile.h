#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include <cmath>

class Projectile : public GameObject {
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float speed;

public:
    Projectile(float startX, float startY, sf::Vector2f direction);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
};

#endif