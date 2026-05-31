#include "XpCrystal.h"
#include <cmath>

XpCrystal::XpCrystal(float x, float y, int value)
    : GameObject(x, y), xpValue(value),
    textureLoaded(false), currentFrame(0), frameTimer(0.f), state(CrystalState::IDLE)
{
    // Ładowanie sprite'ów
    if(texture.loadFromFile("assets/map/collectibles/xpCrystal.png")) {
        texture.setSmooth(false);
        textureLoaded = true;

        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, FRAME_SIZE, FRAME_SIZE));

        // Skalowanie
        sprite.setScale(1.25f, 1.25f);

        sf::FloatRect lb = sprite.getLocalBounds();
        sprite.setOrigin(lb.width / 2.f, lb.height / 2.f);
        sprite.setPosition(position);
    } else {
        // Nie załadowana tekstura
        fallbackShape.setSize(sf::Vector2f(12.f, 12.f));
        fallbackShape.setFillColor(sf::Color::Cyan);
        fallbackShape.setOrigin(6.f, 6.f);
        fallbackShape.setPosition(position);
        fallbackShape.setRotation(45.f);
    }
}

void XpCrystal::update(float deltaTime) {
    // Animacja
    frameTimer += deltaTime;
    if(frameTimer >= FRAME_SPEED) {
        frameTimer -= FRAME_SPEED;
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        if(textureLoaded) {
            sprite.setTextureRect(sf::IntRect(currentFrame * FRAME_SIZE, 0, FRAME_SIZE, FRAME_SIZE));
        }
    }

    // Ruch w trybie przyciągania
    if(state == CrystalState::HOMING) {
        float dx = targetPos.x - position.x;
        float dy = targetPos.y - position.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if(dist > 1.f) {
            // Prędkość rośnie z bliższą odległością
            float speed = HOMING_SPEED + (1.f - std::min(dist / 200.f, 1.f)) * 300.f;
            position.x += (dx / dist) * speed * deltaTime;
            position.y += (dy / dist) * speed * deltaTime;
        }
    }

    if(textureLoaded) {
        sprite.setPosition(position);
    } else {
        fallbackShape.setPosition(position);
    }
}

void XpCrystal::draw(sf::RenderWindow& window) {
    if(textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(fallbackShape);
    }
}

sf::FloatRect XpCrystal::getBounds() const {
    if(textureLoaded) {
        return sprite.getGlobalBounds();
    }
    return fallbackShape.getGlobalBounds();
}