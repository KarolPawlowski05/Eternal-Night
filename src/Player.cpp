#include "Player.h"

// Konstruktor
Player::Player(float x, float y)
    : GameObject(x, y),
    // Animacja ruchu
    currentFrame(0), frameTimer(0.f), frameSpeed(0.1f), animState(AnimState::IDLE), animDir(AnimDir::DOWN), texturesLoaded(false),
    // Animacja ataku
    attackAnimFrame(0), attackAnimTimer(0.f), showAttackAnim(false), attackCombo(0),
    // Inne
    hp(100), maxHp(100), invincibilityTimer(0.f),
    speed(250.f),
    lastDirection(0.f, -1.f),
    attackSize(80.f, 60.f), attackCooldown(0.7f), attackTimer(0.f), attackDuration(0.3f), durationTimer(0.f), isAttacking(false), specialCooldown(5.0f), specialTimer(10.0f), wantsToShootSpecial(false),
    dashSpeed(650.f), dashCooldown(2.0f), dashTimer(2.0f), dashDuration(0.25f), dashDurationTimer(0.f), isDashing(false),
    xp(0), maxXp(20), level(1),
    damageBonus(0), armor(0), critChance(0.05f), enemiesKilled(0), potionsCollected(0), vampirismChance(0.0f), pickupRadiusBonus(0.f), dodgeChance(0.0f), hpRegenRate(0), hpRegenTimer(0.f)
{
    loadTextures();

    // Sprite gracza
    sprite.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    sprite.setPosition(position);
    sprite.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));

    // Sprite ataku
    attackSprite.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    sprite.setPosition(position);
    sprite.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));

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

    // Konfiguracja wskaźnika ataku (pomocniczy hitbox)
    attackIndicator.setSize(attackSize);
    attackIndicator.setOrigin(0.f, attackSize.y / 2.f);
    attackIndicator.setFillColor(sf::Color(255, 0, 0, 0));

    // Konfiguracja Aury Ognia
    hasFireAura = false;
    fireAuraRadius = 100.f;
    fireAuraShape.setRadius(fireAuraRadius);
    fireAuraShape.setOrigin(fireAuraRadius, fireAuraRadius);
    fireAuraShape.setFillColor(sf::Color(255, 80, 0, 70));

    // Konfiguracja Orbitującego Ostrza
    hasOrbitingSword = false;
    orbitAngle = 0.f;
    orbitSpeed = 200.f; // 200 stopni na sekundę
    if(orbitSwordTexture.loadFromFile("assets/player/attacks/orbitingSword/sword.png")) {
        orbitSwordTexture.setSmooth(false);
        orbitSwordShape.setTexture(orbitSwordTexture);
        sf::FloatRect lb = orbitSwordShape.getLocalBounds();
        orbitSwordShape.setOrigin(lb.width / 2.f, lb.height / 2.f);
    }

    // Konfiguracja paska przeładowania kuszy
    specialCooldownBarBackground.setSize(sf::Vector2f(50.f, 4.f));
    specialCooldownBarBackground.setFillColor(sf::Color(50, 0, 50)); // Ciemnofioletowy tło
    specialCooldownBarBackground.setOrigin(25.f, 2.f);

    specialCooldownBarForeground.setSize(sf::Vector2f(50.f, 4.f));
    specialCooldownBarForeground.setFillColor(sf::Color(255, 0, 255)); // Magenta (Kusza)
    specialCooldownBarForeground.setOrigin(25.f, 2.f);
}

// Ładowanie tekstur
void Player::loadTextures() {
    // Kolejność: DOWN=0, LEFT=1, RIGHT=2, UP=3
    const char* idlePaths[4] = {
        "assets/player/idle/idle_down.png",
        "assets/player/idle/idle_left.png",
        "assets/player/idle/idle_right.png",
        "assets/player/idle/idle_up.png"
    };
    const char* runPaths[4] = {
        "assets/player/run/run_down.png",
        "assets/player/run/run_left.png",
        "assets/player/run/run_right.png",
        "assets/player/run/run_up.png"
    };
    // attack1 i attack2
    const char* attackPaths[2][4] = {
        {
            "assets/player/attacks/baseAttack/attack1_down.png",
            "assets/player/attacks/baseAttack/attack1_left.png",
            "assets/player/attacks/baseAttack/attack1_right.png",
            "assets/player/attacks/baseAttack/attack1_up.png"
        },
        {
            "assets/player/attacks/baseAttack/attack2_down.png",
            "assets/player/attacks/baseAttack/attack2_left.png",
            "assets/player/attacks/baseAttack/attack2_right.png",
            "assets/player/attacks/baseAttack/attack2_up.png"
        }
    };

    bool allOk = true;
    for(int d = 0; d < 4; ++d) {
        if(!textures[0][d].loadFromFile(idlePaths[d])) allOk = false;
        if(!textures[1][d].loadFromFile(runPaths[d])) allOk = false;
        for(int c = 0; c < 2; ++c) {
            if(!attackTextures[c][d].loadFromFile(attackPaths[c][d])) allOk = false;
        }
    }
    texturesLoaded = allOk;

    if(texturesLoaded) {
        sprite.setTexture(textures[0][static_cast<int>(AnimDir::DOWN)]);
        attackSprite.setTexture(attackTextures[0][static_cast<int>(AnimDir::DOWN)]);
    }
}

// Aktualizacja animacji ruchu
void Player::updateAnimation(const sf::Vector2f& movement, bool isMoving, float deltaTime) {
    if(!texturesLoaded) return;

    // Wyznaczanie nowego stanu i kierunku
    AnimState newState = isMoving ? AnimState::RUN : AnimState::IDLE;
    AnimDir newDir = animDir;

    if(isMoving) {
        // Wybieranie kierunku na podstawie dominującej osi
        if(std::abs(movement.y) >= std::abs(movement.x)) {
            newDir = (movement.y > 0.f) ? AnimDir::DOWN : AnimDir::UP;
        } else {
            newDir = (movement.x > 0.f) ? AnimDir::RIGHT : AnimDir::LEFT;
        }
    }

    // Zmiana tekstury jeśli zmienił się stan lub kierunek
    if(newState != animState || newDir != animDir) {
        animState = newState;
        animDir = newDir;
        currentFrame = 0;   // Reset klatki przy zmianie animacji
        frameTimer = 0.f;
        sprite.setTexture(textures[static_cast<int>(animState)][static_cast<int>(animDir)]);
    }

    // Przesuwanie klatki animacji
    frameTimer += deltaTime;
    if(frameTimer >= frameSpeed) {
        frameTimer -= frameSpeed;
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
    }

    sprite.setTextureRect(sf::IntRect(currentFrame * FRAME_W, 0, FRAME_W, FRAME_H));
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

// Uruchamia hitbox i animacje
void Player::castAttack() {
    // Hitbox aktywny
    isAttacking = true;
    durationTimer = attackDuration;

    // Wybieranie kieruneku na podstawie pozycji myszy
    AnimDir dir;
    if(std::abs(mouseDirection.y) >= std::abs(mouseDirection.x)) {
        dir = (mouseDirection.y > 0.f) ? AnimDir::DOWN : AnimDir::UP;
    } else {
        dir = (mouseDirection.x > 0.f) ? AnimDir::RIGHT : AnimDir::LEFT;
    }

    // Ustawienie odpowiedniej tekstury (1 lub 2)
    if(texturesLoaded) {
        attackSprite.setTexture(attackTextures[attackCombo][static_cast<int>(dir)]);
    }
    attackCombo = (attackCombo + 1) % 2; // Zmiana wariantu

    // Reset animacji
    attackAnimFrame = 0;
    attackAnimTimer = 0.f;
    showAttackAnim = true;
    attackSprite.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));
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
        sf::Color blinkColor = static_cast<int>(invincibilityTimer * 10) % 2 == 0 ? sf::Color(255, 255, 255, 100) : sf::Color::White;
        sprite.setColor(blinkColor);
        attackSprite.setColor(blinkColor);
    } else if(!isDashing) {
        sprite.setColor(sf::Color::White);
        attackSprite.setColor(sf::Color::White);
    }

    // Kierunek myszy
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

    // Wyłącz hitbox po upływie attackDuration
    if(isAttacking) {
        durationTimer -= deltaTime;
        if(durationTimer <= 0.f) isAttacking = false;
    }

    // Animacja ataku
    if(showAttackAnim) {
        attackAnimTimer += deltaTime;
        if(attackAnimTimer >= ATTACK_ANIM_SPEED) {
            attackAnimTimer -= ATTACK_ANIM_SPEED;
            attackAnimFrame++;
            if(attackAnimFrame >= FRAME_COUNT) {
                // Animacja skonczona
                showAttackAnim = false;
                attackAnimFrame = 0;
            } else {
                attackSprite.setTextureRect(sf::IntRect(attackAnimFrame * FRAME_W, 0, FRAME_W, FRAME_H));
            }
        }
        attackSprite.setPosition(position);
    }

    // Strzał kuszą
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
    sf::Vector2f movement(0.f, 0.f);
    bool isMoving = false;

    if(isDashing) {
        position += lastDirection * dashSpeed * deltaTime;
        sf::Color dashColor(180, 180, 255, 200);
        sprite.setColor(dashColor);
        attackSprite.setColor(dashColor);
        dashDurationTimer -= deltaTime;

        if(dashDurationTimer <= 0.f) {
            isDashing = false;
            sprite.setColor(sf::Color::White);
            attackSprite.setColor(sf::Color::White);
        }
    } else {
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

            // Do animacji
            isMoving = true;
        }
    }

    // Animacja
    updateAnimation(movement, isMoving, deltaTime);

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

    // Aktualizacja Pasywnych Broni
    if (hasFireAura) {
        fireAuraShape.setPosition(position);
    }
    if (hasOrbitingSword) {
        orbitAngle += orbitSpeed * deltaTime;
        if (orbitAngle >= 360.f) orbitAngle -= 360.f;

        float rad = orbitAngle * 3.14159f / 180.f;
        float orbitDistance = 100.f; // Jak daleko od gracza lata miecz
        orbitSwordShape.setPosition(position.x + std::cos(rad) * orbitDistance, position.y + std::sin(rad) * orbitDistance);
        orbitSwordShape.setRotation(orbitAngle - 45.f); // Ostrze zwrócone w stronę lotu
    }

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

    // Pasek kuszy
    float spPercent = std::min(1.f, specialTimer / specialCooldown);
    specialCooldownBarForeground.setSize(sf::Vector2f(50.f * spPercent, 4.f));
    specialCooldownBarBackground.setPosition(position.x, position.y + 44.f);
    specialCooldownBarForeground.setPosition(position.x, position.y + 44.f);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(hpBarBackground);
    window.draw(hpBarForeground);
    window.draw(attackCooldownBarBackground);
    window.draw(attackCooldownBarForeground);
    window.draw(specialCooldownBarBackground);
    window.draw(specialCooldownBarForeground);
    if (hasFireAura) window.draw(fireAuraShape);
    if (hasOrbitingSword) window.draw(orbitSwordShape);
    if(showAttackAnim) {
        window.draw(attackSprite);
    } else {
        window.draw(sprite);
    }
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect(position.x - 24.f, position.y - 32.f, 48.f, 68.f);
}

sf::FloatRect Player::getAttackBounds() const {
    return attackIndicator.getGlobalBounds();
}

void Player::addXp(int amount) {
    xp += amount;
}

void Player::applyUpgrade(int choice) {
    switch (choice) {
    case 0: maxHp += 20; hp = maxHp; break;
    case 1: speed *= 1.15f; break;
    case 2: attackCooldown *= 0.90f; break;
    case 3: damageBonus += 5; break;
    case 4: armor += 2; break;
    case 5: critChance += 0.05f; break;
    case 6: vampirismChance += 0.05f; break;
    case 7: pickupRadiusBonus += 40.f; break;
    case 8: dodgeChance += 0.10f; break;
    case 9: specialCooldown *= 0.80f; break;
    case 10: hpRegenRate += 1; break;
    case 11: // AURA OGNIA
        if (!hasFireAura) { hasFireAura = true; }
        else { fireAuraRadius += 25.f; fireAuraShape.setRadius(fireAuraRadius); fireAuraShape.setOrigin(fireAuraRadius, fireAuraRadius); }
        break;
    case 12: // ORBITUJĄCE OSTRZE
        if (!hasOrbitingSword) { hasOrbitingSword = true; }
        else { orbitSpeed += 80.f; } // Miecz lata coraz szybciej
        break;
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