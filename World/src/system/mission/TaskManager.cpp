#include "TaskManager.h"
#include "role/character/MainCharacter.h"
#include <iostream>
#include <algorithm>

TaskManager::TaskManager() { loadAllTasks(); }

void TaskManager::loadAllTasks() {
    tasks.clear();

    // ===== 每日任务（5个）=====
    tasks.push_back({"daily_stamina", "stamina", "consume 50 stamina today", "daily", "stamina", 50, 0, 200, 0, 0, false, false, false});
    tasks.push_back({"daily_shop_buy", "shop", "buy 1 item in shop", "daily", "shop_buy", 1, 0, 200, 0, 0, false, false, false});
    tasks.push_back({"daily_clear_stage", "stage", "clear 2 stages", "daily", "clear_stage", 2, 0, 200, 10, 0, false, false, false});
    tasks.push_back({"daily_online", "online", "online 30 minutes", "daily", "online_time", 30, 0, 200, 0, 0, false, false, false});
    tasks.push_back({"daily_login", "login", "login game", "daily", "login", 1, 0, 200, 0, 0, false, false, false});

    // ===== 成就（4个）=====
    tasks.push_back({"achieve_collect_5", "collect 5 chars", "get 5 characters", "achievement", "collect_char", 5, 0, 0, 50, 0, false, false, false});
    tasks.push_back({"achieve_clear_C", "clear zone C", "clear all stages in zone C", "achievement", "clear_zone", 1, 0, 0, 50, 0, false, false, false});
    tasks.push_back({"achieve_recruit_3", "recruit 3 times", "recruit 3 times in total", "achievement", "recruit", 3, 0, 0, 50, 0, false, false, false});
    tasks.push_back({"achieve_login_5", "login 5 days", "login 5 days in total", "achievement", "login_days", 5, 0, 0, 50, 0, false, false, false});

    // ===== 新手任务（4个）=====
    tasks.push_back({"novice_login_3", "login 3 days", "login 3 days", "novice", "login_days", 3, 0, 200, 10, 0, false, false, false});
    tasks.push_back({"novice_shop_buy_5", "shop 5 times", "buy items in shop 5 times", "novice", "shop_buy", 5, 0, 200, 10, 0, false, false, false});
    tasks.push_back({"novice_clear_A", "clear zone A", "clear all stages in zone A", "novice", "clear_zone", 1, 0, 200, 10, 0, false, false, false});
    tasks.push_back({"novice_equip_4", "equip 4 items", "equip 2 items on 2 characters", "novice", "equip", 4, 0, 200, 10, 0, false, false, false});
}

void TaskManager::updateProgress(const std::string& conditionType, int amount) {
    for (auto& task : tasks) {
        if (task.conditionType == conditionType && !task.isCompleted) {
            task.currentProgress += amount;
            if (task.currentProgress >= task.targetValue) {
                task.currentProgress = task.targetValue;
                task.isCompleted = true;
                std::cout << "[OK] Task completed: " << task.name << std::endl;
            }
        }
    }
}

void TaskManager::displayTasksByType(const std::string& type) const {
    std::string label;
    if (type == "daily") label = "Daily Tasks";
    else if (type == "achievement") label = "Achievements";
    else if (type == "novice") label = "Novice Tasks";
    else label = type;

    std::cout << "\n========== " << label << " ==========" << std::endl;
    bool has = false;

    for (const auto& task : tasks) {
        if (task.type == type) {
            has = true;
            std::cout << (task.isCompleted ? "[V] " : "[ ] ") << task.name
                      << " (" << task.currentProgress << "/" << task.targetValue << ")"
                      << " | Rewards:";

            if (type == "achievement") {
                std::cout << " Diamond+" << task.rewardDiamond;
            } else {
                if (task.rewardGold > 0) std::cout << " Gold+" << task.rewardGold;
                if (task.rewardDiamond > 0) {
                    if (task.rewardGold > 0) std::cout << " +";
                    std::cout << " Diamond+" << task.rewardDiamond;
                }
            }

            if (task.isClaimed) std::cout << " [Claimed]";
            std::cout << std::endl;
        }
    }

    if (!has) {
        std::cout << "(No tasks)" << std::endl;
    } else {
        if (type == "daily") {
            std::cout << "\n[!] Complete all daily tasks for extra 10 Diamonds" << std::endl;
        } else if (type == "novice") {
            std::cout << "\n[!] Complete all novice tasks for extra 1 Recruit Card" << std::endl;
        }
    }
    std::cout << std::endl;
}

void TaskManager::displayAllTasks() const {
    displayTasksByType("daily");
    displayTasksByType("achievement");
    displayTasksByType("novice");
}

Task* TaskManager::getTask(const std::string& id) {
    for (auto& task : tasks) {
        if (task.id == id) return &task;
    }
    return nullptr;
}

void TaskManager::resetDailyTasks() {
    for (auto& task : tasks) {
        if (task.type == "daily") {
            task.currentProgress = 0;
            task.isCompleted = false;
            task.isClaimed = false;
        }
    }
}

std::vector<Task*> TaskManager::getClaimableTasks() {
    std::vector<Task*> result;
    for (auto& task : tasks) {
        if (task.isCompleted && !task.isClaimed) result.push_back(&task);
    }
    return result;
}

bool TaskManager::claimTask(const std::string& id, MainCharacter* player) {
    Task* task = getTask(id);
    if (!task || !task->isCompleted || task->isClaimed) return false;
    task->isClaimed = true;
    if (task->rewardGold > 0) player->addGold(task->rewardGold);
    if (task->rewardDiamond > 0) player->addDiamond(task->rewardDiamond);
    if (task->rewardExp > 0) player->addExp(task->rewardExp);
    std::cout << "Claimed reward: " << task->name << std::endl;
    return true;
}

bool TaskManager::checkDailyAllDone(MainCharacter* player) {
    int done = 0;
    for (const auto& task : tasks) {
        if (task.type == "daily" && task.isCompleted && task.isClaimed) done++;
    }
    if (done >= 5) {
        player->addDiamond(10);
        std::cout << "[!] All daily tasks complete! Extra 10 Diamonds!" << std::endl;
        return true;
    }
    return false;
}

int TaskManager::getTaskCountByType(const std::string& type) const {
    int count = 0;
    for (const auto& task : tasks) {
        if (task.type == type) count++;
    }
    return count;
}

int TaskManager::getCompletedCountByType(const std::string& type) const {
    int count = 0;
    for (const auto& task : tasks) {
        if (task.type == type && task.isCompleted) count++;
    }
    return count;
}
