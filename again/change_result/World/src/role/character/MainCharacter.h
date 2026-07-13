#pragma once

#include <string>

class MainCharacter {
private:
    std::string name;
    std::string birthday;
    std::string signature;
    bool isMale;
    int level;
    int exp;
    int expToNext;
    int gold;
    int diamond;
    int stamina;
    int maxStamina;
    const std::string major;

public:
    MainCharacter(const std::string& name, const std::string& birthday,
                  const std::string& signature, bool isMale);

    // ---- getter ----
    std::string getName() const;
    std::string getBirthday() const;
    std::string getSignature() const;
    bool getIsMale() const;
    int getLevel() const;
    int getExp() const;
    int getExpToNext() const;
    int getGold() const;
    int getDiamond() const;
    int getStamina() const;
    int getMaxStamina() const;
    std::string getMajor() const;

    // ---- setter ----
    void setName(const std::string& name);
    void setBirthday(const std::string& birthday);
    void setSignature(const std::string& signature);
    void setGold(int gold);
    void setDiamond(int diamond);
    void setStamina(int stamina);
    void setMaxStamina(int maxStamina);
    void setLevel(int level);
    void setExp(int exp);
    void setExpToNext(int expToNext);

    // ---- 经验 & 等级 ----
    void addExp(int amount);
    void levelUp();
    bool canLevelUp() const;
    int getExpForNextLevel() const;  // 计算下一级所需经验：等级 × 500

    // ---- 经济 ----
    void addGold(int amount);
    void addDiamond(int amount);
    bool hasEnoughGold(int amount) const;
    bool hasEnoughDiamond(int amount) const;

    // ---- 体力 ----
    bool consumeStamina(int amount);
    void restoreStamina(int amount);

    // ---- 形象 ----
    std::string getAvatarFileName() const;
    std::string getIdPhotoFileName() const;
    std::string getPortraitFileName() const;

    // ---- 信息 ----
    void displayInfo() const;
};