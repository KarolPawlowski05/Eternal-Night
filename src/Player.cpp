#include "Player.h"

Player::Player(float x, float y) : GameObject(x, y), hp(100), maxHp(100), invincibilityTimer(0.f), speed(250.f), lastDirection(0.f, -1.f), attackSize(80.f, 60.f), attackCooldown(1.5f), attackTimer(0.f), attackDuration(0.1f), durationTimer(0.f), isAttacking(false), specialCooldown(5.0f), specialTimer(10.0f), wantsToShootSpecial(false), dashSpeed(650.f), dashCooldown(2.0f), dashTimer(2.0f), dashDuration(0.25f), dashDurationTimer(0.f), isDashing(false), xp(0), maxXp(20), level(1), pendingLevelUp(false), damageBonus(0), armor(0), critChance(0.05f), enemiesKilled(0), potionsCollected(0), vampirismChance(0.0f), pickupRadiusBonus(0.f), dodgeChance(0.0f), hpRegenRate(0), hpRegenTimer(0.f) {
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

        // Szansa na całkowite uniknięcie ciosu (Miss)
        bool isDodged = (static_cast<float>(rand() % 100) / 100.f) < dodgeChance;
        if (isDodged) return;

        // Pancerz zmniejsza obrażenia (gracz zawsze otrzyma minimum 1 dmg)
        int finalDamage = amount - armor;
        if(finalDamage < 1) finalDamage = 1;

        hp -= finalDamage;
        invincibilityTimer = 1.0f;
        if(hp < 0) hp = 0;
    }
}

void Player::castAttack() {
    isAttacking = true;
    durationTimer = attackDuration;

    attackIndicator.setFillColor(sf::Color(255, 0, 0, 255));
}

void Player::update(float deltaTime) {

    // Pasywna regeneracja zdrowia
    if (hpRegenRate > 0) {
        hpRegenTimer += deltaTime;
        if (hpRegenTimer >= 5.0f) { // Co 5 sekund
            heal(hpRegenRate);
            hpRegenTimer = 0.f;
        }
    }

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

void Player::addXp(int amount) {
    xp += amount;
    if(xp >= maxXp) {
        xp -= maxXp; // Przeniesienie nadmiaru XP na kolejny poziom
        level++;
        pendingLevelUp = true;
    }
}

void Player::applyUpgrade(int choice) {
    switch (choice) {
    case 0: maxHp += 20; hp = maxHp; break;      // +20 Max HP i leczenie
    case 1: speed *= 1.15f; break;               // +15% Prędkości ruchu
    case 2: attackCooldown *= 0.90f; break;      // -10% CD Ataku
    case 3: damageBonus += 5; break;             // +5 Obrażeń
    case 4: armor += 2; break;                   // +2 Pancerza
    case 5: critChance += 0.05f; break;          // +5% Szansy na krytyk
    case 6: vampirismChance += 0.05f; break;     // +5% Szansy na kradzież życia
    case 7: pickupRadiusBonus += 40.f; break;    // MAGNES: +40px do zasięgu podnoszenia
    case 8: dodgeChance += 0.10f; break;         // UNIK: +10% szansy
    case 9: specialCooldown *= 0.80f; break;     // KUSZA: -20% cooldownu
    case 10: hpRegenRate += 1; break;            // REGENERACJA: +1 HP co 5 sekund
    }
}
void Player::heal(int amount) {
    hp += amount;
    if(hp > maxHp) {
        hp = maxHp;
    }
}

int Player::getDamage(int baseDamage) const {
    // Losowanie szansy na krytyk
    bool isCrit = (static_cast<float>(rand() % 100) / 100.f) < critChance;
    int finalDamage = baseDamage + damageBonus;
    return isCrit ? (finalDamage * 2) : finalDamage;
}

void Player::triggerVampirism() {
    if (vampirismChance > 0.0f) {
        bool isVamp = (static_cast<float>(rand() % 100) / 100.f) < vampirismChance;
        if (isVamp) {
            heal(2); // Drobne uleczenie po zabójstwie
        }
    }
}