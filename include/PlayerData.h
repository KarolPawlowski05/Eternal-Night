#ifndef PLAYERDATA_H
#define PLAYERDATA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// System animacji
struct AnimationData {
    sf::Texture* textures[2][4] = { nullptr }; // [AnimState][AnimDir]
    sf::Texture* attackTextures[2][4] = { nullptr };           // [comboVariant][AnimDir]
    sf::Sprite sprite;
    sf::Sprite attackSprite;

    int currentFrame;
    float frameTimer;
    float frameSpeed;
    int animState;      // 0=IDLE, 1=RUN
    int animDir;        // 0=DOWN, 1=LEFT, 2=RIGHT, 3=UP
    int attackAnimFrame;
    float attackAnimTimer;
    bool showAttackAnim;
    int attackCombo;
    bool texturesLoaded;

    AnimationData();
};

// System walki
struct CombatData {
    sf::RectangleShape attackIndicator;
    sf::Vector2f attackSize;
    float attackCooldown;
    float attackTimer;
    float attackDuration;
    float durationTimer;
    bool isAttacking;
    bool prevLMBPressed;

    // Atak specjalny
    float specialCooldown;
    float specialTimer;
    bool wantsToShootSpecial;

    CombatData();
};

// System poruszania
struct MovementData {
    float speed;
    sf::Vector2f lastDirection;
    sf::Vector2f mouseDirection;
    sf::Vector2f mousePosition;

    // Unik
    float dashSpeed;
    float dashCooldown;
    float dashTimer;
    float dashDuration;
    float dashDurationTimer;
    bool isDashing;

    // Dźwięk
    float footstepTimer;
    float footstepInterval; // Czas między krokami
    int footstepVariant;

    MovementData();
};

// System HP
struct HealthData {
    int hp;
    int maxHp;
    float invincibilityTimer;

    // UI
    sf::RectangleShape hpBarBackground;
    sf::RectangleShape hpBarForeground;
    sf::RectangleShape attackCooldownBarBackground;
    sf::RectangleShape attackCooldownBarForeground;
    sf::RectangleShape specialCooldownBarBackground;
    sf::RectangleShape specialCooldownBarForeground;
    sf::RectangleShape dashCooldownBarBackground;
    sf::RectangleShape dashCooldownBarForeground;

    HealthData();
};


// System progresji
struct ProgressionData {
    int xp;
    int maxXp;
    int level;
    int enemiesKilled;

    ProgressionData();
};

// System statystyk
struct StatsData {
    int damageBonus;
    int armor;
    float critChance;
    float vampirismChance;
    float pickupRadiusBonus;
    float dodgeChance;
    int hpRegenRate;
    float hpRegenTimer;

    // Mnożniki skalowania gracza
    float dmgMultiplier;      // Mnożnik do każdej fali lvl up
    float hpMultiplier;       // Mnożnik HP bonusu per level

    // CAPY DLA STATYSTYK - zapobieganie gamebreakingom
    static constexpr float MAX_CRIT_CHANCE = 0.50f;           // Maks 50% szansy na kryt
    static constexpr float MAX_VAMPIRISM_CHANCE = 0.30f;       // Maks 30% vampiryzmu
    static constexpr float MAX_DODGE_CHANCE = 0.60f;          // Maks 60% uników
    static constexpr int MAX_ARMOR = 100;                      // Maks 100 pancerza
    static constexpr float MAX_ATTACK_SPEED = 0.1f;           // Maks cooldown (szybko)
    static constexpr float MAX_SPEED = 800.f;                 // Maks szybkość
    static constexpr int MAX_HP_REGEN_RATE = 50;              // Maks regeneracja/5 sekund
    static constexpr float MAX_PICKUP_RADIUS = 500.f;         // Maks zasięg zbierania
    static constexpr int MAX_HP = 500;                        // Maks HP gracza

    StatsData();
};

// Bronie pasywne
struct PassiveWeaponsData {
    // Aura ognia
    bool hasFireAura;
    sf::CircleShape fireAuraShape;
    float fireAuraRadius;

    // Orbitujące ostrze
    bool hasOrbitingSword;
    sf::Texture orbitSwordTexture;
    sf::Sprite orbitSwordShape;
    float orbitAngle;
    float orbitSpeed;
    int swordCount;

    // Różdżka
    bool hasWand;
    float wandCooldown;
    float wandTimer;
    int wandProjectiles;
    int wandDamageBonus;

    PassiveWeaponsData();
};

#endif