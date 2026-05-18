#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>

class GameObject {
protected:
    sf::Vector2f position;
    bool active; // Jeśli false, obiekt zostanie usunięty

public:
    GameObject(float x, float y) : position(x, y), active(true) {}
    virtual ~GameObject() = default; // Do usuwania obiektów pochodnych

    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;

    bool isActive() const { return active; }
    void destroy() {active = false; }
    sf::Vector2f getPosition() const { return position; }
};

#endif