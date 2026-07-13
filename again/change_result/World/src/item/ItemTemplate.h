#pragma once

#include <string>
#include <vector>

class ItemTemplate {
private:
    std::string id;
    std::string name;
    std::string description;
    std::string category;      // "food" / "potion" / "weapon" / "material" / "special"
    int price;
    int sellPrice;
    std::string effectType;    // "restoreStamina" / "restoreHp" / "buffAttack" / "buffDefense" / "buffMp" / "upgrade" / "recruit"
    int effectValue;
    std::string extraEffect;
    int level;                 // 装备等级，非装备为0
    std::string currency;      // "gold" / "diamond"

public:
    ItemTemplate(const std::string& id,
                 const std::string& name,
                 const std::string& description,
                 const std::string& category,
                 int price,
                 int sellPrice,
                 const std::string& effectType,
                 int effectValue,
                 const std::string& extraEffect = "",
                 int level = 0,
                 const std::string& currency = "gold");

    // ---- getter ----
    std::string getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getCategory() const;
    int getPrice() const;
    int getSellPrice() const;
    std::string getEffectType() const;
    int getEffectValue() const;
    std::string getExtraEffect() const;
    int getLevel() const;
    std::string getCurrency() const;

    void displayInfo() const;
    static std::vector<ItemTemplate> loadAllTemplates();
};