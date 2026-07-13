#pragma once

#include <string>
#include <vector>

// ============================================================
// 装备结构
// ============================================================
struct Equipment {
    std::string id;          // 装备ID
    std::string name;        // 装备名称
    std::string type;        // "weapon" / "book" / "shield"
    int level;               // 1=初级, 2=中级, 3=高级
    double attackBonus;      // 攻击加成百分比
    double defenseBonus;     // 防御加成百分比
    double mpBonus;          // 蓝条加成百分比
    int hpRestore;           // 生命恢复固定值（仅魔法书）
};

// ============================================================
// 技能结构
// ============================================================
struct Skill {
    std::string name;
    int mpCost;
    std::string target;      // "enemy_single" / "enemy_all" / "self" / "ally_all"
    std::string effect;      // "attack" / "heal_percent" / "full_heal" / "restore_mp" / "buff_crit"
    int effectValue;
    std::string description;
    std::string subEffect;
    int subEffectValue;
    std::string subTarget;
};

class Combatant {
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int baseAttack;
    int baseDefense;
    int baseMaxMp;
    int attack;
    int defense;
    int mp;
    int maxMp;
    std::string college;
    double critRate;
    std::vector<Skill> skills;
    std::vector<Equipment> equipped;  // 已装备列表（最多2件）

public:
    Combatant(const std::string& name, const std::string& college);

    // ---- getter ----
    std::string getName() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;
    int getMp() const;
    int getMaxMp() const;
    std::string getCollege() const;
    double getCritRate() const;
    int getBaseAttack() const;
    int getBaseDefense() const;
    int getBaseMaxMp() const;

    // ---- setter ----
    void setName(const std::string& name);
    void setLevel(int level);
    void setHp(int hp);
    void setMaxHp(int maxHp);
    void setAttack(int attack);
    void setDefense(int defense);
    void setMp(int mp);
    void setMaxMp(int maxMp);
    void setCollege(const std::string& college);
    void setCritRate(double critRate);

    // ---- 核心方法 ----
    void levelUp();          // 消耗升级石，等级×5个
    int takeDamage(int damage);
    bool isAlive() const;
    void heal(int amount);
    void restoreMp(int amount);
    void displayInfo() const;

    // ---- 技能系统 ----
    void initSkills();
    void displaySkills() const;
    Skill* getSkill(int index);
    int getSkillCount() const;
    void resetMp();
    bool hasEnoughMp(int cost) const;
    bool consumeMp(int cost);
    Skill* getNormalAttack();
    Skill* getSkill1();
    Skill* getSkill2();
    Skill* getUltimate();

    // ---- 装备系统 ----
    bool equipItem(const Equipment& eq);       // 穿戴装备
    bool unequipItem(int slot);                // 卸下装备（0或1）
    void recalcStats();                        // 重新计算属性
    void displayEquipped() const;              // 显示已装备
    int getEquippedCount() const;              // 已装备数量
    std::vector<Equipment> getEquipped() const;
    int getUpgradeStoneCost() const;           // 升级所需升级石 = 等级 × 5
};