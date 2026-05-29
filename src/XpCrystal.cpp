#include "XpCrystal.h"

XpCrystal::XpCrystal(float x, float y, int value) : GameObject(x, y), xpValue(value) {
    shape.setSize(sf::Vector2f(12.f, 12.f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(6.f, 6.f);
    shape.setPosition(position);
    shape.setRotation(45.f);
}

void XpCrystal::update(float deltaTime) {
}

void XpCrystal::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect XpCrystal::getBounds() const {
    return shape.getGlobalBounds();
}