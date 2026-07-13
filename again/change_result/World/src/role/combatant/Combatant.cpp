#include "Combatant.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Combatant::Combatant(const std::string& name, const std::string& college)
    : name(name), college(college), level(1), critRate(0.05) {
    if (college == "能源学院") {
        maxHp = 1000; baseAttack = 200; baseDefense = 200; baseMaxMp = 100;
    } else if (college == "医学院") {
        maxHp = 1100; baseAttack = 100; baseDefense = 200; baseMaxMp = 100;
    } else if (college == "机械学院") {
        maxHp = 900; baseAttack = 300; baseDefense = 200; baseMaxMp = 100;
    } else {
        maxHp = 1000; baseAttack = 200; baseDefense = 200; baseMaxMp = 100;
    }
    attack = baseAttack;
    defense = baseDefense;
    maxMp = baseMaxMp;
    hp = maxHp;
    mp = 0;
    initSkills();
}

// ---- getter 实现 ----
std::string Combatant::getName() const { return name; }
int Combatant::getLevel() const { return level; }
int Combatant::getHp() const { return hp; }
int Combatant::getMaxHp() const { return maxHp; }
int Combatant::getAttack() const { return attack; }
int Combatant::getDefense() const { return defense; }
int Combatant::getMp() const { return mp; }
int Combatant::getMaxMp() const { return maxMp; }
std::string Combatant::getCollege() const { return college; }
double Combatant::getCritRate() const { return critRate; }
int Combatant::getBaseAttack() const { return baseAttack; }
int Combatant::getBaseDefense() const { return baseDefense; }
int Combatant::getBaseMaxMp() const { return baseMaxMp; }

// ---- setter 实现 ----
void Combatant::setName(const std::string& n) { name = n; }
void Combatant::setLevel(int l) { level = std::max(1, l); }
void Combatant::setHp(int h) { hp = std::max(0, std::min(maxHp, h)); }
void Combatant::setMaxHp(int m) { maxHp = std::max(1, m); if (hp > maxHp) hp = maxHp; }
void Combatant::setAttack(int a) { attack = std::max(0, a); }
void Combatant::setDefense(int d) { defense = std::max(0, d); }
void Combatant::setMp(int m) { mp = std::max(0, std::min(maxMp, m)); }
void Combatant::setMaxMp(int m) { maxMp = std::max(1, m); if (mp > maxMp) mp = maxMp; }
void Combatant::setCollege(const std::string& c) { college = c; }
void Combatant::setCritRate(double cr) { critRate = std::max(0.0, std::min(1.0, cr)); }

int Combatant::getUpgradeStoneCost() const {
    return level * 5;  // 等级 × 5
}

// ============================================================
// levelUp — 使用升级石升级（不提高能量值上限）
// ============================================================
void Combatant::levelUp() {
    ++level;

    // 保留升级前的生命/蓝量比例
    double hpRatio = (maxHp > 0) ? static_cast<double>(hp) / maxHp : 1.0;
    double mpRatio = (maxMp > 0) ? static_cast<double>(mp) / maxMp : 1.0;

    // 三个属性提升 20%（不提高能量值上限）
    attack  = static_cast<int>(attack  * 1.2);
    defense = static_cast<int>(defense * 1.2);
    maxHp   = static_cast<int>(maxHp   * 1.2);
    // maxMp 保持不变（不提高能量值上限）

    hp = static_cast<int>(maxHp * hpRatio);
    mp = static_cast<int>(maxMp * mpRatio);

    std::cout << "🎉 " << name << " 升到 " << level << " 级！"
              << " 攻击 " << attack << " | 防御 " << defense
              << " | 生命 " << maxHp << " | 蓝量 " << maxMp << std::endl;
}

int Combatant::takeDamage(int rawDamage) {
    if (rawDamage <= 0) return 0;
    int reduction = (int)(defense * 0.4);
    int actualDamage = rawDamage - reduction;
    int minDamage = std::max(1, (int)(rawDamage * 0.1));
    if (actualDamage < minDamage) actualDamage = minDamage;
    if (actualDamage < 0) actualDamage = 1;
    hp -= actualDamage;
    if (hp < 0) hp = 0;
    return actualDamage;
}

bool Combatant::isAlive() const { return hp > 0; }
void Combatant::heal(int amount) {
    if (amount <= 0) return;
    int before = hp;
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}
void Combatant::restoreMp(int amount) {
    if (amount <= 0) return;
    mp += amount;
    if (mp > maxMp) mp = maxMp;
}

void Combatant::displayInfo() const {
    std::cout << "\n========== " << name << " ==========" << std::endl;
    std::cout << "学院：" << college << "  等级：" << level << std::endl;
    std::cout << "HP: " << hp << "/" << maxHp << "  MP: " << mp << "/" << maxMp << std::endl;
    std::cout << "攻击：" << attack << "（基础 " << baseAttack << "）" << std::endl;
    std::cout << "防御：" << defense << "（基础 " << baseDefense << "）" << std::endl;
    std::cout << "暴击率：" << (critRate * 100) << "%" << std::endl;
    displayEquipped();
    std::cout << "==============================\n" << std::endl;
}

// ============================================================
// 装备系统
// ============================================================
bool Combatant::equipItem(const Equipment& eq) {
    if (equipped.size() >= 2) {
        std::cout << "装备已满（最多2件）" << std::endl;
        return false;
    }
    // 检查是否已穿戴同类型装备
    for (const auto& e : equipped) {
        if (e.type == eq.type) {
            std::cout << "已穿戴同类型装备，请先卸下" << std::endl;
            return false;
        }
    }
    equipped.push_back(eq);
    recalcStats();
    return true;
}

bool Combatant::unequipItem(int slot) {
    if (slot < 0 || slot >= (int)equipped.size()) return false;
    equipped.erase(equipped.begin() + slot);
    recalcStats();
    return true;
}

void Combatant::recalcStats() {
    attack = baseAttack;
    defense = baseDefense;
    maxMp = baseMaxMp;
    for (const auto& eq : equipped) {
        attack += (int)(baseAttack * eq.attackBonus);
        defense += (int)(baseDefense * eq.defenseBonus);
        maxMp += (int)(baseMaxMp * eq.mpBonus);
        // 魔法书额外生命恢复在战斗中使用
    }
    if (hp > maxHp) hp = maxHp;
    if (mp > maxMp) mp = maxMp;
}

void Combatant::displayEquipped() const {
    if (equipped.empty()) {
        std::cout << "装备：无" << std::endl;
        return;
    }
    std::cout << "装备：" << std::endl;
    for (size_t i = 0; i < equipped.size(); ++i) {
        const auto& eq = equipped[i];
        std::cout << "  [" << i << "] " << eq.name;
        if (eq.attackBonus > 0) std::cout << " 攻击+" << (int)(eq.attackBonus * 100) << "%";
        if (eq.defenseBonus > 0) std::cout << " 防御+" << (int)(eq.defenseBonus * 100) << "%";
        if (eq.mpBonus > 0) std::cout << " 蓝条+" << (int)(eq.mpBonus * 100) << "%";
        if (eq.hpRestore > 0) std::cout << " 生命恢复+" << eq.hpRestore;
        std::cout << std::endl;
    }
}

int Combatant::getEquippedCount() const { return (int)equipped.size(); }
std::vector<Equipment> Combatant::getEquipped() const { return equipped; }

// ============================================================
// 技能系统（同之前完整版本，此处省略重复代码）
// ============================================================
void Combatant::initSkills() {
    skills.clear();
    skills.push_back({"普攻", 0, "enemy_single", "attack", 100,
                      "对敌方单体造成100%伤害", "", 0, ""});
    if (college == "能源学院") {
        if (name == "能源学者") {
            skills.push_back({"能量汲取", 50, "enemy_single", "attack", 100, "对敌方单体100%伤害，恢复20蓝", "restore_mp", 20, "self"});
            skills.push_back({"自我修复", 50, "self", "heal_percent", 10, "恢复10%HP，恢复20蓝", "restore_mp", 20, "self"});
            skills.push_back({"群体充能", 100, "ally_all", "restore_mp", 40, "群体恢复40蓝", "", 0, ""});
        } else if (name == "能源新星") {
            skills.push_back({"能量汲取", 50, "enemy_single", "attack", 100, "对敌方单体100%伤害，恢复20蓝", "restore_mp", 20, "self"});
            skills.push_back({"自我再生", 50, "self", "heal_percent", 20, "恢复20%HP", "", 0, ""});
            skills.push_back({"群体充能", 100, "ally_all", "restore_mp", 40, "群体恢复40蓝", "", 0, ""});
        } else if (name == "能源守卫") {
            skills.push_back({"能量冲击", 50, "enemy_single", "attack", 110, "对敌方单体110%伤害", "", 0, ""});
            skills.push_back({"能量震荡", 50, "enemy_all", "attack", 50, "对敌方群体50%伤害", "", 0, ""});
            skills.push_back({"群体充能打击", 100, "enemy_single", "attack", 100, "群体恢复30蓝+单体100%伤害", "restore_mp", 30, "ally_all"});
        }
    } else if (college == "医学院") {
        if (name == "医者") {
            skills.push_back({"治愈术", 50, "self", "heal_percent", 30, "恢复30%HP", "", 0, ""});
            skills.push_back({"治愈打击", 50, "enemy_single", "attack", 100, "对敌方单体100%伤害", "", 0, ""});
            skills.push_back({"群体治愈", 100, "ally_all", "heal_percent", 30, "群体恢复30%HP", "", 0, ""});
        } else if (name == "急救员") {
            skills.push_back({"急救术", 50, "self", "heal_percent", 20, "恢复20%HP", "", 0, ""});
            skills.push_back({"急救打击", 50, "enemy_single", "attack", 120, "对敌方单体120%伤害", "", 0, ""});
            skills.push_back({"完全治愈", 100, "self", "full_heal", 100, "完全恢复HP", "", 0, ""});
        } else if (name == "药剂师") {
            skills.push_back({"药剂恢复", 50, "self", "heal_percent", 20, "恢复20%HP", "", 0, ""});
            skills.push_back({"药剂喷洒", 50, "enemy_all", "attack", 50, "对敌方群体50%伤害", "", 0, ""});
            skills.push_back({"群体治愈", 100, "ally_all", "heal_percent", 30, "群体恢复30%HP", "", 0, ""});
        }
    } else if (college == "机械学院") {
        if (name == "机械师") {
            skills.push_back({"爆发打击", 50, "enemy_single", "attack", 120, "对敌方单体120%伤害", "", 0, ""});
            skills.push_back({"精准打击", 50, "enemy_single", "attack", 100, "暴击+25%，对敌方单体100%伤害", "buff_crit", 25, "self"});
            skills.push_back({"火力压制", 100, "enemy_all", "attack", 110, "对敌方群体110%伤害", "", 0, ""});
        } else if (name == "机械先锋") {
            skills.push_back({"狂暴打击", 50, "enemy_single", "attack", 100, "暴击+10%，对敌方单体100%伤害", "buff_crit", 10, "self"});
            skills.push_back({"致命打击", 50, "enemy_single", "attack", 100, "暴击+15%，对敌方单体100%伤害", "buff_crit", 15, "self"});
            skills.push_back({"终极爆发", 100, "enemy_single", "attack", 150, "对敌方单体150%伤害", "", 0, ""});
        } else if (name == "机械卫士") {
            skills.push_back({"机械修复", 50, "enemy_single", "attack", 100, "恢复10%HP+对敌方单体100%伤害", "heal_percent", 10, "self"});
            skills.push_back({"机械扫射", 50, "enemy_all", "attack", 50, "对敌方群体50%伤害", "", 0, ""});
            skills.push_back({"机械风暴", 100, "enemy_all", "attack", 100, "对敌方群体100%伤害", "", 0, ""});
        }
    }
}

void Combatant::displaySkills() const {
    std::cout << "\n========== " << name << " 技能 ==========" << std::endl;
    for (size_t i = 0; i < skills.size(); ++i) {
        const auto& s = skills[i];
        std::cout << "[" << i << "] " << s.name << " | 消耗:" << s.mpCost << "蓝 | " << s.description << std::endl;
    }
    std::cout << "====================================\n" << std::endl;
}

Skill* Combatant::getSkill(int index) {
    if (index < 0 || index >= (int)skills.size()) return nullptr;
    return &skills[index];
}
int Combatant::getSkillCount() const { return (int)skills.size(); }
void Combatant::resetMp() { mp = 0; }
bool Combatant::hasEnoughMp(int cost) const { return mp >= cost; }
bool Combatant::consumeMp(int cost) {
    if (!hasEnoughMp(cost)) return false;
    mp -= cost;
    return true;
}
Skill* Combatant::getNormalAttack() { return getSkill(0); }
Skill* Combatant::getSkill1() { return getSkill(1); }
Skill* Combatant::getSkill2() { return getSkill(2); }
Skill* Combatant::getUltimate() { return getSkill(3); }