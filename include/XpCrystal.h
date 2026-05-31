#ifndef XPCRYSTAL_H
#define XPCRYSTAL_H

#include "GameObject.h"

enum class CrystalState { IDLE, HOMING };

class XpCrystal : public GameObject {
private:
    // Sprite i animacja
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded;
    int currentFrame;
    float frameTimer;
    static constexpr int FRAME_COUNT = 4;
    static constexpr int FRAME_SIZE = 16;
    static constexpr float FRAME_SPEED = 0.12f;

    // Gdy nie ma tekstury
    sf::RectangleShape fallbackShape;

    // Logika zbierania
    CrystalState state;
    sf::Vector2f targetPos; // Pozycja gracza
    static constexpr float HOMING_SPEED = 10.f;

    int xpValue;

public:
    XpCrystal(float x, float y, int value);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    int getXpValue() const { return xpValue; }

    // Uruchamianie trybu przyciągania
    void startHoming() { state = CrystalState::HOMING; }
    bool isHoming() const { return state == CrystalState::HOMING; }

    // Aktualizacja celu lotu
    void setTargetPos(sf::Vector2f pos) { targetPos = pos; }
};

#endif