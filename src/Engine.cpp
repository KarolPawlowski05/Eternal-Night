#include "Engine.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Bonus.h"
#include "Obstacle.h"
#include "XpCrystal.h"
#include <cstdlib>
#include <memory>
#include <cmath>

Engine::Engine() {
    // Rozmiar okna
    window.create(sf::VideoMode(1280, 720), "Eternal Night");
    window.setFramerateLimit(60);

    // Gra startuje w Menu Głównym
    currentState = GameState::MAIN_MENU;
    gameTime = 0.f;

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
        // SZYBKI SPAWN KONKRETNEGO BOSSA (Klawisze 1, 2, 3, 4 nad literami)
        if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::Num1 && debugMode) {
                if (waveManager != nullptr) waveManager->forceSpawnBoss(0); // 1 = Korzenny Dusiciel
            }
            else if(event.key.code == sf::Keyboard::Num2 && debugMode) {
                if (waveManager != nullptr) waveManager->forceSpawnBoss(1); // 2 = Zjawa Burzy
            }
            else if(event.key.code == sf::Keyboard::Num3 && debugMode) {
                if (waveManager != nullptr) waveManager->forceSpawnBoss(2); // 3 = Wampir-Książę
            }
            else if(event.key.code == sf::Keyboard::Num4 && debugMode) {
                if (waveManager != nullptr) waveManager->forceSpawnBoss(3); // 4 = Golem Chaosu
            }
        }
        // PRZEŁĄCZNIK SPAWNU MOBÓW (Włącza/Wyłącza zwykłych wrogów)
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
            if (waveManager != nullptr && debugMode) {
                waveManager->toggleSpawning();
            }
        }
        // GOD MODE (Nieśmiertelność)
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
            if (player != nullptr && debugMode) {
                player->toggleGodMode();
            }
        }
        // OBSŁUGA MYSZKI W ZALEŻNOŚCI OD EKRANU
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window),window.getDefaultView());

            if (currentState == GameState::MAIN_MENU) {
                if (uiManager.isStartClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    resetGame();
                    currentState = GameState::PLAYING;
                } else if (uiManager.isQuitClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    window.close();
                } else if (uiManager.isScoresClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    currentState = GameState::SCORES;
                }
            }
            else if (currentState == GameState::PAUSED) {
                if (uiManager.isResumeClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    currentState = GameState::PLAYING;
                } else if (uiManager.isPauseReturnClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    currentState = GameState::MAIN_MENU;
                }
                // Kontrolki głośności
                else if (uiManager.isMusicMinusClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    AssetManager::setMusicVolume(AssetManager::getMusicVolume() - 10.f);
                } else if (uiManager.isMusicPlusClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    AssetManager::setMusicVolume(AssetManager::getMusicVolume() + 10.f);
                } else if (uiManager.isSfxMinusClicked(mousePos)) {
                    AssetManager::setSfxVolume(AssetManager::getSfxVolume() - 10.f);
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                } else if (uiManager.isSfxPlusClicked(mousePos)) {
                    AssetManager::setSfxVolume(AssetManager::getSfxVolume() + 10.f);
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                }
            }
            else if (currentState == GameState::GAME_OVER) {
                if (uiManager.isGameOverReturnClicked(mousePos)) {
                    AssetManager::playSound("assets/audio/sfx/buttonClick.wav");
                    currentState = GameState::MAIN_MENU;
                    playerName = "";
                    nameSaved = false;
                }
            }
            else if (currentState == GameState::LEVEL_UP) {
                int upgrade = uiManager.getClickedUpgrade(mousePos);
                if (upgrade != -1) {
                    AssetManager::playSound("assets/audio/sfx/upgradeSelect.wav");
                    player->applyUpgrade(upgrade);
                    currentState = GameState::PLAYING;
                }
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
                uiManager.updateNameInput(playerName);
            }
            // Obsługa wciśnięcia Enter (zapis do pliku)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !playerName.empty()) {
                int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);

                // Zapisujemy przez ScoreManagera
                scoreManager.saveScore(playerName, score);

                nameSaved = true; // Flaga, żeby nie dało się zapisać dwa razy
            }
        }
        // Obsługa ekranu wyników
        if (currentState == GameState::SCORES) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
                if (uiManager.isScoresReturnClicked(mousePos)) {
                    currentState = GameState::MAIN_MENU;
                }
            }
        }
    }
}

void Engine::resetGame() {
    entityManager.clear();

    // Tworzymy od nowa gracza na środku ekranu
    player = std::make_shared<Player>(640.f, 360.f);
    entityManager.addEntity(player);
    mapManager.reset(player->getPosition());

    // Resetujemy statystyki fali i czasu
    gameTime = 0.f;
    waveManager = std::make_unique<WaveManager>(player, entityManager.getEntities());
    waveManager->reset();

    // LOSOWE GENEROWANIE PRZESZKÓD (METODA SKUPISKOWA)
    int numClusters = 30; // 30 dużych skupisk na mapie

    for (int c = 0; c < numClusters; ++c) {
        float clusterX = player->getPosition().x + (rand() % 6000) - 3000;
        float clusterY = player->getPosition().y + (rand() % 6000) - 3000;
        int clusterMainType = rand() % 3;
        int itemsInCluster = 5 + (rand() % 14);

        for (int i = 0; i < itemsInCluster; ++i) {
            bool isColliding = true;
            std::shared_ptr<Obstacle> newObstacle;
            int attempts = 0; // Zabezpieczenie przed nieskończoną pętlą

            // Pętla szuka wolnego miejsca, uderzy maksymalnie 15 razy
            while (isColliding && attempts < 15) {
                float offsetX = (rand() % 1000) - 500;
                float offsetY = (rand() % 1000) - 500;

                ObstacleType type;
                if (rand() % 100 < 85) {
                    if (clusterMainType == 0) type = ObstacleType::TREE;
                    else if (clusterMainType == 1) type = ObstacleType::BUSH;
                    else type = ObstacleType::ROCK;
                } else {
                    int randomType = rand() % 3;
                    if (randomType == 0) type = ObstacleType::TREE;
                    else if (randomType == 1) type = ObstacleType::BUSH;
                    else type = ObstacleType::ROCK;
                }

                newObstacle = std::make_shared<Obstacle>(clusterX + offsetX, clusterY + offsetY, type);
                isColliding = false;

                // Sprawdzanie nakładania się na inne PRZESZKODY
                for (const auto& obj : entityManager.getEntities()) {
                    if (dynamic_cast<Obstacle*>(obj.get())) {
                        if (newObstacle->getBounds().intersects(obj->getBounds())) {
                            isColliding = true;
                            break; // Znaleziono kolizję przerywamy sprawdzanie i losujemy nowe kordy
                        }
                    }
                }
                attempts++;
            }

            // Jeśli znaleziono bezpieczne miejsce wrzucamy na mapę
            if (!isColliding && newObstacle) {
                entityManager.addEntity(newObstacle);
            }
        }
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

            for (const auto& obj : entityManager.getEntities()) {
                if (newPotion->getBounds().intersects(obj->getBounds())) {
                    isColliding = true;
                    break; // Przerywamy pętlę i losujemy pozycję od nowa
                }
            }
        }
        // Znalazło bezpieczne miejsce, dodajemy miksturę na mapę
        entityManager.addEntity(newPotion);
    }

    // Muzyka
    AssetManager::playMusic("assets/audio/music/bgMusic.ogg");
}

void Engine::update(float deltaTime) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), gameView);
    player->setMousePosition(mousePos);

    if(player->getWantsToShootSpecial()) {
        sf::Vector2f spawnPos = player->getPosition();
        sf::Vector2f dir = player->getLastDirection();
        entityManager.addEntity(std::make_shared<Projectile>(spawnPos.x, spawnPos.y, dir));
        player->resetSpecialShot();
    }
    // Różdżka - samonaprowadzająca
    if (player->wandReady()) {
        // Znajdź najbliższego wroga
        float closestDist = 99999.f;
        Enemy* closestEnemy = nullptr;
        for (auto& obj : entityManager.getEntities()) {
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
            AssetManager::playSound("assets/audio/sfx/wandShoot.wav");
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
                entityManager.addEntity(proj);
            }
            player->resetWandTimer();
        }
    }
    player->tickWandTimer(deltaTime);

    entityManager.updateAll(deltaTime);

    collisionSystem.update(player, entityManager.getEntities(), uiManager.getFont());

    entityManager.cleanupDeadEntities();

    // Zarządzanie muzyką
    bool bossActive = (waveManager && waveManager->getActiveBoss() != nullptr);

    if (bossActive && !bossMusicPlaying) {
        AssetManager::stopMusic();
        AssetManager::playMusic("assets/audio/music/bossMusic.ogg");
        bossMusicPlaying = true;
    }
    else if (!bossActive && bossMusicPlaying) {
        AssetManager::stopMusic();
        AssetManager::playMusic("assets/audio/music/bgMusic.ogg");
        bossMusicPlaying = false;
    }

    if(player->checkLevelUp()) {
        uiManager.generateUpgrades(player);
        currentState = GameState::LEVEL_UP;
        player->acknowledgeLevelUp();
    }

    if(player->getHp() <= 0) {
        AssetManager::playSound("assets/audio/sfx/gameOver.wav");
        AssetManager::stopMusic();
        bossMusicPlaying = false;
        currentState = GameState::GAME_OVER;
    }

    mapManager.update(player, entityManager.getEntities());
    // Respawnianie potionów co 3 fale
    if(waveManager && waveManager->shouldRespawnPotions()) {
        bool isColliding = true;
        std::shared_ptr<Bonus> newPotion;

        // Spróbuj znaleźć wolne miejsce (maks 20 prób)
        int attempts = 0;
        while (isColliding && attempts < 20) {
            float randX = (rand() % 4000) - 2000 + player->getPosition().x;
            float randY = (rand() % 4000) - 2000 + player->getPosition().y;
            newPotion = std::make_shared<Bonus>(randX, randY, BonusType::POTION);

            isColliding = false;
            for (const auto& obj : entityManager.getEntities()) {
                if (newPotion->getBounds().intersects(obj->getBounds())) {
                    isColliding = true;
                    break;
                }
            }
            attempts++;
        }

        if (!isColliding && newPotion) {
            entityManager.addEntity(newPotion);
        }

        waveManager->acknowledgeSpawnedPotions();
    }
}

void Engine::render() {
    window.clear(sf::Color(30, 30, 40));

    if (currentState == GameState::MAIN_MENU) {
        // Ustawienie tymczasowej kamery tylko dla menu
        gameView.setSize(1280.f, 720.f);
        gameView.setCenter(640.f, 360.f);
        window.setView(gameView);
        mapManager.render(window, gameView);

        // Wracamy do widoku interfejsu żeby napisy nie były zniekształcone
        window.setView(window.getDefaultView());
        //  Rysowanie przycisków i tytułów
        uiManager.renderMainMenu(window);
    } else if(currentState == GameState::SCORES) {
        uiManager.renderScores(window, scoreManager.loadScores());
    } else {
        //  KAMERA ŚWIATA (Podąża za graczem)
        gameView.setSize(1280.f, 720.f);
        gameView.setCenter(player->getPosition());
        window.setView(gameView);

        // RYSOWANIE GRY (Dla stanów PLAYING, LEVEL_UP, GAME_OVER)
        mapManager.render(window, gameView);

        // Rysowanie wszystkich obiektów
        entityManager.renderAll(window);
        // Rysowanie hitboxów musi odbywać się w świecie gry
        if(debugMode) drawDebugOverlay();
        // Kamera interfejsu
        window.setView(window.getDefaultView());
        uiManager.renderHUD(window, player, gameTime, waveManager.get());

        if (waveManager && waveManager->getActiveBoss() != nullptr) {
            auto boss = waveManager->getActiveBoss();
            uiManager.drawBossHealthBar(window, boss->getHp(), boss->getMaxHp(), boss->getName());
        }

        if      (currentState == GameState::LEVEL_UP)   uiManager.renderLevelUp(window);
        else if (currentState == GameState::GAME_OVER) {
            int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);
            uiManager.renderGameOver(window, score, playerName, nameSaved);
        }
        else if (currentState == GameState::PAUSED)     uiManager.renderPause(window);
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

            gameTime += deltaTime;
            if(waveManager) waveManager->update(deltaTime);
        }

        render();
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
    for(auto& obj : entityManager.getEntities()) {
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

    // Orbitujące Ostrza
    if (player->getHasOrbitingSword()) {
        for (const auto& bounds : player->getOrbitingSwordsBounds()) {
            sf::RectangleShape rect(sf::Vector2f(bounds.width, bounds.height));
            rect.setPosition(bounds.left, bounds.top);
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Yellow);
            rect.setOutlineThickness(1.f);
            window.draw(rect);
        }
    }

    // Hitbox aury ognia
    if(player->getHasFireAura()) {
        drawDebugCircle(player->getPosition(), player->getFireAuraRadius(), sf::Color(255, 100, 0));
    }

    // Napis
    window.draw(debugLabel);
}