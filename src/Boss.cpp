#include "Boss.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

Boss::Boss(float x, float y, BossType type, std::shared_ptr<Player> player, float scalingMultiplier)
    : GameObject(x, y), type(type), target(player), damageCooldown(0.f),
    phase(0), isInvulnerable(false), cdAbility1(0.f), cdAbility2(0.f), cdAbility3(0.f), cdAbility4(0.f),
    actionState(0), actionTimer(0.f), actionStep(0), slowTimer(0.f), currentSlowAmount(0.f)
{
    sprite.setSize(sf::Vector2f(80.f, 80.f));
    sprite.setOrigin(40.f, 40.f);
    sprite.setPosition(position);
    lastPlayerPos = target ? target->getPosition() : sf::Vector2f(0,0);

    // INICJALIZACJA STATYSTYK BAZOWYCH
    if (type == BossType::ROOT_STRANGLER) {
        baseColor = sf::Color(139, 69, 19); // Brązowo-drzewny
        baseSpeed = 40.f; //
        maxHp = 500; //
        armor = 2; //
    } else if (type == BossType::STORM_WRAITH) {
        baseColor = sf::Color(0, 191, 255); // Burzowy niebieski
        baseSpeed = 85.f; //
        maxHp = 800; //
        armor = 2; //
    } else if (type == BossType::VAMPIRE_PRINCE) {
        baseColor = sf::Color(138, 43, 226); // Purpurowy wampir
        baseSpeed = 70.f; //
        maxHp = 1000; //
        armor = 6; //
    } else if (type == BossType::CHAOS_GOLEM) {
        baseColor = sf::Color(47, 79, 79); // Ciemny kamień
        baseSpeed = 35.f; //
        maxHp = 1200; //
        armor = 10; //
        cdAbility4 = 12.f; // Timer pierwszego spawnu kryształów
    }

    maxHp = static_cast<int>(maxHp * scalingMultiplier);
    baseSpeed *= (1.0f + (scalingMultiplier - 1.0f) * 0.3f);
    speed = baseSpeed;
    hp = maxHp;
    sprite.setFillColor(baseColor);
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
        sprite.setFillColor(sf::Color::White);
        if (hp <= 0) destroy();
        return finalDamage;
    }
    return 0;
}

std::string Boss::getName() const {
    if (type == BossType::ROOT_STRANGLER) return "Korzenny Dusiciel";
    if (type == BossType::STORM_WRAITH) return "Zjawa Burzy";
    if (type == BossType::VAMPIRE_PRINCE) return "Wampir-Ksiaze";
    if (type == BossType::CHAOS_GOLEM) return "Kamienny Golem Chaosu";
    return "Unknown Boss";
}

std::vector<std::shared_ptr<GameObject>> Boss::extractSpawns() {
    auto spawns = pendingSpawns;
    pendingSpawns.clear();
    return spawns;
}

void Boss::update(float deltaTime) {
    if (damageCooldown > 0.f) {
        damageCooldown -= deltaTime;
        if (damageCooldown <= 0.f) sprite.setFillColor(baseColor);
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
        // Cofa gracza w każdej klatce o procent jego własnego ruchu, by symulować spowolnienie
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

    //  KORZENNY DUSICIEL Strażnik lasu
    if (type == BossType::ROOT_STRANGLER) {
        // Faza 2: Poniżej 50% HP
        if (hp < maxHp * 0.5f && phase == 0) {
            phase = 1;
            baseSpeed = 65.f; // Przyspiesza do 65 px/s
            baseColor = sf::Color(100, 0, 0); // Ciemnoczerwony
            sprite.setFillColor(baseColor);

            // 6 dodatkowych statycznych kolców (0 speed) na mapie
            for(int i=0; i<6; i++) {
                sf::Vector2f dropPos(position.x + (rand()%400-200), position.y + (rand()%400-200));
                pendingSpawns.push_back(std::make_shared<Projectile>(dropPos.x, dropPos.y, sf::Vector2f(1,0), 0.f, true));
            }
        }

        if (actionState == 0) {
            // Uderzenie korzeniami: Wysyła 3 korzenie. Czas: 2s (1s w fazie 2)
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f; actionStep = 0;
            }
            // Splątanie: Promień 80 px. Cooldown 6s
            else if (cdAbility2 <= 0.f && distance < 80.f) {
                currentSlowAmount = 0.70f; // Prędkość spada do 30%
                slowTimer = 1.5f; // Czas trwania 1.5s
                cdAbility2 = 6.f; //
            }
            // Deszcz kolców: 12 kolców losowo. Cooldown 5s
            else if (cdAbility3 <= 0.f) {
                for(int i = 0; i < 12; i++) {
                    float angle = (rand() % 360) * 3.14159f / 180.f;
                    sf::Vector2f pDir(std::cos(angle), std::sin(angle));
                    // Promień 300px symulujemy dając pociskom zasięg
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, pDir, 150.f, true));
                }
                cdAbility3 = 5.f; //
            }
        }
        else if (actionState == 1) { // Kanałowanie korzeni
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.5f) { // Opóźnienie 0.5s
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, dir, 300.f, true));
                actionStep++;
                actionTimer = 0.f;
                if (actionStep >= 3) { // 3 korzenie
                    actionState = 0;
                    cdAbility1 = (phase == 1) ? 1.0f : 2.0f; // Szał korzeni w F2
                }
            }
        }
    }
    //  ZJAWA BURZY Upiór zrodzony z pioruna
    else if (type == BossType::STORM_WRAITH) {
        // Faza 2: Poniżej 40% HP
        if (hp < maxHp * 0.4f && phase == 0) {
            phase = 1;
            // Piekło elektryczne: 8 stacjonarnych pól na mapie
            for(int i=0; i<8; i++) {
                sf::Vector2f dropPos(position.x + (rand()%600-300), position.y + (rand()%600-300));
                pendingSpawns.push_back(std::make_shared<Projectile>(dropPos.x, dropPos.y, sf::Vector2f(1,0), 0.f, true));
            }
        }

        // Piekło elektryczne - Fading (Faza 2)
        if (phase == 1) {
            cdAbility4 -= deltaTime;
            if (cdAbility4 <= 0.f) {
                isInvulnerable = true;
                sprite.setFillColor(sf::Color(0, 191, 255, 80)); // Półprzezroczysty
                cdAbility4 = 3.f; // Co 3s
                actionTimer = 1.0f; // Trwa 1s
            }
            if (isInvulnerable) {
                actionTimer -= deltaTime;
                if (actionTimer <= 0.f) {
                    isInvulnerable = false;
                    sprite.setFillColor(baseColor);
                }
            }
        }

        if (actionState == 0) {
            // Piorunowy Teleport: Co 4s
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f; savedTargetPos = playerPos;
            }
            // Łańcuch piorunów: 5 pocisków elektrycznych. Cooldown 4s
            else if (cdAbility2 <= 0.f) {
                actionState = 2; actionTimer = 0.f; actionStep = 0;
            }
            // Klony burzowe (Tylko Faza 1): 2 fałszywe kopie. Jednorazowe
            else if (cdAbility3 <= 0.f && phase == 0) {
                pendingSpawns.push_back(std::make_shared<Enemy>(position.x + 80, position.y + 80, EnemyType::ZJAWA, target, 0.05f, 1.f)); // Klony z ~1 HP
                pendingSpawns.push_back(std::make_shared<Enemy>(position.x - 80, position.y - 80, EnemyType::ZJAWA, target, 0.05f, 1.f));
                cdAbility3 = 999.f; // Raz na walkę
            }
        }
        else if (actionState == 1) { // Szarża i Teleport
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.4f) { // Telegraf 0.4s
                position = savedTargetPos; // Teleport
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(1,0), 0.f, true)); // AoE dmg uderzenia

                hp += 10; if(hp > maxHp) hp = maxHp; // Leczenie po uderzeniu zmniejszone z 30 do 10 HP

                actionState = 0;
                cdAbility1 = (phase == 2) ? 3.5f : 5.0f;
            }
        }
        else if (actionState == 2) { // Pioruny
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.15f) { // Pociski w szybkiej sekwencji co 0.15s
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, dir, 400.f, true));
                actionStep++;
                actionTimer = 0.f;
                if (actionStep >= 5) { // 5 pocisków
                    actionState = 0;
                    cdAbility2 = 4.0f; //
                }
            }
        }
    }
    //WAMPIR-KSIĄŻĘ Wieczny krwiopijca
    else if (type == BossType::VAMPIRE_PRINCE) {
        if (hp < maxHp * 0.33f && phase < 2) { phase = 2; } // Faza 3 (<33%)
        else if (hp < maxHp * 0.66f && phase < 1) { phase = 1; cdAbility3 = 6.f; } // Faza 2 (<66%)

        // Faza 3: Krew musi płynąć
        if (phase == 2) {
            cdAbility4 -= deltaTime;
            if (cdAbility4 <= 0.f) {
                if (distance < 150.f) { // Aura 150px
                    target->takeDamage(1); // Gracz traci 1 HP
                    hp += 1; if(hp > maxHp) hp = maxHp; // Boss leczy 1 HP
                }
                cdAbility4 = 0.5f; // Co 0.5s
            }
        }

        if (actionState == 0) {
            // Szarża wampira: Cooldown 3s, w fazie 3 spada do 1.5s
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f; savedTargetPos = playerPos;
            }
            // Rój nietoperzy: 4 małe nietoperze, cooldown 8s
            else if (cdAbility2 <= 0.f) {
                for(int i=0; i<4; i++) { //
                    pendingSpawns.push_back(std::make_shared<Enemy>(position.x, position.y, EnemyType::WAMPIR, target, 0.2f, 1.5f));
                }
                cdAbility2 = 8.f; //
            }
            // Forma Mgły (Faza 2+): Aktywuje się co 6 sekund w Fazie 3
            else if (phase >= 1 && cdAbility3 <= 0.f) {
                actionState = 2; actionTimer = 0.f;
                isInvulnerable = true; // Odporny na wszystko
                sprite.setFillColor(sf::Color(138, 43, 226, 100));
            }
        }
        else if (actionState == 1) { // Szarża
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.4f) { // Telegraf 0.4s purpurowy krąg
                position = savedTargetPos; // Teleport
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(1,0), 0.f, true)); // AoE dmg uderzenia
                hp += 30; if(hp > maxHp) hp = maxHp; // Leczy 30 HP

                actionState = 0;
                cdAbility1 = (phase == 2) ? 1.5f : 3.0f; //
            }
        }
        else if (actionState == 2) { // Forma Mgły
            actionTimer += deltaTime;
            currentSpeed = baseSpeed * 2.f; // 2x szybciej w stronę gracza

            if (actionTimer >= 2.0f) { // Trwa 2s
                isInvulnerable = false;
                sprite.setFillColor(baseColor);

                // Eksplozja krwi (AoE) po dotarciu
                for(int i=0; i<8; i++) {
                    float a = i * (3.14159f / 4.f);
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(std::cos(a), std::sin(a)), 150.f, true));
                }

                actionState = 0;
                cdAbility3 = 6.0f; // Automatycznie co 6s
            }
        }
    }
    //  KAMIENNY GOLEM CHAOSU Zniszczenie ucieleśnione
    else if (type == BossType::CHAOS_GOLEM) {
        if (hp < maxHp * 0.60f && phase == 0) { phase = 1; } // Faza 2 (<60%)

        // Mechanika kryształów (Regeneracja HP)
        summonedMinions.erase(std::remove_if(summonedMinions.begin(), summonedMinions.end(),
                                             [](const std::shared_ptr<Enemy>& e) { return !e->isActive(); }), summonedMinions.end());

        if (!summonedMinions.empty()) {
            hp += 15 * deltaTime; // Golem regeneruje 15 HP/s dopóki kryształ żyje
            if (hp > maxHp) hp = maxHp;

            // Unieruchamia kryształy by były stacjonarne na mapie
            for (size_t i = 0; i < summonedMinions.size(); i++) {
                if (i < minionPositions.size()) {
                    summonedMinions[i]->setPosition(minionPositions[i]);
                }
            }
        }

        if (actionState == 0) {
            // Kamienne uderzenie (Krzyż): Cooldown 3s
            if (cdAbility1 <= 0.f) {
                actionState = 1; actionTimer = 0.f;
            }
            // Salwa głazów: 6 głazów w losowe kierunki. Cooldown 5s
            else if (cdAbility2 <= 0.f) {
                for(int i=0; i<6; i++) { //
                    float angle = (rand() % 360) * 3.14159f / 180.f;
                    sf::Vector2f pDir(std::cos(angle), std::sin(angle));
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, pDir, 220.f, true)); // Prędkość 220 px/s
                }
                cdAbility2 = 5.f; //
            }
            // Trzęsienie ziemi (Faza 2): Spowalnia gracza. Cooldown 8s
            else if (cdAbility3 <= 0.f && phase >= 1) {
                actionState = 2; actionTimer = 0.f;
            }
            // Kryształy chaosu (<30% HP mechanika powtarzalna)
            else if (cdAbility4 <= 0.f && hp < maxHp * 0.30f && summonedMinions.empty()) {
                minionPositions.clear();
                for(int i=0; i<3; i++) { // 3 świecące kryształy
                    sf::Vector2f cPos(position.x + (rand()%400-200), position.y + (rand()%400-200));
                    auto crystal = std::make_shared<Enemy>(cPos.x, cPos.y, EnemyType::KAMIKAZE, nullptr, 16.f, 0.f); // Używamy zdrowego potwora jako bazy krysztalu (16.f * 5 hp = 80 HP)
                    pendingSpawns.push_back(crystal);
                    summonedMinions.push_back(crystal);
                    minionPositions.push_back(cPos);
                }
                cdAbility4 = 12.f; // Kryształy odradzają się po 12s od zniszczenia
            }
        }
        else if (actionState == 1) { // Kamienne uderzenie
            overrideMovement = true;
            actionTimer += deltaTime;
            if (actionTimer >= 0.6f) { // Telegraf 0.6s wibracja
                // Fala krzyżowa (4 kierunki)
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(1,0), 300.f, true));
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(-1,0), 300.f, true));
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(0,1), 300.f, true));
                pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(0,-1), 300.f, true));

                actionState = 0;
                cdAbility1 = 3.f; //
            }
        }
        else if (actionState == 2) { // Trzęsienie ziemi
            overrideMovement = true; // Boss stoi w miejscu
            actionTimer += deltaTime;
            if (actionTimer >= 1.2f) { // Animacja trwa 1.2s
                // Uderzenie oburącz AoE 400px, nakłada slowa
                if (distance < 400.f) {
                    currentSlowAmount = 0.5f; // Spowolnienie gracza (50% prędkości)
                    slowTimer = 2.0f; // przez 2s
                    target->takeDamage(20);
                }
                // Ekran drży (symulacja przez wypuszczenie gęstego ringu ostrzegawczego)
                for(int i=0; i<16; i++) {
                    float a = i * (3.14159f / 8.f);
                    pendingSpawns.push_back(std::make_shared<Projectile>(position.x, position.y, sf::Vector2f(std::cos(a), std::sin(a)), 500.f, true));
                }

                actionState = 0;
                cdAbility3 = 8.f; //
            }
        }
    }

    // Standardowe poruszanie się, jeśli nie zablokowane
    if (!overrideMovement && distance > 0 && currentSpeed != 0.f) {
        position.x += dir.x * currentSpeed * deltaTime;
        position.y += dir.y * currentSpeed * deltaTime;
    }

    sprite.setPosition(position);
}

void Boss::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Boss::getBounds() const {
    return sprite.getGlobalBounds();
}