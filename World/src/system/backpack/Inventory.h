#pragma once

#include <string>
#include <map>
#include "item/ItemTemplate.h"
#include "Combatant.h"

class Inventory {
private:
    std::map<std::string, int> items;   // 物品ID → 数量

public:
    Inventory() = default;

    // 物品管理
    void addItem(const std::string& itemId, int count = 1);
    bool removeItem(const std::string& itemId, int count = 1);
    int  getCount(const std::string& itemId) const;
    bool hasItem(const std::string& itemId, int count = 1) const;

    // 展示
    void displayAll() const;
    void displayCategory(const std::string& category) const;

    // 使用物品（作用于目标）
    bool useItem(const std::string& itemId, Combatant* target);

    // 出售
    bool sellItem(const std::string& itemId, int count, class MainCharacter* player);
    int  getSellPrice(const std::string& itemId) const;

    // 其它
    const std::map<std::string, int>& getAllItems() const { return items; }
    void clear();
    int  getTotalCount() const;
};

