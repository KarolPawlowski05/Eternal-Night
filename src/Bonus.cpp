#include "Bonus.h"

Bonus::Bonus(float x, float y, BonusType bonusType) : GameObject(x, y), type(bonusType) {
    sprite.setSize(sf::Vector2f(20.f, 20.f));
    sprite.setOrigin(10.f, 10.f);
    sprite.setPosition(position);

    // Wygląd zależy od typu bonusu (Mikstura HP, Mikstura niewrażliwości itp. WIP)
    if (type == BonusType::POTION) {
        sprite.setFillColor(sf::Color::Magenta); // Mikstura
    }
}

void Bonus::update(float deltaTime) {
    // Bonus leży w miejscu i czeka na zebranie
}

void Bonus::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Bonus::getBounds() const {
    return sprite.getGlobalBounds();
}

BonusType Bonus::getType() const {
    return type;
}