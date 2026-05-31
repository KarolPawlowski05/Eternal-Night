#include "DamageNumber.h"
#include <string>

DamageNumber::DamageNumber(float x, float y, int damage, bool isCrit, const sf::Font& font)
    : GameObject(x, y), lifetime(MAX_LIFETIME), currentScale(1.f), isCrit(isCrit)
{
    // Krytyk - ciemnoczerwony i troche większy; zwykły - jasnoczerwony
    text.setFont(font);
    text.setString(std::to_string(damage));
    text.setCharacterSize(isCrit ? 22 : 16);

    if(isCrit) {
        text.setFillColor(sf::Color(180, 0, 0, 255));
        text.setOutlineColor(sf::Color(255, 200, 0, 200));
        text.setOutlineThickness(1.5f);
        vy = -95.f;
    } else {
        text.setFillColor(sf::Color(255, 90, 90, 255));
        text.setOutlineColor(sf::Color(80, 0, 0, 180));
        text.setOutlineThickness(1.0f);
        vy = -75.f;
    }

    // Wycentrowanie tekstu na pozycji trafienia
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(position);
}

void DamageNumber::update(float deltaTime) {
    lifetime -= deltaTime;
    if(lifetime <= 0.f) {
        destroy();
        return;
    }

    // Unoszenie w górę
    position.y += vy * deltaTime;

    // Szybki wzrost skali (pierwsze 20% czasu "życia")
    float progress = 1.f - (lifetime / MAX_LIFETIME); // 0 -> 1 przez czas życia
    if(progress < 0.2f) {
        currentScale = 1.f + (isCrit ? 0.8f : 0.4f) * (progress / 0.2f);
    } else {
        // Kurczenie sie i zanikanie
        float fadeProgress = (progress - 0.2f) / 0.8f; // 0 -> 1 w fazie zanikania
        currentScale = (isCrit ? 1.8f : 1.4f) * (1.f - fadeProgress * 0.5f);

        // Zanikanie alpha
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 *  (1.f - fadeProgress));
        sf::Color col = text.getFillColor();
        col.a = alpha;
        text.setFillColor(col);

        sf::Color outlineCol = text.getOutlineColor();
        outlineCol.a = static_cast<sf::Uint8>(alpha * 0.8f);
        text.setOutlineColor(outlineCol);
    }

    text.setScale(currentScale, currentScale);
    text.setPosition(position);
}

void DamageNumber::draw(sf::RenderWindow& window) {
    window.draw(text);
}

sf::FloatRect DamageNumber::getBounds() const {
    return sf::FloatRect(position.x - 1.f, position.y - 1.f, 2.f, 2.f);
}