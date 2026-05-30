#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"

enum class ObstacleType { TREE, BUSH, ROCK };

class Obstacle : public GameObject {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    ObstacleType type;

    // Jeśli tekstura sie nie wczyta
    sf::RectangleShape fallbackShape;
    bool textureLoaded;
public:
    Obstacle(float x, float y, ObstacleType obsType);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
};

#endif