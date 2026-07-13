#pragma once

#include <vector>
#include <string>
#include "Enemy.h"

// 波次配置
struct WaveConfig {
    std::string enemyType;   // "rat" / "sunflower" / "bat" / "boss_bat"
    int count;               // 数量
    int hp;                  // 生命值
    int attack;              // 攻击力
    int defense;             // 防御力
};

// 关卡配置
struct StageConfig {
    int stageId;
    std::string zone;        // "A" / "B" / "C"
    std::vector<WaveConfig> waves;
    int firstClearGold;
    int firstClearDiamond;
    int firstClearExp;
    int sweepExp;
    int sweepGold;
    bool isBoss;             // 是否为大关
};

class EnemyManager {
private:
    std::vector<Enemy> currentEnemies;
    std::vector<Enemy> allEnemies;
    int currentStage;
    bool stageCleared;
    std::vector<StageConfig> stageConfigs;
    std::vector<int> clearedStages;
    std::vector<int> clearedZones;  // 已通关区域 "A"=1, "B"=2, "C"=3

    void loadStageConfigs();
    Enemy createEnemyFromWave(const std::string& type, int hp, int attack, int defense, int index);
    void buildAllEnemies();

public:
    EnemyManager();

    void generateNewStage();
    std::vector<Enemy>& getCurrentEnemies();
    Enemy* getEnemyById(const std::string& id);
    bool isStageCleared() const;
    int getCurrentStage() const;
    void displayCurrentEnemies() const;
    void resetAll();
    void initFirstStage();
    void buildStage(int stageId);

    // 扫荡
    bool canSweep(int stageId) const;
    int getSweepExp(int stageId) const;
    int getSweepGold(int stageId) const;

    // 关卡奖励
    int getFirstClearGold(int stageId) const;
    int getFirstClearDiamond(int stageId) const;
    int getFirstClearExp(int stageId) const;

    // 已通关
    void markStageCleared(int stageId);
    bool isStageCleared(int stageId) const;
    std::vector<int> getClearedStages() const;

    // 区域解锁
    bool isZoneUnlocked(const std::string& zone) const;
    void markZoneCleared(const std::string& zone);
    bool isZoneCleared(const std::string& zone) const;

    // 关卡解锁
    bool isStageUnlocked(int stageId) const;  // 第1关始终解锁，其他关需要前一关已通关

    // 获取关卡配置
    const StageConfig* getStageConfig(int stageId) const;
    int getTotalStages() const;
};