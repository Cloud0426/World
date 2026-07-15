
#include "RecruitSystem.h"
#include "role/character/MainCharacter.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

RecruitSystem::RecruitSystem() : recruitCount(0) {
    static bool seeded = false;
    if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }
    initFighters();
}

RecruitSystem::~RecruitSystem() { clear(); }

void RecruitSystem::initFighters() {
    allFighters.clear();
    allFighters.push_back(Combatant("能源学者", "能源学院"));
    allFighters.push_back(Combatant("能源新星", "能源学院"));
    allFighters.push_back(Combatant("能源守卫", "能源学院"));
    allFighters.push_back(Combatant("医者", "医学院"));
    allFighters.push_back(Combatant("急救员", "医学院"));
    allFighters.push_back(Combatant("药剂师", "医学院"));
    allFighters.push_back(Combatant("机械师", "机械学院"));
    allFighters.push_back(Combatant("机械先锋", "机械学院"));
    allFighters.push_back(Combatant("机械卫士", "机械学院"));
    allFighters.push_back(Combatant("陈尔愿", "机械学院"));  // 🆕 陈尔愿
}

bool RecruitSystem::isOwned(const std::string& name, const std::string& college) const {
    for (const auto* f : ownedFighters) {
        if (f->getName() == name && f->getCollege() == college) return true;
    }
    return false;
}

void RecruitSystem::addFighter(Combatant* fighter) {
    ownedFighters.push_back(fighter);
}

void RecruitSystem::addFighterDirect(Combatant* fighter) {
    if (fighter) {
        ownedFighters.push_back(fighter);
        recruitCount++;
    }
}

Combatant* RecruitSystem::recruit(Inventory* inventory, MainCharacter* player) {
    if (allFighters.empty() || !inventory || !player) return nullptr;

    // 🆕 固定招募：如果没有陈尔愿，必定获得陈尔愿
    if (!isOwned("陈尔愿", "机械学院")) {
        Combatant* newFighter = new Combatant("陈尔愿", "机械学院");
        // 给陈尔愿额外加强一点初始属性
        newFighter->setAttack(200);
        newFighter->setDefense(120);
        newFighter->setMaxHp(3500);
        newFighter->setHp(3500);
        addFighter(newFighter);
        recruitCount++;
        std::cout << "\n招募结果：机械学院 - 陈尔愿" << std::endl;
        std::cout << "获得新角色——陈尔愿！" << std::endl;
        return newFighter;
    }

    // 已有陈尔愿，走普通随机招募逻辑
    int idx = std::rand() % allFighters.size();
    const Combatant& tmpl = allFighters[idx];
    std::string name = tmpl.getName();
    std::string college = tmpl.getCollege();

    std::cout << "\n招募结果：" << college << " - " << name << std::endl;

    if (isOwned(name, college)) {
        player->addDiamond(100);
        std::cout << "已拥有该角色，转化为100钻石！" << std::endl;
        return nullptr;
    }

    Combatant* newFighter = new Combatant(tmpl);
    addFighter(newFighter);
    recruitCount++;
    std::cout << "获得新角色！" << std::endl;
    return newFighter;
}

void RecruitSystem::displayOwnedFighters() const {
    std::cout << "\n========== 我的伙伴 ==========" << std::endl;
    if (ownedFighters.empty()) {
        std::cout << "（暂无角色）" << std::endl;
        return;
    }
    for (size_t i = 0; i < ownedFighters.size(); ++i) {
        const auto* f = ownedFighters[i];
        std::cout << "[" << (i + 1) << "] " << f->getName()
                  << " [" << f->getCollege() << "] Lv." << f->getLevel()
                  << " HP:" << f->getHp() << "/" << f->getMaxHp()
                  << " ATK:" << f->getAttack() << " DEF:" << f->getDefense()
                  << " MP:" << f->getMp() << "/" << f->getMaxMp()
                  << " 装备:" << f->getEquippedCount() << "/2" << std::endl;
    }
    std::cout << "总计：" << ownedFighters.size() << " 个角色" << std::endl;
    std::cout << "==============================\n" << std::endl;
}

void RecruitSystem::displayFighterDetail(int index) const {
    if (index < 1 || index > (int)ownedFighters.size()) {
        std::cout << "无效编号" << std::endl;
        return;
    }
    ownedFighters[index - 1]->displayInfo();
    ownedFighters[index - 1]->displaySkills();
}

int RecruitSystem::getFighterCount() const { return (int)ownedFighters.size(); }
Combatant* RecruitSystem::getFighter(int index) {
    if (index < 0 || index >= (int)ownedFighters.size()) return nullptr;
    return ownedFighters[index];
}

bool RecruitSystem::upgradeFighter(int index, Inventory* inventory) {
    if (!inventory) return false;
    if (index < 1 || index > (int)ownedFighters.size()) {
        std::cout << "无效编号" << std::endl;
        return false;
    }
    Combatant* f = ownedFighters[index - 1];
    int need = f->getUpgradeStoneCost();
    if (!inventory->hasItem("upgrade_stone", need)) {
        std::cout << "升级石不足！需要 " << need << " 个" << std::endl;
        return false;
    }
    inventory->removeItem("upgrade_stone", need);
    f->levelUp();
    return true;
}

void RecruitSystem::clear() {
    for (auto* f : ownedFighters) delete f;
    ownedFighters.clear();
}

int RecruitSystem::getRecruitCount() const { return recruitCount; }