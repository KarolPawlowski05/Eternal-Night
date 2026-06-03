#include "Config.h"

std::map<int, EnemyConfig::EnemyStats> EnemyConfig::ENEMY_STATS = {
    {0, {55.f, 50, sf::Color::Magenta, 30.f}},      // Drowner
    {1, {75.f, 35, sf::Color::White, 30.f}},        // Noonwraith
    {2, {30.f, 200, sf::Color::Green, 45.f}},       // Rock troll
    {3, {150.f, 5, sf::Color(255, 165, 0), 20.f}},  // Rotfiend
    {4, {60.f, 80, sf::Color(80, 80, 80), 30.f}},   // Foglet
    {5, {65.f, 80, sf::Color(200, 0, 0), 36.f}},    // Fleder
    {6, {50.f, 50, sf::Color::Cyan, 30.f}}          // Wraith
};