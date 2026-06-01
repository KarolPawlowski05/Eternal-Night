#ifndef ENEMY_H
#define ENEMY_H
#include "GameObject.h"
#include <memory>

class Player;

enum class EnemyType {TRUPOJADY, UPIOR, OGROWATE, KAMIKAZE, CIEN, WAMPIR, ZJAWA}; //Typy wrogow

class Enemy : public GameObject {
private:
    sf::RectangleShape sprite; //Wizualna reprezentacja przeciwnika
    float speed; //Predkosc wroga
    EnemyType type; //Typ wroga
    std::shared_ptr<Player> target;

    int hp;
    int maxHp;
    float damageCooldown; // Żeby atak obszarowy/kusza nie zadawały obrażeń co klatkę
    float fireAuraCooldown; // Zegar dla aury ognia
    float swordCooldown;    // Zegar dla miecza
    sf::Color baseColor;  // Do przywracania koloru po "błysku" otrzymania obrażeń

    sf::RectangleShape hpBarBackground; // Czarne tło paska HP
    sf::RectangleShape hpBarForeground; // Czerwony pasek HP
    float baseSpeed;
    float abilityTimer = 0.f;
    bool isKamikaze;
    bool isGhost;
    bool isVampire;

    bool isFaded = false;
    bool isBatForm = false;
    bool readyToThrow = false;
public:
    Enemy(float x, float y, EnemyType type, std::shared_ptr<Player> player, float hpMult = 1.0f, float speedMult = 1.0f);

    //nadpisanie metod wirtualnych z klasy bazowej GameObject
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    // Zwraca faktycznie zadane obrażenia (0 = zablokowane przez cooldown)
    int takeDamage(int amount, int damageType = 0);
    int getXpReward() const; // Zwraca ilość XP do wypadnięcia
    void setPosition(sf::Vector2f newPos) { position = newPos; sprite.setPosition(position); }
    void heal(int amount) { hp += amount; if(hp > maxHp) hp = maxHp; }
    bool getIsKamikaze() const { return isKamikaze; }
    bool getIsGhost() const { return isGhost; }
    bool getIsVampire() const { return isVampire; }

    // Gettery potrzebne dla silnika
    bool getIsFaded() const { return isFaded; }
    bool checkAndResetThrow() { if(readyToThrow) { readyToThrow = false; return true; } return false; }
};

#endif
