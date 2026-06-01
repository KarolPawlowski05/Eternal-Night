#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"

enum class GameState { MAIN_MENU, PLAYING, LEVEL_UP, GAME_OVER, PAUSED, SCORES };
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
    sf::Text hudStatsText;

    int offeredUpgrades[3];
    void generateUpgrades();

    // System Nieskończonych Fal
    int currentWave;
    float waveTimer;
    float timeBetweenWaves; // Czas między kolejnymi falami
    void spawnWave();       // Zastępuje stare spawnEnemy()

    // Tło trawy
    sf::Texture grassTexture;
    sf::Sprite grassBackground;

    // Debug overlay (klawisz F3)
    bool debugMode;
    void drawDebugOverlay();
    void drawDebugRect(sf::FloatRect bounds, sf::Color color);
    void drawDebugCircle(sf::Vector2f center, float radius, sf::Color color);
    sf::Text debugLabel;

    float gameTime; // Całkowity czas gry do timera i wyniku

    // Pasek XP na górze ekranu
    sf::RectangleShape xpBarBackground;
    sf::RectangleShape xpBarForeground;
    sf::RectangleShape xpBarBorder;
    sf::Text           xpLevelText;

    // Timer
    sf::Text timerText;

    // Licznik zabitych
    sf::Text killCountText;
    sf::RectangleShape skullIcon; // Placeholder ikonki czaszki

    // Elementy UI Menu Głównego
    sf::Text menuTitle;
    sf::RectangleShape btnStart;
    sf::Text textStart;
    sf::RectangleShape btnQuit;
    sf::Text textQuit;

    // Elementy UI Ekranu Śmierci
    sf::Text textGameOver;
    sf::Text textFinalScore;
    sf::RectangleShape btnReturn;
    sf::Text textReturn;

    // Elementy UI Menu Pauzy
    sf::Text textPaused;
    sf::RectangleShape btnResume;
    sf::Text textResume;
    sf::RectangleShape btnPauseReturn;
    sf::Text textPauseReturn;

    //Kamera podążająca za graczem
    sf::View gameView;

    // System nieskończonej mapy
    sf::Vector2f lastObstacleSpawnPos;
    void manageInfiniteMap();
    void drawTerrain();
    int getTileType(int tileX, int tileY);

    void resetGame();

    // tworzenie DamageNumber
    void spawnDamageNumber(float x, float y, int damage, bool isCrit);

    // Ekran wyników
    sf::Text textScoresTitle;
    sf::RectangleShape btnScores;
    sf::Text textBtnScores;
    sf::Text textScoresList;
    sf::RectangleShape btnReturnFromScores;
    sf::Text textReturnFromScores;

    // Wpisywanie imienia na Game Over
    sf::Text textEnterName;
    sf::Text textNameInput;
    std::string playerName;
    bool nameSaved;

    void saveScore(const std::string& name, int score);
    std::string loadScores();
public:
    Engine();
    void run();
};

#endif