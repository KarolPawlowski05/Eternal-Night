#include "Engine.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Bonus.h"
#include "Obstacle.h"
#include "XpCrystal.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <cmath>

Engine::Engine() {
    // Rozmiar okna
    window.create(sf::VideoMode(1280, 720), "Eternal Night");

    // Gra startuje w Menu Głównym
    currentState = GameState::MAIN_MENU;
    gameTime = 0.f;

    xpBarBackground.setSize(sf::Vector2f(1280.f, 15.f)); // Pasek na całą szerokość ekranu
    xpBarBackground.setFillColor(sf::Color(30, 30, 30));
    xpBarBackground.setPosition(0.f, 0.f);

    xpBarForeground.setSize(sf::Vector2f(0.f, 15.f));
    xpBarForeground.setFillColor(sf::Color(0, 150, 255)); // Błękitny kolor XP
    xpBarForeground.setPosition(0.f, 0.f);


    // Tło trawy

    if(grassTexture.loadFromFile("assets/map/background/floor/grass.png")) {
        grassTexture.setRepeated(true);
        grassTexture.setSmooth(false);

        grassBackground.setTexture(grassTexture);
        grassBackground.setTextureRect(sf::IntRect(0, 0, 640, 360));
        grassBackground.setScale(2.f, 2.f);
        grassBackground.setPosition(0.f, 0.f);
    }

    // Ładowanie czcionki i konfiguracja całego interfejsu użytkownika (UI)
    if(font.loadFromFile("assets/fonts/PixelGame.otf")) {
        sf::FloatRect textBounds;

        // 1. EKRAN AWANSU (LEVEL UP) & HUD
        textTitle.setFont(font);
        textTitle.setString("AWANS! WYBIERZ ULEPSZENIE");
        textTitle.setCharacterSize(40);
        textTitle.setFillColor(sf::Color::White);
        textBounds = textTitle.getLocalBounds();
        textTitle.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textTitle.setPosition(640.f, 120.f); // Środek ekranu w poziomie

        card1.setSize(sf::Vector2f(250.f, 350.f)); card1.setPosition(200.f, 200.f);
        textCard1.setFont(font); textCard1.setCharacterSize(24); textCard1.setPosition(220.f, 350.f);

        card2.setSize(sf::Vector2f(250.f, 350.f)); card2.setPosition(500.f, 200.f);
        textCard2.setFont(font); textCard2.setCharacterSize(24); textCard2.setPosition(520.f, 350.f);

        card3.setSize(sf::Vector2f(250.f, 350.f)); card3.setPosition(800.f, 200.f);
        textCard3.setFont(font); textCard3.setCharacterSize(24); textCard3.setPosition(820.f, 350.f);

        hudStatsText.setFont(font);
        hudStatsText.setCharacterSize(16);
        hudStatsText.setFillColor(sf::Color(200, 200, 200));
        hudStatsText.setPosition(20.f, 20.f);

        // 2. MENU GŁÓWNE
        menuTitle.setFont(font);
        menuTitle.setString("ETERNAL NIGHT");
        menuTitle.setCharacterSize(80);
        menuTitle.setFillColor(sf::Color(255, 200, 0));
        textBounds = menuTitle.getLocalBounds();
        menuTitle.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        menuTitle.setPosition(640.f, 140.f);

        // Przycisk START
        btnStart.setSize(sf::Vector2f(300.f, 60.f));
        btnStart.setPosition(490.f, 300.f);
        btnStart.setFillColor(sf::Color(50, 50, 100));

        textStart.setFont(font);
        textStart.setString("Start game");
        textStart.setCharacterSize(30);
        textBounds = textStart.getLocalBounds();
        textStart.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        // Pozycja: środek przycisku
        textStart.setPosition(490.f + 150.f, 300.f + 30.f);

        // Przycisk QUIT
        btnQuit.setSize(sf::Vector2f(300.f, 60.f));
        btnQuit.setPosition(490.f, 400.f);
        btnQuit.setFillColor(sf::Color(100, 50, 50));

        textQuit.setFont(font);
        textQuit.setString("Quit");
        textQuit.setCharacterSize(30);
        textBounds = textQuit.getLocalBounds();
        textQuit.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textQuit.setPosition(490.f + 150.f, 400.f + 30.f);

        // 3. MENU PAUZY
        textPaused.setFont(font);
        textPaused.setString("PAUZA");
        textPaused.setCharacterSize(80);
        textPaused.setFillColor(sf::Color::Yellow);
        textBounds = textPaused.getLocalBounds();
        textPaused.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textPaused.setPosition(640.f, 190.f);

        // Przycisk WZNÓW (RESUME)
        btnResume.setSize(sf::Vector2f(300.f, 60.f));
        btnResume.setPosition(490.f, 300.f);
        btnResume.setFillColor(sf::Color(50, 100, 50));

        textResume.setFont(font);
        textResume.setString("Wznow gre");
        textResume.setCharacterSize(30);
        textBounds = textResume.getLocalBounds();
        textResume.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textResume.setPosition(490.f + 150.f, 300.f + 30.f);

        // Przycisk POWRÓT Z PAUZY
        btnPauseReturn.setSize(sf::Vector2f(300.f, 60.f));
        btnPauseReturn.setPosition(490.f, 400.f);
        btnPauseReturn.setFillColor(sf::Color(100, 50, 50));

        textPauseReturn.setFont(font);
        textPauseReturn.setString("Wroc do menu");
        textPauseReturn.setCharacterSize(30);
        textBounds = textPauseReturn.getLocalBounds();
        textPauseReturn.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textPauseReturn.setPosition(490.f + 150.f, 400.f + 30.f);

        // 4. EKRAN ŚMIERCI (GAME OVER)
        // Napis ZGINĄŁEŚ! na środku ekranu
        textGameOver.setFont(font);
        textGameOver.setString("ZGINALES!");
        textGameOver.setCharacterSize(80);
        textGameOver.setFillColor(sf::Color::Red);
        textBounds = textGameOver.getLocalBounds();
        textGameOver.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textGameOver.setPosition(640.f, 190.f);

        // Wynik ostateczny na środku ekranu
        textFinalScore.setFont(font);
        textFinalScore.setCharacterSize(40);
        // Wyśrodkowanie dla textFinalScore zrobimy dynamicznie w update(),
        // ale ustawiamy mu bazowy punkt origin na środek
        textBounds = textFinalScore.getLocalBounds();
        textFinalScore.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textFinalScore.setPosition(640.f, 320.f);

        // Przycisk POWRÓT DO MENU
        btnReturn.setSize(sf::Vector2f(300.f, 60.f));
        btnReturn.setPosition(490.f, 450.f);
        btnReturn.setFillColor(sf::Color(80, 80, 80));

        textReturn.setFont(font);
        textReturn.setString("Powrot do menu");
        textReturn.setCharacterSize(30);
        textBounds = textReturn.getLocalBounds();
        textReturn.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        textReturn.setPosition(490.f + 150.f, 450.f + 30.f);
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
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (currentState == GameState::MAIN_MENU) {
                if (btnStart.getGlobalBounds().contains(mousePos)) {
                    resetGame(); // Resetuje i ładuje gracza/przeszkody
                    currentState = GameState::PLAYING;
                } else if (btnQuit.getGlobalBounds().contains(mousePos)) {
                    window.close();
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
                }
            }
            else if (currentState == GameState::LEVEL_UP) {
                if (card1.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[0]); currentState = GameState::PLAYING; }
                else if (card2.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[1]); currentState = GameState::PLAYING; }
                else if (card3.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[2]); currentState = GameState::PLAYING; }
            }
        }
    }
}

void Engine::resetGame() {
    gameObjects.clear();

    // Tworzymy od nowa gracza na środku ekranu
    player = std::make_shared<Player>(640.f, 360.f);
    gameObjects.push_back(player);

    // Resetujemy statystyki fali i czasu
    currentWave = 1;
    timeBetweenWaves = 20.0f;
    waveTimer = 20.0f; // Od razu respi pierwszą falę
    gameTime = 0.f;    // Resetujemy stoper i punkty

    // LOSOWE GENEROWANIE PRZESZKÓD
    for (int i = 0; i < 5; ++i) {
        float randX = rand() % 1200 + 40; // Żeby nie wystawały poza ekran
        float randY = rand() % 640 + 40;
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
            float randX = rand() % 1200 + 40;
            float randY = rand() % 640 + 40;

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

void Engine::update(float deltaTime) {
    // Przekazanie pozycji kursora
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    player->setMousePosition(mousePos);

    // Tworzenie pocisku
    if(player->getWantsToShootSpecial()) {
        sf::Vector2f spawnPos = player->getPosition();
        sf::Vector2f dir = player->getLastDirection();

        gameObjects.push_back(std::make_shared<Projectile>(spawnPos.x, spawnPos.y, dir));

        player->resetSpecialShot();
    }
    // Aktualizacja wszystkich obiektów w grze
    for(auto& obj : gameObjects) {
        if(obj->isActive()) {
            obj->update(deltaTime);
        }
    }

    std::vector<std::shared_ptr<GameObject>> newObjects;
    // System kolizji
    for(auto& obj : gameObjects) {
        if(!obj->isActive()) continue;

        // Rzutowanie obiektu na wroga
        auto enemy = dynamic_cast<Enemy*>(obj.get());
        if(enemy) {
            // Kolizja: Gracz - Wróg
            if(player->getBounds().intersects(enemy->getBounds())) {
                if (enemy->getIsKamikaze()) {
                    player->takeDamage(30);
                    enemy->takeDamage(9999);
                } else {
                    player->takeDamage(10);
                    if (enemy->getIsVampire()) {
                        enemy->heal(15);
                    }
                }
            }

            // Kolizja: Atak obszarowy gracza - Wróg
            if(player->getIsAttacking() && player->getAttackBounds().intersects(enemy->getBounds())) {
                enemy->takeDamage(player->getDamage(25));

                if(!enemy->isActive()) {
                    player->incrementKills();
                    player->triggerVampirism();
                    newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                }
            }
            // Kolizja: Aura Ognia (Pasywna)
            if (player->getHasFireAura()) {
                float dx = enemy->getPosition().x - player->getPosition().x;
                float dy = enemy->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist <= player->getFireAuraRadius()) {
                    enemy->takeDamage(player->getDamage(3), 1);

                    if(!enemy->isActive()) {
                        player->incrementKills();
                        player->triggerVampirism();
                        newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                    }
                }
            }

            // Kolizja: Orbitujące Ostrze (Pasywna)
            if (player->getHasOrbitingSword()) {
                if (player->getOrbitingSwordBounds().intersects(enemy->getBounds())) {
                    enemy->takeDamage(player->getDamage(15), 2);

                    if(!enemy->isActive()) {
                        player->incrementKills();
                        player->triggerVampirism();
                        newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                    }
                }
            }

            // Kolizja: Kusza - Wróg
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;

                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile) {
                    if(projectile->getBounds().intersects(enemy->getBounds())) {
                        enemy->takeDamage(player->getDamage(50));

                        if(!enemy->isActive()) {
                            player->incrementKills();
                            player->triggerVampirism();
                            newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                        }

                        projectile->destroy();
                    }
                }
            }
            //KOLIZJA WRÓG - WRÓG:
        if (!enemy->getIsGhost()) {
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
    }

        // Kolizje z XP
        auto xp = dynamic_cast<XpCrystal*>(obj.get());
        if(xp) {
            if(player->getPickupBounds().intersects(xp->getBounds())) {
                player->addXp(10); // Dodanie XP
                xp->destroy();
            }
        }

        // Kolizje z Bonusem
        auto bonus = dynamic_cast<Bonus*>(obj.get());
        if(bonus) {
            if(player->getPickupBounds().intersects(bonus->getBounds())) {
                // Sprawdzamy czy podniesiony bonus to mikstura
                if(bonus->getType() == BonusType::POTION) {
                    player->heal(20);
                    player->incrementPotions();
                }

                bonus->destroy(); // Zebranie bonusu
            }
        }

        //Kolizje z Przeszkodą
        auto obstacle = dynamic_cast<Obstacle*>(obj.get());
        if(obstacle) {
            sf::FloatRect obsBounds = obstacle->getBounds();
            sf::FloatRect intersection; // Zmienna przechowująca pole nałożenia się obiektów

            // Kolizja: Gracz - Przeszkoda
            if(player->getBounds().intersects(obsBounds, intersection)) {
                sf::Vector2f newPos = player->getPosition();

                // Sprawdzamy, z której strony wypchnąć gracza (tam gdzie głębokość wejścia w obiekt jest mniejsza)
                if(intersection.width < intersection.height) {
                    // Wypychanie w poziomie
                    if(player->getBounds().left < obsBounds.left) newPos.x -= intersection.width;
                    else newPos.x += intersection.width;
                } else {
                    // Wypychanie w pionie
                    if(player->getBounds().top < obsBounds.top) newPos.y -= intersection.height;
                    else newPos.y += intersection.height;
                }
                player->setPosition(newPos); // Fizyczne wypchnięcie gracza
            }

            // Kolizja: Wróg - Przeszkoda
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto otherEnemy = dynamic_cast<Enemy*>(otherObj.get());
                if(otherEnemy) {
                    if (!otherEnemy->getIsGhost()) {
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
                        otherEnemy->setPosition(newEnemyPos); // Fizyczne wypchnięcie wroga
                    }
                }
            }
        }
    }
}

    // Przesypujemy nowe kryształy do głównej listy gry
    for(auto& newObj : newObjects) {
        gameObjects.push_back(newObj);
    }

    // Usuwanie nieaktywnych obiektów
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const std::shared_ptr<GameObject>& obj) { return !obj->isActive(); }),
        gameObjects.end()
    );

    // Sprawdzenie czy gracz awansował
    if(player->checkLevelUp()) {
        generateUpgrades();
        currentState = GameState::LEVEL_UP;
        player->acknowledgeLevelUp();
    }
    // Sprawdzenie śmierci gracza
    if (player->getHp() <= 0) {
        currentState = GameState::GAME_OVER;
        int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);
        textFinalScore.setString("Twoj ostateczny wynik: " + std::to_string(score));

        // Dynamiczne wyśrodkowanie nowego tekstu z punktacją
        sf::FloatRect scoreBounds = textFinalScore.getLocalBounds();
        textFinalScore.setOrigin(scoreBounds.left + scoreBounds.width / 2.f, scoreBounds.top + scoreBounds.height / 2.f);
        textFinalScore.setPosition(640.f, 320.f);
    }
}

void Engine::render() {
    window.clear(sf::Color(30, 30, 40));
    window.draw(grassBackground);
    if (currentState == GameState::MAIN_MENU) {
        // RYSOWANIE MENU GŁÓWNEGO
        window.draw(menuTitle);
        window.draw(btnStart); window.draw(textStart);
        window.draw(btnQuit); window.draw(textQuit);
    }
    else if (currentState == GameState::PLAYING || currentState == GameState::LEVEL_UP || currentState == GameState::GAME_OVER || currentState == GameState::PAUSED) {
        // RYSOWANIE GRY (Dla stanów PLAYING, LEVEL_UP, GAME_OVER)
        window.draw(grassBackground);


    // Rysowanie wszystkich obiektów
    for(auto& obj : gameObjects) {
        if(obj->isActive()) {
            obj->draw(window);
        }
    }

    // 1. Aktualizacja i rysowanie paska XP na górze
    float xpPercent = static_cast<float>(player->getXp()) / static_cast<float>(player->getMaxXp());
    xpBarForeground.setSize(sf::Vector2f(1280.f * xpPercent, 15.f));
    window.draw(xpBarBackground);
    window.draw(xpBarForeground);

    // 2. Formatowanie czasu, wyniku i statystyk w lewym górnym rogu
    int minutes = static_cast<int>(gameTime) / 60;
    int seconds = static_cast<int>(gameTime) % 60;
    std::string timeStr = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
    int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);

    //Rysowanie HUD
    std::string statsStr = "CZAS: " + timeStr +
                           "\nWYNIK: " + std::to_string(score) +
                           "\nPOZIOM: " + std::to_string(player->getLevel()) +
                           "\nFALA: " + std::to_string(currentWave) + " (Kolejna za: " + std::to_string((int)(timeBetweenWaves - waveTimer)) + "S)" +
                           "\nZABICI: " + std::to_string(player->getEnemiesKilled()) +
                           "\nMIKSTURY: " + std::to_string(player->getPotionsCollected()) +
                           "\nPANCERZ: " + std::to_string(player->getArmor()) +
                           "\nBONUS DMG: +" + std::to_string(player->getDamageBonus()) +
                           "\nKRYTYK: " + std::to_string(static_cast<int>(player->getCritChance() * 100)) + "%" +
                           "\nWAMPIRYZM: " + std::to_string(static_cast<int>(player->getVampirismChance() * 100)) + "%" +
                           "\nUNIK: " + std::to_string(static_cast<int>(player->getDodgeChance() * 100)) + "%" +
                           "\nREGEN: " + std::to_string(player->getHpRegenRate()) + " HP/5s" +
                           "\nSZYBKOSC: " + std::to_string(static_cast<int>(player->getSpeed()));
    hudStatsText.setString(statsStr);
    hudStatsText.setPosition(20.f, 25.f);
    window.draw(hudStatsText);

    window.draw(xpBarBackground);
    window.draw(xpBarForeground);

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
    else if (currentState == GameState::GAME_OVER) {
        sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 220)); // Mocne przyciemnienie tła
        window.draw(overlay);

        // Rysowanie napisów ekranu śmierci
        window.draw(textGameOver);
        window.draw(textFinalScore);
        window.draw(btnReturn); window.draw(textReturn);
    }
    // RYSOWANIE MENU PAUZY
    else if (currentState == GameState::PAUSED) {
        sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        window.draw(textPaused);
        window.draw(btnResume); window.draw(textResume);
        window.draw(btnPauseReturn); window.draw(textPauseReturn);
    }

    if(debugMode) drawDebugOverlay();
    }
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

            waveTimer += deltaTime;
            gameTime += deltaTime; //Aktualizacja czasu gry

            // Kiedy minie czas, zrzucamy na gracza całą falę na raz
            if (waveTimer >= timeBetweenWaves) {
                spawnWave();
                currentWave++;   // Zwiększamy poziom trudności
                waveTimer = 0.f; // Resetujemy zegar do kolejnej fali
            }
        }

        render();
    }
}

void Engine::spawnWave() {
    int enemiesToSpawn = 5 + (currentWave * 2);

    // Mnożniki
    float hpM = 1.0f + (currentWave * 0.08f);
    float speedM = 1.0f + (currentWave * 0.03f);

    //Pozycja startowa to gracz a promień to odległość poza ekranem
    sf::Vector2f center = player->getPosition();
    float radius = 800.f;

    //Spawnowanie całej grupy
    for(int i = 0; i < enemiesToSpawn; ++i) {
        // Losujemy kąt dookoła gracza (od 0 do 360 stopni i zamieniamy na radiany)
        float angle = (rand() % 360) * 3.14159f / 180.f;

        //Obliczanie pozycji na okręgu
        float spawnX = center.x + cos(angle) * radius;
        float spawnY = center.y + sin(angle) * radius;

        // 4. Dobór wrogów: 40% to zawsze standardowy tłum, reszta to odblokowane potwory
        EnemyType typ;
        int typeChance = rand() % 100;

        if (typeChance < 40) {
            typ = EnemyType::TRUPOJADY;
        } else {
            // Z każdą kolejną falą odblokowuje się nowy, groźniejszy typ wroga
            int unlockedTypes = std::min(currentWave, 6);
            int specialRoll = rand() % unlockedTypes;

            if (specialRoll == 0) typ = EnemyType::UPIOR;
            else if (specialRoll == 1) typ = EnemyType::OGROWATE;
            else if (specialRoll == 2) typ = EnemyType::KAMIKAZE;
            else if (specialRoll == 3) typ = EnemyType::CIEN;
            else if (specialRoll == 4) typ = EnemyType::WAMPIR;
            else if (specialRoll == 5) typ = EnemyType::ZJAWA;
        }

        gameObjects.push_back(std::make_shared<Enemy>(spawnX, spawnY, typ, player, hpM, speedM));
    }
}

void Engine::generateUpgrades() {
    std::vector<int> selected;
    while(selected.size() < 3) {
        int r = rand() % 13;
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

    // Napis
    window.draw(debugLabel);
}

