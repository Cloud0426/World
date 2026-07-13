#include "Inventory.h"
#include "role/character/MainCharacter.h"
#include "role/combatant/Combatant.h"  // 包含 Equipment 结构体
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

// ============================================================
// 添加物品
// ============================================================
void Inventory::addItem(const std::string& itemId, int count) {
    if (count <= 0) return;
    items[itemId] += count;
}

// ============================================================
// 移除物品
// ============================================================
bool Inventory::removeItem(const std::string& itemId, int count) {
    if (count <= 0) return true;
    auto it = items.find(itemId);
    if (it == items.end() || it->second < count) {
        return false;
    }
    it->second -= count;
    if (it->second <= 0) {
        items.erase(it);
    }
    return true;
}

// ============================================================
// 获取某物品数量
// ============================================================
int Inventory::getCount(const std::string& itemId) const {
    auto it = items.find(itemId);
    return (it != items.end()) ? it->second : 0;
}

// ============================================================
// 检查是否有足够数量
// ============================================================
bool Inventory::hasItem(const std::string& itemId, int count) const {
    return getCount(itemId) >= count;
}

// ============================================================
// 物品ID → 中文名称 映射表（回退方案）
// ============================================================
static std::string getItemChineseName(const std::string& itemId) {
    static const std::map<std::string, std::string> nameMap = {
        {"upgrade_stone",   "升级石"},
        {"weapon_shield_1", "初级护盾"},
        {"weapon_shield_2", "中级护盾"},
        {"weapon_shield_3", "高级护盾"},
        {"weapon_sword_1",  "初级剑"},
        {"weapon_sword_2",  "中级剑"},
        {"weapon_sword_3",  "高级剑"},
        {"weapon_book_1",   "初级魔法书"},
        {"weapon_book_2",   "中级魔法书"},
        {"weapon_book_3",   "高级魔法书"},
        {"recruit_card",    "招募卡"},
        {"food_001",        "罐头"},
        {"food_002",        "压缩饼干"},
        {"food_003",        "瓶装水"},
        {"potion_001",      "急救包"},
        {"potion_002",      "外伤药包"},
        {"potion_003",      "止痛片"},
    };
    auto it = nameMap.find(itemId);
    if (it != nameMap.end()) return it->second;
    return itemId;
}

// ============================================================
// 显示背包所有物品
// ============================================================
void Inventory::displayAll() const {
    auto allTemplates = ItemTemplate::loadAllTemplates();

    std::cout << "\n========== 背包 ==========" << std::endl;
    if (items.empty()) {
        std::cout << "（背包为空）" << std::endl;
        std::cout << "==========================\n" << std::endl;
        return;
    }

    // 把物品放入 vector 排序，保证稳定输出
    std::vector<std::pair<std::string, int>> sortedItems(items.begin(), items.end());
    int idx = 1;
    for (const auto& pair : sortedItems) {
        const std::string& itemId = pair.first;
        int count = pair.second;

        // 优先从模板获取名称，其次用映射表
        std::string name = itemId;
        for (const auto& tmpl : allTemplates) {
            if (tmpl.getId() == itemId) {
                name = tmpl.getName();
                break;
            }
        }
        if (name == itemId) {
            name = getItemChineseName(itemId);
        }

        std::cout << "[" << idx << "] " << name << " x" << count << std::endl;
        ++idx;
    }
    std::cout << "==========================\n" << std::endl;
}

// ============================================================
// 显示某类别物品
// ============================================================
void Inventory::displayCategory(const std::string& category) const {
    auto allTemplates = ItemTemplate::loadAllTemplates();

    std::string categoryLabel;
    if (category == "food")        categoryLabel = "食物类";
    else if (category == "potion") categoryLabel = "药品类";
    else if (category == "weapon") categoryLabel = "装备类";
    else                           categoryLabel = category;

    std::cout << "\n========== " << categoryLabel << " ==========" << std::endl;

    bool hasAny = false;
    for (const auto& pair : items) {
        const std::string& itemId = pair.first;
        int count = pair.second;

        for (const auto& tmpl : allTemplates) {
            if (tmpl.getId() == itemId && tmpl.getCategory() == category) {
                std::cout << tmpl.getName() << " x" << count
                          << "  - " << tmpl.getDescription() << std::endl;
                hasAny = true;
                break;
            }
        }
    }

    if (!hasAny) {
        std::cout << "（该类无物品）" << std::endl;
    }
    std::cout << "============================\n" << std::endl;
}

// ============================================================
// 使用物品（作用于目标）
// ============================================================
bool Inventory::useItem(const std::string& itemId, Combatant* target) {
    if (!target) return false;
    if (!hasItem(itemId, 1)) return false;

    // 查找物品模板
    auto allTemplates = ItemTemplate::loadAllTemplates();
    const ItemTemplate* tmpl = nullptr;
    for (const auto& t : allTemplates) {
        if (t.getId() == itemId) {
            tmpl = &t;
            break;
        }
    }
    if (!tmpl) return false;

    const std::string& category = tmpl->getCategory();
    const std::string& effectType = tmpl->getEffectType();
    int effectValue = tmpl->getEffectValue();

    // 根据类别和效果类型执行
    if (category == "food") {
        // 食物：恢复体力（restoreStamina）
        if (effectType == "restoreStamina") {
            // Combatant 若有体力相关方法可在此调用
            std::cout << "使用 [" << tmpl->getName() << "] "
                      << "恢复体力 " << effectValue << " 点。" << std::endl;
        }
    }
    else if (category == "potion") {
        // 药品：恢复生命（restoreHp）
        if (effectType == "restoreHp") {
            target->heal(effectValue);
            std::cout << "使用 [" << tmpl->getName() << "] "
                      << "恢复生命 " << effectValue << " 点。" << std::endl;
        }
    }
    else if (category == "weapon") {
        // 装备：创建 Equipment 结构体并装备到角色
        Equipment eq;
        eq.id = itemId;
        eq.name = tmpl->getName();
        eq.level = tmpl->getLevel();

        // 判断装备类型
        if (itemId.find("sword") != std::string::npos) {
            eq.type = "weapon";
        } else if (itemId.find("book") != std::string::npos) {
            eq.type = "book";
        } else if (itemId.find("shield") != std::string::npos) {
            eq.type = "shield";
        } else {
            eq.type = "weapon";
        }

        // 解析加成属性
        eq.attackBonus = 0.0;
        eq.defenseBonus = 0.0;
        eq.mpBonus = 0.0;
        eq.hpRestore = 0;

        const std::string& id = itemId;
        if (id.find("sword") != std::string::npos) {
            if (tmpl->getEffectType() == "buffAttack")
                eq.attackBonus = tmpl->getEffectValue() / 100.0;
        } else if (id.find("shield") != std::string::npos) {
            if (tmpl->getEffectType() == "buffDefense")
                eq.defenseBonus = tmpl->getEffectValue() / 100.0;
        } else if (id.find("book") != std::string::npos) {
            if (tmpl->getEffectType() == "buffMp")
                eq.mpBonus = tmpl->getEffectValue() / 100.0;
            const std::string& extra = tmpl->getExtraEffect();
            if (extra.find("生命恢复+") != std::string::npos) {
                size_t pos = extra.find("生命恢复+");
                std::string numStr = extra.substr(pos + 5);
                if (!numStr.empty()) {
                    try { eq.hpRestore = std::stoi(numStr); }
                    catch (...) { eq.hpRestore = 0; }
                }
            }
        }

        std::string detail = tmpl->getDescription();
        if (!tmpl->getExtraEffect().empty()) {
            detail += "，" + tmpl->getExtraEffect();
        }

        if (target->equipItem(eq)) {
            std::cout << "已装备 [" << tmpl->getName() << "]（Lv." << tmpl->getLevel() << "）到 "
                      << target->getName() << "：" << detail << std::endl;
        } else {
            std::cout << "装备失败：[" << tmpl->getName() << "] 无法装备到 "
                      << target->getName() << std::endl;
            return false;
        }
    }

    // 使用后数量减1
    removeItem(itemId, 1);
    return true;
}

// ============================================================
// sellItem — 出售物品
// ============================================================
bool Inventory::sellItem(const std::string& itemId, int count, MainCharacter* player) {
    if (!player) return false;
    if (count <= 0) return false;
    if (!hasItem(itemId, count)) {
        std::cout << "背包中没有足够的该物品。" << std::endl;
        return false;
    }

    // 获取出售价格
    int price = getSellPrice(itemId);
    if (price < 0) {
        std::cout << "无法找到该物品的售价信息。" << std::endl;
        return false;
    }

    int totalGold = price * count;

    // 从背包移除
    if (!removeItem(itemId, count)) {
        return false;
    }

    // 增加金币
    player->addGold(totalGold);

    std::cout << "出售成功！获得 " << totalGold << " 金币。" << std::endl;
    return true;
}

// ============================================================
// getSellPrice — 获取物品出售价格
// ============================================================
int Inventory::getSellPrice(const std::string& itemId) const {
    auto allTemplates = ItemTemplate::loadAllTemplates();
    for (const auto& tmpl : allTemplates) {
        if (tmpl.getId() == itemId) {
            return tmpl.getSellPrice();
        }
    }
    return -1;  // 未找到
}

// ============================================================
// 清空背包
// ============================================================
void Inventory::clear() {
    items.clear();
}

// ============================================================
// 获取物品总数量
// ============================================================
int Inventory::getTotalCount() const {
    int total = 0;
    for (const auto& pair : items) {
        total += pair.second;
    }
    return total;
}
