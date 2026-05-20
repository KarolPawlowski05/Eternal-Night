#include "Enemy.h"

Enemy::Enemy(float x, float y, EnemyType enemyType ) : GameObject(x,y), type(enemyType){
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
    //ruch w strone gracza zostanie zaimplementowany w f4
    sprite.setPosition(position);
}

void Enemy::draw(sf::RenderWindow& window){
    window.draw(sprite);
}

sf::FloatRect Enemy::getBounds() const{
    return sprite.getGlobalBounds();
}