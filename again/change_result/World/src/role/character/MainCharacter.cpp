#include "MainCharacter.h"
#include <iostream>
#include <algorithm>

MainCharacter::MainCharacter(const std::string& name, const std::string& birthday,
                             const std::string& signature, bool isMale)
    : name(name), birthday(birthday), signature(signature),
      isMale(isMale), level(1), exp(0), gold(0), diamond(0),
      stamina(100), maxStamina(100), major("计算机系") {
    expToNext = getExpForNextLevel();
}

std::string MainCharacter::getName() const { return name; }
std::string MainCharacter::getBirthday() const { return birthday; }
std::string MainCharacter::getSignature() const { return signature; }
bool MainCharacter::getIsMale() const { return isMale; }
int MainCharacter::getLevel() const { return level; }
int MainCharacter::getExp() const { return exp; }
int MainCharacter::getExpToNext() const { return expToNext; }
int MainCharacter::getGold() const { return gold; }
int MainCharacter::getDiamond() const { return diamond; }
int MainCharacter::getStamina() const { return stamina; }
int MainCharacter::getMaxStamina() const { return maxStamina; }
std::string MainCharacter::getMajor() const { return major; }

void MainCharacter::setName(const std::string& n) { name = n; }
void MainCharacter::setBirthday(const std::string& b) { birthday = b; }
void MainCharacter::setSignature(const std::string& s) { signature = s; }
void MainCharacter::setGold(int g) { gold = std::max(0, g); }
void MainCharacter::setDiamond(int d) { diamond = std::max(0, d); }
void MainCharacter::setStamina(int s) { stamina = std::max(0, std::min(maxStamina, s)); }
void MainCharacter::setMaxStamina(int m) { maxStamina = std::max(1, m); if (stamina > maxStamina) stamina = maxStamina; }
void MainCharacter::setLevel(int l) { level = std::max(1, l); }
void MainCharacter::setExp(int e) { exp = std::max(0, e); }
void MainCharacter::setExpToNext(int e) { expToNext = std::max(1, e); }

int MainCharacter::getExpForNextLevel() const {
    return level * 500;  // 等级 × 500
}

bool MainCharacter::canLevelUp() const {
    return exp >= expToNext;
}

void MainCharacter::levelUp() {
    if (!canLevelUp()) return;
    exp -= expToNext;
    ++level;
    expToNext = getExpForNextLevel();
    std::cout << "🎉 主控升到 " << level << " 级！下一级需 " << expToNext << " 经验" << std::endl;
}

void MainCharacter::addExp(int amount) {
    if (amount <= 0) return;
    exp += amount;
    std::cout << "获得 " << amount << " 经验（" << exp << "/" << expToNext << "）" << std::endl;
    while (canLevelUp()) {
        levelUp();
    }
}

void MainCharacter::addGold(int amount) {
    gold += amount;
    if (gold < 0) gold = 0;
}

void MainCharacter::addDiamond(int amount) {
    diamond += amount;
    if (diamond < 0) diamond = 0;
}

bool MainCharacter::hasEnoughGold(int amount) const { return gold >= amount; }
bool MainCharacter::hasEnoughDiamond(int amount) const { return diamond >= amount; }

bool MainCharacter::consumeStamina(int amount) {
    if (amount <= 0) return true;
    if (stamina < amount) {
        std::cout << "体力不足！需要 " << amount << "，当前 " << stamina << std::endl;
        return false;
    }
    stamina -= amount;
    return true;
}

void MainCharacter::restoreStamina(int amount) {
    if (amount <= 0) return;
    stamina += amount;
    if (stamina > maxStamina) stamina = maxStamina;
}

std::string MainCharacter::getAvatarFileName() const {
    return isMale ? "avatar_m.png" : "avatar_f.png";
}

std::string MainCharacter::getIdPhotoFileName() const {
    return isMale ? "idphoto_m.png" : "idphoto_f.png";
}

std::string MainCharacter::getPortraitFileName() const {
    return isMale ? "portrait_m.png" : "portrait_f.png";
}

void MainCharacter::displayInfo() const {
    std::cout << "\n========== 主控信息 ==========" << std::endl;
    std::cout << "名称：" << name << std::endl;
    std::cout << "性别：" << (isMale ? "男" : "女") << std::endl;
    std::cout << "生日：" << birthday << std::endl;
    std::cout << "专业：" << major << std::endl;
    std::cout << "签名：" << signature << std::endl;
    std::cout << "等级：" << level << std::endl;
    std::cout << "经验：" << exp << "/" << expToNext << std::endl;
    std::cout << "金币：" << gold << std::endl;
    std::cout << "钻石：" << diamond << std::endl;
    std::cout << "体力：" << stamina << "/" << maxStamina << std::endl;
    std::cout << "==============================\n" << std::endl;
}