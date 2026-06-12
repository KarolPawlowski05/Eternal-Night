#include "Boss.h"
#include "Config.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

// ---------------------------------------------------------------
// Pomocnicze: zwraca ścieżkę do tekstury pocisku tego bossa
// ---------------------------------------------------------------
const char* Boss::getProjectileTexturePath() const {
    switch (type) {
    case BossType::ROOT_STRANGLER:  return TexturePaths::PROJ_ROOT_STRANGLER;
    case BossType::STORM_WRAITH:    return TexturePaths::PROJ_STORM_WRAITH;
    case BossType::VAMPIRE_PRINCE:  return TexturePaths::PROJ_VAMPIRE_PRINCE;
    case BossType::CHAOS_GOLEM:     return TexturePaths::PROJ_CHAOS_GOLEM;
    default:                        return "";
    }
}

// ---------------------------------------------------------------
// Pomocnicze: ustawia kolor na obu reprezentacjach wizualnych
// ---------------------------------------------------------------
void Boss::setSpriteColor(const sf::Color& color) {
    sprite.setFillColor(color);
    if (textureLoaded) bossSprite.setColor(color);
}

Boss::Boss(float x, float y, BossType type, std::shared_ptr<Player> player, float scalingMultiplier)
    : GameObject(x, y), type(type), target(player), damageCooldown(0.f),
    phase(0), isInvulnerable(false), cdAbility1(0.f), cdAbility2(0.f), cdAbility3(0.f), cdAbility4(0.f),
    actionState(0), actionTimer(0.f), actionStep(0), slowTimer(0.f), currentSlowAmount(0.f),
    textureLoaded(false)
{
    sprite.setSize(sf::Vector2f(80.f, 80.f));
    sprite.setOrigin(40.f, 40.f);
    sprite.setPosition(position);
    lastPlayerPos = target ? target->getPosition() : sf::Vector2f(0,0);

    // INICJALIZACJA STATYSTYK BAZOWYCH
    if (type == BossType::ROOT_STRANGLER) {
        baseColor = sf::Color(139, 69, 19);
        baseSpeed = 40.f;
        maxHp = 500;
        armor = 2;
    } else if (type == BossType::STORM_WRAITH) {
        baseColor = sf::Color(0, 191, 255);
        baseSpeed = 85.f;
        maxHp = 800;
        armor = 2;
    } else if (type == BossType::VAMPIRE_PRINCE) {
        baseColor = sf::Color(138, 43, 226);
        baseSpeed = 70.f;
        maxHp = 1000;
        armor = 6;
    } else if (type == BossType::CHAOS_GOLEM) {
        baseColor = sf::Color(47, 79, 79);
        baseSpeed = 35.f;
        maxHp = 1200;
        armor = 10;
        cdAbility4 = 12.f;
    }

    maxHp = static_cast<int>(maxHp * scalingMultiplier);
    baseSpeed *= (1.0f + (scalingMultiplier - 1.0f) * 0.3f);
    speed = baseSpeed;
    hp = maxHp;
    sprite.setFillColor(baseColor);

    // --- ŁADOWANIE TEKSTURY ---
    const char* texPath = nullptr;
    if      (type == BossType::ROOT_STRANGLER)  texPath = TexturePaths::BOSS_ROOT_STRANGLER;
    else if (type == BossType::STORM_WRAITH)     texPath = TexturePaths::BOSS_STORM_WRAITH;
    else if (type == BossType::VAMPIRE_PRINCE)   texPath = TexturePaths::BOSS_VAMPIRE_PRINCE;
    else if (type == BossType::CHAOS_GOLEM)      texPath = TexturePaths::BOSS_CHAOS_GOLEM;

    if (texPath) {
        bossTexture = AssetManager::loadTexture(texPath);
        if (bossTexture) {
            textureLoaded = true;
            bossSprite.setTexture(*bossTexture);

            // Skalowanie do rozmiaru hitboxa (80x80)
            sf::FloatRect lb = bossSprite.getLocalBounds();
            bossSprite.setOrigin(lb.width / 2.f, lb.height / 2.f);
            bossSprite.setScale(80.f / lb.width, 80.f / lb.height);
            bossSprite.setPosition(position);
        }
    }
}

int Boss::takeDamage(int amount, int damageType) {
    if (isInvulnerable) return 0;

    // Mechanika słabości Golema: Aura ognia zadaje -50% obrażeń
    if (type == BossType::CHAOS_GOLEM && damageType == 1) {
        amount /= 2;
    }

    // Pancerz redukuje obrażenia (minimum 1)
    int finalDamage = amount - armor;
    if (finalDamage < 1) finalDamage = 1;

    if (damageCooldown <= 0.f) {
        hp -= finalDamage;
        damageCooldown = 0.15f;
        setSpriteColor(sf::Color::White); // Błysk trafienia
        if (hp <= 0) destroy();
        return finalDamage;
    }
    return 0;
}

std::string Boss::getName() const {
    if (type == BossType::ROOT_STRANGLER) return "Root Strangler";
    if (type == BossType::STORM_WRAITH)   return "Storm Wraith";
    if (type == BossType::VAMPIRE_PRINCE) return "Vampire Prince";
    if (type == BossType::CHAOS_GOLEM)    return "Chaos Golem";
    return "Unknown Boss";
}

std::vector<std::shared_ptr<GameObject>> Boss::extractSpawns() {
    auto spawns = pendingSpawns;
    pendingSpawns.clear();
    return spawns;
}

void Boss::update(float deltaTime) {
    // Odliczanie cooldownu trafienia i przywrócenie koloru
    if (damageCooldown > 0.f) {
        damageCooldown -= deltaTime;
        if (damageCooldown <= 0.f) setSpriteColor(baseColor);
    }

    if (!target) return;

    sf::Vector2f playerPos = target->getPosition();
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    float distance = std::hypot(dx, dy);

    sf::Vector2f dir(0.f, 0.f);
    if (distance > 0) {
        dir.x = dx / distance;
        dir.y = dy / distance;
    }

    // --- SYSTEM KONTROLI TŁUMU (Spowolnienie gracza) ---
    sf::Vector2f playerVelocity = playerPos - lastPlayerPos;
    if (slowTimer > 0.f) {
        slowTimer -= deltaTime;
        target->setPosition(playerPos - (playerVelocity * currentSlowAmount));
    }
    lastPlayerPos = target->getPosition();

    // Redukcja globalnych timerów umiejętności
    if(cdAbility1 > 0.f) cdAbility1 -= deltaTime;
    if(cdAbility2 > 0.f) cdAbility2 -= deltaTime;
    if(cdAbility3 > 0.f) cdAbility3 -= deltaTime;
    if(cdAbility4 > 0.f) cdAbility4 -= deltaTime;

    float currentSpeed = speed;
    bool overrideMovement = false;

    // Skrót – ścieżka pocisku tego bossa
    const char* projPath = getProjectileTexturePath();

    //  KORZENNY DUSICIEL
    if (type == BossType::ROOT_STRANGLER) {
        // Faza 2: Poniżej 50% HP
        if (hp < maxHp * 0.5f && phase == 0) {
            phase = 1;
            baseSpeed = 65.f;
            baseColor = sf::Color(100, 0, 0);
            setSpriteColor(baseColor);

            for(int i=0; i<6; i++) {
                sf::Vector2f dropPos(position.x + (rand()%400-200), position.y + (rand()%400-200));
                pendingSpawns.push_back(std::make_shared<Projectile>(dropPos.x, dropPos.y, sf::Vector2f(1,0), 0.f, true, false, projPath));
            }
        }

        if (actionState == 0) {
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f; actionStep = 0;
            }
            else if (cdAbility2 <= 0.f && distance < 80.f) {
                currentSlowAmount = 0.70f;
                slowTimer = 1.5f;
                cdAbility2 = 6.f;
            }
            else if (cdAbility3 <= 0.f) {
                for(int i = 0; i < 12; i++) {
                    float angle = (rand() % 360) * 3.14159f / 180.f;
                    sf::Vector2f pDir(std::cos(angle), std::sin(angle));
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, pDir, 150.f, true, false, projPath));
                }
                cdAbility3 = 5.f;
            }
        }
        else if (actionState == 1) { // Kanałowanie korzeni
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.5f) {
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, dir, 300.f, true, false, projPath));
                actionStep++;
                actionTimer = 0.f;
                if (actionStep >= 3) {
                    actionState = 0;
                    cdAbility1 = (phase == 1) ? 1.0f : 2.0f;
                }
            }
        }
    }
    //  ZJAWA BURZY
    else if (type == BossType::STORM_WRAITH) {
        // Faza 2: Poniżej 40% HP
        if (hp < maxHp * 0.4f && phase == 0) {
            phase = 1;
            for(int i=0; i<8; i++) {
                sf::Vector2f dropPos(position.x + (rand()%600-300), position.y + (rand()%600-300));
                pendingSpawns.push_back(std::make_shared<Projectile>(dropPos.x, dropPos.y, sf::Vector2f(1,0), 0.f, true, false, projPath));
            }
        }

        // Fading (Faza 2)
        if (phase == 1) {
            cdAbility4 -= deltaTime;
            if (cdAbility4 <= 0.f) {
                isInvulnerable = true;
                setSpriteColor(sf::Color(255, 255, 255, 80)); // Półprzezroczysty
                cdAbility4 = 3.f;
                actionTimer = 1.0f;
            }
            if (isInvulnerable) {
                actionTimer -= deltaTime;
                if (actionTimer <= 0.f) {
                    isInvulnerable = false;
                    setSpriteColor(baseColor);
                }
            }
        }

        if (actionState == 0) {
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f; savedTargetPos = playerPos;
            }
            else if (cdAbility2 <= 0.f) {
                actionState = 2; actionTimer = 0.f; actionStep = 0;
            }
            else if (cdAbility3 <= 0.f && phase == 0) {
                pendingSpawns.push_back(std::make_shared<Enemy>(position.x + 80, position.y + 80, EnemyType::ZJAWA, target, 0.05f, 1.f));
                pendingSpawns.push_back(std::make_shared<Enemy>(position.x - 80, position.y - 80, EnemyType::ZJAWA, target, 0.05f, 1.f));
                cdAbility3 = 999.f;
            }
        }
        else if (actionState == 1) { // Teleport
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.4f) {
                position = savedTargetPos;
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(1,0), 0.f, true, false, projPath));
                hp += 10; if(hp > maxHp) hp = maxHp;
                actionState = 0;
                cdAbility1 = (phase == 2) ? 3.5f : 5.0f;
            }
        }
        else if (actionState == 2) { // Pioruny
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.15f) {
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, dir, 400.f, true, false, projPath));
                actionStep++;
                actionTimer = 0.f;
                if (actionStep >= 5) {
                    actionState = 0;
                    cdAbility2 = 4.0f;
                }
            }
        }
    }
    // WAMPIR-KSIĄŻĘ
    else if (type == BossType::VAMPIRE_PRINCE) {
        if (hp < maxHp * 0.33f && phase < 2) { phase = 2; }
        else if (hp < maxHp * 0.66f && phase < 1) { phase = 1; cdAbility3 = 6.f; }

        // Faza 3: Aura krwi
        if (phase == 2) {
            cdAbility4 -= deltaTime;
            if (cdAbility4 <= 0.f) {
                if (distance < 150.f) {
                    target->takeDamage(1);
                    hp += 1; if(hp > maxHp) hp = maxHp;
                }
                cdAbility4 = 0.5f;
            }
        }

        if (actionState == 0) {
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f; savedTargetPos = playerPos;
            }
            else if (cdAbility2 <= 0.f) {
                for(int i=0; i<4; i++) {
                    pendingSpawns.push_back(std::make_shared<Enemy>(position.x, position.y, EnemyType::WAMPIR, target, 0.2f, 1.5f));
                }
                cdAbility2 = 8.f;
            }
            else if (phase >= 1 && cdAbility3 <= 0.f) {
                actionState = 2; actionTimer = 0.f;
                isInvulnerable = true;
                setSpriteColor(sf::Color(255, 255, 255, 100)); // Forma mgły
            }
        }
        else if (actionState == 1) { // Szarża
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.4f) {
                position = savedTargetPos;
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(1,0), 0.f, true, false, projPath));
                hp += 30; if(hp > maxHp) hp = maxHp;
                actionState = 0;
                cdAbility1 = (phase == 2) ? 1.5f : 3.0f;
            }
        }
        else if (actionState == 2) { // Forma Mgły
            actionTimer += deltaTime;
            currentSpeed = baseSpeed * 2.f;

            if (actionTimer >= 2.0f) {
                isInvulnerable = false;
                setSpriteColor(baseColor);

                for(int i=0; i<8; i++) {
                    float a = i * (3.14159f / 4.f);
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(std::cos(a), std::sin(a)), 150.f, true, false, projPath));
                }
                actionState = 0;
                cdAbility3 = 6.0f;
            }
        }
    }
    //  KAMIENNY GOLEM CHAOSU
    else if (type == BossType::CHAOS_GOLEM) {
        if (hp < maxHp * 0.60f && phase == 0) { phase = 1; }

        // Mechanika kryształów (Regeneracja HP)
        summonedMinions.erase(std::remove_if(summonedMinions.begin(), summonedMinions.end(),
                                             [](const std::shared_ptr<Enemy>& e) { return !e->isActive(); }), summonedMinions.end());

        if (!summonedMinions.empty()) {
            hp += 15 * deltaTime;
            if (hp > maxHp) hp = maxHp;

            for (size_t i = 0; i < summonedMinions.size(); i++) {
                if (i < minionPositions.size()) {
                    summonedMinions[i]->setPosition(minionPositions[i]);
                }
            }
        }

        if (actionState == 0) {
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f;
            }
            else if (cdAbility2 <= 0.f) {
                for(int i=0; i<6; i++) {
                    float angle = (rand() % 360) * 3.14159f / 180.f;
                    sf::Vector2f pDir(std::cos(angle), std::sin(angle));
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, pDir, 220.f, true, false, projPath));
                }
                cdAbility2 = 5.f;
            }
            else if (cdAbility3 <= 0.f && phase >= 1) {
                actionState = 2; actionTimer = 0.f;
            }
            else if (cdAbility4 <= 0.f && hp < maxHp * 0.30f && summonedMinions.empty()) {
                minionPositions.clear();
                for(int i=0; i<3; i++) {
                    sf::Vector2f cPos(position.x + (rand()%400-200), position.y + (rand()%400-200));
                    auto crystal = std::make_shared<Enemy>(cPos.x, cPos.y, EnemyType::KAMIKAZE, nullptr, 16.f, 0.f);
                    pendingSpawns.push_back(crystal);
                    summonedMinions.push_back(crystal);
                    minionPositions.push_back(cPos);
                }
                cdAbility4 = 12.f;
            }
        }
        else if (actionState == 1) { // Kamienne uderzenie – krzyż
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.6f) {
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f( 1, 0), 300.f, true, false, projPath));
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(-1, 0), 300.f, true, false, projPath));
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f( 0, 1), 300.f, true, false, projPath));
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f( 0,-1), 300.f, true, false, projPath));
                actionState = 0;
                cdAbility1 = 3.f;
            }
        }
        else if (actionState == 2) { // Trzęsienie ziemi
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 1.2f) {
                if (distance < 400.f) {
                    currentSlowAmount = 0.5f;
                    slowTimer = 2.0f;
                    target->takeDamage(20);
                }
                for(int i=0; i<16; i++) {
                    float a = i * (3.14159f / 8.f);
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(std::cos(a), std::sin(a)), 500.f, true, false, projPath));
                }
                actionState = 0;
                cdAbility3 = 8.f;
            }
        }
    }

    // Standardowe poruszanie się, jeśli nie zablokowane
    if (!overrideMovement && distance > 0 && currentSpeed != 0.f) {
        position.x += dir.x * currentSpeed * deltaTime;
        position.y += dir.y * currentSpeed * deltaTime;
    }

    sprite.setPosition(position);
    if (textureLoaded) bossSprite.setPosition(position);
}

void Boss::draw(sf::RenderWindow& window) {
    if (textureLoaded) {
        window.draw(bossSprite);
    } else {
        window.draw(sprite);
    }
}

sf::FloatRect Boss::getBounds() const {
    return sprite.getGlobalBounds(); // RectangleShape zawsze odzwierciedla pozycję/rozmiar hitboxa
}