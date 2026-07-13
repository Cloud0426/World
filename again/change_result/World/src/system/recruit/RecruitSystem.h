#pragma once

#include <vector>
#include <string>
#include "Combatant.h"
#include "system/backpack/Inventory.h"

class RecruitSystem {
private:
    std::vector<Combatant*> ownedFighters;
    std::vector<Combatant> allFighters;
    int recruitCount;

    bool isOwned(const std::string& name, const std::string& college) const;
    void addFighter(Combatant* fighter);

public:
    RecruitSystem();
    ~RecruitSystem();

        void initFighters();
    Combatant* recruit(Inventory* inventory, MainCharacter* player);
    void addFighterDirect(Combatant* fighter);
    void displayOwnedFighters() const;
    void displayFighterDetail(int index) const;
    int getFighterCount() const;
    void buildStage(int stageId);  // 用于构建指定关卡
    Combatant* getFighter(int index);
    bool upgradeFighter(int index, Inventory* inventory);
    void clear();
    int getRecruitCount() const;
};