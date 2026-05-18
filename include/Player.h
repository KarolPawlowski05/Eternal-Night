#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <cmath>

class  Player : public GameObject {
private:
    sf::RectangleShape sprite;  // Kształt gracza

    float speed;                // Prędkość liniowa w px/s
    sf::Vector2f lastDirection; // Ostatni kierunek ruchu
public:
    Player(float x, float y);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    sf::Vector2f getLastDirection() const { return lastDirection; }
};

#endif