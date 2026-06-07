#include "Player.h"
#include "Config.h"

AnimationData::AnimationData()
    : currentFrame(0), frameTimer(0.f), frameSpeed(PlayerConfig::FRAME_SPEED),
    animState(0), animDir(0), attackAnimFrame(0), attackAnimTimer(0.f),
    showAttackAnim(false), attackCombo(0), texturesLoaded(false) {}

CombatData::CombatData()
    : attackSize(80.f, 60.f), attackCooldown(PlayerConfig::ATTACK_COOLDOWN),
    attackTimer(0.f), attackDuration(PlayerConfig::ATTACK_DURATION),
    durationTimer(0.f), isAttacking(false), prevLMBPressed(false),
    specialCooldown(PlayerConfig::SPECIAL_COOLDOWN), specialTimer(PlayerConfig::SPECIAL_COOLDOWN),
    wantsToShootSpecial(false) {}

MovementData::MovementData()
    : speed(PlayerConfig::BASE_SPEED), lastDirection(0.f, -1.f),
    dashSpeed(PlayerConfig::DASH_SPEED), dashCooldown(PlayerConfig::DASH_COOLDOWN),
    dashTimer(PlayerConfig::DASH_COOLDOWN), dashDuration(PlayerConfig::DASH_DURATION),
    dashDurationTimer(0.f), isDashing(false), footstepTimer(0.f), footstepInterval(0.41f), footstepVariant(0) {}

HealthData::HealthData()
    : hp(PlayerConfig::BASE_HP), maxHp(PlayerConfig::BASE_HP),
    invincibilityTimer(0.f) {}

ProgressionData::ProgressionData()
    : xp(0), maxXp(PlayerConfig::INITIAL_XP_THRESHOLD), level(1),
    enemiesKilled(0) {}

StatsData::StatsData()
    : damageBonus(0), armor(0), critChance(0.05f),
    vampirismChance(0.0f), pickupRadiusBonus(0.f),
    dodgeChance(0.0f), hpRegenRate(0), hpRegenTimer(0.f) {}

PassiveWeaponsData::PassiveWeaponsData()
    : hasFireAura(false), fireAuraRadius(100.f),
    hasOrbitingSword(false), orbitAngle(0.f), orbitSpeed(200.f),
    hasWand(false), wandCooldown(3.0f), wandTimer(3.0f),
    wandProjectiles(1), wandDamageBonus(0) {}