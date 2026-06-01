#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <cmath>
#include <array>

// Kierunki animacji
enum class AnimDir { DOWN = 0, LEFT = 1, RIGHT = 2, UP = 3};
// Stany animacji
enum class AnimState { IDLE = 0, RUN = 1};

class  Player : public GameObject {
private:
    sf::Texture textures[2][4];         // [AnimState][AnimDir] - 8 arkuszy sprite'ów
    sf::Sprite sprite;                  // Kształt gracza
    static constexpr int FRAME_COUNT = 8;
    static constexpr int FRAME_W = 192;
    static constexpr int FRAME_H = 160;
    int currentFrame;
    float frameTimer;
    float frameSpeed;                   // Sekundy na klatkę
    AnimState animState;
    AnimDir animDir;
    bool texturesLoaded;

    // Animacja ataku
    sf::Texture attackTextures[2][4];   // [comboVariant 0/1]
    sf::Sprite attackSprite;
    int attackAnimFrame;                // Bieżąca klatka animacji ataku
    float attackAnimTimer;
    static constexpr float ATTACK_ANIM_SPEED = 0.07f; // s/klatkę -> 8 klatek = 0.56s
    bool showAttackAnim;
    int attackCombo;

    sf::RectangleShape attackIndicator; // Prostokąt pokazujący obszar ataku

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

    sf::RectangleShape specialCooldownBarBackground;
    sf::RectangleShape specialCooldownBarForeground;

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

    //Pasywne Bronie
    bool hasFireAura;
    sf::CircleShape fireAuraShape;
    float fireAuraRadius;

    bool hasOrbitingSword;
    bool hasWand;
    float wandCooldown;
    float wandTimer;
    int wandProjectiles;
    int wandDamageBonus;
    sf::Texture orbitSwordTexture;
    sf::Sprite orbitSwordShape;
    float orbitAngle;
    float orbitSpeed;

    // System levelowania
    int xp;
    int maxXp; // Próg 20 na poziom
    int level;

    // Rozbudowane statystyki
    int damageBonus;
    int armor;
    float critChance;
    int enemiesKilled;
    int potionsCollected;
    float vampirismChance;
    float pickupRadiusBonus;
    float dodgeChance;
    int hpRegenRate;
    float hpRegenTimer;

    // Metody pomocnicze animacji
    void loadTextures();
    void updateAnimation(const sf::Vector2f& movement, bool isMoving, float deltaTime);
public:
    Player(float x, float y);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    void takeDamage(int amount);
    void heal(int amount);
    void setMousePosition(sf::Vector2f mousePos) { mousePosition = mousePos; }

    sf::FloatRect getAttackBounds() const;
    bool getIsAttacking() const { return isAttacking ; }
    bool getIsDashing() const { return isDashing; }

    bool getWantsToShootSpecial() const { return wantsToShootSpecial; }
    void resetSpecialShot() { wantsToShootSpecial = false; }

    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getLastDirection() const { return mouseDirection; }
    void setPosition(sf::Vector2f newPos) { position = newPos; sprite.setPosition(position); }

    // Funkcje do Levelowania
    void addXp(int amount);
    bool checkLevelUp() const { return xp >= maxXp; }
    void acknowledgeLevelUp() {
        xp -= maxXp;
        level++;
        maxXp = static_cast<int>(maxXp * 1.30f);
    }

    // Funkcja nakładająca wybrane ulepszenie
    void applyUpgrade(int choice);

    // Funkcje systemu statystyk
    int getDamage(int baseDamage, bool* wasCrit = nullptr) const;
    void incrementKills() { enemiesKilled++; }
    void incrementPotions() { potionsCollected++; }
    void triggerVampirism();

    int getLevel() const { return level; }
    int getEnemiesKilled() const { return enemiesKilled; }
    int getPotionsCollected() const { return potionsCollected; }
    int getArmor() const { return armor; }
    int getDamageBonus() const { return damageBonus; }
    float getCritChance() const { return critChance; }
    float getVampirismChance() const { return vampirismChance; }
    float getSpeed() const { return speed; }

    sf::FloatRect getPickupBounds() const {
        float baseRadius = 30.f;
        float totalRadius = baseRadius + pickupRadiusBonus;

        return sf::FloatRect(position.x - totalRadius, position.y - totalRadius, totalRadius * 2.f, totalRadius * 2.f);
    }

    float getDodgeChance() const { return dodgeChance; }
    int getHpRegenRate() const { return hpRegenRate; }
    bool getHasFireAura() const { return hasFireAura; }
    float getFireAuraRadius() const { return fireAuraRadius; }

    bool getHasOrbitingSword() const { return hasOrbitingSword; }
    sf::FloatRect getOrbitingSwordBounds() const { return orbitSwordShape.getGlobalBounds(); }
    // XP
    int getXp() const { return xp; }
    int getMaxXp() const { return maxXp; }

    int getHp() const { return hp; }
    //Gettery do różdżki
    bool getHasWand() const { return hasWand; }
    float getWandCooldown() const { return wandCooldown; }
    float getWandTimer() const { return wandTimer; }
    void tickWandTimer(float dt) { wandTimer += dt; }
    bool wandReady() const { return hasWand && wandTimer >= wandCooldown; }
    void resetWandTimer() { wandTimer = 0.f; }
    int getWandProjectiles() const { return wandProjectiles; }
    int getWandDamageBonus() const { return wandDamageBonus; }


};

#endif