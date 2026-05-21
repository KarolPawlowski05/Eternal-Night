#include "Enemy.h"
#include <Player.h>
#include <cmath>

Enemy::Enemy(float x, float y, EnemyType enemyType, std::shared_ptr<Player> playerTarget) : GameObject(x, y), type(enemyType), target(playerTarget){
    //konfiguracja wygladu wroga
    sprite.setSize(sf::Vector2f(30.f, 30.f));
    sprite.setOrigin(15.f, 15.f);
    sprite.setPosition(position);

    //Predkosc i kolor wroga zależne od typu
    if(type == EnemyType::TRUPOJADY){
        sprite.setFillColor(sf::Color::Magenta);
        speed = 100.f;
    }
    else if (type == EnemyType::OGROWATE){
        sprite.setFillColor(sf::Color::Green);
        speed = 50.f;
    }
    else if (type == EnemyType::UPIOR){
        sprite.setFillColor(sf::Color::White);
        speed = 180.f;
    }
}

void Enemy::update(float deltaTime){
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