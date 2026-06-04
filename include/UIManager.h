#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "Player.h"
#include "WaveManager.h"

class UIManager {
private:
    sf::Font font;

    // Pasek XP
    sf::RectangleShape xpBarBackground, xpBarForeground, xpBarBorder;
    sf::Text xpLevelText;

    // HUD (Timer, zabójstwa, statystyki)
    sf::Text timerText;
    sf::Text killCountText;
    sf::RectangleShape skullIcon;
    sf::Text hudStatsText;

    // Ekran ulepszeń
    sf::RectangleShape card1, card2, card3;
    sf::Text textTitle, textCard1, textCard2, textCard3;
    int offeredUpgrades[3];

    // Menu Główne
    sf::Text menuTitle;
    sf::RectangleShape btnStart;    sf::Text textStart;
    sf::RectangleShape btnQuit;     sf::Text textQuit;
    sf::RectangleShape btnScores;   sf::Text textBtnScores;

    // Ekran Śmierci
    sf::Text textGameOver;          sf::Text textFinalScore;
    sf::RectangleShape btnReturn;   sf::Text textReturn;
    sf::Text textEnterName;         sf::Text textNameInput;

    // Menu Pauzy
    sf::Text textPaused;
    sf::RectangleShape btnResume;       sf::Text textResume;
    sf::RectangleShape btnPauseReturn;  sf::Text textPauseReturn;

    // Ekran Wyników
    sf::Text textScoresTitle;
    sf::Text textScoresList;
    sf::RectangleShape btnReturnFromScores; sf::Text textReturnFromScores;

public:
    UIManager();

    // Metody renderujące poszczególne ekrany
    void renderMainMenu(sf::RenderWindow& window);
    void renderHUD(sf::RenderWindow& window, const std::shared_ptr<Player>& player, float gameTime, WaveManager* waveManager);
    void renderLevelUp(sf::RenderWindow& window);
    void renderGameOver(sf::RenderWindow& window, int score, const std::string& playerName, bool nameSaved);
    void renderPause(sf::RenderWindow& window);
    void renderScores(sf::RenderWindow& window, const std::string& scoresText);

    // Aktualizacje UI
    void generateUpgrades();
    void updateNameInput(const std::string& name);

    // Sprawdzanie kliknięć (zwracają true, jeśli punkt pos znajduje się na przycisku)
    bool isStartClicked(sf::Vector2f pos) const;
    bool isQuitClicked(sf::Vector2f pos) const;
    bool isScoresClicked(sf::Vector2f pos) const;
    bool isResumeClicked(sf::Vector2f pos) const;
    bool isPauseReturnClicked(sf::Vector2f pos) const;
    bool isGameOverReturnClicked(sf::Vector2f pos) const;
    bool isScoresReturnClicked(sf::Vector2f pos) const;

    // Zwraca ID ulepszenia (lub -1, jeśli nie kliknięto żadnej karty)
    int getClickedUpgrade(sf::Vector2f pos) const;

    const sf::Font& getFont() const { return font; }
};

#endif