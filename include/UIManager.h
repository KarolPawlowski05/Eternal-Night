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

    // Strzałka bossa
    sf::Sprite bossArrowSprite;

    // HUD (Timer, zabójstwa, statystyki)
    sf::Text timerText;
    sf::Text killCountText;
    sf::Sprite skullSprite;
    sf::Text hudStatsText;

    // Ekran ulepszeń
    sf::RectangleShape card1, card2, card3;
    sf::Text textTitle, textCard1, textCard2, textCard3;
    int offeredUpgrades[3];

    // Menu Główne
    sf::Sprite menuBackgroundSprite;
    sf::Text menuTitle;
    sf::Sprite btnStartSprite;
    sf::Sprite btnQuitSprite;
    sf::Sprite btnScoresSprite;
    sf::Sprite btnOptionsSprite;

    // Ogłoszenie fali
    sf::Text waveAnnouncementText;
    float waveAnnouncementTimer = 2.8f;
    static constexpr float ANNOUNCEMENT_DURATION = 2.8f;

    // Ekran Śmierci
    sf::Text textGameOver;          sf::Text textFinalScore;
    sf::RectangleShape btnReturn;   sf::Text textReturn;
    sf::Text textEnterName;         sf::Text textNameInput;

    // Menu Pauzy
    sf::Text textPaused;
    sf::Sprite btnResumeSprite;
    sf::Sprite btnPauseReturnSprite;

    // Ekran Wyników
    sf::Text textScoresTitle;
    sf::Text textScoresList;
    sf::RectangleShape btnReturnFromScores; sf::Text textReturnFromScores;

    // Przyciski do muzyki
    sf::Sprite btnMusicMinusSprite;
    sf::Sprite btnMusicPlusSprite;
    sf::Sprite btnSfxMinusSprite;
    sf::Sprite btnSfxPlusSprite;
    sf::Text textMusicVol;
    sf::Text textSfxVol;
    std::shared_ptr<Player> lastPlayer;  // Referencja do gracza dla dynamicznych opisów

public:
    UIManager();

    // Metody renderujące poszczególne ekrany
    void renderMainMenu(sf::RenderWindow& window);
    void renderHUD(sf::RenderWindow& window, const std::shared_ptr<Player>& player, float gameTime, WaveManager* waveManager);
    void renderLevelUp(sf::RenderWindow& window);
    void renderGameOver(sf::RenderWindow& window, int score, const std::string& playerName, bool nameSaved, int level, int kills, float survivalTime);
    void renderPause(sf::RenderWindow& window);
    void renderScores(sf::RenderWindow& window, const std::string& scoresText);
    void renderOptions(sf::RenderWindow& window);

    // Aktualizacje UI
    void generateUpgrades();
    void updateNameInput(const std::string& name);

    // Ogłoszenie fali
    void showWaveAnnouncement(int waveNumber, bool isBoss);
    void updateAnnouncement(float deltaTime);

    // Sprawdzanie kliknięć (zwracają true, jeśli punkt pos znajduje się na przycisku)
    bool isStartClicked(sf::Vector2f pos) const;
    bool isQuitClicked(sf::Vector2f pos) const;
    bool isScoresClicked(sf::Vector2f pos) const;
    bool isResumeClicked(sf::Vector2f pos) const;
    bool isPauseReturnClicked(sf::Vector2f pos) const;
    bool isGameOverReturnClicked(sf::Vector2f pos) const;
    bool isScoresReturnClicked(sf::Vector2f pos) const;
    bool isOptionsClicked(sf::Vector2f pos) const;

    // Zwraca ID ulepszenia (lub -1, jeśli nie kliknięto żadnej karty)
    int getClickedUpgrade(sf::Vector2f pos) const;
    int getOfferedUpgrade(int index) const;

    const sf::Font& getFont() const { return font; }
    // Wyświetlanie paska życia bossa
    void drawBossHealthBar(sf::RenderWindow& window, int currentHp, int maxHp, const std::string& bossName);

    // Strzałka wskazująca bossa poza ekranem
    void drawBossDirectionArrow(sf::RenderWindow& window, sf::Vector2f playerPos, sf::Vector2f bossPos);

    // Muzyka
    bool isMusicMinusClicked(sf::Vector2f pos) const;
    bool isMusicPlusClicked(sf::Vector2f pos) const;
    bool isSfxMinusClicked(sf::Vector2f pos) const;
    bool isSfxPlusClicked(sf::Vector2f pos) const;
    void generateUpgrades(const std::shared_ptr<Player>& player);
};

#endif