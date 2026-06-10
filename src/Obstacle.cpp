#include "Obstacle.h"
#include "AssetManager.h"

Obstacle::Obstacle(float x, float y, ObstacleType obsType) : GameObject(x, y), type(obsType), textureLoaded(false) {
    const char* path = nullptr;

    if(type == ObstacleType::TREE)        path = "assets/map/obstacles/tree.png";
    else if (type == ObstacleType::BUSH)   path = "assets/map/obstacles/bush.png";
    else if (type == ObstacleType::ROCK)   path = "assets/map/obstacles/rock.png";

    if(path) {
        texture = AssetManager::loadTexture(path);

        if(texture) {
            textureLoaded = true;
            sprite.setTexture(*texture);

            sf::FloatRect lb = sprite.getLocalBounds();
            sprite.setOrigin(lb.width / 2.f, lb.height / 2.f);
            sprite.setPosition(position);
        }
    }
    // 1. LOSOWA SKALA (od 0.7x do 1.3x)
    scale = 0.7f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.6f));
    sprite.setScale(scale, scale);

    // 2. LOSOWY ODCIEŃ (Tinting)
    if (type == ObstacleType::TREE || type == ObstacleType::BUSH) {
        // Drzewa i krzaki: manipulujemy odcieniami zieleni
        sf::Uint8 r = 180 + rand() % 75; // 180-255
        sf::Uint8 g = 200 + rand() % 55; // 200-255 (zawsze dominująca zieleń)
        sf::Uint8 b = 180 + rand() % 75; // 180-255
        sprite.setColor(sf::Color(r, g, b));
    } else if (type == ObstacleType::ROCK) {
        // Skały: manipulujemy szarościami i jasnością
        sf::Uint8 shade = 150 + rand() % 105; // 150-255
        sprite.setColor(sf::Color(shade, shade, shade));
    }

    // Jeśli sie nie wczyta
    if(!textureLoaded) {
        if (type == ObstacleType::TREE) {
            fallbackShape.setSize(sf::Vector2f(40.f, 80.f));
            fallbackShape.setFillColor(sf::Color(34, 139, 34));
        } else if (type == ObstacleType::BUSH) {
            fallbackShape.setSize(sf::Vector2f(43.f, 43.f));
            fallbackShape.setFillColor(sf::Color(0, 200, 80));
        } else if (type == ObstacleType::ROCK) {
            fallbackShape.setSize(sf::Vector2f(26.f, 27.f));
            fallbackShape.setFillColor(sf::Color(105, 105, 105));
        }
        sf::Vector2f sz = fallbackShape.getSize();
        fallbackShape.setOrigin(sz.x / 2.f, sz.y / 2.f);
        fallbackShape.setPosition(position);
    }
}

void Obstacle::update(float deltaTime) {
    // Przeszkoda się nie rusza
}

void Obstacle::draw(sf::RenderWindow& window) {
    if(textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(fallbackShape);
    }
}

sf::FloatRect Obstacle::getBounds() const {
    if(type == ObstacleType::TREE) {
        return sf::FloatRect(position.x - (13.f * scale), position.y + (60.f * scale), 28.f * scale, 35.f * scale);
    } else if(type == ObstacleType::BUSH) {
        return sf::FloatRect(position.x - (21.f * scale), position.y - (21.f * scale), 43.f * scale, 47.f * scale);
    } else if(type == ObstacleType::ROCK) {
        return sf::FloatRect(position.x - (15.f * scale), position.y - (15.f * scale), 30.f * scale, 30.f * scale);
    }
    return sf::FloatRect();
}