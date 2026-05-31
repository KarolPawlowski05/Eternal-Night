#include "Enemy.h"
#include <Player.h>
#include <cmath>
#include <algorithm>

Enemy::Enemy(float x, float y, EnemyType type, std::shared_ptr<Player> player, float hpMult, float speedMult) : GameObject(x, y)
{

    this->target = player;
    this->type = type;

    damageCooldown = 0.f;
    fireAuraCooldown = 0.f;
    swordCooldown = 0.f;
    abilityTimer = static_cast<float>(rand() % 300) / 100.f;
    isKamikaze = false;
    isGhost = false;
    isVampire = false;

    //konfiguracja wygladu wroga
    sprite.setSize(sf::Vector2f(30.f, 30.f));
    sprite.setOrigin(15.f, 15.f);
    sprite.setPosition(position);

    //Predkosc i kolor wroga zależne od typu
    if (type == EnemyType::TRUPOJADY) {
        baseColor = sf::Color::Magenta; speed = 50.f * speedMult; maxHp = 50 * hpMult;
    }
    else if (type == EnemyType::OGROWATE) {
        baseColor = sf::Color::Green; speed = 25.f * speedMult; maxHp = 150 * hpMult;
    }
    else if (type == EnemyType::UPIOR) {
        baseColor = sf::Color::White; speed = 65.f * speedMult; maxHp = 25 * hpMult;
    }
    else if (type == EnemyType::KAMIKAZE) {
        baseColor = sf::Color(255, 165, 0); // Pomarańczowy
        speed = 85.f * speedMult; maxHp = 20 * hpMult;
        isKamikaze = true;
    }
    else if (type == EnemyType::CIEN) {
        baseColor = sf::Color(80, 80, 80); // Ciemnoszary
        speed = 55.f * speedMult; maxHp = 40 * hpMult;
    }
    else if (type == EnemyType::WAMPIR) {
        baseColor = sf::Color(200, 0, 0); // Karmazynowy
        speed = 60.f * speedMult; maxHp = 70 * hpMult;
        isVampire = true;
    }
    else if (type == EnemyType::ZJAWA) {
        baseColor = sf::Color::Cyan; // Błękitny (zjawa)
        speed = 45.f * speedMult; maxHp = 30 * hpMult;
        isGhost = true;
    }

    hp = maxHp;
    baseSpeed = speed;
    sprite.setFillColor(baseColor);

    // Paski HP
    hpBarBackground.setSize(sf::Vector2f(40.f, 5.f));
    hpBarBackground.setFillColor(sf::Color::Black);
    hpBarBackground.setOrigin(20.f, 2.5f);

    hpBarForeground.setSize(sf::Vector2f(40.f, 5.f));
    hpBarForeground.setFillColor(sf::Color::Red);
    hpBarForeground.setOrigin(20.f, 2.5f);
}

int Enemy::takeDamage(int amount, int damageType) {
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
        sprite.setFillColor(sf::Color::Red);
        if (hp <= 0) destroy();
        return amount;
    }
    return 0; // Zablokowane przez cooldown
}

int Enemy::getXpReward() const {
    switch (type) {
    case EnemyType::TRUPOJADY: return 10;
    case EnemyType::UPIOR:     return 15;
    case EnemyType::OGROWATE:  return 30;
    case EnemyType::KAMIKAZE:  return 5;
    case EnemyType::CIEN:      return 20;
    case EnemyType::WAMPIR:    return 25;
    case EnemyType::ZJAWA:     return 25;
    default:                   return 10;
    }
}

void Enemy::update(float deltaTime){
    if (damageCooldown > 0.f) damageCooldown -= deltaTime;
    if (fireAuraCooldown > 0.f) fireAuraCooldown -= deltaTime;
    if (swordCooldown > 0.f) swordCooldown -= deltaTime;
    if(damageCooldown <= 0.f && fireAuraCooldown <= 0.f && swordCooldown <= 0.f) {
        sprite.setFillColor(baseColor);
    }

    if (target) {
        sf::Vector2f playerPos = target->getPosition();

        // Obliczamy różnicę pozycji
        float dx = playerPos.x - position.x;
        float dy = playerPos.y - position.y;

        // Obliczamy odległość z twierdzenia Pitagorasa
        float distance = sqrt(dx * dx + dy * dy);

        //Skile specjlane wrogów
        // 1. Szarża Upiora (doskok co 3 sekundy)
        if (type == EnemyType::UPIOR) {
            abilityTimer += deltaTime;
            if (abilityTimer >= 3.0f && abilityTimer <= 3.3f) speed = baseSpeed * 3.5f;
            else if (abilityTimer > 3.3f) { speed = baseSpeed; abilityTimer = 0.f; }
        }

        // 2. Niewidzialność Cienia (ukrywanie się)
        if (type == EnemyType::CIEN) {
            if (distance < 150.f) {
                sprite.setFillColor(sf::Color(80, 80, 80, 255)); // W pełni widoczny
                hpBarBackground.setFillColor(sf::Color(0,0,0,255));
                hpBarForeground.setFillColor(sf::Color(255,0,0,255));
            } else {
                sprite.setFillColor(sf::Color(80, 80, 80, 30));  // Prawie niewidzialny (Alpha=30)
                hpBarBackground.setFillColor(sf::Color(0,0,0,0)); // Ukrywa paski HP
                hpBarForeground.setFillColor(sf::Color(0,0,0,0));
            }
        }

        if (distance > 0) {
            // Normalizujemy wektor żeby wróg nie poruszał się szybciej po skosie
            float dirX = dx / distance;
            float dirY = dy / distance;

            // Zmieniamy matematyczną pozycję wroga
            position.x += dirX * speed * deltaTime;
            position.y += dirY * speed * deltaTime;
        }
    }
    sprite.setPosition(position);

    float hpPercent = std::max(0.f, static_cast<float>(hp) / maxHp);
    hpBarForeground.setSize(sf::Vector2f(40.f * hpPercent, 5.f));
    hpBarBackground.setPosition(position.x, position.y + 25.f);
    hpBarForeground.setPosition(position.x, position.y + 25.f);
}

void Enemy::draw(sf::RenderWindow& window){
    window.draw(sprite);

    window.draw(hpBarBackground);
    window.draw(hpBarForeground);
}

sf::FloatRect Enemy::getBounds() const{
    return sprite.getGlobalBounds();
}