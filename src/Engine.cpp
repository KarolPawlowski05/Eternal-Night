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
    for (int i = 0; i < 10; ++i) {
        float randX = rand() % 1200 + 40; // Losowanie żeby nie wystawały poza ekran
        float randY = rand() % 640 + 40;
        int randType = rand() % 3;

        ObstacleType type;
        if (randType == 0) type = ObstacleType::DRZEWO;
        else if (randType == 1) type = ObstacleType::KRZAK;
        else type = ObstacleType::GLAZ;

        gameObjects.push_back(std::make_shared<Obstacle>(randX, randY, type));
    }

    // --- LOSOWE GENEROWANIE BONUSÓW ---
    for (int i = 0; i < 5; ++i) {
        float randX = rand() % 1200 + 40;
        float randY = rand() % 640 + 40;
        int randType = rand() % 2;

        BonusType type;
        if (randType == 0) type = BonusType::XP_CRYSTAL;
        else type = BonusType::POTION;

        gameObjects.push_back(std::make_shared<Bonus>(randX, randY, type));
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
    }
}

void Engine::update(float deltaTime) {
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
                enemy->takeDamage(10); // WIP

                if(!enemy->isActive()) {
                    gameObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                }
            }

            // Kolizja: Kusza - Wróg
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;

                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile) {
                    if(projectile->getBounds().intersects(enemy->getBounds())) {
                        enemy->takeDamage(20); // WIP

                        if(!enemy->isActive()) {
                            gameObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                        }

                        projectile->destroy();
                    }
                }
            }
        }
        //Kolizje z Bonusem
        auto bonus = dynamic_cast<Bonus*>(obj.get());
        if(bonus) {
            if(player->getBounds().intersects(bonus->getBounds())) {
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
    // Usuwanie nieaktywnych obiektów
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const std::shared_ptr<GameObject>& obj) { return !obj->isActive(); }),
        gameObjects.end()
    );
}

void Engine::render() {
    window.clear(sf::Color(30, 30, 30));

    // Rysowanie wszystkich obiektów
    for(auto& obj : gameObjects) {
        if(obj->isActive()) {
            obj->draw(window);
        }
    }
    window.display();
}

void Engine::run() {
    while(window.isOpen()) {
        // Obliczanie czasu, jaki upłynął od ostatniej klatki
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        update(deltaTime);

        //Automatyczny spawn wrogów co 2 sekundy
        if(spawnClock.getElapsedTime().asSeconds()>=2.0f){
            spawnEnemy();
            spawnClock.restart();
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
        spawnY = 40.f;
    }
    else if(krawedz == 1){ //Dół
        spawnX = rand() % 1280;
        spawnY = 680.f;
    }
    else if(krawedz == 2){ //Lewo
        spawnX = 40.f;
        spawnY = rand() % 720;
    }
    else{ //Prawo
        spawnX = 1240.f;
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

