#pragma once

#include <vector>
#include <string>
#include "role/character/MainCharacter.h"
#include "system/backpack/Inventory.h"

// 前置声明，避免循环包含
class Combatant;

struct ShopItem {
    std::string id;
    std::string name;
    std::string category;      // "food" / "potion" / "weapon" / "material" / "special"
    int price;
    std::string currency;      // "gold" / "diamond"
    std::string description;
    int dailyLimit;            // 每日限购数量，0表示不限
    int dailySold;             // 今日已售数量
    int level;                 // 装备等级 1-3
    double attackBonus;        // 攻击加成百分比
    double defenseBonus;       // 防御加成百分比
    double mpBonus;            // 蓝条加成百分比
    int hpRestore;             // 生命恢复固定值（仅魔法书）
};

class Shop {
private:
    std::vector<ShopItem> shopItems;

public:
    Shop();

    void loadShopItems();
    void displayItems() const;
    
    bool buyItem(int index, int count, MainCharacter* player, Inventory* inventory,
                 std::vector<Combatant*>& fighters);
    
    const ShopItem* findItemById(const std::string& id) const;
    const std::vector<ShopItem>& getShopItems() const;
    void resetDailyLimits();
};