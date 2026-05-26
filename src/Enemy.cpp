#include "Enemy.h"
#include <Player.h>
#include <cmath>
#include <algorithm>

Enemy::Enemy(float x, float y, EnemyType enemyType, std::shared_ptr<Player> playerTarget) : GameObject(x, y), type(enemyType), target(playerTarget), damageCooldown(0.f) {
    //konfiguracja wygladu wroga
    sprite.setSize(sf::Vector2f(30.f, 30.f));
    sprite.setOrigin(15.f, 15.f);
    sprite.setPosition(position);

    //Predkosc i kolor wroga zależne od typu
    if(type == EnemyType::TRUPOJADY){
        sprite.setFillColor(sf::Color::Magenta);
        baseColor = sf::Color::Magenta;
        speed = 100.f;
        maxHp = 50; // Duże HP
    }
    else if (type == EnemyType::OGROWATE){
        sprite.setFillColor(sf::Color::Green);
        baseColor = sf::Color::Green;
        speed = 50.f;
        maxHp = 150; // Ogromne HP
    }
    else if (type == EnemyType::UPIOR){
        sprite.setFillColor(sf::Color::White);
        baseColor = sf::Color::White;
        speed = 180.f;
        maxHp = 20; // Małe HP
    }

    hp = maxHp;
    sprite.setFillColor(baseColor);

    // Paski HP
    hpBarBackground.setSize(sf::Vector2f(40.f, 5.f));
    hpBarBackground.setFillColor(sf::Color::Black);
    hpBarBackground.setOrigin(20.f, 2.5f);

    hpBarForeground.setSize(sf::Vector2f(40.f, 5.f));
    hpBarForeground.setFillColor(sf::Color::Red);
    hpBarForeground.setOrigin(20.f, 2.5f);
}

void Enemy::takeDamage(int amount) {
    if (damageCooldown <= 0.f) {
        hp -= amount;
        damageCooldown = 0.5f; // Wróg jest "odporny" na kolejne hity przez 0.5s
        sprite.setFillColor(sf::Color::Red); // Efekt wizualny trafienia

        if (hp <= 0) {
            destroy();
        }
    }
}

int Enemy::getXpReward() const {
    if (type == EnemyType::TRUPOJADY) return 10;
    if (type == EnemyType::OGROWATE) return 20;
    if (type == EnemyType::UPIOR) return 5;
    return 10;
}

void Enemy::update(float deltaTime){
    if (damageCooldown > 0.f) {
        damageCooldown -= deltaTime;
        if(damageCooldown <= 0.f) {
            sprite.setFillColor(baseColor);
        }
    }

    if (target) {
        sf::Vector2f playerPos = target->getPosition();

        // Obliczamy różnicę pozycji
        float dx = playerPos.x - position.x;
        float dy = playerPos.y - position.y;

        // Obliczamy odległość z twierdzenia Pitagorasa
        float distance = sqrt(dx * dx + dy * dy);

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