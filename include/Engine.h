#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"

class Player;

class Engine {
private:
    sf::RenderWindow window;
    sf::Clock clock; // Do obliczania deltaTime

    std::vector<std::shared_ptr<GameObject>> gameObjects; // Głowny kontener na wszystkie obiekty gry
    std::shared_ptr<Player> player; // Bezpośredni dostęp do gracza

    void handleEvents();
    void update(float deltaTime);
    void render();

public:
    Engine();
    void run();
};

#endif