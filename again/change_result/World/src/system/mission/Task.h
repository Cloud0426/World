#pragma once

#include <string>

struct Task {
    std::string id;                 // 任务唯一标识
    std::string name;               // 任务名称
    std::string description;        // 任务描述
    std::string type;               // 任务类型："daily" / "achievement" / "novice"
    std::string conditionType;      // 条件类型
    int targetValue;                // 目标数值
    int currentProgress;            // 当前进度
    int rewardGold;                 // 奖励金币
    int rewardDiamond;              // 奖励钻石
    int rewardExp;                  // 奖励经验
    bool isCompleted;               // 是否已完成目标
    bool isClaimed;                 // 是否已领取奖励

    // 构造函数
    Task(const std::string& id,
         const std::string& name,
         const std::string& description,
         const std::string& type,
         const std::string& conditionType,
         int targetValue,
         int rewardGold = 0,
         int rewardDiamond = 0,
         int rewardExp = 0,
         int currentProgress = 0,
         bool isCompleted = false,
         bool isClaimed = false);

    // ---- getter ----
    std::string getId()              const;
    std::string getName()            const;
    std::string getDescription()     const;
    std::string getType()            const;
    std::string getConditionType()   const;
    int         getTargetValue()     const;
    int         getCurrentProgress() const;
    int         getRewardGold()      const;
    int         getRewardDiamond()   const;
    int         getRewardExp()       const;
    bool        getIsCompleted()     const;
    bool        getIsClaimed()       const;

    // ---- 核心方法 ----
    void addProgress(int amount = 1);           // 增加进度，自动检查完成
    bool checkCompletion();                     // 检查是否达到目标，更新 isCompleted
    void claimReward();                         // 领取奖励，设置 isClaimed = true
    bool canClaim() const;                      // 已完成且未领取
    void reset();                               // 重置任务进度（每日任务用）
    void displayInfo() const;                   // 显示任务信息
};
