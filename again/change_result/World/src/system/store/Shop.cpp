#include "Shop.h"
#include "role/combatant/Combatant.h"  // 包含 Combatant 以使用 Equipment 结构体
#include <iostream>
#include <algorithm>

Shop::Shop() { 
    loadShopItems(); 
}

void Shop::loadShopItems() {
    shopItems.clear();

    auto allTemplates = ItemTemplate::loadAllTemplates();
    for (const auto& tmpl : allTemplates) {
        // 判断是否限购（食物每日限购1）
        int dailyLimit = (tmpl.getCategory() == "food") ? 1 : 0;

        // 解析装备属性
        double attackBonus = 0.0, defenseBonus = 0.0, mpBonus = 0.0;
        int hpRestore = 0;
        int level = tmpl.getLevel();

        if (tmpl.getCategory() == "weapon") {
            // 根据 ID 判断装备类型
            const std::string& id = tmpl.getId();
            if (id.find("sword") != std::string::npos) {
                // 剑：攻击加成
                if (tmpl.getEffectType() == "buffAttack")
                    attackBonus = tmpl.getEffectValue() / 100.0;
            } else if (id.find("shield") != std::string::npos) {
                // 护盾：防御加成
                if (tmpl.getEffectType() == "buffDefense")
                    defenseBonus = tmpl.getEffectValue() / 100.0;
            } else if (id.find("book") != std::string::npos) {
                // 魔法书：蓝条加成 + 生命恢复
                if (tmpl.getEffectType() == "buffMp")
                    mpBonus = tmpl.getEffectValue() / 100.0;
                // 从 extraEffect 中解析 hpRestore
                const std::string& extra = tmpl.getExtraEffect();
                if (extra.find("生命恢复+") != std::string::npos) {
                    size_t pos = extra.find("生命恢复+");
                    std::string numStr = extra.substr(pos + 5);
                    if (!numStr.empty()) {
                        try {
                            hpRestore = std::stoi(numStr);
                        } catch (...) {
                            hpRestore = 0;
                        }
                    }
                }
            }
        }

        ShopItem item;
        item.id = tmpl.getId();
        item.name = tmpl.getName();
        item.category = tmpl.getCategory();
        item.price = tmpl.getPrice();
        item.currency = tmpl.getCurrency();
        item.description = tmpl.getDescription();
        item.dailyLimit = dailyLimit;
        item.dailySold = 0;
        item.level = level;
        item.attackBonus = attackBonus;
        item.defenseBonus = defenseBonus;
        item.mpBonus = mpBonus;
        item.hpRestore = hpRestore;

        shopItems.push_back(item);
    }
}

void Shop::displayItems() const {
    std::cout << "\n========== 商店 ==========" << std::endl;
    
    auto printCat = [&](const std::string& cat, const std::string& label) {
        bool first = true;
        for (size_t i = 0; i < shopItems.size(); ++i) {
            const auto& item = shopItems[i];
            if (item.category == cat) {
                if (first) { 
                    std::cout << "\n--- " << label << " ---\n"; 
                    first = false; 
                }
                std::cout << "[" << (i + 1) << "] " << item.name
                          << " | " << item.price << " " << item.currency;
                if (item.dailyLimit > 0) {
                    std::cout << " | 今日" << item.dailySold << "/" << item.dailyLimit;
                }
                std::cout << " | " << item.description << std::endl;
            }
        }
    };
    
    printCat("food", "食物（每日限购1）");
    printCat("potion", "药品");
    printCat("weapon", "装备");
    printCat("material", "材料");
    printCat("special", "特殊");
    
    std::cout << "\n==========================\n" << std::endl;
}

bool Shop::buyItem(int index, int count, MainCharacter* player, Inventory* inventory,
                   std::vector<Combatant*>& fighters) {
    if (!player || !inventory || count <= 0) return false;
    if (index < 1 || index > (int)shopItems.size()) {
        std::cout << "无效编号" << std::endl;
        return false;
    }
    
    ShopItem& item = shopItems[index - 1];
    
    // 检查每日限购
    if (item.dailyLimit > 0 && item.dailySold >= item.dailyLimit) {
        std::cout << "今日已售罄" << std::endl;
        return false;
    }
    
    // 检查余额
    int total = item.price * count;
    bool has = (item.currency == "gold") ? player->hasEnoughGold(total) : player->hasEnoughDiamond(total);
    if (!has) {
        std::cout << "余额不足！需要 " << total << " " << item.currency << std::endl;
        return false;
    }
    
    // 扣款
    if (item.currency == "gold") {
        player->addGold(-total);
    } else {
        player->addDiamond(-total);
    }

    // 处理购买
    if (item.category == "weapon") {
        // 装备直接穿到第一个可装备的角色
        Equipment eq;
        eq.id = item.id;
        eq.name = item.name;
        
        // 判断装备类型
        if (item.id.find("sword") != std::string::npos) {
            eq.type = "weapon";
        } else if (item.id.find("book") != std::string::npos) {
            eq.type = "book";
        } else if (item.id.find("shield") != std::string::npos) {
            eq.type = "shield";
        } else {
            eq.type = "weapon";
        }
        
        eq.level = item.level;
        eq.attackBonus = item.attackBonus;
        eq.defenseBonus = item.defenseBonus;
        eq.mpBonus = item.mpBonus;
        eq.hpRestore = item.hpRestore;

        bool equipped = false;
        for (auto* f : fighters) {
            if (f->getEquippedCount() < 2) {
                if (f->equipItem(eq)) {
                    equipped = true;
                    std::cout << item.name << " 已装备到 " << f->getName() << std::endl;
                    break;
                }
            }
        }
        if (!equipped) {
            inventory->addItem(item.id, 1);
            std::cout << item.name << " 已放入背包（无可用角色）" << std::endl;
        }
    } else {
        inventory->addItem(item.id, count);
        std::cout << "购买成功！" << item.name << " x" << count << std::endl;
    }
    
    // 更新每日销量
    if (item.dailyLimit > 0) {
        item.dailySold += count;
    }
    
    return true;
}

const ShopItem* Shop::findItemById(const std::string& id) const {
    for (const auto& item : shopItems) {
        if (item.id == id) return &item;
    }
    return nullptr;
}

const std::vector<ShopItem>& Shop::getShopItems() const { 
    return shopItems; 
}

void Shop::resetDailyLimits() {
    for (auto& item : shopItems) {
        if (item.dailyLimit > 0) {
            item.dailySold = 0;
        }
    }
    std::cout << "商店每日限购已重置" << std::endl;
}