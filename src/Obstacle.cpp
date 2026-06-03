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
        return sf::FloatRect(position.x - 13.f, position.y + 60.f, 28.f, 35.f);
    } else if(type == ObstacleType::BUSH) {
        return sf::FloatRect(position.x - 21.f, position.y - 21.f, 43.f, 47.f);
    } else if(type == ObstacleType::ROCK) {
        return sf::FloatRect(position.x - 15.f, position.y - 15.f, 30.f, 30.f);
    }
    return sf::FloatRect();
}