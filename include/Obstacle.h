#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"

enum class ObstacleType { DRZEWO, KRZAK, GLAZ };

class Obstacle : public GameObject {
private:
    sf::RectangleShape sprite;
    ObstacleType type;
public:
    Obstacle(float x, float y, ObstacleType obsType);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
};

#endif