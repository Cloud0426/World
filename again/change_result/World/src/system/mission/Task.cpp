#include "Task.h"
#include <iostream>
#include <algorithm>

// ============================================================
// 构造函数
// ============================================================
Task::Task(const std::string& id,
           const std::string& name,
           const std::string& description,
           const std::string& type,
           const std::string& conditionType,
           int targetValue,
           int rewardGold,
           int rewardDiamond,
           int rewardExp,
           int currentProgress,
           bool isCompleted,
           bool isClaimed)
    : id(id), name(name), description(description),
      type(type), conditionType(conditionType),
      targetValue(targetValue),
      currentProgress(currentProgress),
      rewardGold(rewardGold), rewardDiamond(rewardDiamond),
      rewardExp(rewardExp),
      isCompleted(isCompleted), isClaimed(isClaimed) {
}

// ============================================================
// Getter
// ============================================================
std::string Task::getId()              const { return id; }
std::string Task::getName()            const { return name; }
std::string Task::getDescription()     const { return description; }
std::string Task::getType()            const { return type; }
std::string Task::getConditionType()   const { return conditionType; }
int         Task::getTargetValue()     const { return targetValue; }
int         Task::getCurrentProgress() const { return currentProgress; }
int         Task::getRewardGold()      const { return rewardGold; }
int         Task::getRewardDiamond()   const { return rewardDiamond; }
int         Task::getRewardExp()       const { return rewardExp; }
bool        Task::getIsCompleted()     const { return isCompleted; }
bool        Task::getIsClaimed()       const { return isClaimed; }

// ============================================================
// addProgress — 增加进度，自动检查是否完成
// ============================================================
void Task::addProgress(int amount) {
    if (amount <= 0) return;
    if (isCompleted) return;           // 已经完成不再累加

    currentProgress += amount;
    if (currentProgress > targetValue) {
        currentProgress = targetValue;
    }

    // 自动检查是否达到目标
    checkCompletion();
}

// ============================================================
// checkCompletion — 检查是否达到目标，更新 isCompleted
// ============================================================
bool Task::checkCompletion() {
    if (isCompleted) return true;      // 已经完成
    if (currentProgress >= targetValue) {
        isCompleted = true;
        std::cout << "✔ 任务完成: " << name << std::endl;
        return true;
    }
    return false;
}

// ============================================================
// claimReward — 领取奖励
// ============================================================
void Task::claimReward() {
    if (!canClaim()) return;
    isClaimed = true;
    std::cout << "已领取奖励: " << name
              << "（金币+" << rewardGold
              << "，钻石+" << rewardDiamond
              << "，经验+" << rewardExp << "）" << std::endl;
}

// ============================================================
// canClaim — 是否可以领取（已完成且未领取）
// ============================================================
bool Task::canClaim() const {
    return isCompleted && !isClaimed;
}

// ============================================================
// reset — 重置任务进度（每日任务使用）
// ============================================================
void Task::reset() {
    currentProgress = 0;
    isCompleted = false;
    isClaimed = false;
}

// ============================================================
// displayInfo — 显示任务信息
// ============================================================
void Task::displayInfo() const {
    std::cout << "[" << type << "] ";

    // 已完成的任务在名称后显示 " ✔"
    if (isCompleted) {
        std::cout << name << " ✔";
    } else {
        std::cout << name;
    }

    std::cout << " | " << description
              << "（" << currentProgress << "/" << targetValue << "）";

    if (rewardGold > 0 || rewardDiamond > 0 || rewardExp > 0) {
        std::cout << " | 奖励:";
        if (rewardGold > 0)    std::cout << " 金币+" << rewardGold;
        if (rewardDiamond > 0) std::cout << " 钻石+" << rewardDiamond;
        if (rewardExp > 0)     std::cout << " 经验+" << rewardExp;
    }

    if (isClaimed) {
        std::cout << " [已领取]";
    }

    std::cout << std::endl;
}
