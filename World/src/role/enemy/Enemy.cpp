#include "Enemy.h"
#include <iostream>
#include <algorithm>

Enemy::Enemy(const std::string& id, const std::string& name, const std::string& description,
             const std::string& element, int maxHp, int attack, int defense,
             int expReward, int goldReward, int diamondReward)
    : id(id), name(name), description(description), element(element),
      maxHp(maxHp), attack(attack), baseAttack(attack), defense(defense),
      maxMp(100), expReward(expReward), goldReward(goldReward),
      diamondReward(diamondReward), alive(true),
      hasTriggeredFirstHit(false), lastTriggeredThreshold(100),
      attackBoost(0) {
    hp = maxHp;
    mp = 0;
}

std::string Enemy::getId() const { return id; }
std::string Enemy::getName() const { return name; }
std::string Enemy::getDescription() const { return description; }
std::string Enemy::getElement() const { return element; }
int Enemy::getHp() const { return hp; }
int Enemy::getMaxHp() const { return maxHp; }
int Enemy::getAttack() const { return attack + attackBoost; }
int Enemy::getBaseAttack() const { return attack; }
int Enemy::getDefense() const { return defense; }
int Enemy::getMp() const { return mp; }
int Enemy::getMaxMp() const { return maxMp; }
int Enemy::getExpReward() const { return expReward; }
int Enemy::getGoldReward() const { return goldReward; }
int Enemy::getDiamondReward() const { return diamondReward; }
bool Enemy::isAlive() const { return alive; }
void Enemy::setMp(int value) {
    mp = std::max(0, std::min(maxMp, value));
}
// ============================================================
// setHp — 设置HP，自动更新alive状态
// ============================================================
void Enemy::setHp(int value) {
    hp = std::max(0, std::min(maxHp, value));
    alive = hp > 0;
}

// ============================================================
// takeDamage — 受到伤害，被攻击时蓝条充能+20
// ============================================================
void Enemy::takeDamage(int damage) {
    if (damage <= 0 || !alive) return;
    hp -= damage;
    if (hp < 0) hp = 0;
    // 被攻击时蓝条充能 +20
    addMp(20);
    if (hp <= 0) {
        alive = false;
        std::cout << name << " 被击败！" << std::endl;
    }
}

// ============================================================
// addMp / resetMp / isMpFull
// ============================================================
void Enemy::addMp(int amount) {
    mp += amount;
    if (mp > maxMp) mp = maxMp;
}

void Enemy::resetMp() { mp = 0; }
bool Enemy::isMpFull() const { return mp >= maxMp; }

// ============================================================
// addAttackBoost — 叠加攻击力（蝙蝠大招用）
// ============================================================
void Enemy::addAttackBoost(int percent) {
    int boost = baseAttack * percent / 100;
    attackBoost += boost;
    std::cout << name << " 攻击力+" << percent << "%（当前总攻击力: " << getAttack() << "）" << std::endl;
}

int Enemy::getAttackBoost() const { return attackBoost; }

// ============================================================
// resetState — 重置所有状态（用于新战斗）
// ============================================================
void Enemy::resetState() {
    hp = maxHp;
    mp = 0;
    alive = true;
    hasTriggeredFirstHit = false;
    lastTriggeredThreshold = 100;
    attackBoost = 0;
}

// ============================================================
// resetTriggers — 重置触发标记
// ============================================================
void Enemy::resetTriggers() {
    hasTriggeredFirstHit = false;
    lastTriggeredThreshold = 100;
}

// ============================================================
// checkFirstHitTrigger — 检查首次受伤触发
// ============================================================
bool Enemy::checkFirstHitTrigger() {
    if (hasTriggeredFirstHit) return false;
    hasTriggeredFirstHit = true;
    return true;
}

// ============================================================
// checkThresholdTrigger — 检查血量阈值触发
//   根据 newHp 计算当前血量百分比，找到对应的阈值
//   单次受击只触发一次（取最低阈值）
// ============================================================
bool Enemy::checkThresholdTrigger(int newHp) {
    int currentPercent = (int)((double)newHp / maxHp * 100);
    // 计算当前阈值（10% 20% 30% ... 90%）
    int threshold = ((currentPercent + 9) / 10) * 10;
    if (threshold >= 100) threshold = 90;
    if (threshold != lastTriggeredThreshold && currentPercent <= threshold) {
        lastTriggeredThreshold = threshold;
        return true;
    }
    return false;
}

int Enemy::getLastTriggeredThreshold() const { return lastTriggeredThreshold; }
void Enemy::setLastTriggeredThreshold(int value) { lastTriggeredThreshold = value; }

// ============================================================
// displayInfo — 显示敌人信息
// ============================================================
void Enemy::displayInfo() const {
    std::cout << name << " [" << element << "] 生命:" << hp << "/" << maxHp
              << " 攻击:" << attack;
    // 只有防御力 > 0 时才显示
    if (defense > 0) {
        std::cout << " 防御:" << defense;
    }
    std::cout << " 蓝条:" << mp << "/" << maxMp
              << (alive ? " [存活]" : " [已败]") << std::endl;
}