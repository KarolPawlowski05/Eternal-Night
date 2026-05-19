#include "Projectile.h"

Projectile::Projectile(float startX, float startY, sf::Vector2f direction) : GameObject(startX, startY), speed(1000.f) {

    velocity = direction * speed;

    shape.setSize(sf::Vector2f(20.f, 4.f));
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(10.f, 2.f);
    shape.setPosition(position);

    // Ustawienie rotacji pocisku zgodnie z kierunkiem lotu
    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    shape.setRotation(angle);
}

void Projectile::update(float deltaTime) {
    // Lot pocisku
    position += velocity * deltaTime;
    shape.setPosition(position);

    // Niszczenie pocisku, gdy wyleci poza okno
    if(position.x < -50.f || position.x > 1330.f || position.y < -50.f || position.y > 770.f) {
        destroy();
    }
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Projectile::getBounds() const {
    return shape.getGlobalBounds();
}