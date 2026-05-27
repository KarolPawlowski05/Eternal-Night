#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"
enum class GameState { PLAYING, LEVEL_UP };
class Player;

class Engine {
private:
    sf::RenderWindow window;
    sf::Clock clock; // Do obliczania deltaTime
    sf::Clock spawnClock; // Odlicza czas do kolejnego spawnu

    std::vector<std::shared_ptr<GameObject>> gameObjects; // Głowny kontener na wszystkie obiekty gry
    std::shared_ptr<Player> player; // Bezpośredni dostęp do gracza

    void handleEvents();
    void update(float deltaTime);
    void render();
    void spawnEnemy();

    GameState currentState;
    sf::Font font;

    // Elementy ekranu ulepszeń
    sf::RectangleShape card1;
    sf::RectangleShape card2;
    sf::RectangleShape card3;
    sf::Text textTitle;
    sf::Text textCard1;
    sf::Text textCard2;
    sf::Text textCard3;

public:
    Engine();
    void run();
};

#endif