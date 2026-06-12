#ifndef BONUS_H
#define BONUS_H

#include "GameObject.h"
#include <memory>

// Typy bonusów
enum class BonusType { POTION };

class Bonus : public GameObject {
private:
    std::shared_ptr<sf::Texture> bonusTexture;
    sf::Sprite bonusSprite;
    sf::RectangleShape fallbackShape; // Zapasowy kształt gdy brak tekstury
    bool textureLoaded;

    BonusType type; // Przechowuje typ bonusu
public:
    Bonus(float x, float y, BonusType bonusType);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    BonusType getType() const;
};

#endif