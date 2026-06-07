#include "Player.h"
#include "Config.h"
#include <algorithm>
#include<AssetManager.h>

// Konstruktor
Player::Player(float x, float y)
    : GameObject(x, y), animation(), combat(), movement(), health(),
    progression(), stats(), weapons(), godMode(false) {

    loadTextures();

    // Sprite gracza
    animation.sprite.setOrigin(PlayerConfig::FRAME_WIDTH / 2.f, PlayerConfig::FRAME_HEIGHT / 2.f);
    animation.sprite.setPosition(position);
    animation.sprite.setTextureRect(sf::IntRect(0, 0, PlayerConfig::FRAME_WIDTH, PlayerConfig::FRAME_HEIGHT));

    // Sprite ataku
    animation.attackSprite.setOrigin(PlayerConfig::FRAME_WIDTH / 2.f, PlayerConfig::FRAME_HEIGHT / 2.f);
    animation.attackSprite.setPosition(position);
    animation.attackSprite.setTextureRect(sf::IntRect(0, 0, PlayerConfig::FRAME_WIDTH, PlayerConfig::FRAME_HEIGHT));

    // Konfiguracja paska HP
    health.hpBarBackground.setSize(sf::Vector2f(50.f, 6.f));
    health.hpBarBackground.setFillColor(sf::Color::Black);
    health.hpBarBackground.setOrigin(25.f, 3.f);

    health.hpBarForeground.setSize(sf::Vector2f(50.f, 6.f));
    health.hpBarForeground.setFillColor(sf::Color::Green);
    health.hpBarForeground.setOrigin(25.f, 3.f);

    // Konfiguracja paska cooldownu ataku
    health.attackCooldownBarBackground.setSize(sf::Vector2f(50.f, 4.f));
    health.attackCooldownBarBackground.setFillColor(sf::Color(50, 50, 0));
    health.attackCooldownBarBackground.setOrigin(25.f, 2.f);

    health.attackCooldownBarForeground.setSize(sf::Vector2f(50.f, 4.f));
    health.attackCooldownBarForeground.setFillColor(sf::Color::Yellow);
    health.attackCooldownBarForeground.setOrigin(25.f, 2.f);

    // Konfiguracja paska przeładowania kuszy
    health.specialCooldownBarBackground.setSize(sf::Vector2f(50.f, 4.f));
    health.specialCooldownBarBackground.setFillColor(sf::Color(50, 0, 50)); // Ciemnofioletowy tło
    health.specialCooldownBarBackground.setOrigin(25.f, 2.f);

    health.specialCooldownBarForeground.setSize(sf::Vector2f(50.f, 4.f));
    health.specialCooldownBarForeground.setFillColor(sf::Color(255, 0, 255)); // Magenta (Kusza)
    health.specialCooldownBarForeground.setOrigin(25.f, 2.f);

    // Konfiguracja wskaźnika ataku (pomocniczy hitbox)
    combat.attackIndicator.setSize(combat.attackSize);
    combat.attackIndicator.setOrigin(0.f, combat.attackSize.y / 2.f);
    combat.attackIndicator.setFillColor(sf::Color(255, 0, 0, 0));

    // Konfiguracja Aury Ognia
    weapons.fireAuraShape.setRadius(weapons.fireAuraRadius);
    weapons.fireAuraShape.setOrigin(weapons.fireAuraRadius, weapons.fireAuraRadius);
    weapons.fireAuraShape.setFillColor(sf::Color(255, 80, 0, 70));

    // Konfiguracja Orbitującego Ostrza
    auto swordTex = AssetManager::loadTexture("assets/player/attacks/orbitingSword/sword.png");
    if(swordTex) {
        swordTex->setSmooth(false);
        weapons.orbitSwordTexture = *swordTex;
        weapons.orbitSwordShape.setTexture(weapons.orbitSwordTexture);

        sf::FloatRect lb = weapons.orbitSwordShape.getLocalBounds();
        weapons.orbitSwordShape.setOrigin(lb.width / 2.f, lb.height / 2.f);
    } else {
        // Loguj błąd, ale nie crashuj gry
        printf("Blad: Nie znaleziono tekstury miecza!\n");
    }
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
        auto idle = AssetManager::loadTexture(idlePaths[d]);
        auto run = AssetManager::loadTexture(runPaths[d]);

        if(idle && run) {
            idle->setSmooth(false);
            run->setSmooth(false);
            animation.textures[0][d] = idle.get();
            animation.textures[1][d] = run.get();
        } else {
            allOk = false;
        }

        for(int c = 0; c < 2; ++c) {
            auto attackTex = AssetManager::loadTexture(attackPaths[c][d]);
            if(attackTex) {
                attackTex->setSmooth(false);
                animation.attackTextures[c][d] = attackTex.get();
            } else {
                allOk = false;
            }
        }
    }

    animation.texturesLoaded = allOk;

    if(animation.texturesLoaded && animation.textures[0][0] && animation.attackTextures[0][0]) {
        animation.sprite.setTexture(*animation.textures[0][0]);
        animation.attackSprite.setTexture(*animation.attackTextures[0][0]);
    }
}

// Aktualizacja animacji ruchu
void Player::updateAnimation(const sf::Vector2f& movement, bool isMoving, float deltaTime) {
    if(!animation.texturesLoaded) return;

    // Wyznaczanie nowego stanu i kierunku
    int newState = isMoving ? 1 : 0; // 1=RUN, 0=IDLE
    int newDir = animation.animDir;

    if(isMoving) {
        // Wybieranie kierunku na podstawie dominującej osi
        if(std::abs(movement.y) >= std::abs(movement.x)) {
            newDir = (movement.y > 0.f) ? 0 : 3; // DOWN lub UP
        } else {
            newDir = (movement.x > 0.f) ? 2 : 1; // RIGHT lub LEFT
        }
    }

    // Zmiana tekstury jeśli zmienił się stan lub kierunek
    if(newState != animation.animState || newDir != animation.animDir) {
        animation.animState = newState;
        animation.animDir = newDir;
        animation.currentFrame = 0;   // Reset klatki przy zmianie animacji
        animation.frameTimer = 0.f;
        if (animation.textures[animation.animState][animation.animDir]) {
            animation.sprite.setTexture(*animation.textures[animation.animState][animation.animDir]); // Dodana *
        }
    }

    // Przesuwanie klatki animacji
    animation.frameTimer += deltaTime;
    if(animation.frameTimer >= animation.frameSpeed) {
        animation.frameTimer -= animation.frameSpeed;
        animation.currentFrame = (animation.currentFrame + 1) % PlayerConfig::FRAME_COUNT;
    }

    animation.sprite.setTextureRect(sf::IntRect(animation.currentFrame * PlayerConfig::FRAME_WIDTH, 0, PlayerConfig::FRAME_WIDTH, PlayerConfig::FRAME_HEIGHT));
}

void Player::takeDamage(int amount) {
    // Godmode odpornosc na obrażenia
    if (godMode) return;

    // Jeśli gracz robi unik lub ma aktywną nietykalność, nie otrzymuje obrażeń
    if(!movement.isDashing && health.invincibilityTimer <= 0.f) {

        // Szansa na całkowite uniknięcie ciosu (Miss)
        bool isDodged = (static_cast<float>(rand() % 100) / 100.f) < stats.dodgeChance;
        if (isDodged) return;

        // Pancerz zmniejsza obrażenia (gracz zawsze otrzyma minimum 1 dmg)
        int finalDamage = amount - stats.armor;
        if(finalDamage < 1) finalDamage = 1;

        health.hp -= finalDamage;
        health.invincibilityTimer = 1.0f;
        if(health.hp < 0) health.hp = 0;

        // Dźwięki
        if(health.hp <= 0) {
            // Dźwięk śmierci
            AssetManager::playSound("assets/audio/sfx/playerDeath.wav");
            AssetManager::stopMusic();
        } else {
            // Zwykłe obrażenia
            AssetManager::playSound("assets/audio/sfx/playerHit.wav", 20.f);
        }
    }
}

// Uruchamia hitbox, animacje i dźwięk
void Player::castAttack() {
    // Hitbox aktywny
    combat.isAttacking = true;
    combat.durationTimer = combat.attackDuration;

    // Wybieranie kieruneku na podstawie pozycji myszy
    int dir;
    if(std::abs(movement.mouseDirection.y) >= std::abs(movement.mouseDirection.x)) {
        dir = (movement.mouseDirection.y > 0.f) ? 0 : 3;
    } else {
        dir = (movement.mouseDirection.x > 0.f) ? 2 : 1;
    }

    // Ustawienie odpowiedniej tekstury (1 lub 2)
    if(animation.texturesLoaded && animation.attackTextures[animation.attackCombo][dir]) {
        animation.attackSprite.setTexture(*animation.attackTextures[animation.attackCombo][dir]);
    }

    // Dźwięk
    const char* swordSfx = (animation.attackCombo == 0) ? "assets/audio/sfx/swordAttack1.wav" : "assets/audio/sfx/swordAttack2.wav";
    AssetManager::playSound(swordSfx, 20.f);

    animation.attackCombo = (animation.attackCombo + 1) % 2; // Zmiana wariantu

    // Reset animacji
    animation.attackAnimFrame = 0;
    animation.attackAnimTimer = 0.f;
    animation.showAttackAnim = true;
    animation.attackSprite.setTextureRect(sf::IntRect(0, 0, PlayerConfig::FRAME_WIDTH, PlayerConfig::FRAME_HEIGHT));
}

void Player::update(float deltaTime) {
    // Pasywna regeneracja zdrowia
    if (stats.hpRegenRate > 0) {
        stats.hpRegenTimer += deltaTime;
        if (stats.hpRegenTimer >= 5.0f) { // Co 5 sekund
            heal(stats.hpRegenRate);
            stats.hpRegenTimer = 0.f;
        }
    }

    // Nietykalność gracza
    if(health.invincibilityTimer > 0.f) {
        health.invincibilityTimer -= deltaTime;
        sf::Color blinkColor = static_cast<int>(health.invincibilityTimer * 10) % 2 == 0 ? sf::Color(255, 255, 255, 100) : sf::Color::White;
        animation.sprite.setColor(blinkColor);
        animation.attackSprite.setColor(blinkColor);
    } else if(!movement.isDashing) {
        animation.sprite.setColor(sf::Color::White);
        animation.attackSprite.setColor(sf::Color::White);
    }

    // Kierunek myszy
    float dx = movement.mousePosition.x - position.x;
    float dy = movement.mousePosition.y - position.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if(dist > 5.f) {
        movement.mouseDirection = sf::Vector2f(dx / dist, dy /dist);
    }

    // Timery
    combat.specialTimer += deltaTime;
    movement.dashTimer += deltaTime;

    if(combat.attackTimer < combat.attackCooldown) {
        combat.attackTimer += deltaTime;
    }

    // Atak pod LPM
    bool lmbNow = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if(lmbNow && !combat.prevLMBPressed && combat.attackTimer >= combat.attackCooldown) {
        castAttack();
        combat.attackTimer = 0.f;
    }
    combat.prevLMBPressed = lmbNow;

    // Wyłącz hitbox po upływie attackDuration
    if(combat.isAttacking) {
        combat.durationTimer -= deltaTime;
        if(combat.durationTimer <= 0.f) combat.isAttacking = false;
    }

    // Animacja ataku
    if(animation.showAttackAnim) {
        animation.attackAnimTimer += deltaTime;
        if(animation.attackAnimTimer >= 0.07f) {
            animation.attackAnimTimer -= 0.07f;
            animation.attackAnimFrame++;
            if(animation.attackAnimFrame >= PlayerConfig::FRAME_COUNT) {
                // Animacja skonczona
                animation.showAttackAnim = false;
                animation.attackAnimFrame = 0;
            } else {
                animation.attackSprite.setTextureRect(sf::IntRect(animation.attackAnimFrame * PlayerConfig::FRAME_WIDTH, 0, PlayerConfig::FRAME_WIDTH, PlayerConfig::FRAME_HEIGHT));
            }
        }
        animation.attackSprite.setPosition(position);
    }

    // Strzał kuszą
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::R) && combat.specialTimer >= combat.specialCooldown) {
        combat.wantsToShootSpecial = true;
        combat.specialTimer = 0.f;
        // Dźwięk
        AssetManager::playSound("assets/audio/sfx/crossbow.wav");
    }

    // Unik
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && movement.dashTimer >= movement.dashCooldown && !movement.isDashing) {
        movement.isDashing = true;
        movement.dashDurationTimer = movement.dashDuration;
        movement.dashTimer = 0.f;
        AssetManager::playSound("assets/audio/sfx/dash.wav");
    }

    // Ruch gracza
    sf::Vector2f movement_vec(0.f, 0.f);
    bool isMoving = false;

    if(movement.isDashing) {
        position += movement.lastDirection * movement.dashSpeed * deltaTime;
        sf::Color dashColor(180, 180, 255, 200);
        animation.sprite.setColor(dashColor);
        animation.attackSprite.setColor(dashColor);
        movement.dashDurationTimer -= deltaTime;

        if(movement.dashDurationTimer <= 0.f) {
            movement.isDashing = false;
            animation.sprite.setColor(sf::Color::White);
            animation.attackSprite.setColor(sf::Color::White);
        }
    } else {
        // Odczyt 8 kierunków
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement_vec.y -= 1.f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement_vec.y += 1.f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement_vec.x -= 1.f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement_vec.x += 1.f;

        // Długość wektora
        float length = std::sqrt(movement_vec.x * movement_vec.x + movement_vec.y * movement_vec.y);

        if(length > 0.f) {
            movement_vec /= length; // Normalizacja wektora ruchu
            movement.lastDirection = movement_vec;

            // Ruch oparty o deltaTime
            position += movement_vec * movement.speed * deltaTime;

            // Do animacji
            isMoving = true;
        }
    }

    // Dźwięki
    if(isMoving) {
        movement.footstepTimer += deltaTime;

        if(movement.footstepTimer >= movement.footstepInterval) {
            if(movement.footstepVariant == 0) {
                AssetManager::playSound("assets/audio/sfx/step1.wav", 2.f);
                movement.footstepVariant = 1;
            } else {
                AssetManager::playSound("assets/audio/sfx/step2.wav", 2.f);
                movement.footstepVariant = 0;
            }
            movement.footstepTimer = 0.f;
        }
    } else {
        movement.footstepTimer = movement.footstepInterval;
    }

    // Animacja
    updateAnimation(movement_vec, isMoving, deltaTime);


    animation.sprite.setPosition(position);

    // Aktualizacja wskaźnika ataku
    float angle = std::atan2(movement.mouseDirection.y, movement.mouseDirection.x) * 180.f / 3.13159265f;
    combat.attackIndicator.setPosition(position);
    combat.attackIndicator.setRotation(angle);

    // Aktualizacja Pasywnych Broni
    if (weapons.hasFireAura) {
        weapons.fireAuraShape.setPosition(position);
    }
    if (weapons.hasOrbitingSword) {
        weapons.orbitAngle += weapons.orbitSpeed * deltaTime;
        if (weapons.orbitAngle >= 360.f) weapons.orbitAngle -= 360.f;
    }

    // Pasek HP
    float hpPercent = std::max(0.f, static_cast<float>(health.hp) / health.maxHp);
    health.hpBarForeground.setSize(sf::Vector2f(50.f * hpPercent, 6.f));
    // ZMIANA KOLORU DLA GOD MODE
    if (godMode) {
        health.hpBarForeground.setFillColor(sf::Color(255, 215, 0)); // Złoty
    } else {
        health.hpBarForeground.setFillColor(sf::Color::Green); // Normalny zielony
    }
    health.hpBarBackground.setPosition(position.x, position.y + 30.f);
    health.hpBarForeground.setPosition(position.x, position.y + 30.f);

    // Pasek Cooldownu
    float cdPercent = std::min(1.f, combat.attackTimer / combat.attackCooldown);
    health.attackCooldownBarForeground.setSize(sf::Vector2f(50.f * cdPercent, 4.f));
    health.attackCooldownBarBackground.setPosition(position.x, position.y + 38.f);
    health.attackCooldownBarForeground.setPosition(position.x, position.y + 38.f);

    // Pasek kuszy
    float spPercent = std::min(1.f, combat.specialTimer / combat.specialCooldown);
    health.specialCooldownBarForeground.setSize(sf::Vector2f(50.f * spPercent, 4.f));
    health.specialCooldownBarBackground.setPosition(position.x, position.y + 44.f);
    health.specialCooldownBarForeground.setPosition(position.x, position.y + 44.f);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(health.hpBarBackground);
    window.draw(health.hpBarForeground);
    window.draw(health.attackCooldownBarBackground);
    window.draw(health.attackCooldownBarForeground);
    window.draw(health.specialCooldownBarBackground);
    window.draw(health.specialCooldownBarForeground);
    if (weapons.hasFireAura) window.draw(weapons.fireAuraShape);
    if (weapons.hasOrbitingSword) {
        float angleStep = 360.f / weapons.swordCount; // Kąt między poszczególnymi mieczami
        float orbitDistance = 100.f;

        for (int i = 0; i < weapons.swordCount; ++i) {
            float currentAngle = weapons.orbitAngle + (i * angleStep);
            float rad = currentAngle * 3.14159f / 180.f;

            weapons.orbitSwordShape.setPosition(position.x + std::cos(rad) * orbitDistance, position.y + std::sin(rad) * orbitDistance);
            weapons.orbitSwordShape.setRotation(currentAngle - 45.f);

            window.draw(weapons.orbitSwordShape);
        }
    }
    if(animation.showAttackAnim) {
        window.draw(animation.attackSprite);
    } else {
        window.draw(animation.sprite);
    }
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect(position.x - 24.f, position.y - 32.f, 48.f, 68.f);
}

sf::FloatRect Player::getAttackBounds() const {
    return combat.attackIndicator.getGlobalBounds();
}

void Player::addXp(int amount) {
    progression.xp += amount;

    // Dźwięk
    AssetManager::playSound("assets/audio/sfx/getXpCrystal.wav", 5.f);
}

void Player::applyUpgrade(int choice) {
    switch (choice) {
    case 0: health.maxHp += 20; health.hp = health.maxHp; break;
    case 1: movement.speed *= 1.15f; break;
    case 2: combat.attackCooldown *= 0.90f; break;
    case 3: stats.damageBonus += 5; break;
    case 4: stats.armor += 2; break;
    case 5: stats.critChance += 0.05f; break;
    case 6: stats.vampirismChance += 0.05f; break;
    case 7: stats.pickupRadiusBonus += 40.f; break;
    case 8: stats.dodgeChance += 0.10f; break;
    case 9: combat.specialCooldown *= 0.80f; break;
    case 10: stats.hpRegenRate += 1; break;
    case 11: // AURA OGNIA
        if (!weapons.hasFireAura) { weapons.hasFireAura = true; }
        else { weapons.fireAuraRadius += 25.f; weapons.fireAuraShape.setRadius(weapons.fireAuraRadius); weapons.fireAuraShape.setOrigin(weapons.fireAuraRadius, weapons.fireAuraRadius); }
        break;
    case 12: // ORBITUJĄCE OSTRZE
        if (!weapons.hasOrbitingSword) {
            weapons.hasOrbitingSword = true;
            weapons.swordCount = 1; // Pierwszy miecz
        } else {
            weapons.swordCount++; // Dodaje kolejny miecz do okręgu
            weapons.orbitSpeed += 10.f; // Lekko przyspiesza przy okazji
        }
        break;
    case 13: // RÓŻDŻKA
        if (!weapons.hasWand) { weapons.hasWand = true; }
        else {
            weapons.wandCooldown *= 0.85f;      // -15% cooldown
            weapons.wandProjectiles += 1;       // +1 pocisk
            weapons.wandDamageBonus += 3;       // +3 dmg
        }
        break;
    }

}

void Player::heal(int amount) {
    health.hp += amount;
    if(health.hp > health.maxHp) {
        health.hp = health.maxHp;
    }
}

void Player::healFromPotion(int amount) {
    heal(amount);
    AssetManager::playSound("assets/audio/sfx/potion.wav");
}

int Player::getDamage(int baseDamage, bool* wasCrit) const {
    // Losowanie szansy na krytyk
    bool isCrit = (static_cast<float>(rand() % 100) / 100.f) < stats.critChance;
    if(wasCrit) *wasCrit = isCrit;
    int finalDamage = baseDamage + stats.damageBonus;
    return isCrit ? (finalDamage * 2) : finalDamage;
}

void Player::triggerVampirism() {
    if (stats.vampirismChance > 0.0f) {
        bool isVamp = (static_cast<float>(rand() % 100) / 100.f) < stats.vampirismChance;
        if (isVamp) {
            heal(2); // Drobne uleczenie po zabójstwie
        }
    }
}
std::vector<sf::FloatRect> Player::getOrbitingSwordsBounds() const {
    std::vector<sf::FloatRect> bounds;

    if (!weapons.hasOrbitingSword || weapons.swordCount == 0 || weapons.orbitSwordShape.getTexture() == nullptr)
        return bounds;

    float angleStep = 360.f / weapons.swordCount;
    float orbitDistance = 100.f;

    for (int i = 0; i < weapons.swordCount; ++i) {
        float currentAngle = weapons.orbitAngle + (i * angleStep);
        float rad = currentAngle * 3.14159f / 180.f;

        sf::Sprite tempSprite = weapons.orbitSwordShape;
        tempSprite.setPosition(position.x + std::cos(rad) * orbitDistance, position.y + std::sin(rad) * orbitDistance);
        tempSprite.setRotation(currentAngle - 45.f);

        bounds.push_back(tempSprite.getGlobalBounds());
    }
    return bounds;
}