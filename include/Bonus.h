#ifndef BONUS_H
#define BONUS_H

#include "GameObject.h"

// Typy bonusów
enum class BonusType { POTION };

class Bonus : public GameObject {
private:
    sf::RectangleShape sprite;
    BonusType type; // Przechowuje typ bonusu
public:
    Bonus(float x, float y, BonusType bonusType);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    BonusType getType() const;
};

#endif