#include "EntityManager.h"
#include <algorithm>

void EntityManager::addEntity(std::shared_ptr<GameObject> entity) {
    entities.push_back(entity);
}

void EntityManager::updateAll(float deltaTime) {
    for(auto& entity : entities) {
        if(entity->isActive()) {
            entity->update(deltaTime);
        }
    }
}

void EntityManager::renderAll(sf::RenderWindow &window) {
    for(auto& entity : entities) {
        if(entity->isActive()) {
            entity->draw(window);
        }
    }
}

void EntityManager::cleanupDeadEntities() {
    entities.erase(
        std::remove_if(entities.begin(), entities.end(), [](const std::shared_ptr<GameObject>& obj) { return !obj->isActive(); }), entities.end());
}

void EntityManager::clear() {
    entities.clear();
}

std::vector<std::shared_ptr<GameObject>>& EntityManager::getEntities() {
    return entities;
}