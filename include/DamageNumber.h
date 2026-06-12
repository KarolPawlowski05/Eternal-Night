#ifndef DAMAGENUMBER_H
#define DAMAGENUMBER_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>

// Unosząca się liczba obrażeń pojawiająca się przy trafieniu
class DamageNumber : public GameObject {
private:
    sf::Text text;
    float lifetime;
    static constexpr float MAX_LIFETIME = 0.85f;

    // Ruch i skala
    float vy;           // Predkosc unoszenia sie (px/s)
    float currentScale;
    bool isCrit;
    bool isHeal;
public:
    // referencja do fontu silnika
    DamageNumber(float x, float y, int damage, bool isCrit, const sf::Font& font, bool isHeal = false);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
};

#endif