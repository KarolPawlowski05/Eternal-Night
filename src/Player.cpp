#include "Player.h"
#include <iostream>

Player::Player(float x, float y) : GameObject(x, y), hp(100), maxHp(100), invincibilityTimer(0.f), speed(250.f), lastDirection(0.f, -1.f), attackSize(80.f, 60.f), attackCooldown(1.5f), attackTimer(0.f), attackDuration(0.1f), durationTimer(0.f), isAttacking(false), specialCooldown(5.0f), specialTimer(10.0f), wantsToShootSpecial(false), dashSpeed(650.f), dashCooldown(2.0f), dashTimer(2.0f), dashDuration(0.25f), dashDurationTimer(0.f), isDashing(false) {
    // Konfiguracja gracza
    sprite.setSize(sf::Vector2f(40.f, 40.f));
    sprite.setFillColor(sf::Color::Blue);
    sprite.setOrigin(20.f, 20.f);
    sprite.setPosition(position);

    // Konfiguracja paska HP
    hpBarBackground.setSize(sf::Vector2f(50.f, 6.f));
    hpBarBackground.setFillColor(sf::Color::Black);
    hpBarBackground.setOrigin(25.f, 3.f);

    hpBarForeground.setSize(sf::Vector2f(50.f, 6.f));
    hpBarForeground.setFillColor(sf::Color::Green);
    hpBarForeground.setOrigin(25.f, 3.f);

    // Konfiguracja paska cooldownu ataku
    attackCooldownBarBackground.setSize(sf::Vector2f(50.f, 4.f));
    attackCooldownBarBackground.setFillColor(sf::Color(50, 50, 0));
    attackCooldownBarBackground.setOrigin(25.f, 2.f);

    attackCooldownBarForeground.setSize(sf::Vector2f(50.f, 4.f));
    attackCooldownBarForeground.setFillColor(sf::Color::Yellow);
    attackCooldownBarForeground.setOrigin(25.f, 2.f);

    // Konfiguracja wskaźnika ataku
    attackIndicator.setSize(attackSize);
    attackIndicator.setOrigin(0.f, attackSize.y / 2.f);
    attackIndicator.setFillColor(sf::Color(255, 0, 0, 40));
}

void Player::takeDamage(int amount) {
    // Jeśli gracz robi unik lub ma aktywną nietykalność, nie otrzymuje obrażeń
    if(!isDashing && invincibilityTimer <= 0.f) {
        hp -= amount;
        invincibilityTimer = 1.0f; // 1 sekunda nietykalności
        if(hp < 0) hp = 0; // Zabezpieczenie przed ujemnym HP
    }
}

void Player::castAttack() {
    isAttacking = true;
    durationTimer = attackDuration;

    attackIndicator.setFillColor(sf::Color(255, 0, 0, 255));
}

void Player::update(float deltaTime) {
    // Nietykalność gracza
    if(invincibilityTimer > 0.f) {
        invincibilityTimer -= deltaTime;
        // Efekt migania gracza WIP
        if(static_cast<int>(invincibilityTimer * 10) % 2 == 0) {
            sprite.setFillColor(sf::Color(0, 0, 255, 100));
        } else {
            sprite.setFillColor(sf::Color::Blue);
        }
    } else if(!isDashing) {
        sprite.setFillColor(sf::Color::Blue);
    }

    float dx = mousePosition.x - position.x;
    float dy = mousePosition.y - position.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if(dist > 5.f) {
        mouseDirection = sf::Vector2f(dx / dist, dy /dist);
    }

    // Timery
    specialTimer += deltaTime;
    dashTimer += deltaTime;

    if(attackTimer < attackCooldown) {
        attackTimer += deltaTime;
    }

    // Atak pod LPM
    bool lmbNow = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if(lmbNow && !prevLMBPressed && attackTimer >= attackCooldown) {
        castAttack();
        attackTimer = 0.f;
    }
    prevLMBPressed = lmbNow;

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
    float angle = std::atan2(mouseDirection.y, mouseDirection.x) * 180.f / 3.13159265f;
    attackIndicator.setPosition(position);
    attackIndicator.setRotation(angle);

    // Pasek HP
    float hpPercent = std::max(0.f, static_cast<float>(hp) / maxHp);
    hpBarForeground.setSize(sf::Vector2f(50.f * hpPercent, 6.f));
    hpBarBackground.setPosition(position.x, position.y + 30.f);
    hpBarForeground.setPosition(position.x, position.y + 30.f);

    // Pasek Cooldownu
    float cdPercent = std::min(1.f, attackTimer / attackCooldown);
    attackCooldownBarForeground.setSize(sf::Vector2f(50.f * cdPercent, 4.f));
    attackCooldownBarBackground.setPosition(position.x, position.y + 38.f);
    attackCooldownBarForeground.setPosition(position.x, position.y + 38.f);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(hpBarBackground);
    window.draw(hpBarForeground);
    window.draw(attackCooldownBarBackground);
    window.draw(attackCooldownBarForeground);
    window.draw(attackIndicator);
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect Player::getAttackBounds() const {
    return attackIndicator.getGlobalBounds();
}