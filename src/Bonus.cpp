#include "Bonus.h"
#include "AssetManager.h"
#include "Config.h"

Bonus::Bonus(float x, float y, BonusType bonusType) : GameObject(x, y), type(bonusType), textureLoaded(false) {
    fallbackShape.setSize(sf::Vector2f(20.f, 20.f));
    fallbackShape.setOrigin(10.f, 10.f);
    fallbackShape.setPosition(position);

    if (type == BonusType::POTION) {
        fallbackShape.setFillColor(sf::Color::Magenta);
    }

    // Ładowanie tekstury z ścieżki zdefiniowanej w Config.h
    const char* texPath = nullptr;
    if (type == BonusType::POTION) texPath = TexturePaths::BONUS_POTION;

    if (texPath) {
        bonusTexture = AssetManager::loadTexture(texPath);
        if (bonusTexture) {
            textureLoaded = true;
            bonusSprite.setTexture(*bonusTexture);

            sf::FloatRect lb = bonusSprite.getLocalBounds();
            bonusSprite.setOrigin(lb.width / 2.f, lb.height / 2.f);
            bonusSprite.setPosition(position);
        }
    }
}

void Bonus::update(float deltaTime) {
    // Bonus leży w miejscu i czeka na zebranie
}

void Bonus::draw(sf::RenderWindow& window) {
    if (textureLoaded) {
        window.draw(bonusSprite);
    } else {
        window.draw(fallbackShape);
    }
}

sf::FloatRect Bonus::getBounds() const {
    if (textureLoaded) {
        return bonusSprite.getGlobalBounds();
    }
    return fallbackShape.getGlobalBounds();
}

BonusType Bonus::getType() const {
    return type;
}