#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"
#include <memory>

enum class ObstacleType { TREE, BUSH, ROCK };

class Obstacle : public GameObject {
private:
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    ObstacleType type;

    float scale;

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