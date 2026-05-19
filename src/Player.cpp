#include "Player.h"

Player::Player(float x, float y) : GameObject(x, y), speed(250.f), lastDirection(0.f, -1.f), attackSize(60.f, 60.f), attackCooldown(3.0f), attackTimer(0.f), attackDuration(0.1f), durationTimer(0.f), isAttacking(false), specialCooldown(10.0f), specialTimer(10.0f), wantsToShootSpecial(false), dashSpeed(650.f), dashCooldown(2.0f), dashTimer(2.0f), dashDuration(0.25f), dashDurationTimer(0.f), isDashing(false) {
    // Konfiguracja gracza
    sprite.setSize(sf::Vector2f(40.f, 40.f));
    sprite.setFillColor(sf::Color::Blue);
    sprite.setOrigin(20.f, 20.f);
    sprite.setPosition(position);

    // Konfiguracja wskaźnika ataku
    attackIndicator.setSize(attackSize);
    attackIndicator.setOrigin(0.f, attackSize.y / 2.f);
    attackIndicator.setFillColor(sf::Color(255, 0, 0, 40));
}

void Player::castAttack() {
    isAttacking = true;
    durationTimer = attackDuration;

    attackIndicator.setFillColor(sf::Color(255, 0, 0, 255));
}

void Player::update(float deltaTime) {
    // Timer ataku
    attackTimer += deltaTime;
    specialTimer += deltaTime;
    dashTimer += deltaTime;

    if(attackTimer >= attackCooldown) {
        castAttack();
        attackTimer = 0.f;
    }

    // Wygaszanie "błysku" ataku po upływie attackDuration
    if(isAttacking) {
        durationTimer -= deltaTime;
        if(durationTimer <= 0.f) {
            isAttacking = false;
            attackIndicator.setFillColor(sf::Color(255, 0, 0, 40));
        }
    }

    // Atak specjalny
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::R) && specialTimer >= specialCooldown) {
        wantsToShootSpecial = true;
        specialTimer = 0.f;
    }

    // Unik
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && dashTimer >= dashCooldown && !isDashing) {
        isDashing = true;
        dashDurationTimer = dashDuration;
        dashTimer = 0.f;
    }

    // Ruch gracza
    if(isDashing) {
        position += lastDirection * dashSpeed * deltaTime;
        // Testowo
        sprite.setFillColor(sf::Color(120, 120, 255));

        dashDurationTimer -= deltaTime;
        if(dashDurationTimer <= 0.f) {
            isDashing = false;
            sprite.setFillColor(sf::Color::Blue); // Testowo
        }
    } else {
        sf::Vector2f movement(0.f, 0.f);

        // Odczyt 8 kierunków
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

            // Obracanie sprite'a
            if(movement.x > 0.f) {
                sprite.setScale(1.f, 1.f); // w prawo
            } else if(movement.x < 0.f) {
                sprite.setScale(-1.f, 1.f); // w lewo
            }
        }
    }

    // Blokada wychodzenia poza okno
    if(position.x < 20.f) position.x = 20.f;
    if(position.x > 1260.f) position.x = 1260.f;
    if(position.y < 20.f) position.y = 20.f;
    if(position.y > 700.f) position.y = 700.f;

    sprite.setPosition(position);

    // Aktualizacja wskaźnika ataku
    attackIndicator.setPosition(position);
    float angle = std::atan2(lastDirection.y, lastDirection.x) * 180.f / 3.14159265f;
    attackIndicator.setRotation(angle);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(attackIndicator);
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect Player::getAttackBounds() const {
    return attackIndicator.getGlobalBounds();
}