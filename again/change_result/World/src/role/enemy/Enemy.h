#pragma once

#include <string>

class Enemy {
private:
    std::string id;
    std::string name;
    std::string description;
    std::string element;      // "mechanical" / "fire" / "water"
    int hp;
    int maxHp;
    int attack;
    int baseAttack;            // 基础攻击力（不含buff叠加）
    int defense;
    int mp;
    int maxMp;
    int expReward;
    int goldReward;
    int diamondReward;
    bool alive;
    bool hasTriggeredFirstHit;    // 首次受伤标记
    int lastTriggeredThreshold;   // 上次触发阈值
    int attackBoost;              // 攻击力叠加值（蝙蝠大招用）

public:
    Enemy(const std::string& id, const std::string& name, const std::string& description,
          const std::string& element, int maxHp, int attack, int defense,
          int expReward, int goldReward, int diamondReward = 0);

    // ---- getter ----
    std::string getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getElement() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;          // 返回基础攻击力+叠加值
    int getBaseAttack() const;      // 返回基础攻击力（无叠加）
    int getDefense() const;
    int getMp() const;
    int getMaxMp() const;
    int getExpReward() const;
    int getGoldReward() const;
    int getDiamondReward() const;
    bool isAlive() const;
    void setMp(int value);

    // ---- setter ----
    void setHp(int value);          // 设置HP，自动更新alive状态

    // ---- 战斗 ----
    void takeDamage(int damage);
    void addMp(int amount);
    void resetMp();
    bool isMpFull() const;
    void addAttackBoost(int percent);  // 叠加攻击力百分比
    int getAttackBoost() const;
    void resetState();              // 重置所有状态

    // ---- AI触发检查 ----
    bool checkFirstHitTrigger();
    bool checkThresholdTrigger(int newHp);
    int getLastTriggeredThreshold() const;
    void setLastTriggeredThreshold(int value);
    void resetTriggers();               // 重置触发标记（用于新战斗）

    // ---- 信息 ----
    void displayInfo() const;
};