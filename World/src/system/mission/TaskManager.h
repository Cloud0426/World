#pragma once

#include <vector>
#include <string>
#include "role/character/MainCharacter.h"

struct Task {
    std::string id;
    std::string name;
    std::string description;
    std::string type;          // "daily" / "achievement" / "novice"
    std::string conditionType; // "stamina" / "shop_buy" / "clear_stage" / "online_time" / "login" /
                               // "collect_char" / "clear_zone" / "recruit" / "login_days" / "equip"
    int targetValue;
    int currentProgress;
    int rewardGold;
    int rewardDiamond;
    int rewardExp;
    bool isCompleted;
    bool isClaimed;
    bool isExtraReward;        // 每日任务全部完成额外奖励标记
};

class TaskManager {
private:
    std::vector<Task> tasks;

public:
    TaskManager();
    void loadAllTasks();
    void updateProgress(const std::string& conditionType, int amount = 1);
    void displayTasksByType(const std::string& type) const;
    void displayAllTasks() const;
    Task* getTask(const std::string& id);
    void resetDailyTasks();
    std::vector<Task*> getClaimableTasks();
    bool claimTask(const std::string& id, MainCharacter* player);
    bool checkDailyAllDone(MainCharacter* player);  // 全部完成额外+10钻石
    int getTaskCountByType(const std::string& type) const;
    int getCompletedCountByType(const std::string& type) const;
};