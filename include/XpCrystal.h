#ifndef XPCRYSTAL_H
#define XPCRYSTAL_H

#include "GameObject.h"

class XpCrystal : public GameObject {
private:
    sf::RectangleShape shape;
    int xpValue;

public:
    XpCrystal(float x, float y, int value);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    int getXpValue() const { return xpValue; }
};

#endif