#include "EnemyManager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

EnemyManager::EnemyManager() : currentStage(1), stageCleared(false) {
    static bool seeded = false;
    if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }
    loadStageConfigs();
    buildAllEnemies();
}

void EnemyManager::loadStageConfigs() {
    stageConfigs.clear();

    // ================================================================
    // A区 第1-5关
    // ================================================================
        // 第1关
    stageConfigs.push_back({1, "A", {{"rat", 3, 3000, 150, 0}}, 1000, 30, 500, 50, 100, false});
    // 第2关
    stageConfigs.push_back({2, "A", {{"rat", 3, 300, 150, 0}, {"sunflower", 1, 800, 100, 0}}, 1500, 30, 500, 50, 100, false});
    // 第3关
    stageConfigs.push_back({3, "A", {{"rat", 3, 400, 150, 0}, {"bat", 1, 800, 200, 0}}, 2000, 30, 500, 50, 100, false});
    // 第4关
    stageConfigs.push_back({4, "A", {{"rat", 3, 400, 200, 0}, {"bat", 1, 800, 200, 0}}, 2000, 30, 500, 50, 100, false});
    // 第5关
    stageConfigs.push_back({5, "A", {{"rat", 3, 500, 100, 0}, {"boss_bat", 1, 500, 300, 0}}, 3000, 30, 500, 50, 100, true});

    // ================================================================
    // B区 第6-10关（属性+20%，奖励+20%）
    // ================================================================
    stageConfigs.push_back({6, "B", {{"rat", 3, 360, 180, 0}}, 1200, 36, 600, 60, 120, false});
    stageConfigs.push_back({7, "B", {{"rat", 3, 360, 180, 0}, {"sunflower", 1, 960, 120, 0}}, 1800, 36, 600, 60, 120, false});
    stageConfigs.push_back({8, "B", {{"rat", 3, 480, 180, 0}, {"bat", 1, 960, 240, 0}}, 2400, 36, 600, 60, 120, false});
    stageConfigs.push_back({9, "B", {{"rat", 3, 480, 240, 0}, {"bat", 1, 960, 240, 0}}, 2400, 36, 600, 60, 120, false});
    stageConfigs.push_back({10, "B", {{"rat", 3, 600, 120, 0}, {"boss_bat", 1, 600, 360, 0}}, 3600, 36, 600, 60, 120, true});

    // ================================================================
    // C区 第11-15关（属性+40%，奖励+40%）
    // ================================================================
    stageConfigs.push_back({11, "C", {{"rat", 3, 420, 210, 0}}, 1400, 42, 700, 70, 140, false});
    stageConfigs.push_back({12, "C", {{"rat", 3, 420, 210, 0}, {"sunflower", 1, 1120, 140, 0}}, 2100, 42, 700, 70, 140, false});
    stageConfigs.push_back({13, "C", {{"rat", 3, 560, 210, 0}, {"bat", 1, 1120, 280, 0}}, 2800, 42, 700, 70, 140, false});
    stageConfigs.push_back({14, "C", {{"rat", 3, 560, 280, 0}, {"bat", 1, 1120, 280, 0}}, 2800, 42, 700, 70, 140, false});
    stageConfigs.push_back({15, "C", {{"rat", 3, 700, 140, 0}, {"boss_bat", 1, 700, 420, 0}}, 4200, 42, 700, 70, 140, true});
}

void EnemyManager::buildAllEnemies() {
    allEnemies.clear();
    int idCounter = 0;
    for (const auto& cfg : stageConfigs) {
        int enemyIndex = 0;
        for (const auto& wave : cfg.waves) {
            for (int i = 0; i < wave.count; ++i) {
                allEnemies.push_back(createEnemyFromWave(
                    wave.enemyType, wave.hp, wave.attack, wave.defense, ++enemyIndex));
            }
        }
    }
}

Enemy EnemyManager::createEnemyFromWave(const std::string& type, int hp, int attack, int defense, int index) {
    static int idCounter = 0;
    std::string id = type + "_" + std::to_string(++idCounter);
    std::string name, desc, element;
    int exp = 0, gold = 0, diamond = 0;

    if (type == "rat") {
        name = "老鼠" + std::to_string(index);
        desc = "机械系敌人";
        element = "mechanical";
        exp = 50;
        gold = 100;
        diamond = 0;
    } else if (type == "sunflower") {
        name = "太阳花" + std::to_string(index);
        desc = "水系敌人";
        element = "water";
        exp = 80;
        gold = 150;
        diamond = 5;
    } else if (type == "bat") {
        name = "蝙蝠" + std::to_string(index);
        desc = "火系敌人";
        element = "fire";
        exp = 100;
        gold = 200;
        diamond = 10;
    } else if (type == "boss_bat") {
        name = "Boss蝙蝠" + std::to_string(index);
        desc = "火系Boss";
        element = "fire";
        exp = 150;
        gold = 300;
        diamond = 15;
    }

    return Enemy(id, name, desc, element, hp, attack, defense, exp, gold, diamond);
}

void EnemyManager::buildStage(int stageId) {
    currentEnemies.clear();
    const StageConfig* cfg = getStageConfig(stageId);
    if (!cfg) return;

    int enemyIndex = 0;
    int idCounter = 0;
    for (const auto& wave : cfg->waves) {
        for (int i = 0; i < wave.count; ++i) {
            currentEnemies.push_back(createEnemyFromWave(
                wave.enemyType, wave.hp, wave.attack, wave.defense, ++enemyIndex));
        }
    }
}

void EnemyManager::generateNewStage() {
    currentEnemies.clear();
    stageCleared = false;
    if (currentStage > 15) {
        std::cout << "所有关卡已通关！" << std::endl;
        return;
    }
    buildStage(currentStage);
    std::cout << "\n=== 第 " << currentStage << " 关 (" << getStageConfig(currentStage)->zone << "区) ===" << std::endl;
    displayCurrentEnemies();
}

std::vector<Enemy>& EnemyManager::getCurrentEnemies() { return currentEnemies; }

Enemy* EnemyManager::getEnemyById(const std::string& id) {
    for (auto& e : currentEnemies) {
        if (e.getId() == id) return &e;
    }
    return nullptr;
}

bool EnemyManager::isStageCleared() const {
    for (const auto& e : currentEnemies) {
        if (e.isAlive()) return false;
    }
    return true;
}

int EnemyManager::getCurrentStage() const { return currentStage; }

void EnemyManager::displayCurrentEnemies() const {
    for (const auto& e : currentEnemies) {
        e.displayInfo();
    }
}

void EnemyManager::resetAll() {
    currentStage = 1;
    clearedStages.clear();
    clearedZones.clear();
    generateNewStage();
}
void EnemyManager::initFirstStage() {
    currentStage = 1;
    stageCleared = false;
    buildStage(1);
    const StageConfig* cfg = getStageConfig(1);
    std::cout << "\n=== 第 1 关 (" << (cfg ? cfg->zone : "A") << "区) ===" << std::endl;
    displayCurrentEnemies();
}

bool EnemyManager::canSweep(int stageId) const {
    return std::find(clearedStages.begin(), clearedStages.end(), stageId) != clearedStages.end();
}

int EnemyManager::getSweepExp(int stageId) const {
    const StageConfig* cfg = getStageConfig(stageId);
    return cfg ? cfg->sweepExp : 0;
}

int EnemyManager::getSweepGold(int stageId) const {
    const StageConfig* cfg = getStageConfig(stageId);
    return cfg ? cfg->sweepGold : 0;
}

int EnemyManager::getFirstClearGold(int stageId) const {
    const StageConfig* cfg = getStageConfig(stageId);
    return cfg ? cfg->firstClearGold : 0;
}

int EnemyManager::getFirstClearDiamond(int stageId) const {
    const StageConfig* cfg = getStageConfig(stageId);
    return cfg ? cfg->firstClearDiamond : 0;
}

int EnemyManager::getFirstClearExp(int stageId) const {
    const StageConfig* cfg = getStageConfig(stageId);
    return cfg ? cfg->firstClearExp : 0;
}

void EnemyManager::markStageCleared(int stageId) {
    if (std::find(clearedStages.begin(), clearedStages.end(), stageId) == clearedStages.end()) {
        clearedStages.push_back(stageId);
    }
}

bool EnemyManager::isStageCleared(int stageId) const {
    return std::find(clearedStages.begin(), clearedStages.end(), stageId) != clearedStages.end();
}

std::vector<int> EnemyManager::getClearedStages() const { return clearedStages; }

bool EnemyManager::isZoneUnlocked(const std::string& zone) const {
    if (zone == "A") return true;
    if (zone == "B") {
        // 检查A区5关是否全部通关
        for (int i = 1; i <= 5; ++i) {
            if (!isStageCleared(i)) return false;
        }
        return true;
    }
    if (zone == "C") {
        // 检查B区5关是否全部通关
        for (int i = 6; i <= 10; ++i) {
            if (!isStageCleared(i)) return false;
        }
        return true;
    }
    return false;
}

void EnemyManager::markZoneCleared(const std::string& zone) {
    int zoneId = (zone == "A") ? 1 : (zone == "B") ? 2 : 3;
    if (std::find(clearedZones.begin(), clearedZones.end(), zoneId) == clearedZones.end()) {
        clearedZones.push_back(zoneId);
    }
}

bool EnemyManager::isZoneCleared(const std::string& zone) const {
    int zoneId = (zone == "A") ? 1 : (zone == "B") ? 2 : 3;
    return std::find(clearedZones.begin(), clearedZones.end(), zoneId) != clearedZones.end();
}

const StageConfig* EnemyManager::getStageConfig(int stageId) const {
    for (const auto& cfg : stageConfigs) {
        if (cfg.stageId == stageId) return &cfg;
    }
    return nullptr;
}

int EnemyManager::getTotalStages() const { return 15; }

bool EnemyManager::isStageUnlocked(int stageId) const {
    if (stageId <= 1) return true;        // 第1关始终解锁
    // 其他关卡需要前一关已通关
    return isStageCleared(stageId - 1);
}