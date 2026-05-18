#include "Player.h"

Player::Player(float x, float y) : GameObject(x, y), speed(250.f), lastDirection(0.f, -1.f) {
    //Konfiguracja gracza
    sprite.setSize(sf::Vector2f(40.f, 40.f));
    sprite.setFillColor(sf::Color::Blue);
    sprite.setOrigin(20.f, 20.f);
    sprite.setPosition(position);
}

void Player::update(float deltaTime) {
    sf::Vector2f movement(0.f, 0.f);

    // Odczyt 8 kierunków - F1
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += 1.f;

    // Długość wektora
    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);

    if(length > 0.f) {
        movement /= length; // Normalizacja wektora ruchu
        lastDirection = movement;

        // Ruch oparty o deltaTime
        position += movement * speed * deltaTime;

        // Blokada wychodzenia poza okno
        if(position.x < 20.f) position.x = 20.f;
        if(position.x > 1260.f) position.x = 1260.f;
        if(position.y < 20.f) position.y = 20.f;
        if(position.y > 700.f) position.y = 700.f;

        sprite.setPosition(position);

        // Obracanie sprite'a
        if(movement.x > 0.f) {
            sprite.setScale(1.f, 1.f); // w prawo
        } else if(movement.x < 0.f) {
            sprite.setScale(-1.f, 1.f); // w lewo
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}