#ifndef ENEMY_H
#define ENEMY_H
#include "GameObject.h"

enum class EnemyType {TRUPOJADY, UPIOR, OGROWATE}; //Typy wrogow

class Enemy : public GameObject {
private:
    sf::RectangleShape sprite; //Wizualna reprezentacja przeciwnika
    float speed; //Predkosc wroga
    EnemyType type; //Typ wroga

public:
    Enemy(float x, float y, EnemyType enemytype);

    //nadpisanie metod wirtualnych z klasy bazowej GameObject
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
};

#endif
