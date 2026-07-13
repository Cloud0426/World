#pragma once

#include <vector>
#include <string>
#include <ctime>
#include "role/character/MainCharacter.h"
#include "role/combatant/Combatant.h"
#include "system/backpack/Inventory.h"
#include "system/store/Shop.h"
#include "system/mission/TaskManager.h"
#include "role/enemy/EnemyManager.h"
#include "system/recruit/RecruitSystem.h"
#include "system/fight/BattleSystem.h"

class GameManager {
private:
    // ---- 核心数据 ----
    MainCharacter* mainChar;
    Inventory* inventory;
    Shop* shop;
    TaskManager* taskManager;
    EnemyManager* enemyManager;
    RecruitSystem* recruitSystem;
    bool isRunning;
    bool hasSave;

    // ============================================================
    // 🆕 新增：时间数据
    // ============================================================
    time_t lastLoginTime;          // 上次登录时间
    time_t lastStaminaRecover;     // 上次体力恢复时间
    int consecutiveDays;           // 连续登录天数
    long long totalOnlineSeconds;  // 累计在线秒数
    long long dailyOnlineSeconds; // 当日在线秒数
    time_t sessionStartTime;       // 本次会话开始时间

    // ---- 私有方法 ----
    void initNewGame();
    void loadGame();
    void saveGame();
    void autoSave();

    // ============================================================
    // 🆕 新增：时间相关方法
    // ============================================================
    void updateOnlineTime();       // 更新在线时长
    void checkDailyReset();        // 检查每日任务重置
    void checkStaminaRecover();    // 检查离线体力恢复
    void autoClaimRewards();       // 自动领取已完成任务的奖励

    void giveInitialItems();

    // ---- 辅助方法 ----
    bool isValidDate(const std::string& date) const;  // 校验生日格式和有效性
    int getChineseCharCount(const std::string& str) const;  // 获取汉字字符数

    // ---- 菜单 ----
    void showMainMenu();
    void showMainCharInfo();
    void showInventory();
    void showShop();
    void showRecruit();
    void showFighters();
    void showMap();
    void showTasks();
    void showMemory();

public:
    GameManager();
    ~GameManager();
    void run();

        // ---- 对外暴露数据给 SplashScreen ----
        MainCharacter* getMainChar() const { return mainChar; }
    Inventory* getInventory() const { return inventory; }
    Shop* getShop() const { return shop; }
        TaskManager* getTaskManager() const { return taskManager; }
    RecruitSystem* getRecruitSystem() const { return recruitSystem; }
    EnemyManager* getEnemyManager() const { return enemyManager; }
    long long getTotalOnlineSeconds() const { return totalOnlineSeconds; }
    long long getDailyOnlineSeconds() const { return dailyOnlineSeconds; }
    std::vector<Combatant*> getFighters() const {
        std::vector<Combatant*> f;
        for (int i = 0; i < recruitSystem->getFighterCount(); ++i)
            f.push_back(recruitSystem->getFighter(i));
        return f;
    }
};