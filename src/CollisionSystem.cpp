#include "CollisionSystem.h"
#include "Enemy.h"
#include "Projectile.h"
#include "XpCrystal.h"
#include "Bonus.h"
#include "Obstacle.h"
#include "Boss.h"
#include "DamageNumber.h"
#include <cmath>
#include <cstdlib>

void CollisionSystem::spawnDamageNumber(float x, float y, int damage, bool isCrit, const sf::Font& font, std::vector<std::shared_ptr<GameObject>>& newObjects) {
    if(damage <= 0) return;
    float offsetX = static_cast<float>((rand() % 30) - 15);
    newObjects.push_back(std::make_shared<DamageNumber>(x + offsetX, y - 20.f, damage, isCrit, font));
}

void CollisionSystem::update(std::shared_ptr<Player>& player, std::vector<std::shared_ptr<GameObject>>& gameObjects, const sf::Font& font) {
    std::vector<std::shared_ptr<GameObject>> newObjects;

    for(auto& obj : gameObjects) {
        if(!obj->isActive()) continue;

        // POCISKI
        auto proj = dynamic_cast<Projectile*>(obj.get());
        if(proj) {
            if(proj->getIsEnemyOwned()) {
                if(proj->getBounds().intersects(player->getBounds())) {
                    player->takeDamage(8);
                    proj->destroy();
                }
            }
            continue;
        }

        // WROGOWIE
        auto enemy = dynamic_cast<Enemy*>(obj.get());
        if(enemy) {
            // Gracz - Wróg
            if(player->getBounds().intersects(enemy->getBounds())) {
                if(enemy->getIsKamikaze()) {
                    player->takeDamage(30);
                    enemy->takeDamage(9999);
                } else {
                    player->takeDamage(10);
                    if(enemy->getIsVampire()) {
                        enemy->heal(5);
                    }
                }
            }

            // 2. Jeśli wróg zginął od dotyku kamikaze, nie sprawdzaj reszty
            if(!enemy->isActive()) continue;

            // Atak obszarowy gracza
            if(player->getIsAttacking() && player->getAttackBounds().intersects(enemy->getBounds())) {
                bool wasCrit = false;
                int dmg = player->getDamage(25, &wasCrit);
                int dealt = enemy->takeDamage(dmg);
                if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit, font, newObjects);
                if(!enemy->isActive()) {
                    player->incrementKills();
                    player->triggerVampirism();
                    newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                }
            }

            // Aura Ognia
            if(player->getHasFireAura()) {
                float dx = enemy->getPosition().x - player->getPosition().x;
                float dy = enemy->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if(dist <= player->getFireAuraRadius()) {
                    bool wasCrit = false;
                    int dmg = player->getDamage(3, &wasCrit);
                    int dealt = enemy->takeDamage(dmg, 1);
                    if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit, font, newObjects);
                    if(!enemy->isActive()) {
                        player->incrementKills();
                        player->triggerVampirism();
                        newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                    }
                }
            }

            // Orbitujące Ostrze
            if(player->getHasOrbitingSword()) {
                auto swords = player->getOrbitingSwordsBounds();
                for (const auto& swordBounds : swords) {
                    // Sprawdź czy enemy nadal istnieje (nie został usunięty w tej samej pętli)
                    if (enemy && enemy->isActive()) {
                        if(swordBounds.intersects(enemy->getBounds())) {
                            bool wasCrit = false;
                            int dmg = player->getDamage(15, &wasCrit);
                            int dealt = enemy->takeDamage(dmg, 2);
                            if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit, font, newObjects);

                            if(!enemy->isActive()) {
                                player->incrementKills();
                                player->triggerVampirism();
                                newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                            }
                            break; // Wróg dostał strzał, koniec sprawdzania innych mieczy dla tego wroga
                        }
                    }
                }
            }

            // Kusza - Wróg
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile && !projectile->getIsEnemyOwned()) {
                    if(projectile->getBounds().intersects(enemy->getBounds())) {
                        bool wasCrit = false;
                        int baseDmg = projectile->getIsWand() ? (35 + player->getWandDamageBonus()) : 50;
                        int dmg = player->getDamage(baseDmg, &wasCrit);
                        int dealt = enemy->takeDamage(dmg);
                        if(dealt > 0) spawnDamageNumber(enemy->getPosition().x, enemy->getPosition().y, dealt, wasCrit, font, newObjects);
                        if(!enemy->isActive()) {
                            player->incrementKills();
                            player->triggerVampirism();
                            newObjects.push_back(std::make_shared<XpCrystal>(enemy->getPosition().x, enemy->getPosition().y, enemy->getXpReward()));
                        }
                        projectile->destroy();
                    }
                }
            }

            // Wróg - Wróg
            if(!enemy->getIsGhost()) {
                for(auto& otherObj : gameObjects) {
                    if(!otherObj->isActive() || obj == otherObj) continue;
                    auto otherEnemy = dynamic_cast<Enemy*>(otherObj.get());
                    if(otherEnemy) {
                        sf::FloatRect intersection;
                        if(enemy->getBounds().intersects(otherEnemy->getBounds(), intersection)) {
                            sf::Vector2f pos = enemy->getPosition();
                            if(intersection.width < intersection.height) {
                                if(enemy->getBounds().left < otherEnemy->getBounds().left) pos.x -= intersection.width / 2.f;
                                else pos.x += intersection.width / 2.f;
                            } else {
                                if(enemy->getBounds().top < otherEnemy->getBounds().top) pos.y -= intersection.height / 2.f;
                                else pos.y += intersection.height / 2.f;
                            }
                            enemy->setPosition(pos);
                        }
                    }
                }
            }


            // Rzut Głazem
            if(enemy->checkAndResetThrow()) {
                sf::Vector2f dir = player->getPosition() - enemy->getPosition();
                float length = std::hypot(dir.x, dir.y);
                if(length > 0 && length < 500.f){
                    dir /= length;
                    newObjects.push_back(std::make_shared<Projectile>(enemy->getPosition().x, enemy->getPosition().y, dir, 200.0f, true));
                }
            }
        }
        // DODANA SEKCJA: KOLIZJE BOSSA
        auto boss = dynamic_cast<Boss*>(obj.get());
        if(boss) {
            //  Gracz - Boss (Obrażenia kontaktowe)
            if(player->getBounds().intersects(boss->getBounds())) {
                player->takeDamage(10);
            }

            //  Atak obszarowy gracza - Boss
            if(player->getIsAttacking() && player->getAttackBounds().intersects(boss->getBounds())) {
                bool wasCrit = false;
                int dmg = player->getDamage(25, &wasCrit);
                int dealt = boss->takeDamage(dmg);
                if(dealt > 0) spawnDamageNumber(boss->getPosition().x, boss->getPosition().y, dealt, wasCrit, font, newObjects);
            }

            //  Aura Ognia - Boss
            if(player->getHasFireAura()) {
                float dx = boss->getPosition().x - player->getPosition().x;
                float dy = boss->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if(dist <= player->getFireAuraRadius()) {
                    bool wasCrit = false;
                    int dmg = player->getDamage(3, &wasCrit);
                    int dealt = boss->takeDamage(dmg);
                    if(dealt > 0) spawnDamageNumber(boss->getPosition().x, boss->getPosition().y, dealt, wasCrit, font, newObjects);
                }
            }

            // Orbitujące Ostrze - Boss
            if(player->getHasOrbitingSword()) {
                for (const auto& swordBounds : player->getOrbitingSwordsBounds()) {
                    if(swordBounds.intersects(boss->getBounds())) {
                        bool wasCrit = false;
                        int dmg = player->getDamage(15, &wasCrit);
                        int dealt = boss->takeDamage(dmg, 2); // Typ 2 = Miecz
                        if(dealt > 0) spawnDamageNumber(boss->getPosition().x, boss->getPosition().y, dealt, wasCrit, font, newObjects);
                        break; // Uderzenie jednym mieczem wystarczy na klatkę
                    }
                }
            }

            //  Kusza i Różdżka - Boss
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto projectile = dynamic_cast<Projectile*>(otherObj.get());
                if(projectile && !projectile->getIsEnemyOwned()) {
                    if(projectile->getBounds().intersects(boss->getBounds())) {
                        bool wasCrit = false;
                        int baseDmg = projectile->getIsWand() ? (35 + player->getWandDamageBonus()) : 50;
                        int dmg = player->getDamage(baseDmg, &wasCrit);

                        int dealt = boss->takeDamage(dmg);
                        if(dealt > 0) spawnDamageNumber(boss->getPosition().x, boss->getPosition().y, dealt, wasCrit, font, newObjects);

                        projectile->destroy();
                    }
                }
            }

            // Sprawdzenie czy boss zginął
            if(!boss->isActive()) {
                player->incrementKills();
                player->triggerVampirism();
                newObjects.push_back(std::make_shared<XpCrystal>(boss->getPosition().x, boss->getPosition().y, 750));
            }
        }

        // KRYSZTAŁY XP
        auto xp = dynamic_cast<XpCrystal*>(obj.get());
        if(xp) {
            if(!xp->isHoming() && player->getPickupBounds().intersects(xp->getBounds())) {
                xp->startHoming();
            }
            if(xp->isHoming()) {
                xp->setTargetPos(player->getPosition());
                float dx = xp->getPosition().x - player->getPosition().x;
                float dy = xp->getPosition().y - player->getPosition().y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if(dist < 18.f) {
                    player->addXp(xp->getXpValue());
                    xp->destroy();
                }
            }
        }

        // BONUSY
        auto bonus = dynamic_cast<Bonus*>(obj.get());
        if(bonus) {
            if(player->getPickupBounds().intersects(bonus->getBounds())) {
                if(bonus->getType() == BonusType::POTION) {
                    player->heal(20);
                }
                bonus->destroy();
            }
        }

        // PRZESZKODY
        auto obstacle = dynamic_cast<Obstacle*>(obj.get());
        if(obstacle) {
            sf::FloatRect obsBounds = obstacle->getBounds();
            sf::FloatRect intersection;

            // Gracz - Przeszkoda
            if(player->getBounds().intersects(obsBounds, intersection)) {
                sf::Vector2f newPos = player->getPosition();
                if(intersection.width < intersection.height) {
                    if(player->getBounds().left < obsBounds.left) newPos.x -= intersection.width;
                    else newPos.x += intersection.width;
                } else {
                    if(player->getBounds().top < obsBounds.top) newPos.y -= intersection.height;
                    else newPos.y += intersection.height;
                }
                player->setPosition(newPos);
            }

            // Wróg - Przeszkoda
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto otherEnemy = dynamic_cast<Enemy*>(otherObj.get());
                if(otherEnemy && !otherEnemy->getIsGhost()) {
                    sf::FloatRect enemyIntersection;
                    if(otherEnemy->getBounds().intersects(obsBounds, enemyIntersection)) {
                        sf::Vector2f newEnemyPos = otherEnemy->getPosition();
                        if(enemyIntersection.width < enemyIntersection.height) {
                            if(otherEnemy->getBounds().left < obsBounds.left) newEnemyPos.x -= enemyIntersection.width;
                            else newEnemyPos.x += enemyIntersection.width;
                        } else {
                            if(otherEnemy->getBounds().top < obsBounds.top) newEnemyPos.y -= enemyIntersection.height;
                            else newEnemyPos.y += enemyIntersection.height;
                        }
                        otherEnemy->setPosition(newEnemyPos);
                    }
                }
            }
        }
        }

    // Dodanie nowo powstałych obiektów (np. kryształów xp i obrażeń) do głównego wektora
    for(auto& newObj : newObjects) {
        gameObjects.push_back(newObj);
    }
}