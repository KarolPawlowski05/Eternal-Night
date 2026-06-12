#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

// Konfiguracja okna gry
namespace GameConfig {
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr const char* WINDOW_TITLE = "Eternal Night";
    constexpr float TARGET_FPS = 60.f;
}

// Konfiguracja gracza
namespace PlayerConfig {
    constexpr float BASE_SPEED = 250.f;
    constexpr float DASH_SPEED = 650.f;
    constexpr float DASH_COOLDOWN = 2.0f;
    constexpr float DASH_DURATION = 0.25f;

    constexpr int BASE_HP = 100;
    constexpr float ATTACK_COOLDOWN = 0.7f;
    constexpr float ATTACK_DURATION = 0.3f;
    constexpr float SPECIAL_COOLDOWN = 5.0f;

    constexpr int INITIAL_XP_THRESHOLD = 40;
    constexpr float XP_THRESHOLD_MULITPLAYER = 1.30f;

    constexpr int FRAME_COUNT = 8;
    constexpr int FRAME_WIDTH = 192;
    constexpr int FRAME_HEIGHT = 160;
    constexpr float FRAME_SPEED = 0.1f;
}

// Konfiguracja wroga
namespace EnemyConfig {
    struct EnemyStats {
        float baseSpeed;
        int baseHp;
        sf::Color color;        // Kolor zapasowego kształty (gdy nie ma tekstury)
        float size;             // Rozmiar hitboxa
        const char* texturePath;// Ścieżka do pliku sprite'a
    };

    extern std::map<int, EnemyStats> ENEMY_STATS;
}

// Konfiguracja UI
namespace UIConfig {
    constexpr float XP_BAR_HEIGHT = 22.f;
    constexpr float HUD_STATS_X = 10.f;
    constexpr float HUD_STATS_Y = 58.f;
}

namespace TexturePaths {

// --- Bossowie ---
constexpr const char* BOSS_ROOT_STRANGLER   = "assets/enemies/bosses/rootStrangler.png";
constexpr const char* BOSS_STORM_WRAITH     = "assets/enemies/bosses/stormWraith.png";
constexpr const char* BOSS_VAMPIRE_PRINCE   = "assets/enemies/bosses/vampirePrince.png";
constexpr const char* BOSS_CHAOS_GOLEM      = "assets/enemies/bosses/chaosGolem.png";

// --- Pociski bossów (jeden sprite na typ bossa) ---
constexpr const char* PROJ_ROOT_STRANGLER   = "assets/projectiles/projRoot.png";
constexpr const char* PROJ_STORM_WRAITH     = "assets/projectiles/projLightning.png";
constexpr const char* PROJ_VAMPIRE_PRINCE   = "assets/projectiles/projBlood.png";
constexpr const char* PROJ_CHAOS_GOLEM      = "assets/projectiles/projStone.png";

// --- Pociski zwykłych wrogów ---
constexpr const char* PROJ_OGRE_ROCK        = "assets/projectiles/projStone.png";

// --- Collectibles ---
constexpr const char* BONUS_POTION          = "assets/map/collectibles/potion.png";
}

#endif