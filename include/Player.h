#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <cmath>

class  Player : public GameObject {
private:
    sf::RectangleShape sprite;          // Kształt gracza
    sf::RectangleShape attackIndicator; //Prostokąt pokazujący obszar ataku

    float speed;                        // Prędkość liniowa w px/s
    sf::Vector2f lastDirection;         // Ostatni kierunek ruchu
    sf::Vector2f mouseDirection;        // Kierunek myszy
    sf::Vector2f mousePosition;         // Pozycja kursora w świecie gry

    // HP
    int hp;
    int maxHp;
    float invincibilityTimer;

    // Pasek HP
    sf::RectangleShape hpBarBackground;
    sf::RectangleShape hpBarForeground;

    // Pasek cooldownu ataku
    sf::RectangleShape attackCooldownBarBackground;
    sf::RectangleShape attackCooldownBarForeground;

    // Unik
    float dashSpeed;
    float dashCooldown;
    float dashTimer;
    float dashDuration;
    float dashDurationTimer;
    bool isDashing;

    // Atak
    sf::Vector2f attackSize;
    float attackCooldown;               // Czas między atakami
    float attackTimer;                  // Licznik czasu od ostatniego ataku
    float attackDuration;               // Jak długo atak jest aktywny wizualnie
    float durationTimer;                // Czas trwania "błysku"
    bool isAttacking;                   // Czy w tej konkretnej klatce trwa zadawanie obrażeń
    bool prevLMBPressed;                // Stan LPM w poprzedniej klatce (do detekcji kliknięcia)(

    void castAttack();                  // Wykonanie ataku

    // Atak specjalny - 1
    float specialCooldown;
    float specialTimer;
    bool wantsToShootSpecial;
public:
    Player(float x, float y);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    void takeDamage(int amount);
    void setMousePosition(sf::Vector2f mousePos) { mousePosition = mousePos; }

    sf::FloatRect getAttackBounds() const;
    bool getIsAttacking() const { return isAttacking ; }
    bool getIsDashing() const { return isDashing; }

    bool getWantsToShootSpecial() const { return wantsToShootSpecial; }
    void resetSpecialShot() { wantsToShootSpecial = false; }

    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getLastDirection() const { return mouseDirection; }
    void setPosition(sf::Vector2f newPos) { position = newPos; sprite.setPosition(position); }
};

#endif