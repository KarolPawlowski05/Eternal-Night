#include "UIManager.h"
#include <algorithm>
#include <cstdlib>

UIManager::UIManager() {
    // Inicjalizacja wszystkich obiektów interfejsu
    xpBarBackground.setSize(sf::Vector2f(1280.f, 22.f));
    xpBarBackground.setFillColor(sf::Color(20, 20, 30));
    xpBarBackground.setPosition(0.f, 0.f);

    xpBarForeground.setSize(sf::Vector2f(0.f, 22.f));
    xpBarForeground.setFillColor(sf::Color(30, 140, 255));
    xpBarForeground.setPosition(0.f, 0.f);

    xpBarBorder.setSize(sf::Vector2f(1280.f, 22.f));
    xpBarBorder.setFillColor(sf::Color::Transparent);
    xpBarBorder.setOutlineColor(sf::Color(180, 130, 30));
    xpBarBorder.setOutlineThickness(2.f);
    xpBarBorder.setPosition(0.f, 0.f);

    if(font.loadFromFile("assets/fonts/PixelGame.otf")) {
        sf::FloatRect tb;

        xpLevelText.setFont(font); xpLevelText.setCharacterSize(14); xpLevelText.setFillColor(sf::Color::White);
        timerText.setFont(font); timerText.setCharacterSize(28); timerText.setFillColor(sf::Color(230, 230, 230)); timerText.setOutlineColor(sf::Color(0, 0, 0, 180)); timerText.setOutlineThickness(2.f);
        killCountText.setFont(font); killCountText.setCharacterSize(16); killCountText.setFillColor(sf::Color(220, 200, 200)); killCountText.setOutlineColor(sf::Color::Black); killCountText.setOutlineThickness(1.5f);
        skullIcon.setSize(sf::Vector2f(14.f, 14.f)); skullIcon.setFillColor(sf::Color(200, 60, 60)); skullIcon.setOutlineColor(sf::Color(80, 0, 0)); skullIcon.setOutlineThickness(1.f);
        hudStatsText.setFont(font); hudStatsText.setCharacterSize(14); hudStatsText.setFillColor(sf::Color(190, 190, 190)); hudStatsText.setPosition(10.f, 58.f);

        textTitle.setFont(font); textTitle.setString("LEVEL UP! CHOOSE UPGRADE"); textTitle.setCharacterSize(40); textTitle.setFillColor(sf::Color::White);
        tb = textTitle.getLocalBounds(); textTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textTitle.setPosition(640.f, 120.f);
        card1.setSize(sf::Vector2f(250.f, 350.f)); card1.setPosition(200.f, 200.f); textCard1.setFont(font); textCard1.setCharacterSize(24); textCard1.setPosition(220.f, 350.f);
        card2.setSize(sf::Vector2f(250.f, 350.f)); card2.setPosition(500.f, 200.f); textCard2.setFont(font); textCard2.setCharacterSize(24); textCard2.setPosition(520.f, 350.f);
        card3.setSize(sf::Vector2f(250.f, 350.f)); card3.setPosition(800.f, 200.f); textCard3.setFont(font); textCard3.setCharacterSize(24); textCard3.setPosition(820.f, 350.f);

        menuTitle.setFont(font); menuTitle.setString("ETERNAL NIGHT"); menuTitle.setCharacterSize(80); menuTitle.setFillColor(sf::Color(255, 200, 0));
        tb = menuTitle.getLocalBounds(); menuTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); menuTitle.setPosition(640.f, 140.f);
        btnStart.setSize(sf::Vector2f(300.f, 60.f)); btnStart.setPosition(490.f, 300.f); btnStart.setFillColor(sf::Color(50, 50, 100));
        textStart.setFont(font); textStart.setString("Start game"); textStart.setCharacterSize(30); tb = textStart.getLocalBounds(); textStart.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textStart.setPosition(640.f, 330.f);
        btnQuit.setSize(sf::Vector2f(300.f, 60.f)); btnQuit.setPosition(490.f, 400.f); btnQuit.setFillColor(sf::Color(100, 50, 50));
        textQuit.setFont(font); textQuit.setString("Quit"); textQuit.setCharacterSize(30); tb = textQuit.getLocalBounds(); textQuit.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textQuit.setPosition(640.f, 430.f);

        textPaused.setFont(font); textPaused.setString("PAUSE"); textPaused.setCharacterSize(80); textPaused.setFillColor(sf::Color::Yellow);
        tb = textPaused.getLocalBounds(); textPaused.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textPaused.setPosition(640.f, 190.f);
        btnResume.setSize(sf::Vector2f(300.f, 60.f)); btnResume.setPosition(490.f, 300.f); btnResume.setFillColor(sf::Color(50, 100, 50));
        textResume.setFont(font); textResume.setString("Resume game"); textResume.setCharacterSize(30); tb = textResume.getLocalBounds(); textResume.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textResume.setPosition(640.f, 330.f);
        btnPauseReturn.setSize(sf::Vector2f(300.f, 60.f)); btnPauseReturn.setPosition(490.f, 400.f); btnPauseReturn.setFillColor(sf::Color(100, 50, 50));
        textPauseReturn.setFont(font); textPauseReturn.setString("Back to menu"); textPauseReturn.setCharacterSize(30); tb = textPauseReturn.getLocalBounds(); textPauseReturn.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textPauseReturn.setPosition(640.f, 430.f);

        // Muzyka w menu pauzy
        btnMusicMinus.setSize(sf::Vector2f(50.f, 50.f)); btnMusicMinus.setPosition(490.f, 500.f); btnMusicMinus.setFillColor(sf::Color(80, 80, 80));
        textMusicMinus.setFont(font); textMusicMinus.setString("<"); textMusicMinus.setCharacterSize(30);
        tb = textMusicMinus.getLocalBounds(); textMusicMinus.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f); textMusicMinus.setPosition(515.f, 525.f);

        btnMusicPlus.setSize(sf::Vector2f(50.f, 50.f)); btnMusicPlus.setPosition(740.f, 500.f); btnMusicPlus.setFillColor(sf::Color(80, 80, 80));
        textMusicPlus.setFont(font); textMusicPlus.setString(">"); textMusicPlus.setCharacterSize(30);
        tb = textMusicPlus.getLocalBounds(); textMusicPlus.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f); textMusicPlus.setPosition(765.f, 525.f);

        textMusicVol.setFont(font); textMusicVol.setCharacterSize(30); textMusicVol.setPosition(640.f, 525.f);

        // SFX
        btnSfxMinus.setSize(sf::Vector2f(50.f, 50.f)); btnSfxMinus.setPosition(490.f, 570.f); btnSfxMinus.setFillColor(sf::Color(80, 80, 80));
        textSfxMinus.setFont(font); textSfxMinus.setString("<"); textSfxMinus.setCharacterSize(30);
        tb = textSfxMinus.getLocalBounds(); textSfxMinus.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f); textSfxMinus.setPosition(515.f, 595.f);

        btnSfxPlus.setSize(sf::Vector2f(50.f, 50.f)); btnSfxPlus.setPosition(740.f, 570.f); btnSfxPlus.setFillColor(sf::Color(80, 80, 80));
        textSfxPlus.setFont(font); textSfxPlus.setString(">"); textSfxPlus.setCharacterSize(30);
        tb = textSfxPlus.getLocalBounds(); textSfxPlus.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f); textSfxPlus.setPosition(765.f, 595.f);

        textSfxVol.setFont(font); textSfxVol.setCharacterSize(30); textSfxVol.setPosition(640.f, 595.f);

        textGameOver.setFont(font); textGameOver.setString("You died"); textGameOver.setCharacterSize(80); textGameOver.setFillColor(sf::Color::Red);
        tb = textGameOver.getLocalBounds(); textGameOver.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textGameOver.setPosition(640.f, 190.f);
        textFinalScore.setFont(font); textFinalScore.setCharacterSize(40); textFinalScore.setPosition(640.f, 320.f);
        btnReturn.setSize(sf::Vector2f(300.f, 60.f)); btnReturn.setPosition(490.f, 450.f); btnReturn.setFillColor(sf::Color(80, 80, 80));
        textReturn.setFont(font); textReturn.setString("Back to menu"); textReturn.setCharacterSize(30); tb = textReturn.getLocalBounds(); textReturn.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textReturn.setPosition(640.f, 480.f);

        btnScores.setSize(sf::Vector2f(300.f, 60.f)); btnScores.setPosition(490.f, 500.f); btnScores.setFillColor(sf::Color(30, 80, 30));
        textBtnScores.setFont(font); textBtnScores.setString("Top 10"); textBtnScores.setCharacterSize(30); tb = textBtnScores.getLocalBounds(); textBtnScores.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textBtnScores.setPosition(640.f, 530.f);

        textScoresTitle.setFont(font); textScoresTitle.setString("TOP 10 SCORES"); textScoresTitle.setCharacterSize(50); textScoresTitle.setFillColor(sf::Color(255, 200, 0)); tb = textScoresTitle.getLocalBounds(); textScoresTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textScoresTitle.setPosition(640.f, 80.f);
        textScoresList.setFont(font); textScoresList.setCharacterSize(22); textScoresList.setFillColor(sf::Color::White); textScoresList.setPosition(300.f, 160.f);
        btnReturnFromScores.setSize(sf::Vector2f(300.f, 60.f)); btnReturnFromScores.setPosition(490.f, 600.f); btnReturnFromScores.setFillColor(sf::Color(80, 80, 80));
        textReturnFromScores.setFont(font); textReturnFromScores.setString("Return to menu"); textReturnFromScores.setCharacterSize(30); tb = textReturnFromScores.getLocalBounds(); textReturnFromScores.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textReturnFromScores.setPosition(640.f, 630.f);

        textEnterName.setFont(font); textEnterName.setString("Enter your name:"); textEnterName.setCharacterSize(28); textEnterName.setFillColor(sf::Color::White); tb = textEnterName.getLocalBounds(); textEnterName.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f); textEnterName.setPosition(640.f, 400.f);
        textNameInput.setFont(font); textNameInput.setCharacterSize(32); textNameInput.setFillColor(sf::Color(255, 200, 0)); textNameInput.setPosition(640.f, 440.f);
    }
}

// Logika aktualizacji tekstu
void UIManager::updateNameInput(const std::string &name) {
    textNameInput.setString(name + "_");
    sf::FloatRect nb = textNameInput.getLocalBounds();
    textNameInput.setOrigin(nb.left + nb.width / 2.f, nb.top + nb.height / 2.f);
}

// Detekcja kliknięć
bool UIManager::isStartClicked(sf::Vector2f pos) const { return btnStart.getGlobalBounds().contains(pos); }
bool UIManager::isQuitClicked(sf::Vector2f pos) const { return btnQuit.getGlobalBounds().contains(pos); }
bool UIManager::isScoresClicked(sf::Vector2f pos) const { return btnScores.getGlobalBounds().contains(pos); }
bool UIManager::isResumeClicked(sf::Vector2f pos) const { return btnResume.getGlobalBounds().contains(pos); }
bool UIManager::isPauseReturnClicked(sf::Vector2f pos) const { return btnPauseReturn.getGlobalBounds().contains(pos); }
bool UIManager::isGameOverReturnClicked(sf::Vector2f pos) const { return btnReturn.getGlobalBounds().contains(pos); }
bool UIManager::isScoresReturnClicked(sf::Vector2f pos) const { return btnReturnFromScores.getGlobalBounds().contains(pos); }

int UIManager::getClickedUpgrade(sf::Vector2f pos) const {
    if(card1.getGlobalBounds().contains(pos)) return offeredUpgrades[0];
    if(card2.getGlobalBounds().contains(pos)) return offeredUpgrades[1];
    if(card3.getGlobalBounds().contains(pos)) return offeredUpgrades[2];
    return -1;
}

// Metody renderujące
void UIManager::renderMainMenu(sf::RenderWindow& window) {
    window.draw(menuTitle);
    window.draw(btnStart); window.draw(textStart);
    window.draw(btnQuit); window.draw(textQuit);
    window.draw(btnScores); window.draw(textBtnScores);
}

void UIManager::renderHUD(sf::RenderWindow &window, const std::shared_ptr<Player> &player, float gameTime, WaveManager *waveManager) {
    float xpPercent = static_cast<float>(player->getXp()) / static_cast<float>(player->getMaxXp());
    xpBarForeground.setSize(sf::Vector2f(1280.f * xpPercent, 22.f));

    window.draw(xpBarBackground);
    window.draw(xpBarForeground);
    window.draw(xpBarBorder);

    xpLevelText.setString("LV " + std::to_string(player->getLevel()));
    sf::FloatRect lvBounds = xpLevelText.getLocalBounds();
    xpLevelText.setOrigin(lvBounds.left + lvBounds.width, lvBounds.top + lvBounds.height / 2.f);
    xpLevelText.setPosition(1272.f, 11.f);
    window.draw(xpLevelText);

    int minutes = static_cast<int>(gameTime) / 60;
    int seconds = static_cast<int>(gameTime) % 60;
    std::string timeStr = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
    timerText.setString(timeStr);
    sf::FloatRect timeBounds = timerText.getLocalBounds();
    timerText.setOrigin(timeBounds.left + timeBounds.width / 2.f, timeBounds.top);
    timerText.setPosition(640.f, 26.f);
    window.draw(timerText);

    killCountText.setString(std::to_string(player->getEnemiesKilled()));
    sf::FloatRect killBounds = killCountText.getLocalBounds();
    float killTextX = 1240.f - killBounds.width - 20.f;
    killCountText.setPosition(killTextX, 27.f);
    window.draw(killCountText);
    skullIcon.setPosition(killTextX + killBounds.width + 4.f, 29.f);
    window.draw(skullIcon);

    int score = static_cast<int>(gameTime * 10) + (player->getEnemiesKilled() * 2);
    std::string statsStr = "SCORE: " + std::to_string(score) +
                           "\nWAVE: " + std::to_string(waveManager ? waveManager->getCurrentWave() : 1) + " (+" + std::to_string(waveManager ? (int)waveManager->getSecondsToNextWave() : 0) + "s)" +
                           "\nARMOR: " + std::to_string(player->getArmor()) +
                           "\nDMG+: +" + std::to_string(player->getDamageBonus()) +
                           "\nCRIT: " + std::to_string(static_cast<int>(player->getCritChance() * 100)) + "%" +
                           "\nVAMPIRE: " + std::to_string(static_cast<int>(player->getVampirismChance() * 100)) + "%" +
                           "\nDODGE: " + std::to_string(static_cast<int>(player->getDodgeChance() * 100)) + "%" +
                           "\nREGEN: " + std::to_string(player->getHpRegenRate()) + " HP/5s" +
                           "\nSPEED: " + std::to_string(static_cast<int>(player->getSpeed()));
    hudStatsText.setString(statsStr);
    window.draw(hudStatsText);
}

void UIManager::renderLevelUp(sf::RenderWindow &window) {
    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    window.draw(card1); window.draw(textCard1);
    window.draw(card2); window.draw(textCard2);
    window.draw(card3); window.draw(textCard3);
    window.draw(textTitle);
}

void UIManager::renderGameOver(sf::RenderWindow &window, int score, const std::string &playerName, bool nameSaved) {
    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 220));
    window.draw(overlay);

    window.draw(textGameOver);
    textFinalScore.setString("Your final score: " + std::to_string(score));
    sf::FloatRect scoreBounds = textFinalScore.getLocalBounds();
    textFinalScore.setOrigin(scoreBounds.left + scoreBounds.width / 2.f, scoreBounds.top + scoreBounds.height / 2.f);

    window.draw(textFinalScore);
    window.draw(btnReturn); window.draw(textReturn);

    if (!nameSaved) {
        window.draw(textEnterName);
        window.draw(textNameInput);
    } else {
        sf::Text savedMsg;
        savedMsg.setFont(font);
        savedMsg.setString("Score saved!");
        savedMsg.setCharacterSize(24);
        savedMsg.setFillColor(sf::Color(0, 255, 0));
        sf::FloatRect sb = savedMsg.getLocalBounds();
        savedMsg.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
        savedMsg.setPosition(640.f, 410.f);
        window.draw(savedMsg);
    }
}

void UIManager::renderPause(sf::RenderWindow &window) {
    sf::RectangleShape overlay(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    window.draw(textPaused);
    window.draw(btnResume); window.draw(textResume);
    window.draw(btnPauseReturn); window.draw(textPauseReturn);

    // Aktualizacja i wyśrodkowanie tekstów z wartościami
    textMusicVol.setString("Music: " + std::to_string(static_cast<int>(AssetManager::getMusicVolume())) + "%");
    sf::FloatRect tb = textMusicVol.getLocalBounds();
    textMusicVol.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);

    textSfxVol.setString("SFX: " + std::to_string(static_cast<int>(AssetManager::getSfxVolume())) + "%");
    tb = textSfxVol.getLocalBounds();
    textSfxVol.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);

    // Rysowanie
    window.draw(btnMusicMinus); window.draw(textMusicMinus);
    window.draw(btnMusicPlus); window.draw(textMusicPlus);
    window.draw(textMusicVol);

    window.draw(btnSfxMinus); window.draw(textSfxMinus);
    window.draw(btnSfxPlus); window.draw(textSfxPlus);
    window.draw(textSfxVol);
}

void UIManager::renderScores(sf::RenderWindow& window, const std::string& scoresText) {
    window.clear(sf::Color(30, 30, 40));
    sf::RectangleShape panel(sf::Vector2f(760.f, 500.f));
    panel.setFillColor(sf::Color(40, 50, 70));
    panel.setOutlineColor(sf::Color(180, 130, 30));
    panel.setOutlineThickness(2.f);
    panel.setPosition(260.f, 50.f);
    window.draw(panel);

    window.draw(textScoresTitle);
    textScoresList.setString(scoresText);
    window.draw(textScoresList);
    window.draw(btnReturnFromScores); window.draw(textReturnFromScores);
}

void UIManager::generateUpgrades() {
    std::vector<int> selected;
    while(selected.size() < 3) {
        int r = rand() % 14;
        if(std::find(selected.begin(), selected.end(), r) == selected.end()) {
            selected.push_back(r);
        }
    }

    for(int i = 0; i < 3; ++i) {
        offeredUpgrades[i] = selected[i];
        std::wstring text;
        sf::Color color;

        switch (selected[i]) {
        case 0: text = L"+20 Max HP\n& Heal"; color = sf::Color(50, 100, 50); break;
        case 1: text = L"+15% Movement\nSpeed"; color = sf::Color(50, 50, 100); break;
        case 2: text = L"-10% Attack\nCooldown"; color = sf::Color(100, 50, 50); break;
        case 3: text = L"+5 Attack\nDamage"; color = sf::Color(120, 40, 40); break;
        case 4: text = L"+2 Armor\nPoints"; color = sf::Color(100, 100, 30); break;
        case 5: text = L"+5% Critical\nHit Chance"; color = sf::Color(100, 50, 100); break;
        case 6: text = L"+5% Heal Chance\n(2 HP) on kill"; color = sf::Color(140, 20, 50); break;
        case 7: text = L"+ Pickup\nRadius\n(Magnet)"; color = sf::Color(100, 100, 150); break;
        case 8: text = L"+10% Dodge\nChance"; color = sf::Color(40, 120, 120); break;
        case 9: text = L"-20% Crossbow\nCooldown"; color = sf::Color(150, 100, 50); break;
        case 10: text = L"Regeneration\n+1 HP every\n5 seconds"; color = sf::Color(50, 150, 50); break;
        case 11: text = L"Fire Aura\n(Unlock / Expand)"; color = sf::Color(200, 80, 0); break;
        case 12: text = L"Orbiting Sword\n(Unlock / Faster)"; color = sf::Color(0, 150, 200); break;
        case 13: text = L"Magic Wand\n(Unlock / Upgrade)"; color = sf::Color(120, 0, 200); break;
        }

        if(i == 0) { textCard1.setString(text); card1.setFillColor(color); }
        else if(i == 1) { textCard2.setString(text); card2.setFillColor(color); }
        else if(i == 2) { textCard3.setString(text); card3.setFillColor(color); }
    }    
}

void UIManager::drawBossHealthBar(sf::RenderWindow& window, int currentHp, int maxHp, const std::string& bossName) {
    if (currentHp <= 0) return;

    // Tło paska
    sf::RectangleShape bg(sf::Vector2f(800.f, 25.f));
    bg.setFillColor(sf::Color(30, 30, 30, 200));
    bg.setOutlineColor(sf::Color::Black);
    bg.setOutlineThickness(2.f);
    bg.setOrigin(400.f, 12.5f);
    bg.setPosition(640.f, 660.f);

    // Wypełnienie HP (czerwone)
    float hpPercent = std::max(0.f, static_cast<float>(currentHp) / maxHp);
    sf::RectangleShape fg(sf::Vector2f(800.f * hpPercent, 25.f));
    fg.setFillColor(sf::Color(180, 0, 0));
    fg.setOrigin(400.f, 12.5f);
    fg.setPosition(640.f, 660.f);

    // Nazwa Bossa
    sf::Text nameText;
    nameText.setFont(font);
    nameText.setString(bossName);
    nameText.setCharacterSize(24);
    nameText.setFillColor(sf::Color::White);
    sf::FloatRect tb = nameText.getLocalBounds();
    nameText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    nameText.setPosition(640.f, 635.f);

    // Rysowanie z użyciem widoku domyślnego żeby pasek się nie ruszał z kamerą
    sf::View prevView = window.getView();
    window.setView(window.getDefaultView());

    window.draw(bg);
    window.draw(fg);
    window.draw(nameText);

    window.setView(prevView);
}
// Metody sprawdzajace kliknięcia
bool UIManager::isMusicMinusClicked(sf::Vector2f pos) const { return btnMusicMinus.getGlobalBounds().contains(pos); }
bool UIManager::isMusicPlusClicked(sf::Vector2f pos) const { return btnMusicPlus.getGlobalBounds().contains(pos); }
bool UIManager::isSfxMinusClicked(sf::Vector2f pos) const { return btnSfxMinus.getGlobalBounds().contains(pos); }
bool UIManager::isSfxPlusClicked(sf::Vector2f pos) const { return btnSfxPlus.getGlobalBounds().contains(pos); }