#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "GameObject.h"

class EntityManager {
private:
    std::vector<std::shared_ptr<GameObject>> entities;

public:
    EntityManager() = default;

    void addEntity(std::shared_ptr<GameObject> entity);
    void updateAll(float deltaTime);
    void renderAll(sf::RenderWindow& window);
    void cleanupDeadEntities();
    void clear();

    // Metoda zwracająca referencję, żeby inne systemy mogły na nich pracować
    std::vector<std::shared_ptr<GameObject>>& getEntities();
};

#endif