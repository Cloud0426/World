#include "BattleSystem.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cmath>

BattleSystem::BattleSystem(MainCharacter* mainChar, TaskManager* taskManager,
                           EnemyManager* enemyManager, Inventory* inventory)
    : mainChar(mainChar), taskManager(taskManager),
      enemyManager(enemyManager), inventory(inventory),
      battleOver(false), victory(false), isPlayerTurn(true),
      isBossWave(false), bossEntered(false), currentEnemyIndex(0) {}

void BattleSystem::startBattle(std::vector<Combatant*> fighters, std::vector<Enemy*> enemies) {
    allFighters = fighters;
    availableFighters = fighters;
    enemyList = enemies;
    currentEnemyIndex = 0;
    battleOver = false;
    victory = false;
    isPlayerTurn = true;
    isBossWave = false;
    bossEntered = false;

    // 重置所有敌人状态
    for (auto* e : enemyList) {
        e->resetState();
        e->resetTriggers();
        e->resetMp();
    }

    // 选择第一个上场角色
    std::cout << "\n========== 战斗开始 ==========" << std::endl;
    std::cout << "请选择第一个上场的角色：" << std::endl;
    for (size_t i = 0; i < availableFighters.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << availableFighters[i]->getName()
                  << "（HP:" << availableFighters[i]->getHp() << "/" << availableFighters[i]->getMaxHp() << "）"
                  << std::endl;
    }
    int choice;
    std::cin >> choice;
    while (choice < 1 || choice > (int)availableFighters.size()) {
        std::cout << "无效选择，请重新输入: ";
        std::cin >> choice;
    }
    currentPlayer = availableFighters[choice - 1];
    availableFighters.erase(availableFighters.begin() + choice - 1);
    currentPlayer->resetMp();

    // 选择第一个敌人
    currentEnemy = enemyList[0];
    std::cout << "\n当前敌人：" << currentEnemy->getName() << std::endl;

    while (!battleOver) {
        displayBattleStatus();

        if (isPlayerTurn) {
            playerTurn();
        } else {
            enemyTurn();
        }

        checkBattleResult();

        // 敌人回合结束后，玩家回合开始前，可选择更换角色
        if (!isPlayerTurn && !battleOver) {
            std::cout << "\n是否更换角色？[1] 是  [2] 否: ";
            int change;
            std::cin >> change;
            if (change == 1 && !availableFighters.empty()) {
                switchPlayer();
            }
            isPlayerTurn = true;
        }
    }

    if (victory) {
        handleVictory();
    } else {
        handleDefeat();
    }
}

void BattleSystem::playerTurn() {
    std::cout << "\n--- 你的回合 ---" << std::endl;
    std::cout << "当前角色：" << currentPlayer->getName()
              << "（HP:" << currentPlayer->getHp() << "/" << currentPlayer->getMaxHp()
              << " MP:" << currentPlayer->getMp() << "/" << currentPlayer->getMaxMp() << "）" << std::endl;

    std::cout << "[1] 普通攻击  [2] 技能1  [3] 技能2  [4] 大招" << std::endl;
    int choice;
    std::cin >> choice;
    executePlayerAction(choice);
}

void BattleSystem::executePlayerAction(int choice) {
    Skill* skill = nullptr;
    switch (choice) {
        case 1: skill = currentPlayer->getNormalAttack(); break;
        case 2: skill = currentPlayer->getSkill1(); break;
        case 3: skill = currentPlayer->getSkill2(); break;
        case 4: skill = currentPlayer->getUltimate(); break;
        default: std::cout << "无效选择" << std::endl; return;
    }

    if (!skill) {
        std::cout << "技能不存在" << std::endl;
        return;
    }

    if (skill->mpCost > 0 && !currentPlayer->consumeMp(skill->mpCost)) {
        std::cout << "蓝量不足！需要 " << skill->mpCost << " 蓝" << std::endl;
        return;
    }

    if (!currentEnemy || !currentEnemy->isAlive()) {
        // 找下一个存活敌人
        for (auto* e : enemyList) {
            if (e->isAlive()) {
                currentEnemy = e;
                break;
            }
        }
        if (!currentEnemy || !currentEnemy->isAlive()) {
            battleOver = true;
            victory = true;
            return;
        }
    }

    std::cout << currentPlayer->getName() << " 使用 " << skill->name << "！" << std::endl;

    // 执行技能主效果
    applyEffect(skill->effect, skill->effectValue, skill->target, currentEnemy, currentPlayer);

    // 执行副效果
    if (!skill->subEffect.empty()) {
        std::string subTarget = skill->subTarget.empty() ? skill->target : skill->subTarget;
        applyEffect(skill->subEffect, skill->subEffectValue, subTarget, currentEnemy, currentPlayer);
    }

    // 攻击后增加蓝条
    currentPlayer->restoreMp((int)(currentPlayer->getMaxMp() * 0.25));
    if (currentPlayer->getMp() > currentPlayer->getMaxMp()) {
        currentPlayer->setMp(currentPlayer->getMaxMp());
    }

    // 检查敌人是否死亡
    if (!currentEnemy->isAlive()) {
        std::cout << currentEnemy->getName() << " 被击败！" << std::endl;
        // 敌人死亡后，下一个敌人自动上场
        bool hasNext = false;
        for (size_t i = currentEnemyIndex + 1; i < enemyList.size(); ++i) {
            if (enemyList[i]->isAlive()) {
                currentEnemyIndex = i;
                currentEnemy = enemyList[i];
                hasNext = true;
                std::cout << currentEnemy->getName() << " 自动上场！" << std::endl;
                // 先发动一次150%攻击
                int damage = (int)(currentEnemy->getAttack() * 1.5);
                int actualDamage = currentPlayer->takeDamage(damage);
                std::cout << currentEnemy->getName() << " 发动突袭！造成 " << actualDamage << " 点伤害！" << std::endl;
                if (!currentPlayer->isAlive()) {
                    std::cout << currentPlayer->getName() << " 被击败！" << std::endl;
                    // 角色死亡，玩家选择下一个角色
                    if (!availableFighters.empty()) {
                        switchPlayer();
                    } else {
                        battleOver = true;
                        victory = false;
                    }
                }
                break;
            }
        }
        // 检查是否所有敌人都死了
        bool allDead = true;
        for (auto* e : enemyList) {
            if (e->isAlive()) { allDead = false; break; }
        }
        if (allDead) {
            battleOver = true;
            victory = true;
            return;
        }
    }

    isPlayerTurn = false;
}

void BattleSystem::enemyTurn() {
    std::cout << "\n--- 敌人回合 ---" << std::endl;

    // 检查蓝条是否满，满则放大招
    if (currentEnemy->isMpFull()) {
        std::cout << currentEnemy->getName() << " 蓝条已满，释放大招！" << std::endl;
        enemyUltimate(currentEnemy);
    } else {
        enemyAttack(currentEnemy);
    }

    if (!currentPlayer->isAlive()) {
        std::cout << currentPlayer->getName() << " 被击败！" << std::endl;
        if (!availableFighters.empty()) {
            switchPlayer();
        } else {
            battleOver = true;
            victory = false;
        }
        return;
    }

    isPlayerTurn = true;
}

void BattleSystem::enemyAttack(Enemy* enemy) {
    if (!enemy || !enemy->isAlive() || !currentPlayer || !currentPlayer->isAlive()) return;
    int damage = enemy->getAttack();
    int actualDamage = currentPlayer->takeDamage(damage);
    // 敌人攻击后增加蓝条
    enemy->addMp(25);
    if (enemy->getMp() > enemy->getMaxMp()) {
        enemy->setMp(enemy->getMaxMp());
    }
    std::cout << enemy->getName() << " 攻击 " << currentPlayer->getName()
              << "，造成 " << actualDamage << " 点伤害" << std::endl;
}

void BattleSystem::enemyUltimate(Enemy* enemy) {
    if (!enemy || !enemy->isAlive()) return;
    int mp = enemy->getMp();
    enemy->resetMp();
    int damage = (int)(enemy->getAttack() * 1.5);
    int actualDamage = currentPlayer->takeDamage(damage);
    std::cout << enemy->getName() << " 释放大招！造成 " << actualDamage << " 点伤害！" << std::endl;
}

void BattleSystem::switchPlayer() {
    if (availableFighters.empty()) {
        std::cout << "没有可上场的角色" << std::endl;
        return;
    }
    std::cout << "\n选择下一个上场的角色：" << std::endl;
    for (size_t i = 0; i < availableFighters.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << availableFighters[i]->getName()
                  << "（HP:" << availableFighters[i]->getHp() << "/" << availableFighters[i]->getMaxHp() << "）"
                  << std::endl;
    }
    int choice;
    std::cin >> choice;
    while (choice < 1 || choice > (int)availableFighters.size()) {
        std::cout << "无效选择，请重新输入: ";
        std::cin >> choice;
    }
    currentPlayer = availableFighters[choice - 1];
    availableFighters.erase(availableFighters.begin() + choice - 1);
    currentPlayer->resetMp();
    std::cout << currentPlayer->getName() << " 上场！" << std::endl;
}

void BattleSystem::switchEnemy() {
    // 敌人死亡后自动切换
    for (size_t i = currentEnemyIndex + 1; i < enemyList.size(); ++i) {
        if (enemyList[i]->isAlive()) {
            currentEnemyIndex = i;
            currentEnemy = enemyList[i];
            std::cout << currentEnemy->getName() << " 自动上场！" << std::endl;
            // 先发动一次150%攻击
            int damage = (int)(currentEnemy->getAttack() * 1.5);
            int actualDamage = currentPlayer->takeDamage(damage);
            std::cout << currentEnemy->getName() << " 发动突袭！造成 " << actualDamage << " 点伤害！" << std::endl;
            if (!currentPlayer->isAlive()) {
                std::cout << currentPlayer->getName() << " 被击败！" << std::endl;
                if (!availableFighters.empty()) {
                    switchPlayer();
                } else {
                    battleOver = true;
                    victory = false;
                }
            }
            break;
        }
    }
}

void BattleSystem::checkBattleResult() {
    if (!currentPlayer->isAlive()) {
        if (!availableFighters.empty()) {
            // 玩家选择下一个角色
            std::cout << "\n" << currentPlayer->getName() << " 已被击败！" << std::endl;
            switchPlayer();
            // 新角色上场后，敌人先发动一次攻击
            if (currentEnemy && currentEnemy->isAlive()) {
                int damage = currentEnemy->getAttack();
                int actualDamage = currentPlayer->takeDamage(damage);
                std::cout << currentEnemy->getName() << " 攻击 " << currentPlayer->getName()
                          << "，造成 " << actualDamage << " 点伤害" << std::endl;
                if (!currentPlayer->isAlive()) {
                    battleOver = true;
                    victory = false;
                }
            }
        } else {
            battleOver = true;
            victory = false;
        }
    }

    bool allDead = true;
    for (auto* e : enemyList) {
        if (e->isAlive()) { allDead = false; break; }
    }
    if (allDead) {
        battleOver = true;
        victory = true;
    }
}

void BattleSystem::displayBattleStatus() const {
    std::cout << "\n========== 战斗状态 ==========" << std::endl;
    if (currentPlayer) {
        std::cout << "玩家：" << currentPlayer->getName()
                  << " HP:" << currentPlayer->getHp() << "/" << currentPlayer->getMaxHp()
                  << " MP:" << currentPlayer->getMp() << "/" << currentPlayer->getMaxMp() << std::endl;
    }
    if (currentEnemy) {
        std::cout << "敌人：" << currentEnemy->getName()
                  << " HP:" << currentEnemy->getHp() << "/" << currentEnemy->getMaxHp()
                  << " MP:" << currentEnemy->getMp() << "/" << currentEnemy->getMaxMp() << std::endl;
    }
    std::cout << "================================\n" << std::endl;
}

void BattleSystem::applyEffect(const std::string& effect, int value, const std::string& target,
                               Enemy* enemy, Combatant* caster) {
    if (effect == "attack") {
        int baseDamage = caster->getAttack() * value / 100;
        double crit = caster->getCritRate();
        bool isCrit = (std::rand() % 100) < (crit * 100);
        if (isCrit) {
            baseDamage = (int)(baseDamage * 1.5);
            std::cout << "⚡ 暴击！";
        }
        if (target == "enemy_single") {
            damageEnemy(enemy, baseDamage);
        } else if (target == "enemy_all") {
            for (auto* e : enemyList) {
                if (e->isAlive()) damageEnemy(e, baseDamage);
            }
        }
    } else if (effect == "heal_percent") {
        if (target == "self") {
            healPlayerPercent(value);
        } else if (target == "ally_all") {
            healPlayerPercent(value);
        }
    } else if (effect == "full_heal") {
        if (target == "self") {
            int healAmt = caster->getMaxHp() - caster->getHp();
            caster->heal(healAmt);
            std::cout << caster->getName() << " 完全恢复生命值！" << std::endl;
        }
    } else if (effect == "restore_mp") {
        if (target == "self") {
            restorePlayerMp(value);
        } else if (target == "ally_all") {
            restorePlayerMp(value);
        }
    } else if (effect == "buff_crit") {
        if (target == "self") {
            buffPlayerCrit(value);
        }
    }
}

void BattleSystem::damageEnemy(Enemy* enemy, int damage) {
    if (!enemy || !enemy->isAlive()) return;
    int oldHp = enemy->getHp();
    enemy->takeDamage(damage);
    int newHp = enemy->getHp();
    std::cout << enemy->getName() << " 受到 " << damage << " 点伤害（剩余HP:" << newHp << "/" << enemy->getMaxHp() << "）" << std::endl;
    // 敌人受击增加蓝条
    enemy->addMp(25);
    if (enemy->getMp() > enemy->getMaxMp()) {
        enemy->setMp(enemy->getMaxMp());
    }
    // 检查阈值触发
    checkEnemyThresholds(enemy, oldHp, newHp);
}

void BattleSystem::checkEnemyThresholds(Enemy* enemy, int oldHp, int newHp) {
    // 保留原阈值逻辑
}

void BattleSystem::healPlayer(int amount) {
    currentPlayer->heal(amount);
}

void BattleSystem::healPlayerPercent(int percent) {
    int amount = currentPlayer->getMaxHp() * percent / 100;
    currentPlayer->heal(amount);
    std::cout << currentPlayer->getName() << " 恢复 " << amount << " HP" << std::endl;
}

void BattleSystem::restorePlayerMp(int amount) {
    currentPlayer->restoreMp(amount);
    std::cout << currentPlayer->getName() << " 恢复 " << amount << " MP" << std::endl;
}

void BattleSystem::buffPlayerCrit(int amount) {
    double current = currentPlayer->getCritRate();
    currentPlayer->setCritRate(std::min(1.0, current + amount / 100.0));
    std::cout << currentPlayer->getName() << " 暴击率+" << amount << "%" << std::endl;
}

void BattleSystem::buffPlayerAttack(int percent) {
    int boost = currentPlayer->getAttack() * percent / 100;
    currentPlayer->setAttack(currentPlayer->getAttack() + boost);
    std::cout << currentPlayer->getName() << " 攻击力+" << percent << "%" << std::endl;
}

void BattleSystem::handleVictory() {
    std::cout << "\n✦ 战斗胜利！✦" << std::endl;
    int totalExp = 0, totalGold = 0, totalDiamond = 0;
    for (auto* e : enemyList) {
        if (!e->isAlive()) {
            totalExp += e->getExpReward();
            totalGold += e->getGoldReward();
            totalDiamond += e->getDiamondReward();
        }
    }
    mainChar->addExp(totalExp);
    mainChar->addGold(totalGold);
    mainChar->addDiamond(totalDiamond);
    std::cout << "获得 " << totalExp << " 经验，" << totalGold << " 金币，" << totalDiamond << " 钻石" << std::endl;
    taskManager->updateProgress("clear_stage", 1);

    // 所有参战角色 MP 重置为 0
    for (auto* f : allFighters) {
        f->resetMp();
    }
    // 所有敌人 MP 重置为 0
    for (auto* e : enemyList) {
        e->resetMp();
    }

        // 血量数值保留原样，不做恢复
    std::cout << "角色血量已保留战斗结束时的状态" << std::endl;
}

void BattleSystem::handleDefeat() {
    std::cout << "\n✘ 战斗失败..." << std::endl;

    // 所有参战角色 MP 重置为 0
    for (auto* f : allFighters) {
        f->resetMp();
    }
    // 所有敌人 MP 重置为 0
    for (auto* e : enemyList) {
        e->resetMp();
    }

        // 血量数值保留原样，不做恢复
    std::cout << "角色血量已保留战斗结束时的状态" << std::endl;
}

Enemy* BattleSystem::getCurrentEnemy() const { return currentEnemy; }
bool BattleSystem::isBattleOver() const { return battleOver; }
bool BattleSystem::isVictory() const { return victory; }