#include "Engine.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"
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
            }

            // Kolizja: Kusza - Wróg
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;

                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile) {
                    if(projectile->getBounds().intersects(enemy->getBounds())) {
                        enemy->takeDamage(20); // WIP
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

