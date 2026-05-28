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

Engine::Engine() {
    // Rozmiar okna
    window.create(sf::VideoMode(1280, 720), "Eternal Night");

    // Gracz na środku ekranu
    player = std::make_shared<Player>(640.f, 360.f);

    // Gracz do kontenera obiektów gry
    gameObjects.push_back(player);

    // --- LOSOWE GENEROWANIE PRZESZKÓD ---
    for (int i = 0; i < 5; ++i) {
        float randX = rand() % 1200 + 40; // Losowanie żeby nie wystawały poza ekran
        float randY = rand() % 640 + 40;
        int randType = rand() % 3;

        ObstacleType type;
        if (randType == 0) type = ObstacleType::DRZEWO;
        else if (randType == 1) type = ObstacleType::KRZAK;
        else type = ObstacleType::GLAZ;

        gameObjects.push_back(std::make_shared<Obstacle>(randX, randY, type));
    }

    // Losowe mikstury (zabezpieczone przed respieniem sie w terenie)
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
                    break;              // Przerywamy pętlę i losujemy pozycję od nowa
                }
            }
        }

        // Gdy program wyjdzie z pętli while oznacza to że znalazł bezpieczne miejsce.
        gameObjects.push_back(newPotion);
    }
    currentState = GameState::PLAYING;

    // Ładowanie czcionki i tworzenie kart ulepszeń
    if(font.loadFromFile("arial.ttf")) {
        textTitle.setFont(font);
        textTitle.setString("AWANS! WYBIERZ ULEPSZENIE");
        textTitle.setCharacterSize(40);
        textTitle.setFillColor(sf::Color::White);
        textTitle.setPosition(350.f, 100.f);

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
    }
}

void Engine::handleEvents() {
    sf::Event event;
    while(window.pollEvent(event)) {
        if(event.type == sf::Event::Closed) {
            window.close();
        }
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
        // Obsługa kliknięć w menu awansu
        if (currentState == GameState::LEVEL_UP && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (card1.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[0]); currentState = GameState::PLAYING; }
            else if (card2.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[1]); currentState = GameState::PLAYING; }
            else if (card3.getGlobalBounds().contains(mousePos)) { player->applyUpgrade(offeredUpgrades[2]); currentState = GameState::PLAYING; }
        }
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
                player->takeDamage(10); // WIP
            }

            // Kolizja: Atak obszarowy gracza - Wróg
            if(player->getIsAttacking() && player->getAttackBounds().intersects(enemy->getBounds())) {
                enemy->takeDamage(player->getDamage(10));

                if(!enemy->isActive()) {
                    player->incrementKills();
                    player->triggerVampirism();
                    newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                }
            }

            // Kolizja: Kusza - Wróg
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;

                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile) {
                    if(projectile->getBounds().intersects(enemy->getBounds())) {
                        enemy->takeDamage(player->getDamage(20));

                        if(!enemy->isActive()) {
                            player->incrementKills();
                            player->triggerVampirism();
                            newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                        }

                        projectile->destroy();
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
}

void Engine::render() {
    window.clear(sf::Color(30, 30, 30));

    // Rysowanie wszystkich obiektów
    for(auto& obj : gameObjects) {
        if(obj->isActive()) {
            obj->draw(window);
        }
    }

    // Rysowanie ekranu ulepszeń nakładanego na grę
    if (currentState == GameState::LEVEL_UP) {
        sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 200)); // Przyciemnienie tła
        window.draw(overlay);

        window.draw(card1); window.draw(textCard1);
        window.draw(card2); window.draw(textCard2);
        window.draw(card3); window.draw(textCard3);
        window.draw(textTitle);
    }
    //Rysowanie HUD
    std::string statsStr = "POZIOM: " + std::to_string(player->getLevel()) +
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
    window.draw(hudStatsText);

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

    window.display();
}

void Engine::run() {
    while(window.isOpen()) {
        // Obliczanie czasu, jaki upłynął od ostatniej klatki
        float deltaTime = clock.restart().asSeconds();

        handleEvents();

        // Pauza
        // Aktualizujemy pozycje i wrogów tylko w trakcie gry
        if (currentState == GameState::PLAYING) {
            update(deltaTime);


            if(spawnClock.getElapsedTime().asSeconds() >= 2.0f){
                spawnEnemy();
                spawnClock.restart();
            }
        }

        render();
    }
}

void Engine::spawnEnemy(){
    int krawedz = rand() % 4;
    float spawnX = 0;
    float spawnY = 0;

    //Losowanie krawedzi ekranu
    if(krawedz == 0){ //Góra
        spawnX = rand() % 1280;
        spawnY = -30.f;
    }
    else if(krawedz == 1){ //Dół
        spawnX = rand() % 1280;
        spawnY = 750.f;
    }
    else if(krawedz == 2){ //Lewo
        spawnX = -30.f;
        spawnY = rand() % 720;
    }
    else{ //Prawo
        spawnX = 1310.f;
        spawnY = rand() % 720;
    }

    int losowyTyp = rand() % 3;
    EnemyType typ;
    if (losowyTyp == 0) typ = EnemyType::TRUPOJADY;
    else if (losowyTyp == 1) typ = EnemyType::UPIOR;
    else typ = EnemyType::OGROWATE;

    // Tworzenie nowego wroga za pomoca shared_ptr
    auto newEnemy = std::make_shared<Enemy>(spawnX, spawnY, typ, player);

    // Dodanie wroga do wspolnego kontenera obiektów gry
    gameObjects.push_back(newEnemy);
}

void Engine::generateUpgrades() {
    std::vector<int> selected;
    while(selected.size() < 3) {
        int r = rand() % 11;
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
        }

        if(i == 0) { textCard1.setString(text); card1.setFillColor(color); }
        else if(i == 1) { textCard2.setString(text); card2.setFillColor(color); }
        else if(i == 2) { textCard3.setString(text); card3.setFillColor(color); }
    }
}