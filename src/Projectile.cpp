#include "Projectile.h"
#include "AssetManager.h"

Projectile::Projectile(float startX, float startY, sf::Vector2f direction, float customSpeed, bool enemy, bool wand, const std::string& texturePath)
    : GameObject(startX, startY), speed(customSpeed), isEnemyOwned(enemy), isWand(wand), hasTarget(false), distanceTraveled(0.f) {

    velocity = direction * speed;

    if (isEnemyOwned) {
        // Głaz ogra
        if (!texturePath.empty()) {
            auto customTex = AssetManager::loadTexture(texturePath);
            if (customTex) {
                arrowTexture = customTex;
                arrowSprite.setTexture(*arrowTexture);
                sf::FloatRect lb = arrowSprite.getLocalBounds();
                arrowSprite.setOrigin(lb.width / 2.f, lb.height / 2.f);
            }
        }

        if(!arrowTexture) {
            auto rockTexture = std::make_shared<sf::Texture>();
            rockTexture->create(16, 16);
            sf::Image img;
            img.create(16, 16, sf::Color(139, 69, 19)); // Brązowy kolor
            rockTexture->loadFromImage(img);

            arrowTexture = rockTexture;
            arrowSprite.setTexture(*arrowTexture);
            arrowSprite.setOrigin(8.f, 8.f);
        }
    } else if (isWand) {
        // Różdżka
        auto wandTexture = std::make_shared<sf::Texture>();
        wandTexture->create(12, 12);
        sf::Image img;
        img.create(12, 12, sf::Color(0, 0, 0, 0)); // przezroczyste tło
        // Rysujemy kółko fioletowe ręcznie
        for (int px = 0; px < 12; px++) {
            for (int py = 0; py < 12; py++) {
                float dx = px - 5.5f;
                float dy = py - 5.5f;
                if (dx*dx + dy*dy <= 25.f) {
                    img.setPixel(px, py, sf::Color(180, 0, 255, 255));
                }
            }
        }
        wandTexture->loadFromImage(img);
        arrowTexture = wandTexture;
        arrowSprite.setTexture(*arrowTexture);
        arrowSprite.setOrigin(6.f, 6.f);
    } else {
        // STRZAŁA Z KUSZY GRACZA
        arrowTexture = AssetManager::loadTexture("assets/player/attacks/crossbow/arrow.png");

        if(arrowTexture) {
            arrowSprite.setTexture(*arrowTexture);
            sf::FloatRect tb = arrowSprite.getLocalBounds();
            arrowSprite.setOrigin(tb.width / 2.f, tb.height / 2.f);
        } else {
            // Error – biały prostokąt w rozmiarze strzałki (20x5)
            auto fallbackTexture = std::make_shared<sf::Texture>();
            fallbackTexture->create(20, 5);
            sf::Image img;
            img.create(20, 5, sf::Color::White);
            fallbackTexture->loadFromImage(img);
            arrowTexture = fallbackTexture;
            arrowSprite.setTexture(*arrowTexture);
            arrowSprite.setOrigin(10.f, 2.5f);
        }
    }
    arrowSprite.setPosition(position);

    // Ustawienie rotacji pocisku zgodnie z kierunkiem lotu
    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    arrowSprite.setRotation(angle);
}

void Projectile::update(float deltaTime) {
    // Lot pocisku
    position += velocity * deltaTime;
    if (isWand && hasTarget) {
        // Skręcanie w stronę celu
        sf::Vector2f dir = homingTarget - position;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 5.f) {
            dir /= len;
            // Płynne skręcanie  mieszamy aktualny kierunek z docelowym
            sf::Vector2f currentDir = velocity / speed;
            currentDir = currentDir * 0.85f + dir * 0.15f;
            float newLen = std::sqrt(currentDir.x * currentDir.x + currentDir.y * currentDir.y);
            if (newLen > 0.f) currentDir /= newLen;
            velocity = currentDir * speed;
        }
        // Znikaj jeśli dotarłeś do celu
        if (len < 15.f) destroy();
    }

    // Różdżka też znika po dystansie
    if (isWand) {
        distanceTraveled += speed * deltaTime;
        if (distanceTraveled > 1000.f) destroy();
    }
    if (isEnemyOwned) {
        distanceTraveled += speed * deltaTime;
        if (distanceTraveled > 800.f) destroy();
    }
    arrowSprite.setPosition(position);

}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(arrowSprite);
}

sf::FloatRect Projectile::getBounds() const {
    return arrowSprite.getGlobalBounds();
}