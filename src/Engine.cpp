#include "Engine.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Bonus.h"
#include "Obstacle.h"
#include "XpCrystal.h"
#include "WaveManager.h"
#include "DamageNumber.h"
#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <cmath>
#include <fstream>
#include <ctime>

namespace fs = std::filesystem;

Engine::Engine() {
    // Rozmiar okna
    window.create(sf::VideoMode(1280, 720), "Eternal Night");
    window.setFramerateLimit(60);

    // Gra startuje w Menu Głównym
    currentState = GameState::MAIN_MENU;
    gameTime = 0.f;

    // Pasek XP
    // Tło paska
    xpBarBackground.setSize(sf::Vector2f(1280.f, 22.f)); // Pasek na całą szerokość ekranu
    xpBarBackground.setFillColor(sf::Color(20, 20, 30));
    xpBarBackground.setPosition(0.f, 0.f);

    // Wypełnienie XP
    xpBarForeground.setSize(sf::Vector2f(0.f, 22.f));
    xpBarForeground.setFillColor(sf::Color(30, 140, 255)); // Błękitny kolor XP
    xpBarForeground.setPosition(0.f, 0.f);

    // Obwódka XP
    xpBarBorder.setSize(sf::Vector2f(1280.f, 22.f));
    xpBarBorder.setFillColor(sf::Color::Transparent);
    xpBarBorder.setOutlineColor(sf::Color(180, 130, 30));
    xpBarBorder.setOutlineThickness(2.f);
    xpBarBorder.setPosition(0.f, 0.f);

    // Tło trawy
    if(grassTexture.loadFromFile("assets/map/background/floor/grass.png")) {
        grassBackground.setTexture(grassTexture);
        grassBackground.setScale(2.f, 2.f);
    }

    // Czcionka i UI
    if(font.loadFromFile("assets/fonts/PixelGame.otf")) {
        sf::FloatRect tb;

        // LVL na prawym końcu paska XP
        xpLevelText.setFont(font);
        xpLevelText.setCharacterSize(14);
        xpLevelText.setFillColor(sf::Color::White);
        xpLevelText.setString("LV 1"); // Pozycja ustalana w render()

        // Timer pod paskiem
        timerText.setFont(font);
        timerText.setCharacterSize(28);
        timerText.setFillColor(sf::Color(230, 230, 230));
        timerText.setOutlineColor(sf::Color(0, 0, 0, 180));
        timerText.setOutlineThickness(2.f);
        timerText.setString("00:00"); // Pozycja ustalana w render()

        // Licznik zabitych
        killCountText.setFont(font);
        killCountText.setCharacterSize(16);
        killCountText.setFillColor(sf::Color(220, 200, 200));
        killCountText.setOutlineColor(sf::Color::Black);
        killCountText.setOutlineThickness(1.5f);
        killCountText.setString("0");

        // Placeholder czaszki
        skullIcon.setSize(sf::Vector2f(14.f, 14.f));
        skullIcon.setFillColor(sf::Color(200, 60, 60));
        skullIcon.setOutlineColor(sf::Color(80, 0, 0));
        skullIcon.setOutlineThickness(1.f); // Pozycja ustalana dynamicznie

        // HUD statystyki
        hudStatsText.setFont(font);
        hudStatsText.setCharacterSize(14);
        hudStatsText.setFillColor(sf::Color(190, 190, 190));
        hudStatsText.setPosition(10.f, 58.f); // Pod timerem

        // Ekran awansu
        textTitle.setFont(font);
        textTitle.setString("LEVEL UP! CHOOSE UPGRADE");
        textTitle.setCharacterSize(40);
        textTitle.setFillColor(sf::Color::White);
        tb = textTitle.getLocalBounds();
        textTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textTitle.setPosition(640.f, 120.f);

        card1.setSize(sf::Vector2f(250.f, 350.f)); card1.setPosition(200.f, 200.f);
        textCard1.setFont(font); textCard1.setCharacterSize(24); textCard1.setPosition(220.f, 350.f);

        card2.setSize(sf::Vector2f(250.f, 350.f)); card2.setPosition(500.f, 200.f);
        textCard2.setFont(font); textCard2.setCharacterSize(24); textCard2.setPosition(520.f, 350.f);

        card3.setSize(sf::Vector2f(250.f, 350.f)); card3.setPosition(800.f, 200.f);
        textCard3.setFont(font); textCard3.setCharacterSize(24); textCard3.setPosition(820.f, 350.f);

        // Menu główne
        menuTitle.setFont(font);
        menuTitle.setString("ETERNAL NIGHT");
        menuTitle.setCharacterSize(80);
        menuTitle.setFillColor(sf::Color(255, 200, 0));
        tb = menuTitle.getLocalBounds();
        menuTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        menuTitle.setPosition(640.f, 140.f);

        // Przycisk START
        btnStart.setSize(sf::Vector2f(300.f, 60.f));
        btnStart.setPosition(490.f, 300.f);
        btnStart.setFillColor(sf::Color(50, 50, 100));

        textStart.setFont(font);
        textStart.setString("Start game");
        textStart.setCharacterSize(30);
        tb = textStart.getLocalBounds();
        textStart.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        // Pozycja: środek przycisku
        textStart.setPosition(640.f, 330.f);

        // Przycisk QUIT
        btnQuit.setSize(sf::Vector2f(300.f, 60.f));
        btnQuit.setPosition(490.f, 400.f);
        btnQuit.setFillColor(sf::Color(100, 50, 50));

        textQuit.setFont(font);
        textQuit.setString("Quit");
        textQuit.setCharacterSize(30);
        tb = textQuit.getLocalBounds();
        textQuit.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textQuit.setPosition(640.f, 430.f);

        // 3. MENU PAUZY
        textPaused.setFont(font);
        textPaused.setString("PAUSE");
        textPaused.setCharacterSize(80);
        textPaused.setFillColor(sf::Color::Yellow);
        tb = textPaused.getLocalBounds();
        textPaused.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textPaused.setPosition(640.f, 190.f);

        // Przycisk WZNÓW (RESUME)
        btnResume.setSize(sf::Vector2f(300.f, 60.f));
        btnResume.setPosition(490.f, 300.f);
        btnResume.setFillColor(sf::Color(50, 100, 50));

        textResume.setFont(font);
        textResume.setString("Resume game");
        textResume.setCharacterSize(30);
        tb = textResume.getLocalBounds();
        textResume.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textResume.setPosition(640.f, 330.f);

        // Przycisk POWRÓT Z PAUZY
        btnPauseReturn.setSize(sf::Vector2f(300.f, 60.f));
        btnPauseReturn.setPosition(490.f, 400.f);
        btnPauseReturn.setFillColor(sf::Color(100, 50, 50));

        textPauseReturn.setFont(font);
        textPauseReturn.setString("Back to menu");
        textPauseReturn.setCharacterSize(30);
        tb = textPauseReturn.getLocalBounds();
        textPauseReturn.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textPauseReturn.setPosition(640.f, 430.f);

        // Ekran śmierci
        textGameOver.setFont(font);
        textGameOver.setString("You died");
        textGameOver.setCharacterSize(80);
        textGameOver.setFillColor(sf::Color::Red);
        tb = textGameOver.getLocalBounds();
        textGameOver.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textGameOver.setPosition(640.f, 190.f);

        // Wynik ostateczny na środku ekranu
        textFinalScore.setFont(font);
        textFinalScore.setCharacterSize(40);
        textFinalScore.setPosition(640.f, 320.f);

        // Przycisk POWRÓT DO MENU
        btnReturn.setSize(sf::Vector2f(300.f, 60.f));
        btnReturn.setPosition(490.f, 450.f);
        btnReturn.setFillColor(sf::Color(80, 80, 80));

        textReturn.setFont(font);
        textReturn.setString("Back to menu");
        textReturn.setCharacterSize(30);

        sf::FloatRect returnBounds = textReturn.getLocalBounds();
        textReturn.setOrigin(returnBounds.left + returnBounds.width / 2.f, returnBounds.top + returnBounds.height / 2.f);
        textReturn.setPosition(640.f, 480.f);


        // Przycisk TOP 10 w menu
        btnScores.setSize(sf::Vector2f(300.f, 60.f));
        btnScores.setPosition(490.f, 500.f);
        btnScores.setFillColor(sf::Color(30, 80, 30));

        textBtnScores.setFont(font);
        textBtnScores.setString("Top 10");
        textBtnScores.setCharacterSize(30);
        tb = textBtnScores.getLocalBounds();
        textBtnScores.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textBtnScores.setPosition(640.f, 530.f);

        // Ekran wyników
        textScoresTitle.setFont(font);
        textScoresTitle.setString("TOP 10 SCORES");
        textScoresTitle.setCharacterSize(50);
        textScoresTitle.setFillColor(sf::Color(255, 200, 0));
        tb = textScoresTitle.getLocalBounds();
        textScoresTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textScoresTitle.setPosition(640.f, 80.f);

        textScoresList.setFont(font);
        textScoresList.setCharacterSize(22);
        textScoresList.setFillColor(sf::Color::White);
        textScoresList.setPosition(300.f, 160.f);

        btnReturnFromScores.setSize(sf::Vector2f(300.f, 60.f));
        btnReturnFromScores.setPosition(490.f, 600.f);
        btnReturnFromScores.setFillColor(sf::Color(80, 80, 80));

        textReturnFromScores.setFont(font);
        textReturnFromScores.setString("Return to menu");
        textReturnFromScores.setCharacterSize(30);
        tb = textReturnFromScores.getLocalBounds();
        textReturnFromScores.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textReturnFromScores.setPosition(640.f, 630.f);

        // Wpisywanie imienia
        textEnterName.setFont(font);
        textEnterName.setString("Enter your name:");
        textEnterName.setCharacterSize(28);
        textEnterName.setFillColor(sf::Color::White);
        tb = textEnterName.getLocalBounds();
        textEnterName.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        textEnterName.setPosition(640.f, 400.f);

        textNameInput.setFont(font);
        textNameInput.setCharacterSize(32);
        textNameInput.setFillColor(sf::Color(255, 200, 0));
        textNameInput.setPosition(640.f, 440.f);

        playerName = "";
        nameSaved = false;
    }

    // Debug
    debugMode = false;
}

void Engine::handleEvents() {
    sf::Event event;
    while(window.pollEvent(event)) {
        if(event.type == sf::Event::Closed) {
            window.close();
        }

        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (currentState == GameState::PLAYING) {
                currentState = GameState::PAUSED; // Włącz pauzę
            } else if (currentState == GameState::PAUSED) {
                currentState = GameState::PLAYING; // Wyłącz pauzę (wznów)
            } else if (currentState == GameState::MAIN_MENU) {
                window.close(); // Wyjdź z gry tylko z poziomu menu głównego
            }
        }
        // Obsługa debug
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3) {
            debugMode = !debugMode;
        }
        // OBSŁUGA MYSZKI W ZALEŻNOŚCI OD EKRANU
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window),window.getDefaultView());

            if (currentState == GameState::MAIN_MENU) {
                if (btnStart.getGlobalBounds().contains(mousePos)) {
                    resetGame(); // Resetuje i ładuje gracza/przeszkody
                    currentState = GameState::PLAYING;
                } else if (btnQuit.getGlobalBounds().contains(mousePos)) {
                    window.close();
                } else if (btnScores.getGlobalBounds().contains(mousePos)) {
                    textScoresList.setString(loadScores());
                    currentState = GameState::SCORES;
                }


            }
            // OBSŁUGA KLIKNIĘĆ W MENU PAUZY
            else if (currentState == GameState::PAUSED) {
                if (btnResume.getGlobalBounds().contains(mousePos)) {
                    currentState = GameState::PLAYING;
                } else if (btnPauseReturn.getGlobalBounds().contains(mousePos)) {
                    currentState = GameState::MAIN_MENU;
                }
            }
            else if (currentState == GameState::GAME_OVER) {
                if (btnReturn.getGlobalBounds().contains(mousePos)) {
                    currentState = GameState::MAIN_MENU; // Wracamy do menu startowego
                    playerName = "";
                    nameSaved = false;
                }
            }
            else if (currentState == GameState::LEVEL_UP) {
                if (card1.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[0]); currentState = GameState::PLAYING; }
                else if (card2.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[1]); currentState = GameState::PLAYING; }
                else if (card3.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[2]); currentState = GameState::PLAYING; }
            }
        }
        // Wpisywanie imienia na Game Over
        if (currentState == GameState::GAME_OVER && !nameSaved) {
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && !playerName.empty()) {
                    // Backspace
                    playerName.pop_back();
                } else if (event.text.unicode >= 32 && event.text.unicode < 128 && playerName.size() < 16) {
                    playerName += static_cast<char>(event.text.unicode);
                }
                textNameInput.setString(playerName + "_");
                sf::FloatRect nb = textNameInput.getLocalBounds();
                textNameInput.setOrigin(nb.left + nb.width / 2.f, nb.top + nb.height / 2.f);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !playerName.empty()) {
                int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);
                saveScore(playerName, score);
                nameSaved = true;
            }
        }

        // Obsługa ekranu wyników
        if (currentState == GameState::SCORES) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
                if (btnReturnFromScores.getGlobalBounds().contains(mousePos)) {
                    currentState = GameState::MAIN_MENU;
                }
            }
        }
    }
}

void Engine::resetGame() {
    gameObjects.clear();

    // Tworzymy od nowa gracza na środku ekranu
    player = std::make_shared<Player>(640.f, 360.f);
    gameObjects.push_back(player);
    lastObstacleSpawnPos = player->getPosition();

    // Resetujemy statystyki fali i czasu
    gameTime = 0.f;
    waveManager = std::make_unique<WaveManager>(player, gameObjects);
    waveManager->reset();

    // LOSOWE GENEROWANIE PRZESZKÓD
    for (int i = 0; i < 25; ++i) {
        float randX = player->getPosition().x + (rand() % 2400) - 1200;
        float randY = player->getPosition().y + (rand() % 2400) - 1200;
        int randType = rand() % 3;

        ObstacleType type;
        if (randType == 0) type = ObstacleType::TREE;
        else if (randType == 1) type = ObstacleType::BUSH;
        else type = ObstacleType::ROCK;

        gameObjects.push_back(std::make_shared<Obstacle>(randX, randY, type));
    }

    // LOSOWE MIKSTURY
    for (int i = 0; i < 3; ++i) {
        bool isColliding = true;
        std::shared_ptr<Bonus> newPotion;

        // Pętla będzie losować tak długo aż znajdzie wolne miejsce
        while (isColliding) {
            float randX = (rand() % 4000) - 2000 + 640;
            float randY = (rand() % 4000) - 2000 + 360;
            // Tworzymy testową miksturę na wylosowanych kordach
            newPotion = std::make_shared<Bonus>(randX, randY, BonusType::POTION);

            isColliding = false;

            for (const auto& obj : gameObjects) {
                if (newPotion->getBounds().intersects(obj->getBounds())) {
                    isColliding = true;
                    break; // Przerywamy pętlę i losujemy pozycję od nowa
                }
            }
        }
        // Znalazło bezpieczne miejsce, dodajemy miksturę na mapę
        gameObjects.push_back(newPotion);
    }
}

// Tworzenie unoszących się obrażeń
void Engine::spawnDamageNumber(float x, float y, int damage, bool isCrit) {
    if(damage <= 0) return;
    // Drobne losowe przesunięcie poziome żeby liuczby sie nie nakładały
    float offsetX = static_cast<float>((rand() % 30) - 15);
    gameObjects.push_back(std::make_shared<DamageNumber>(x + offsetX, y - 20.f, damage, isCrit, font));
}

void Engine::update(float deltaTime) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), gameView);
    player->setMousePosition(mousePos);

    if(player->getWantsToShootSpecial()) {
        sf::Vector2f spawnPos = player->getPosition();
        sf::Vector2f dir = player->getLastDirection();
        gameObjects.push_back(std::make_shared<Projectile>(spawnPos.x, spawnPos.y, dir));
        player->resetSpecialShot();
    }
    // Różdżka - samonaprowadzająca
    if (player->wandReady()) {
        // Znajdź najbliższego wroga
        float closestDist = 99999.f;
        Enemy* closestEnemy = nullptr;
        for (auto& obj : gameObjects) {
            if (!obj->isActive()) continue;
            auto e = dynamic_cast<Enemy*>(obj.get());
            if (e) {
                float dx = e->getPosition().x - player->getPosition().x;
                float dy = e->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx*dx + dy*dy);
                if (dist < closestDist) {
                    closestDist = dist;
                    closestEnemy = e;
                }
            }
        }

        if (closestEnemy && closestDist < 600.f) {
            int count = player->getWandProjectiles();
            for (int i = 0; i < count; i++) {
                // Lekkie losowe rozproszenie dla wielu pocisków
                float spreadAngle = (count > 1) ? (i - (count-1)/2.f) * 15.f : 0.f;
                float rad = spreadAngle * 3.14159f / 180.f;
                sf::Vector2f dir = closestEnemy->getPosition() - player->getPosition();
                float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
                if (len > 0) dir /= len;
                // Obróć wektor o spreadAngle
                sf::Vector2f rotDir(
                    dir.x * std::cos(rad) - dir.y * std::sin(rad),
                    dir.x * std::sin(rad) + dir.y * std::cos(rad)
                    );
                auto proj = std::make_shared<Projectile>(
                    player->getPosition().x, player->getPosition().y,
                    rotDir, 350.f, false, true
                    );
                proj->setHomingTarget(closestEnemy->getPosition());
                gameObjects.push_back(proj);
            }
            player->resetWandTimer();
        }
    }
    player->tickWandTimer(deltaTime);

    for(auto& obj : gameObjects) {
        if(obj->isActive()) {
            obj->update(deltaTime);
        }
    }

    std::vector<std::shared_ptr<GameObject>> newObjects;

    for(auto& obj : gameObjects) {
        if(!obj->isActive()) continue;

        // POCISKI
        auto proj = dynamic_cast<Projectile*>(obj.get());
        if(proj) {
            if(proj->getIsEnemyOwned()) {
                if(proj->getBounds().intersects(player->getBounds())) {
                    player->takeDamage(15);
                    proj->destroy();
                }
            }
            continue;
        }

        // WROGOWIE
        auto enemy = dynamic_cast<Enemy*>(obj.get());
        if(enemy) {
            // Gracz - Wróg
            if(player->getBounds().intersects(enemy->getBounds())) {
                if(enemy->getIsKamikaze()) {
                    player->takeDamage(30);
                    enemy->takeDamage(9999);
                } else {
                    player->takeDamage(10);
                    if(enemy->getIsVampire()) {
                        enemy->heal(5);
                    }
                }
            }

            // Atak obszarowy
            if(player->getIsAttacking() && player->getAttackBounds().intersects(enemy->getBounds())) {
                bool wasCrit = false;
                int dmg = player->getDamage(25, &wasCrit);
                int dealt = enemy->takeDamage(dmg);
                if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit);
                if(!enemy->isActive()) {
                    player->incrementKills();
                    player->triggerVampirism();
                    newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                }
            }

            // Aura Ognia
            if(player->getHasFireAura()) {
                float dx = enemy->getPosition().x - player->getPosition().x;
                float dy = enemy->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if(dist <= player->getFireAuraRadius()) {
                    bool wasCrit = false;
                    int dmg = player->getDamage(3, &wasCrit);
                    int dealt = enemy->takeDamage(dmg, 1);
                    if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit);
                    if(!enemy->isActive()) {
                        player->incrementKills();
                        player->triggerVampirism();
                        newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                    }
                }
            }

            // Orbitujące Ostrze
            if(player->getHasOrbitingSword()) {
                if(player->getOrbitingSwordBounds().intersects(enemy->getBounds())) {
                    bool wasCrit = false;
                    int dmg = player->getDamage(15, &wasCrit);
                    int dealt = enemy->takeDamage(dmg, 2);
                    if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit);
                    if(!enemy->isActive()) {
                        player->incrementKills();
                        player->triggerVampirism();
                        newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                    }
                }
            }

            // Kusza - Wróg
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile && !projectile->getIsEnemyOwned()) {
                    if(projectile->getBounds().intersects(enemy->getBounds())) {
                        bool wasCrit = false;
                        int baseDmg = projectile->getIsWand() ? (35 + player->getWandDamageBonus()) : 50;
                        int dmg = player->getDamage(baseDmg, &wasCrit);
                        int dealt = enemy->takeDamage(dmg);
                        if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit);
                        if(!enemy->isActive()) {
                            player->incrementKills();
                            player->triggerVampirism();
                            newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                        }
                        projectile->destroy();
                    }
                }
            }

            // Wróg - Wróg
            if(!enemy->getIsGhost()) {
                for(auto& otherObj : gameObjects) {
                    if(!otherObj->isActive() || obj == otherObj) continue;
                    auto otherEnemy = dynamic_cast<Enemy*>(otherObj.get());
                    if(otherEnemy) {
                        sf::FloatRect intersection;
                        if(enemy->getBounds().intersects(otherEnemy->getBounds(), intersection)) {
                            sf::Vector2f pos = enemy->getPosition();
                            if(intersection.width < intersection.height) {
                                if(enemy->getBounds().left < otherEnemy->getBounds().left) pos.x -= intersection.width / 2.f;
                                else pos.x += intersection.width / 2.f;
                            } else {
                                if(enemy->getBounds().top < otherEnemy->getBounds().top) pos.y -= intersection.height / 2.f;
                                else pos.y += intersection.height / 2.f;
                            }
                            enemy->setPosition(pos);
                        }
                    }
                }
            }

            // Rzut Głazem
            if(enemy->checkAndResetThrow()) {
                sf::Vector2f dir = player->getPosition() - enemy->getPosition();
                float length = std::hypot(dir.x, dir.y);
                if(length > 0 && length < 500.f){
                dir /= length;
                newObjects.push_back(std::make_shared<Projectile>(enemy->getPosition().x, enemy->getPosition().y, dir, 200.0f, true));
                }
            }
        } // koniec if(enemy)

        // KRYSZTAŁY XP
        auto xp = dynamic_cast<XpCrystal*>(obj.get());
        if(xp) {
            if(!xp->isHoming() && player->getPickupBounds().intersects(xp->getBounds())) {
                xp->startHoming();
            }
            if(xp->isHoming()) {
                xp->setTargetPos(player->getPosition());
                float dx = xp->getPosition().x - player->getPosition().x;
                float dy = xp->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if(dist < 18.f) {
                    player->addXp(xp->getXpValue());
                    xp->destroy();
                }
            }
        }

        // BONUSY
        auto bonus = dynamic_cast<Bonus*>(obj.get());
        if(bonus) {
            if(player->getPickupBounds().intersects(bonus->getBounds())) {
                if(bonus->getType() == BonusType::POTION) {
                    player->heal(20);
                }
                bonus->destroy();
            }
        }

        // PRZESZKODY
        auto obstacle = dynamic_cast<Obstacle*>(obj.get());
        if(obstacle) {
            sf::FloatRect obsBounds = obstacle->getBounds();
            sf::FloatRect intersection;

            // Gracz - Przeszkoda
            if(player->getBounds().intersects(obsBounds, intersection)) {
                sf::Vector2f newPos = player->getPosition();
                if(intersection.width < intersection.height) {
                    if(player->getBounds().left < obsBounds.left) newPos.x -= intersection.width;
                    else newPos.x += intersection.width;
                } else {
                    if(player->getBounds().top < obsBounds.top) newPos.y -= intersection.height;
                    else newPos.y += intersection.height;
                }
                player->setPosition(newPos);
            }

            // Wróg - Przeszkoda
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto otherEnemy = dynamic_cast<Enemy*>(otherObj.get());
                if(otherEnemy && !otherEnemy->getIsGhost()) {
                    sf::FloatRect enemyIntersection;
                    if(otherEnemy->getBounds().intersects(obsBounds, enemyIntersection)) {
                        sf::Vector2f newEnemyPos = otherEnemy->getPosition();
                        if(enemyIntersection.width < enemyIntersection.height) {
                            if(otherEnemy->getBounds().left < obsBounds.left) newEnemyPos.x -= enemyIntersection.width;
                            else newEnemyPos.x += enemyIntersection.width;
                        } else {
                            if(otherEnemy->getBounds().top < obsBounds.top) newEnemyPos.y -= enemyIntersection.height;
                            else newEnemyPos.y += enemyIntersection.height;
                        }
                        otherEnemy->setPosition(newEnemyPos);
                    }
                }
            }
        } // koniec if(obstacle)

        } // koniec for(obj)

    for(auto& newObj : newObjects) {
        gameObjects.push_back(newObj);
    }

    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
                       [](const std::shared_ptr<GameObject>& obj) { return !obj->isActive(); }),
        gameObjects.end()
        );

    if(player->checkLevelUp()) {
        generateUpgrades();
        currentState = GameState::LEVEL_UP;
        player->acknowledgeLevelUp();
    }

    if(player->getHp() <= 0) {
        currentState = GameState::GAME_OVER;
        int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);
        textFinalScore.setString("Twoj ostateczny wynik: " + std::to_string(score));
        sf::FloatRect scoreBounds = textFinalScore.getLocalBounds();
        textFinalScore.setOrigin(scoreBounds.left + scoreBounds.width / 2.f, scoreBounds.top + scoreBounds.height / 2.f);
        textFinalScore.setPosition(640.f, 320.f);
    }

    manageInfiniteMap();
}

void Engine::renderHUD() {
    // Pasek XP
    float xpPercent = static_cast<float>(player->getXp()) / static_cast<float>(player->getMaxXp());
    xpBarForeground.setSize(sf::Vector2f(1280.f * xpPercent, 22.f));

    window.draw(xpBarBackground);
    window.draw(xpBarForeground);
    window.draw(xpBarBorder);

    // "Lv X"
    xpLevelText.setString("LV " + std::to_string(player->getLevel()));
    sf::FloatRect lvBounds = xpLevelText.getLocalBounds();
    xpLevelText.setOrigin(lvBounds.left + lvBounds.width, lvBounds.top + lvBounds.height / 2.f);
    xpLevelText.setPosition(1272.f, 11.f); // Prawa strona, środek paska
    window.draw(xpLevelText);

    // Timer
    int minutes = static_cast<int>(gameTime) / 60;
    int seconds = static_cast<int>(gameTime) % 60;
    std::string timeStr = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
    timerText.setString(timeStr);
    sf::FloatRect timeBounds = timerText.getLocalBounds();
    timerText.setOrigin(timeBounds.left + timeBounds.width / 2.f, timeBounds.top);
    timerText.setPosition(640.f, 26.f); // Wyśrodkowany pod paskiem XP
    window.draw(timerText);

    // Licznik zabitych
    killCountText.setString(std::to_string(player->getEnemiesKilled()));
    sf::FloatRect killBounds = killCountText.getLocalBounds();
    // Tekst z liczbą, po prawej ikonka czaszki
    float killTextX = 1240.f - killBounds.width - 20.f;
    killCountText.setPosition(killTextX, 27.f);
    window.draw(killCountText);

    // Czaszka
    skullIcon.setPosition(killTextX + killBounds.width + 4.f, 29.f);
    window.draw(skullIcon);

    // Statystyki gracza
    int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);

    //Rysowanie HUD
    std::string statsStr = "SCORE: " + std::to_string(score) +
                           "\nWAVE: " + std::to_string(waveManager ? waveManager->getCurrentWave() : 1) + " (+" + std::to_string(waveManager ? (int)waveManager->getSecondsToNextWave() : 0) + "s)" +
                           "\nARMOR: " + std::to_string(player->getArmor()) +
                           "\nDMG+: +" + std::to_string(player->getDamageBonus()) +
                           "\nCRIT: " + std::to_string(static_cast<int>(player->getCritChance() * 100)) + "%" +
                           "\nVAMPIRE: " + std::to_string(static_cast<int>(player->getVampirismChance() * 100)) + "%" +
                           "\nDODGE: " + std::to_string(static_cast<int>(player->getDodgeChance() * 100)) + "%" +
                           "\nREGEN: " + std::to_string(player->getHpRegenRate()) + " HP/5s" +
                           "\nSPEED: " + std::to_string(static_cast<int>(player->getSpeed()));
    hudStatsText.setString(statsStr);
    window.draw(hudStatsText);

    window.draw(xpBarBackground);
    window.draw(xpBarForeground);
}

void Engine::renderLevelUpOverlay() {
    // Rysowanie menu awansu
    if (currentState == GameState::LEVEL_UP) {
        sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        window.draw(card1); window.draw(textCard1);
        window.draw(card2); window.draw(textCard2);
        window.draw(card3); window.draw(textCard3);
        window.draw(textTitle);
    }
}

void Engine::renderGameOverOverlay() {
    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 220)); // Mocne przyciemnienie tła
    window.draw(overlay);

    // Rysowanie napisów ekranu śmierci
    window.draw(textGameOver);
    window.draw(textFinalScore);
    window.draw(btnReturn); window.draw(textReturn);
    if (!nameSaved) {
        window.draw(textEnterName);
        window.draw(textNameInput);
    } else {
        sf::Text savedMsg;
        savedMsg.setFont(font);
        savedMsg.setString("Score saved!");
        savedMsg.setCharacterSize(24);
        savedMsg.setFillColor(sf::Color(0, 255, 0));
        sf::FloatRect sb = savedMsg.getLocalBounds();
        savedMsg.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
        savedMsg.setPosition(640.f, 410.f);
        window.draw(savedMsg);
    }
}

void Engine::renderPauseOverlay() {
    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    window.draw(textPaused);
    window.draw(btnResume); window.draw(textResume);
    window.draw(btnPauseReturn); window.draw(textPauseReturn);
}

void Engine::renderScoresScreen() {
    window.clear(sf::Color(30, 30, 40));
    sf::RectangleShape panel(sf::Vector2f(760.f, 500.f));
    panel.setFillColor(sf::Color(40, 50, 70));
    panel.setOutlineColor(sf::Color(180, 130, 30));
    panel.setOutlineThickness(2.f);
    panel.setPosition(260.f, 50.f);
    window.draw(panel);
    window.draw(textScoresTitle);
    window.draw(textScoresList);
    window.draw(btnReturnFromScores);
    window.draw(textReturnFromScores);
}

void Engine::render() {
    window.clear(sf::Color(30, 30, 40));

    if (currentState == GameState::MAIN_MENU) {
        // Ustawienie tymczasowej kamery tylko dla menu
        gameView.setSize(1280.f, 720.f);
        gameView.setCenter(640.f, 360.f);
        window.setView(gameView);

        drawTerrain();
        // Wracamy do widoku interfejsu żeby napisy nie były zniekształcone
        window.setView(window.getDefaultView());
        //  Rysowanie przycisków i tytułów
        window.draw(menuTitle);
        window.draw(btnStart); window.draw(textStart);
        window.draw(btnQuit); window.draw(textQuit);
        window.draw(btnScores);
        window.draw(textBtnScores);

    } else {
        //  KAMERA ŚWIATA (Podąża za graczem)
        gameView.setSize(1280.f, 720.f);
        gameView.setCenter(player->getPosition());
        window.setView(gameView);
        // RYSOWANIE GRY (Dla stanów PLAYING, LEVEL_UP, GAME_OVER)
        drawTerrain();
        // Rysowanie wszystkich obiektów
        for(auto& obj : gameObjects) {
            if(obj->isActive()) {
                obj->draw(window);
            }
        }
        // Rysowanie hitboxów musi odbywać się w świecie gry
        if(debugMode) drawDebugOverlay();
        // Kamera interfejsu
        window.setView(window.getDefaultView());
        renderHUD();

        if      (currentState == GameState::LEVEL_UP)   renderLevelUpOverlay();
        else if (currentState == GameState::GAME_OVER)  renderGameOverOverlay();
        else if (currentState == GameState::PAUSED)     renderPauseOverlay();
    }
    if (currentState == GameState::SCORES) renderScoresScreen();
    window.display();
}

void Engine::run() {
    while(window.isOpen()) {
        // Obliczanie czasu, jaki upłynął od ostatniej klatki
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        //Pauza
        if (currentState == GameState::PLAYING) {
            update(deltaTime);

            gameTime += deltaTime;
            if(waveManager) waveManager->update(deltaTime);
        }

        render();
    }
}

void Engine::generateUpgrades() {
    std::vector<int> selected;
    while(selected.size() < 3) {
        int r = rand() % 14;
        if(std::find(selected.begin(), selected.end(), r) == selected.end()) {
            selected.push_back(r);
        }
    }

    for(int i = 0; i < 3; ++i) {
        offeredUpgrades[i] = selected[i];
        std::wstring text;
        sf::Color color;

        switch (selected[i]) {
        case 0: text = L"+20 Max HP\n& Leczenie"; color = sf::Color(50, 100, 50); break;
        case 1: text = L"+15% Szybkości\nRuchu"; color = sf::Color(50, 50, 100); break;
        case 2: text = L"-10% Czasu\nOdnowienia\nAtaku"; color = sf::Color(100, 50, 50); break;
        case 3: text = L"+5 Obrażeń\nAtaku"; color = sf::Color(120, 40, 40); break;
        case 4: text = L"+2 Punkty\nPancerza"; color = sf::Color(100, 100, 30); break;
        case 5: text = L"+5% Szansy\nna Krytyk"; color = sf::Color(100, 50, 100); break;
        case 6: text = L"+5% Szansy na\nLeczenie (2HP)\npo zabójstwie"; color = sf::Color(140, 20, 50); break;
        case 7: text = L"+ Zasięg\nPodnoszenia\n(Magnes)"; color = sf::Color(100, 100, 150); break;
        case 8: text = L"+10% Szansy\nna Uniknięcie\nObrażeń"; color = sf::Color(40, 120, 120); break;
        case 9: text = L"-20% Czasu\nOdnowienia\nKuszy"; color = sf::Color(150, 100, 50); break;
        case 10: text = L"Regeneracja\n+1 HP co\n5 sekund"; color = sf::Color(50, 150, 50); break;
        case 11: text = L"Aura Ognia\n(Odblokuj / Powiększ)"; color = sf::Color(200, 80, 0); break;
        case 12: text = L"Orbitujące Ostrze\n(Odblokuj / Przyspiesz)"; color = sf::Color(0, 150, 200); break;
        case 13: text = L"Różdżka\n(Odblokuj / Ulepsz)"; color = sf::Color(120, 0, 200); break;
        }

        if(i == 0) { textCard1.setString(text); card1.setFillColor(color); }
        else if(i == 1) { textCard2.setString(text); card2.setFillColor(color); }
        else if(i == 2) { textCard3.setString(text); card3.setFillColor(color); }
    }
}

// Rysowanie prostokąta hitboxa
void Engine::drawDebugRect(sf::FloatRect b, sf::Color color) {
    sf::RectangleShape rect(sf::Vector2f(b.width, b.height));
    rect.setPosition(b.left, b.top);
    rect.setFillColor(sf::Color(color.r, color.g, color.b, 40));
    rect.setOutlineColor(color);
    rect.setOutlineThickness(1.f);
    window.draw(rect);
}

void Engine::drawDebugCircle(sf::Vector2f center, float radius, sf::Color color) {
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(center);
    circle.setFillColor(sf::Color(color.r, color.g, color.b, 30));
    circle.setOutlineColor(color);
    circle.setOutlineThickness(1.f);
    window.draw(circle);
}

void Engine::drawDebugOverlay() {
    for(auto& obj : gameObjects) {
        if(!obj->isActive() || obj.get() == player.get()) continue;

        sf::FloatRect b = obj->getBounds();
        sf::Color color;

        if      (dynamic_cast<Enemy*>       (obj.get()))    color = sf::Color(255, 60, 60);
        else if (dynamic_cast<Projectile*>  (obj.get()))    color = sf::Color(255, 255, 0);
        else if (dynamic_cast<XpCrystal*>   (obj.get()))    color = sf::Color(0, 220, 220);
        else if (dynamic_cast<Bonus*>       (obj.get()))    color = sf::Color(220, 0, 200);
        else if (dynamic_cast<Obstacle*>    (obj.get()))    color = sf::Color(180, 120, 40);
        else                                                color = sf::Color(200, 200, 200);

        drawDebugRect(b, color);
    }

    // Gracz
    // Hitbox ciała
    drawDebugRect(player->getBounds(), sf::Color(0, 255, 0));

    // Hitbox ataku
    if(player->getIsAttacking()) {
        drawDebugRect(player->getAttackBounds(), sf::Color(255, 180, 0));
    }

    // Zasięg podnoszenia
    drawDebugCircle(player->getPosition(), player->getPickupBounds().width / 2.f, sf::Color(80, 80, 255));

    // Orbitujace ostrze
    if(player->getHasOrbitingSword()) {
        drawDebugRect(player->getOrbitingSwordBounds(), sf::Color(0, 255, 220));
    }

    // Hitbox aury ognia
    if(player->getHasFireAura()) {
        drawDebugCircle(player->getPosition(), player->getFireAuraRadius(), sf::Color(255, 100, 0));
    }

    // Napis
    window.draw(debugLabel);
}


void Engine::manageInfiniteMap() {
    sf::Vector2f playerPos = player->getPosition();

    // 1. USUWANIE STARYCH OBIEKTÓW (Ochrona przed wyciekiem pamięci)
    for (auto& obj : gameObjects) {
        if (!obj->isActive()) continue;

        if (dynamic_cast<Obstacle*>(obj.get()) || dynamic_cast<XpCrystal*>(obj.get()) ||
            dynamic_cast<Bonus*>(obj.get()) || dynamic_cast<Enemy*>(obj.get()) ||
            dynamic_cast<Projectile*>(obj.get())) {

            float dx = obj->getPosition().x - playerPos.x;
            float dy = obj->getPosition().y - playerPos.y;
            float distanceSq = dx * dx + dy * dy;

            if (distanceSq > 2500.f * 2500.f) {
                obj->destroy();
            }
        }
    }

    // GENEROWANIE NOWYCH PRZESZKÓD PRZED GRACZEM
    float distMoved = std::hypot(playerPos.x - lastObstacleSpawnPos.x, playerPos.y - lastObstacleSpawnPos.y);

    // Zespawnuj nową linię drzew za każdym razem, gdy gracz przejdzie 350 pikseli
    if (distMoved > 350.f) {
        lastObstacleSpawnPos = playerPos;

        int obstaclesToSpawn = rand() % 4 + 2; // Losowo od 2 do 5 nowych obiektów
        for (int i = 0; i < obstaclesToSpawn; ++i) {
            // Generujemy na okręgu wokół gracza
            float angle = (rand() % 360) * 3.14159f / 180.f;
            float radius = 1300.f + (rand() % 300);

            float spawnX = playerPos.x + cos(angle) * radius;
            float spawnY = playerPos.y + sin(angle) * radius;

            ObstacleType type;
            int randType = rand() % 3;
            if (randType == 0) type = ObstacleType::TREE;
            else if (randType == 1) type = ObstacleType::BUSH;
            else type = ObstacleType::ROCK;

            gameObjects.push_back(std::make_shared<Obstacle>(spawnX, spawnY, type));
        }
    }
}

// Ta funkcja decyduje jaki kafelek ma leżeć na danych kordach (X, Y)
int Engine::getTileType(int tileX, int tileY) {
    int hash = tileX * 374761393 + tileY * 668265263;
    hash = (hash ^ (hash >> 13)) * 1274126177;
    return std::abs(hash) % 100;
}

// Ta funkcja rysuje tylko te kafelki które widać na ekranie
void Engine::drawTerrain() {
    float tileSize = 64.f;

    sf::Vector2f center = gameView.getCenter();
    sf::Vector2f size = gameView.getSize();

    //  Obliczamy krawędzie ekranu (i dodajemy +1/-1 kafelka marginesu żeby nie znikały na rogach)
    int leftTile   = static_cast<int>((center.x - size.x / 2.f) / tileSize) - 1;
    int rightTile  = static_cast<int>((center.x + size.x / 2.f) / tileSize) + 1;
    int topTile    = static_cast<int>((center.y - size.y / 2.f) / tileSize) - 1;
    int bottomTile = static_cast<int>((center.y + size.y / 2.f) / tileSize) + 1;

    //  Rysujemy siatkę widoczną tylko w kamerze
    for (int x = leftTile; x <= rightTile; ++x) {
        for (int y = topTile; y <= bottomTile; ++y) {

            int type = getTileType(x, y);

            // USTAWIANIE POZYCJI KAFELKA
            grassBackground.setPosition(x * tileSize, y * tileSize);

            grassBackground.setColor(sf::Color::White);
            window.draw(grassBackground);

            window.draw(grassBackground);
        }
    }
}

void Engine::saveScore(const std::string& name, int score) {
    // Tworzenie folderu jeśli nie istnieje (działa na Windows)
    fs::create_directories("assets/scores");

    // Wczytaj istniejące wyniki
    std::vector<std::pair<int, std::string>> scores;
    std::ifstream inFile("assets/scores/scores.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            scores.push_back({0, line}); // przechowujemy surową linię
        }
        inFile.close();
    }

    // Pobierz aktualną datę
    time_t t = time(nullptr);
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localtime(&t));

    // Dopisz nowy wynik jako linię: SCORE|NAME|DATE
    std::ofstream outFile("assets/scores/scores.txt", std::ios::app);
    if (outFile.is_open()) {
        outFile << score << "|" << name << "|" << dateStr << "\n";
        outFile.close();
    }
}

std::string Engine::loadScores() {
    std::vector<std::tuple<int, std::string, std::string>> scores;

    std::ifstream inFile("assets/scores/scores.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            size_t p1 = line.find('|');
            size_t p2 = line.rfind('|');
            if (p1 != std::string::npos && p2 != std::string::npos && p1 != p2) {
                int sc = std::stoi(line.substr(0, p1));
                std::string nm = line.substr(p1 + 1, p2 - p1 - 1);
                std::string dt = line.substr(p2 + 1);
                scores.push_back({sc, nm, dt});
            }
        }
        inFile.close();
    }

    // Sortuj malejąco po wyniku
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b);
    });

    // Zbuduj string do wyświetlenia
    std::string result = "";
    int count = std::min((int)scores.size(), 10);
    for (int i = 0; i < count; i++) {
        result += std::to_string(i + 1) + ".  " +
                  std::get<1>(scores[i]) + "     " +
                  std::to_string(std::get<0>(scores[i])) + "     " +
                  std::get<2>(scores[i]) + "\n\n";
    }
    if (result.empty()) result = "No scores yet!";
    return result;
}