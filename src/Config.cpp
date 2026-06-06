#include "Config.h"

std::map<int, EnemyConfig::EnemyStats> EnemyConfig::ENEMY_STATS = {
    {0, {55.f, 50, sf::Color::Magenta, 30.f, "assets/enemies/basic/drowner.png"}},      // Drowner
    {1, {75.f, 35, sf::Color::White, 30.f, "assets/enemies/basic/noonWraith.png"}},        // Noonwraith
    {2, {30.f, 200, sf::Color::Green, 45.f, "assets/enemies/basic/rockTroll.png"}},       // Rock troll
    {3, {150.f, 5, sf::Color(255, 165, 0), 20.f, "assets/enemies/basic/rotfiend.png"}},  // Rotfiend
    {4, {60.f, 80, sf::Color(80, 80, 80), 30.f, "assets/enemies/basic/foglet.png"}},   // Foglet
    {5, {65.f, 80, sf::Color(200, 0, 0), 36.f, "assets/enemies/basic/fleder.png"}},    // Fleder
    {6, {50.f, 50, sf::Color::Cyan, 30.f, "assets/enemies/basic/wraith.png"}}          // Wraith
};