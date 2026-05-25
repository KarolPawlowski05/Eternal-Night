#ifndef ENEMY_H
#define ENEMY_H
#include "GameObject.h"
#include <memory>

class Player;

enum class EnemyType {TRUPOJADY, UPIOR, OGROWATE}; //Typy wrogow

class Enemy : public GameObject {
private:
    sf::RectangleShape sprite; //Wizualna reprezentacja przeciwnika
    float speed; //Predkosc wroga
    EnemyType type; //Typ wroga
    std::shared_ptr<Player> target;

    int hp;
    float damageCooldown; // Żeby atak obszarowy/kusza nie zadawały obrażeń co klatkę
    sf::Color baseColor;  // Do przywracania koloru po "błysku" otrzymania obrażeń
public:
    Enemy(float x, float y, EnemyType enemytype,std::shared_ptr<Player> playerTarget);

    //nadpisanie metod wirtualnych z klasy bazowej GameObject
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void takeDamage(int amount);
    void setPosition(sf::Vector2f newPos) { position = newPos; sprite.setPosition(position); }
};

#endif
