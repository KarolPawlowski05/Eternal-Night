#include "Projectile.h"

Projectile::Projectile(float startX, float startY, sf::Vector2f direction) : GameObject(startX, startY), speed(1000.f) {

    velocity = direction * speed;

    if(arrowTexture.loadFromFile("assets/player/attacks/crossbow/arrow.png")) {
        arrowSprite.setTexture(arrowTexture);
        sf::FloatRect tb = arrowSprite.getLocalBounds();
        arrowSprite.setOrigin(tb.width / 2.f, tb.height / 2.f);
    } else {
        // Error – biały prostokąt w rozmiarze strzałki (20x5)
        arrowTexture.create(20, 5);
        sf::Image img;
        img.create(20, 5, sf::Color::White);
        arrowTexture.loadFromImage(img);
        arrowSprite.setTexture(arrowTexture);
        arrowSprite.setOrigin(10.f, 2.5f);
    }

    arrowSprite.setPosition(position);

    // Ustawienie rotacji pocisku zgodnie z kierunkiem lotu
    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    arrowSprite.setRotation(angle);
}

void Projectile::update(float deltaTime) {
    // Lot pocisku
    position += velocity * deltaTime;
    arrowSprite.setPosition(position);

    // Niszczenie pocisku, gdy wyleci poza okno
    if(position.x < -50.f || position.x > 1330.f || position.y < -50.f || position.y > 770.f) {
        destroy();
    }
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(arrowSprite);
}

sf::FloatRect Projectile::getBounds() const {
    return arrowSprite.getGlobalBounds();
}