#include "Enemy.h"
#include "Player.h"
#include "Config.h"
#include "AssetManager.h"
#include <cmath>
#include <algorithm>

Enemy::Enemy(float x, float y, EnemyType type, std::shared_ptr<Player> player, float hpMult, float speedMult) : GameObject(x, y)
{

    this->target = player;
    this->type = type;
    this->currentState = EnemyState::CHASING; //Domyślny stan po spawnie

    damageCooldown = 0.f;
    fireAuraCooldown = 0.f;
    swordCooldown = 0.f;
    abilityTimer = static_cast<float>(rand() % 300) / 100.f;

    isFaded = false;
    isBatForm = false;
    readyToThrow = false;
    facingLeft = false;
    pulseTimer = 0.f;

    // Flagi specjalne - tylko typ decyduje
    isKamikaze =    (type == EnemyType::KAMIKAZE);
    isGhost =       (type == EnemyType::ZJAWA);
    isVampire =     (type == EnemyType::WAMPIR);

    // Z EnemyConfig
    int typeId = static_cast<int>(type);
    const auto& cfg = EnemyConfig::ENEMY_STATS.at(typeId);

    baseColor   = cfg.color;
    speed       = cfg.baseSpeed * speedMult;
    baseSpeed   = speed;
    maxHp       = static_cast<int>(cfg.baseHp * hpMult);
    hp          = maxHp;
    enemySz     = cfg.size;

    // Konfiguracja fallbackShape
    fallbackShape.setSize(sf::Vector2f(enemySz, enemySz));
    fallbackShape.setOrigin(enemySz / 2.f, enemySz / 2.f);
    fallbackShape.setPosition(position);
    fallbackShape.setFillColor(baseColor);

    // Konfiguracja sprite'a
    enemyTexture = AssetManager::loadTexture(cfg.texturePath);

    if(enemyTexture) {
        textureLoaded = true;
        enemySprite.setTexture(*enemyTexture);

        // Ustawienie środka
        sf::FloatRect bounds = enemySprite.getLocalBounds();
        enemySprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

        // Dopasowanie wielkości tekstury do rozmiaru hitboxa
        baseScale = 1.0f;
        enemySprite.setScale(baseScale, baseScale);
        enemySprite.setPosition(position);
    } else {
        textureLoaded = false;
        baseScale = 1.0f;
    }

    // Paski HP
    hpBarBackground.setSize(sf::Vector2f(40.f, 5.f));
    hpBarBackground.setFillColor(sf::Color::Black);
    hpBarBackground.setOrigin(20.f, 2.5f);

    hpBarForeground.setSize(sf::Vector2f(40.f, 5.f));
    hpBarForeground.setFillColor(sf::Color::Red);
    hpBarForeground.setOrigin(20.f, 2.5f);
}

void Enemy::updateSpriteVisuals() {
    // Brak zniekształceń sprite'a
    sf::Color spriteColor = sf::Color::White;
    // Kształ zapasowy ma kolor z config.cpp
    sf::Color shapeColor = baseColor;

    float currentScale = baseScale;
    float currentSz = enemySz;

    // Specjalne zachowania
    if(isBatForm) { // Wampir
        spriteColor = sf::Color(150, 0, 150);
        shapeColor = sf::Color(150, 0, 150);
        currentScale = baseScale * 0.5f;
        currentSz = 14.f;
    } else if (type == EnemyType::CIEN) { // Cień
        if(isFaded) {
            int flicker = static_cast<int>(abilityTimer * 10) % 2;
            spriteColor = sf::Color(80, 80, 80, flicker == 0 ? 30 : 80);
            shapeColor = spriteColor;
        } else {
            // Zarządzanie przezroczystością na podstawie dystansu
            if (target) {
                float dx = target->getPosition().x - position.x;
                float dy = target->getPosition().y - position.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                spriteColor = sf::Color(80, 80, 80, distance < 150.f ? 255 : 30);
                shapeColor = spriteColor;
            }
        }
    }

    // Błysk otrzymywanych obrażeń
    if(damageCooldown > 0.f || fireAuraCooldown > 0.f || swordCooldown > 0.f) {
        spriteColor = sf::Color::Red;
        shapeColor = sf::Color::Red;
    }

    // Orientacja sprite'a
    float scaleX = facingLeft ? -currentScale : currentScale;

    // Aplikowanie zmian na odpowiedni kształt
    if(textureLoaded) {
        enemySprite.setColor(spriteColor);
        enemySprite.setScale(scaleX, currentScale);
    } else {
        fallbackShape.setFillColor(shapeColor);
        fallbackShape.setSize(sf::Vector2f(currentSz, currentSz));
        fallbackShape.setOrigin(currentSz / 2.f, currentSz / 2.f);
    }
}

int Enemy::takeDamage(int amount, int damageType) {

    // Cień podczas migotania jest odporny na dmg
    if (isFaded) {
        return 0;
    }

    bool tookDamage = false;
    if (damageType == 1) { // Typ 1: Aura Ognia
        if (fireAuraCooldown <= 0.f) {
            hp -= amount;
            fireAuraCooldown = 0.5f;
            tookDamage = true;
        }
    } else if (damageType == 2) { // Typ 2: Orbitujące Ostrze
        if (swordCooldown <= 0.f) {
            hp -= amount;
            swordCooldown = 0.5f;
            tookDamage = true;
        }
    } else { // Typ 0: Zwykłe ataki z ręki i z kuszy
        if (damageCooldown <= 0.f) {
            hp -= amount;
            damageCooldown = 0.5f;
            tookDamage = true;
        }
    }

    if (tookDamage) {
        if (hp <= 0) {
            destroy();
            AssetManager::playSound("assets/audio/sfx/EnemyDied.wav");
        } else {
            AssetManager::playSound("assets/audio/sfx/EnemyHit.wav");
        }
        return amount;
    }
    return 0; // Zablokowane przez cooldown
}

int Enemy::getXpReward() const {
    switch (type) {
    case EnemyType::TRUPOJADY: return 5;
    case EnemyType::UPIOR:     return 10;
    case EnemyType::OGROWATE:  return 20;
    case EnemyType::KAMIKAZE:  return 8;
    case EnemyType::CIEN:      return 12;
    case EnemyType::WAMPIR:    return 15;
    case EnemyType::ZJAWA:     return 15;
    default:                   return 10;
    }
}

void Enemy::update(float deltaTime){
    // Zmniejszenie cooldownów
    if (damageCooldown > 0.f) damageCooldown -= deltaTime;
    if (fireAuraCooldown > 0.f) fireAuraCooldown -= deltaTime;
    if (swordCooldown > 0.f) swordCooldown -= deltaTime;

    // Jeśli gracz zginął, wróg stoi w miejscu
    if(!target) {
        enemySprite.setPosition(position);
        fallbackShape.setPosition(position);
        updateSpriteVisuals();
        return;
    }

    sf::Vector2f playerPos = target->getPosition();

    // Obliczamy różnicę pozycji
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;

    // Obliczamy odległość z twierdzenia Pitagorasa
    float distance = sqrt(dx * dx + dy * dy);

    abilityTimer += deltaTime;

    switch (currentState) {
    case EnemyState::CHASING: {
        if (type == EnemyType::UPIOR && abilityTimer >= 5.0f) {
            currentState = EnemyState::SPECIAL_ACTION;
            // Szarża zawsze w gracza, ignoruje ścieżki A*
            if (distance > 0) dashDir = sf::Vector2f(dx / distance, dy / distance);
            else dashDir = sf::Vector2f(1.f, 0.f);

            abilityTimer = 0.f;
        } else if (type == EnemyType::CIEN && abilityTimer >= 4.0f) {
            currentState = EnemyState::SPECIAL_ACTION;
        } else if (type == EnemyType::OGROWATE && abilityTimer >= 8.0f) {
            currentState = EnemyState::SPECIAL_ACTION;
        } else if (type == EnemyType::WAMPIR && hp < maxHp * 0.5f && !isBatForm) {
            currentState = EnemyState::SPECIAL_ACTION;
        }

        if (distance > 0) {
            float dirX = dx / distance;
            float dirY = dy / distance;

            // Sprite odwróci się tylko, jeśli ruch w bok jest wyraźny
            if (dirX < -0.1f) facingLeft = true;
            else if (dirX > 0.1f) facingLeft = false;

            // 2. BEZWŁADNOŚĆ (Płynny start, stop i skręcanie)
            sf::Vector2f desiredVelocity(dirX * speed, dirY * speed);

            //Płynne dążenie obecnej prędkości do pożądanej
            velocity.x += (desiredVelocity.x - velocity.x) * 5.0f * deltaTime;
            velocity.y += (desiredVelocity.y - velocity.y) * 5.0f * deltaTime;

            sf::Vector2f finalVelocity = velocity + externalForce;

            position.x += finalVelocity.x * deltaTime;
            position.y += finalVelocity.y * deltaTime;
        }
        break;
    }

    case EnemyState::SPECIAL_ACTION: {
        if (type == EnemyType::UPIOR) {
            // FAZA 1: Przygotowanie (Telegraphing) - od 0.0s do 0.4s
            if (abilityTimer < 0.4f) {
                // Upiór "zbiera siły", stoi w miejscu.
                // To daje graczowi szansę na reakcję i niweluje efekt zacinania się

                // Zwracamy go twarzą w stronę, w którą zaraz skoczy
                facingLeft = (dashDir.x < 0);
            }
            // FAZA 2: Właściwa Szarża - od 0.4s do 0.6s (bardzo dynamiczna)
            else if (abilityTimer >= 0.4f && abilityTimer < 0.6f) {
                float dashSpeed = baseSpeed * 6.0f; // Podkręcamy prędkość

                sf::Vector2f moveVelocity(dashDir.x * dashSpeed, dashDir.y * dashSpeed);

                //Upiór nadal musi być odpychany przez inne potwory podczas dasha
                moveVelocity += externalForce;

                position.x += moveVelocity.x * deltaTime;
                position.y += moveVelocity.y * deltaTime;
            }
            // FAZA 3: Powrót do zwykłego pościgu
            else {
                abilityTimer = 0.f;
                currentState = EnemyState::CHASING;
            }
        } else if (type == EnemyType::CIEN) {
            if (abilityTimer < 5.0f) {
                isFaded = true;
                hpBarBackground.setFillColor(sf::Color(0,0,0,0));
                hpBarForeground.setFillColor(sf::Color(0,0,0,0));

                // Cień musi śledzić gracza będąc niewidzialnym
                if (distance > 0) {
                    float dirX = dx / distance;
                    float dirY = dy / distance;
                    facingLeft = (dirX < 0);
                    position.x += dirX * speed * deltaTime;
                    position.y += dirY * speed * deltaTime;
                }
            } else {
                isFaded = false;
                abilityTimer = 0.f;
                currentState = EnemyState::CHASING;
            }
        } else if (type == EnemyType::OGROWATE) {
            readyToThrow = true;
            abilityTimer = 0.f;
            currentState = EnemyState::CHASING;
        } else if (type == EnemyType::WAMPIR) {
            isBatForm = true;
            speed = baseSpeed * 1.9f;
            currentState = EnemyState::CHASING;
        }
        break;
    }
    }

    if (type == EnemyType::CIEN && currentState != EnemyState::SPECIAL_ACTION) {
        if (distance < 150.f) {
            hpBarBackground.setFillColor(sf::Color(0,0,0,255));
            hpBarForeground.setFillColor(sf::Color(255,0,0,255));
        } else {
            hpBarBackground.setFillColor(sf::Color(0,0,0,0));
            hpBarForeground.setFillColor(sf::Color(0,0,0,0));
        }
    }

    // Płynne tłumienie siły odpychającej niezależne od FPS
    float friction = 10.0f;
    externalForce.x -= externalForce.x * friction * deltaTime;
    externalForce.y -= externalForce.y * friction * deltaTime;

    enemySprite.setPosition(position);
    fallbackShape.setPosition(position);

    enemySprite.setPosition(position);
    fallbackShape.setPosition(position);

    // Zaaplikowanie wszystkich zmian wizualnych z danej klatki
    updateSpriteVisuals();

    float hpPercent = std::max(0.f, static_cast<float>(hp) / maxHp);
    hpBarForeground.setSize(sf::Vector2f(40.f * hpPercent, 5.f));
    hpBarBackground.setPosition(position.x, position.y + 25.f);
    hpBarForeground.setPosition(position.x, position.y + 25.f);
}

void Enemy::draw(sf::RenderWindow& window){
    if(textureLoaded) {
        window.draw(enemySprite);
    } else {
        window.draw(fallbackShape);
    }

    window.draw(hpBarBackground);
    window.draw(hpBarForeground);
}

sf::FloatRect Enemy::getBounds() const{
    if(textureLoaded) {
        return enemySprite.getGlobalBounds();
    }
    return fallbackShape.getGlobalBounds();
}