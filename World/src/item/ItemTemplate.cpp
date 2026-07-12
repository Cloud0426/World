#include "ItemTemplate.h"
#include <iostream>
#include <algorithm>

// ============================================================
// 构造函数
// ============================================================
ItemTemplate::ItemTemplate(const std::string& id,
                           const std::string& name,
                           const std::string& description,
                           const std::string& category,
                           int price,
                           int sellPrice,
                           const std::string& effectType,
                           int effectValue,
                           const std::string& extraEffect,
                           int level,
                           const std::string& currency)
    : id(id), name(name), description(description),
      category(category), price(price), sellPrice(sellPrice),
      effectType(effectType), effectValue(effectValue),
      extraEffect(extraEffect), level(level), currency(currency) {
}

// ============================================================
// Getter
// ============================================================
std::string ItemTemplate::getId()          const { return id; }
std::string ItemTemplate::getName()        const { return name; }
std::string ItemTemplate::getDescription() const { return description; }
std::string ItemTemplate::getCategory()    const { return category; }
int         ItemTemplate::getPrice()       const { return price; }
int         ItemTemplate::getSellPrice()   const { return sellPrice; }
std::string ItemTemplate::getEffectType()  const { return effectType; }
int         ItemTemplate::getEffectValue() const { return effectValue; }
std::string ItemTemplate::getExtraEffect() const { return extraEffect; }
int         ItemTemplate::getLevel()       const { return level; }
std::string ItemTemplate::getCurrency()    const { return currency; }

// ============================================================
// displayInfo — 显示物品信息
// ============================================================
void ItemTemplate::displayInfo() const {
    std::cout << "ID: "         << id          << "\n";
    std::cout << "名称: "       << name        << "\n";
    std::cout << "描述: "       << description << "\n";
    std::cout << "类别: "       << category    << "\n";
    std::cout << "价格: "       << price       << " " << currency << "\n";
    std::cout << "出售价: "     << sellPrice   << " " << currency << "\n";
    std::cout << "效果类型: "   << effectType  << "\n";
    std::cout << "效果数值: "   << effectValue << "\n";
    if (!extraEffect.empty()) {
        std::cout << "附加效果: " << extraEffect << "\n";
    }
    if (level > 0) {
        std::cout << "装备等级: " << level << "\n";
    }
    std::cout << std::endl;
}

// ============================================================
// loadAllTemplates — 返回所有预设物品模板
// ============================================================
std::vector<ItemTemplate> ItemTemplate::loadAllTemplates() {
    std::vector<ItemTemplate> templates;

    // ============================================================
    // 食物类（钻石购买）
    // ============================================================
    templates.push_back(ItemTemplate(
        "food_001", "罐头", "恢复体力50点",
        "food", 50, 25, "restoreStamina", 50, "", 0, "diamond"));

    templates.push_back(ItemTemplate(
        "food_002", "压缩饼干", "恢复体力30点",
        "food", 20, 10, "restoreStamina", 30, "", 0, "diamond"));

    templates.push_back(ItemTemplate(
        "food_003", "瓶装水", "恢复体力20点",
        "food", 10, 5, "restoreStamina", 20, "", 0, "diamond"));

    // ============================================================
    // 药品类（金币购买）
    // ============================================================
    templates.push_back(ItemTemplate(
        "potion_001", "急救包", "恢复生命80点",
        "potion", 500, 250, "restoreHp", 80, "", 0, "gold"));

    templates.push_back(ItemTemplate(
        "potion_002", "外伤药包", "恢复生命40点",
        "potion", 350, 175, "restoreHp", 40, "", 0, "gold"));

    templates.push_back(ItemTemplate(
        "potion_003", "止痛片", "恢复生命25点",
        "potion", 250, 125, "restoreHp", 25, "", 0, "gold"));

    // ============================================================
    // 装备类 - 剑（金币购买）
    // ============================================================
    templates.push_back(ItemTemplate(
        "weapon_sword_1", "初级剑", "攻击+5%",
        "weapon", 500, 250, "buffAttack", 5, "", 1, "gold"));

    templates.push_back(ItemTemplate(
        "weapon_sword_2", "中级剑", "攻击+10%",
        "weapon", 1000, 500, "buffAttack", 10, "", 2, "gold"));

    templates.push_back(ItemTemplate(
        "weapon_sword_3", "高级剑", "攻击+15%",
        "weapon", 1500, 750, "buffAttack", 15, "", 3, "gold"));

    // ============================================================
    // 装备类 - 魔法书（金币购买）
    // ============================================================
    templates.push_back(ItemTemplate(
        "weapon_book_1", "初级魔法书", "蓝条+5%，生命恢复+10",
        "weapon", 500, 250, "buffMp", 5, "生命恢复+10", 1, "gold"));

    templates.push_back(ItemTemplate(
        "weapon_book_2", "中级魔法书", "蓝条+10%，生命恢复+15",
        "weapon", 1000, 500, "buffMp", 10, "生命恢复+15", 2, "gold"));

    templates.push_back(ItemTemplate(
        "weapon_book_3", "高级魔法书", "蓝条+15%，生命恢复+20",
        "weapon", 1500, 750, "buffMp", 15, "生命恢复+20", 3, "gold"));

    // ============================================================
    // 装备类 - 护盾（金币购买）
    // ============================================================
    templates.push_back(ItemTemplate(
        "weapon_shield_1", "初级护盾", "防御+5%",
        "weapon", 500, 250, "buffDefense", 5, "", 1, "gold"));

    templates.push_back(ItemTemplate(
        "weapon_shield_2", "中级护盾", "防御+10%",
        "weapon", 1000, 500, "buffDefense", 10, "", 2, "gold"));

    templates.push_back(ItemTemplate(
        "weapon_shield_3", "高级护盾", "防御+15%",
        "weapon", 1500, 750, "buffDefense", 15, "", 3, "gold"));

    // ============================================================
    // 特殊物品
    // ============================================================
    templates.push_back(ItemTemplate(
        "upgrade_stone", "升级石", "用于战斗角色升级",
        "material", 200, 100, "upgrade", 0, "", 0, "gold"));

    templates.push_back(ItemTemplate(
        "recruit_card", "招募卡", "消耗1张招募卡随机获得1个战斗角色",
        "special", 150, 75, "recruit", 0, "", 0, "diamond"));

    return templates;
}