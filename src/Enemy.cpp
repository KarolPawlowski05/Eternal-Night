#include "Enemy.h"
#include <Player.h>
#include <cmath>
#include <iostream>

Enemy::Enemy(float x, float y, EnemyType enemyType, std::shared_ptr<Player> playerTarget) : GameObject(x, y), type(enemyType), target(playerTarget){
    //konfiguracja wygladu wroga
    sprite.setSize(sf::Vector2f(30.f, 30.f));
    sprite.setOrigin(15.f, 15.f);
    sprite.setPosition(position);

    //Predkosc i kolor wroga zależne od typu
    if(type == EnemyType::TRUPOJADY){
        sprite.setFillColor(sf::Color::Magenta);
        baseColor = sf::Color::Magenta;
        speed = 100.f;
        hp = 50; // Duże HP
    }
    else if (type == EnemyType::OGROWATE){
        sprite.setFillColor(sf::Color::Green);
        baseColor = sf::Color::Green;
        speed = 50.f;
        hp = 150; // Ogromne HP
    }
    else if (type == EnemyType::UPIOR){
        sprite.setFillColor(sf::Color::White);
        baseColor = sf::Color::White;
        speed = 180.f;
        hp = 20; // Małe HP
    }
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
}

void Enemy::draw(sf::RenderWindow& window){
    window.draw(sprite);
}

sf::FloatRect Enemy::getBounds() const{
    return sprite.getGlobalBounds();
}