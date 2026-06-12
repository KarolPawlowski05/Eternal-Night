#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "WaveManager.h"
#include "ScoreManager.h"
#include "MapManager.h"
#include "UIManager.h"
#include "CollisionSystem.h"
#include "EntityManager.h"

enum class GameState { MAIN_MENU, PLAYING, LEVEL_UP, GAME_OVER, PAUSED, SCORES, OPTIONS };
class Player;

class Engine {
private:
    sf::RenderWindow window;
    sf::Clock clock; // Do obliczania deltaTime

    std::shared_ptr<Player> player; // Bezpośredni dostęp do gracza
    std::unique_ptr<WaveManager> waveManager;

    void handleEvents();
    void update(float deltaTime);
    void render();

    void renderGame();

    GameState currentState;

    // Screen shake
    float shakeDuration = 0.f;
    float shakeIntensity = 0.f;
    float prevPlayerInvTimer = 0.f;
    void triggerShake(float duration, float intensity);

    // Debug overlay (klawisz F3)
    bool debugMode;
    void drawDebugOverlay();
    void drawDebugRect(sf::FloatRect bounds, sf::Color color);
    void drawDebugCircle(sf::Vector2f center, float radius, sf::Color color);
    sf::Text debugLabel;

    float gameTime; // Całkowity czas gry do timera i wyniku

    //Kamera podążająca za graczem
    sf::View gameView;

    void resetGame();

    // Wpisywanie imienia na Game Over
    std::string playerName;
    bool nameSaved;

    ScoreManager scoreManager;
    MapManager mapManager;
    UIManager uiManager;
    CollisionSystem collisionSystem;
    EntityManager entityManager;

    bool bossMusicPlaying = false;

public:
    Engine();
    void run();
};

#endif