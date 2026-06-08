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
#include <queue>
#include <algorithm>

void CollisionSystem::spawnDamageNumber(float x, float y, int damage, bool isCrit, const sf::Font& font, std::vector<std::shared_ptr<GameObject>>& newObjects) {
    if(damage <= 0) return;
    if(isCrit) {
        AssetManager::playSound("assets/audio/sfx/critHit.wav");
    }
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
                    if(otherEnemy && !otherEnemy->getIsGhost()) {

                        sf::Vector2f pos1 = enemy->getPosition();
                        sf::Vector2f pos2 = otherEnemy->getPosition();
                        float dx = pos1.x - pos2.x;
                        float dy = pos1.y - pos2.y;
                        float distSq = dx*dx + dy*dy;
                        float minDist = 35.f; // Minimalny odstęp między potworami

                        if(distSq > 0 && distSq < minDist * minDist) {
                            float dist = std::sqrt(distSq);
                            float force = (minDist - dist) * 15.0f; // Czym bliżej, tym mocniej odpycha
                            enemy->addForce(sf::Vector2f((dx/dist) * force, (dy/dist) * force));
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
        // KOLIZJE BOSSA
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
                AssetManager::playSound("assets/audio/sfx/bossDeath.wav");
                AssetManager::playMusic("assets/audio/music/bgMusic.ogg");
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
                    player->healFromPotion(20);
                }
                bonus->destroy();
            }
        }

        // PRZESZKODY
        auto obstacle = dynamic_cast<Obstacle*>(obj.get());
        if(obstacle) {
            sf::FloatRect texBounds = obstacle->getBounds();

            //Hitbox to teraz zaledwie dół (np. sam pień drzewa).
            sf::FloatRect obsBounds(texBounds.left + texBounds.width*0.2f,
                                    texBounds.top + texBounds.height*0.6f,
                                    texBounds.width*0.6f,
                                    texBounds.height*0.3f);

            float ocx = obsBounds.left + obsBounds.width/2.f;
            float ocy = obsBounds.top + obsBounds.height/2.f;

            //KOLIZJA GRACZ PRZESZKODA
            sf::FloatRect intersect;
            if(player->getBounds().intersects(obsBounds, intersect)) {
                sf::Vector2f newPos = player->getPosition();
                float pcx = player->getBounds().left + player->getBounds().width/2.f;
                float pcy = player->getBounds().top + player->getBounds().height/2.f;

                if(intersect.width < intersect.height) {
                    if(pcx < ocx) newPos.x -= intersect.width;
                    else newPos.x += intersect.width;
                } else {
                    if(pcy < ocy) newPos.y -= intersect.height;
                    else newPos.y += intersect.height;
                }
                player->setPosition(newPos);
            }
            // Wróg - Przeszkoda
            for(auto& otherObj : gameObjects) {
                if(!otherObj->isActive()) continue;
                auto otherEnemy = dynamic_cast<Enemy*>(otherObj.get());
                if(otherEnemy && !otherEnemy->getIsGhost()) {
                    // Sztywne wypychanie gwarantuje płynne opływanie krawędzi bez trzęsienia
                    if(otherEnemy->getBounds().intersects(obsBounds, intersect)) {
                        sf::Vector2f newPos = otherEnemy->getPosition();
                        float cx = otherEnemy->getBounds().left + otherEnemy->getBounds().width/2.f;
                        float cy = otherEnemy->getBounds().top + otherEnemy->getBounds().height/2.f;

                        if(intersect.width < intersect.height) {
                            if(cx < ocx) newPos.x -= intersect.width;
                            else newPos.x += intersect.width;
                        } else {
                            if(cy < ocy) newPos.y -= intersect.height;
                            else newPos.y += intersect.height;
                        }
                        otherEnemy->setPosition(newPos); // Aplikuje korektę od razu
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