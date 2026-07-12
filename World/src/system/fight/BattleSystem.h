#pragma once

#include <vector>
#include <string>
#include "role/combatant/Combatant.h"
#include "role/enemy/Enemy.h"
#include "role/enemy/EnemyManager.h"
#include "role/character/MainCharacter.h"
#include "system/mission/TaskManager.h"
#include "system/backpack/Inventory.h"
#include "Combatant.h"

class BattleSystem {
private:
    std::vector<Combatant*> allFighters;      // 所有上场角色
    std::vector<Combatant*> availableFighters; // 当前可用角色（未上场/未死亡）
    Combatant* currentPlayer;                  // 当前上场的角色
    Enemy* currentEnemy;                       // 当前敌人
    std::vector<Enemy*> enemyList;             // 当前关卡所有敌人
    int currentEnemyIndex;                     // 当前敌人索引
    MainCharacter* mainChar;
    TaskManager* taskManager;
    EnemyManager* enemyManager;
    Inventory* inventory;
    bool battleOver;
    bool victory;
    bool isPlayerTurn;
    bool isBossWave;                           // 是否Boss波
    bool bossEntered;                          // Boss是否已上场

    // 战斗状态
    void displayBattleStatus() const;
    void playerTurn();
    void enemyTurn();
    void executePlayerAction(int choice);
    void executeEnemyAction(Enemy* enemy);
    void switchPlayer();
    void switchEnemy();
    void checkBattleResult();

    // 敌人AI
    void enemyAttack(Enemy* enemy);
    void enemyUltimate(Enemy* enemy);
    void checkEnemyThresholds(Enemy* enemy, int oldHp, int newHp);

    // 技能执行
    void executeSkill(Skill* skill, Enemy* target);
    void applyEffect(const std::string& effect, int value, const std::string& target,
                     Enemy* enemy, Combatant* caster);

    // 伤害与恢复
    void damageEnemy(Enemy* enemy, int damage);
    void healPlayer(int amount);
    void healPlayerPercent(int percent);
    void restorePlayerMp(int amount);
    void buffPlayerCrit(int amount);
    void buffPlayerAttack(int percent);

public:
    BattleSystem(MainCharacter* mainChar, TaskManager* taskManager,
                 EnemyManager* enemyManager, Inventory* inventory);

    void startBattle(std::vector<Combatant*> fighters, std::vector<Enemy*> enemies);
    bool isBattleOver() const;
    bool isVictory() const;
    void handleVictory();
    void handleDefeat();
    Enemy* getCurrentEnemy() const;
};