#include "Obstacle.h"

Obstacle::Obstacle(float x, float y, ObstacleType obsType) : GameObject(x, y), type(obsType) {
    // Różne wymiary i kolory dla różnych przeszkód
    if (type == ObstacleType::DRZEWO) {
        sprite.setSize(sf::Vector2f(40.f, 80.f));
        sprite.setFillColor(sf::Color(34, 139, 34)); // Ciemnozielone drzewo
    } else if (type == ObstacleType::KRZAK) {
        sprite.setSize(sf::Vector2f(50.f, 50.f));
        sprite.setFillColor(sf::Color(0, 255, 127)); // Jasnozielony krzak
    } else if (type == ObstacleType::GLAZ) {
        sprite.setSize(sf::Vector2f(60.f, 60.f));
        sprite.setFillColor(sf::Color(105, 105, 105)); // Szary głaz
    }

    // Ustawienie punktu centralnego na podstawie wylosowanego rozmiaru
    sprite.setOrigin(sprite.getSize().x / 2.f, sprite.getSize().y / 2.f);
    sprite.setPosition(position);
}

void Obstacle::update(float deltaTime) {
    // Przeszkoda się nie rusza
}

void Obstacle::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Obstacle::getBounds() const {
    return sprite.getGlobalBounds();
}