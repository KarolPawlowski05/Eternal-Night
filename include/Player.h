#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "PlayerData.h"
#include "AssetManager.h"
#include <cmath>

class Player : public GameObject {
private:
    // Zorganizowane w osobne logiczne systemy
    AnimationData animation;
    CombatData combat;
    MovementData movement;
    HealthData health;
    ProgressionData progression;
    StatsData stats;
    PassiveWeaponsData weapons;

    bool godMode;

    // Ghost trail podczas dasha
    struct GhostFrame {
        sf::Sprite sprite;
        float alpha;
    };
    std::vector<GhostFrame> dashGhosts;
    float ghostSpawnTimer = 0.f;

    // Metody pomocnicze
    void loadTextures();
    void updateAnimation(const sf::Vector2f& movement, bool isMoving, float deltaTime);
    void castAttack();

    void applyLevelUpBonuses();    // Automatyczne skalowanie przy level up
    void clampStats();              // Egzekwowanie capów
public:
    Player(float x, float y);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    // HP
    void takeDamage(int amount);
    void heal(int amount);
    void healFromPotion(int amount);
    int getHp() const { return health.hp; }

    // Walka
    sf::FloatRect getAttackBounds() const;
    bool getIsAttacking() const { return combat.isAttacking; }
    bool getIsDashing() const { return movement.isDashing; }
    bool getWantsToShootSpecial() const { return combat.wantsToShootSpecial; }
    void resetSpecialShot() { combat.wantsToShootSpecial = false; }
    int getDamage(int baseDamage, bool* wasCrit = nullptr) const;
    float getInvincibilityTimer() const { return health.invincibilityTimer; }

    // Ruch
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getLastDirection() const { return movement.mouseDirection; }
    void setMousePosition(sf::Vector2f mousePos) { movement.mousePosition = mousePos; }
    void setPosition(sf::Vector2f newPos) { position = newPos; animation.sprite.setPosition(position); }

    // Progresja
    void addXp(int amount);
    bool checkLevelUp() const { return progression.xp >= progression.maxXp; }
    void acknowledgeLevelUp() {
        progression.xp -= progression.maxXp;
        progression.level++;
        progression.maxXp = static_cast<int>(progression.maxXp * 1.30f);
        //Automatyczne skalowanie gracza
        applyLevelUpBonuses();
        AssetManager::playSound("assets/audio/sfx/levelUp.wav");
    }
    void applyUpgrade(int choice);
    int getLevel() const { return progression.level; }
    int getXp() const { return progression.xp; }
    int getMaxXp() const { return progression.maxXp; }
    int getEnemiesKilled() const { return progression.enemiesKilled; }
    void incrementKills() { progression.enemiesKilled++; }

    // Statystyki
    int getArmor() const { return stats.armor; }
    int getDamageBonus() const { return stats.damageBonus; }
    float getCritChance() const { return stats.critChance; }
    float getVampirismChance() const { return stats.vampirismChance; }
    float getDodgeChance() const { return stats.dodgeChance; }
    float getSpeed() const { return movement.speed; }
    int getHpRegenRate() const { return stats.hpRegenRate; }
    void triggerVampirism();

    // Metody do sprawdzenia capów i wyświetlania upgrade'ów
    bool isStatAtCap(int upgradeChoice) const;
    int getUpgradeValue(int upgradeChoice) const;
    std::wstring getUpgradeDescription(int upgradeChoice) const;

    // Zbieranie
    sf::FloatRect getPickupBounds() const {
        float baseRadius = 30.f;
        float totalRadius = baseRadius + stats.pickupRadiusBonus;
        return sf::FloatRect(position.x - totalRadius, position.y - totalRadius, totalRadius * 2.f, totalRadius * 2.f);
    }

    // Bronie pasywne
    bool getHasFireAura() const { return weapons.hasFireAura; }
    float getFireAuraRadius() const { return weapons.fireAuraRadius; }

    bool getHasOrbitingSword() const { return weapons.hasOrbitingSword; }
    std::vector<sf::FloatRect> getOrbitingSwordsBounds() const;

    bool getHasWand() const { return weapons.hasWand; }
    float getWandCooldown() const { return weapons.wandCooldown; }
    float getWandTimer() const { return weapons.wandTimer; }
    void tickWandTimer(float dt) { weapons.wandTimer += dt; }
    bool wandReady() const { return weapons.hasWand && weapons.wandTimer >= weapons.wandCooldown; }
    void resetWandTimer() { weapons.wandTimer = 0.f; }
    int getWandProjectiles() const { return weapons.wandProjectiles; }
    int getWandDamageBonus() const { return weapons.wandDamageBonus; }

    // GOD MODE
    void toggleGodMode() { godMode = !godMode; }
    bool getGodMode() const { return godMode; }

    int getMaxHp() const { return health.maxHp; }


};

#endif