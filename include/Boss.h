#ifndef BOSS_H
#define BOSS_H

#include "GameObject.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"
#include <memory>
#include <string>
#include <vector>

enum class BossType { ROOT_STRANGLER, STORM_WRAITH, VAMPIRE_PRINCE, CHAOS_GOLEM };

class Boss : public GameObject {
private:
    sf::RectangleShape sprite;
    std::shared_ptr<sf::Texture> bossTexture;
    sf::Sprite bossSprite;
    bool textureLoaded;

    BossType type;
    std::shared_ptr<Player> target;

    float speed;
    float baseSpeed;
    int hp;
    int maxHp;
    int armor;
    float damageCooldown;

    // --- System Faz i Statusów ---
    int phase;
    bool isInvulnerable;
    sf::Color baseColor;

    // --- Timery poszczególnych umiejętności ---
    float cdAbility1;
    float cdAbility2;
    float cdAbility3;
    float cdAbility4;

    // --- Zmienne do sekwencji (Channeling / Telegrafy) ---
    int actionState;
    float actionTimer;
    int actionStep;
    sf::Vector2f savedTargetPos;

    // --- Symulacja wpływu na gracza (Slow/CC) ---
    sf::Vector2f lastPlayerPos;
    float slowTimer;
    float currentSlowAmount;

    // --- Pamięć przyzwanych obiektów (Kryształy, Klony, Nietoperze) ---
    std::vector<std::shared_ptr<Enemy>> summonedMinions;
    std::vector<sf::Vector2f> minionPositions; // Dla statycznych kryształów

    std::vector<std::shared_ptr<GameObject>> pendingSpawns;

    // --- Pomocnicze ---

    // Aplikuje kolor na obie reprezentacje wizualne (sprite + kształt zapasowy)
    void setSpriteColor(const sf::Color& color);

    // Zwraca ścieżkę do tekstury pocisku dla danego bossa (z TexturePaths w Config.h)
    const char* getProjectileTexturePath() const;

public:
    Boss(float x, float y, BossType type, std::shared_ptr<Player> player, float scalingMultiplier);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    int takeDamage(int amount, int damageType = 0);
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    std::string getName() const;
    bool getIsInvulnerable() const { return isInvulnerable; }

    std::vector<std::shared_ptr<GameObject>> extractSpawns();
};

#endif