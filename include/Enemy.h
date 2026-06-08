#ifndef ENEMY_H
#define ENEMY_H
#include "GameObject.h"
#include <memory>

class Player;

enum class EnemyType {TRUPOJADY, UPIOR, OGROWATE, KAMIKAZE, CIEN, WAMPIR, ZJAWA}; //Typy wrogow
//Definicja możliwych stanów dla wrogów
enum class EnemyState {
    CHASING,        // Standardowy pościg za graczem
    SPECIAL_ACTION  // Wykonywanie unikalnej umiejętności (szarża, znikanie, rzut głazem itp.)
};

class Enemy : public GameObject {
private:
    // Wizualizacja
    sf::RectangleShape fallbackShape; // Kształt zapasowy
    sf::Sprite enemySprite; // Sprite z teksturą
    std::shared_ptr<sf::Texture> enemyTexture;
    bool textureLoaded;
    float baseScale; // Skala dopasowująca sprite do rozmiaru hitboxa
    float enemySz; // Rozmiar hitboxa z konfiga (do obliczeń skali)
    float pulseTimer; // Timer do efektów pulsujących
    bool facingLeft; // Czy sprite jest odwrócony
    // Oblicza i aplikuje kolor/skalę/flip sprite'a na podstawie bieżącego stanu
    void updateSpriteVisuals();

    float speed; //Predkosc wroga
    EnemyType type; //Typ wroga
    std::shared_ptr<Player> target;
    EnemyState currentState; //Przechowuje obecny stan tego przeciwnika
    sf::Vector2f velocity{0.f, 0.f}; // Płynny wektor ruchu

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

    sf::Vector2f externalForce{0.f, 0.f}; //Siła odpychania z zewnątrz
    sf::Vector2f dashDir; // Zapisany wektor kierunku szarży
    sf::Vector2f pathDir{0.f, 0.f}; // Zapisany wektor algorytmu A*
    bool hasPath = false;           // Czy wróg znajduje się na siatce algorytmu
public:
    Enemy(float x, float y, EnemyType type, std::shared_ptr<Player> player, float hpMult = 1.0f, float speedMult = 1.0f);

    //nadpisanie metod wirtualnych z klasy bazowej GameObject
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    // Zwraca faktycznie zadane obrażenia (0 = zablokowane przez cooldown)
    int takeDamage(int amount, int damageType = 0);
    int getXpReward() const; // Zwraca ilość XP do wypadnięcia
    void setPosition(sf::Vector2f newPos) {
        position = newPos;
        enemySprite.setPosition(position);
        fallbackShape.setPosition(position);
    }
    void heal(int amount) { hp += amount; if(hp > maxHp) hp = maxHp; }

    bool getIsKamikaze() const { return isKamikaze; }
    bool getIsGhost() const { return isGhost; }
    bool getIsVampire() const { return isVampire; }

    // Gettery potrzebne dla silnika
    bool getIsFaded() const { return isFaded; }
    bool checkAndResetThrow() { if(readyToThrow) { readyToThrow = false; return true; } return false; }
    void addForce(const sf::Vector2f& force) { externalForce += force; }
    void setPathDir(sf::Vector2f dir) { pathDir = dir; hasPath = true; }
    void clearPathDir() { hasPath = false; }
};

#endif
