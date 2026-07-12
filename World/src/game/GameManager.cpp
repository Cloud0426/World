// ============================================================
// GameManager.cpp
// ============================================================
#include "GameManager.h"
#include "ui/SplashScreen.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================
// 构造函数
// ============================================================
GameManager::GameManager()
    : mainChar(nullptr), inventory(nullptr), shop(nullptr),
      taskManager(nullptr), enemyManager(nullptr), recruitSystem(nullptr),
      isRunning(true), hasSave(false), consecutiveDays(0),
      totalOnlineSeconds(0), dailyOnlineSeconds(0), sessionStartTime(0), lastLoginTime(0), lastStaminaRecover(0) {
    
    std::srand((unsigned)std::time(nullptr));
    
        // 初始化所有子系统对象（空状态），图形界面会负责填充
        mainChar = new MainCharacter("", "2000-01-01", "", true);
        inventory = new Inventory();
        shop = new Shop();
        taskManager = new TaskManager();
        enemyManager = new EnemyManager();
        recruitSystem = new RecruitSystem();
    
        // 检查存档
        std::ifstream check("save.dat");
        if (check.good()) {
            hasSave = true;
            check.close();
            loadGame();
        }
        // 无存档时由 ShowSplashScreen 处理
}

// ============================================================
// 析构函数
// ============================================================
GameManager::~GameManager() {
    delete mainChar;
    delete inventory;
    delete shop;
    delete taskManager;
    delete enemyManager;
    delete recruitSystem;
}

// ============================================================
// run — 主循环
// ============================================================
void GameManager::run() {
    while (isRunning) {
        showMainMenu();
    }
}

// ============================================================
// autoClaimRewards — 自动领取所有已完成任务的奖励
// ============================================================
void GameManager::autoClaimRewards() {
    auto claimable = taskManager->getClaimableTasks();
    for (auto* t : claimable) {
        if (!t->isClaimed) {
            if (taskManager->claimTask(t->id, mainChar)) {
                std::cout << "🎉 自动领取任务奖励：" << t->name << std::endl;
            }
        }
    }
    // 检查每日任务全部完成后的额外奖励
    taskManager->checkDailyAllDone(mainChar);
}

// ============================================================
// getChineseCharCount — 获取字符串中汉字字符的个数（UTF-8 编码）
// ============================================================
int GameManager::getChineseCharCount(const std::string& str) const {
    int count = 0;
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = (unsigned char)str[i];
        // UTF-8 多字节序列判断
        if (c >= 0xF0) {
            // 4字节字符（如部分表情符号），跳过
            i += 4;
        } else if (c >= 0xE0) {
            // 3字节字符（如中文字符）
            count++;
            i += 3;
        } else if (c >= 0xC0) {
            // 2字节字符
            count++;
            i += 2;
        } else {
            // 1字节字符（ASCII）
            count++;
            i += 1;
        }
    }
    return count;
}

// ============================================================
// isValidDate — 校验生日格式为 YYYY-MM-DD 且日期真实有效
// ============================================================
bool GameManager::isValidDate(const std::string& date) const {
    // 格式长度校验
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    // 检查前4位、第5-6位、第8-9位是否都是数字
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return false;
    }

    int year  = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day   = std::stoi(date.substr(8, 2));

    // 年份范围
    if (year < 1900 || year > 2026) return false;
    // 月份范围
    if (month < 1 || month > 12) return false;
    // 日期范围
    if (day < 1 || day > 31) return false;

    // 每月最大天数
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // 闰年判断
    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeap) daysInMonth[2] = 29;

    return day <= daysInMonth[month];
}

// ============================================================
// initNewGame — 创建新游戏
// ============================================================
void GameManager::initNewGame() {
    std::cout << "\n========== 创建新角色 ==========" << std::endl;
    std::string name, birthday, signature;
    int gender;

    // 角色名称
    std::cout << "输入角色名称: ";
    std::cin >> name;

    // 生日校验
    while (true) {
        std::cout << "输入生日 (YYYY-MM-DD): ";
        std::cin >> birthday;
        if (isValidDate(birthday)) break;
        std::cout << "日期无效！请按 YYYY-MM-DD 格式输入有效日期（1900-2026年）" << std::endl;
    }

    // 个性签名校验（不超过10个字）
    std::cout << "输入个性签名（不超过10个字）: ";
    std::cin.ignore();
    std::getline(std::cin, signature);
    while (getChineseCharCount(signature) > 10) {
        std::cout << "个性签名不能超过10个字，请重新输入: ";
        std::getline(std::cin, signature);
    }

    // 性别校验
    while (true) {
        std::cout << "选择性别 [1]男 [2]女: ";
        std::cin >> gender;
        if (gender == 1 || gender == 2) break;
        std::cout << "无效选择，请重新输入（1=男，2=女）" << std::endl;
    }

    mainChar = new MainCharacter(name, birthday, signature, gender == 1);
    inventory = new Inventory();
    shop = new Shop();
    taskManager = new TaskManager();
    enemyManager = new EnemyManager();
    recruitSystem = new RecruitSystem();

    // ✅ 先显示角色创建成功
    std::cout << "\n角色创建成功！" << std::endl;
    mainChar->displayInfo();

    // ✅ 再发放初始物资
    giveInitialItems();

    // 初始化时间
    time_t now = time(nullptr);
    lastLoginTime = now;
    lastStaminaRecover = now;
    consecutiveDays = 1;
    totalOnlineSeconds = 0;
    dailyOnlineSeconds = 0;
    sessionStartTime = now;

    // ✅ 最后触发登录任务并自动发放奖励
    taskManager->updateProgress("login", 1);
    taskManager->updateProgress("login_days", 1);
    autoClaimRewards();

    saveGame();
}
// ============================================================
// giveInitialItems — 发放初始物资
// ============================================================
void GameManager::giveInitialItems() {
    // 金币
    mainChar->addGold(500);

    // 止痛片 x3
    inventory->addItem("potion_003", 3);

    // 升级石 x2
    inventory->addItem("upgrade_stone", 2);

    // 初级护盾 x1（直接放入背包）
    inventory->addItem("weapon_shield_1", 1);

    std::cout << "初始物资已发放：500金币，3个止痛片，1个初级护盾，2个升级石" << std::endl;

    // 初始角色：机械师
    Combatant* mechanic = new Combatant("机械师", "机械学院");
    recruitSystem->addFighterDirect(mechanic);
    std::cout << "恭喜你获得新的伙伴：机械师！一起并肩作战吧！" << std::endl;

    // 🆕 更新角色收集任务进度
    taskManager->updateProgress("collect_char", 1);
    taskManager->updateProgress("recruit", 1);
}

// ============================================================
// loadGame — 加载存档
// ============================================================
void GameManager::loadGame() {
    std::ifstream file("save.dat");
    if (!file) {
        std::cout << "存档文件不存在" << std::endl;
        return;
    }

    // 先创建各系统对象，准备接收数据
    if (!mainChar) mainChar = new MainCharacter("", "2000-01-01", "加载存档", true);
    if (!inventory) inventory = new Inventory();
    if (!shop) shop = new Shop();
    if (!taskManager) taskManager = new TaskManager();
    if (!enemyManager) {
        enemyManager = new EnemyManager();
    }
    if (!recruitSystem) recruitSystem = new RecruitSystem();

    // 存档解析临时变量
    std::vector<std::string> pendingFighterEquips;  // 存装备行，等角色创建完后处理

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "main_name") {
            mainChar->setName(value);
        } else if (key == "main_birthday") {
            mainChar->setBirthday(value);
        } else if (key == "main_signature") {
            mainChar->setSignature(value);
        } else if (key == "main_isMale") {
            // 通过构造函数已设置
        } else if (key == "main_level") {
            mainChar->setLevel(std::stoi(value));
        } else if (key == "main_exp") {
            mainChar->setExp(std::stoi(value));
        } else if (key == "main_expToNext") {
            mainChar->setExpToNext(std::stoi(value));
        } else if (key == "main_gold") {
            mainChar->setGold(std::stoi(value));
        } else if (key == "main_diamond") {
            mainChar->setDiamond(std::stoi(value));
        } else if (key == "main_stamina") {
            mainChar->setStamina(std::stoi(value));
        } else if (key == "main_maxStamina") {
            mainChar->setMaxStamina(std::stoi(value));
        } else if (key == "inv_item") {
            // 解析 "itemId,count"
            size_t comma = value.find(',');
            if (comma != std::string::npos) {
                std::string itemId = value.substr(0, comma);
                int count = std::stoi(value.substr(comma + 1));
                inventory->addItem(itemId, count);
            }
        } else if (key == "fighter") {
            // 解析 "name,college,level,hp,maxHp,attack,defense,mp,maxMp,critRate"
            std::stringstream ss(value);
            std::string token;
            std::vector<std::string> parts;
            while (std::getline(ss, token, ',')) parts.push_back(token);
            if (parts.size() >= 10) {
                Combatant* f = new Combatant(parts[0], parts[1]);
                f->setLevel(std::stoi(parts[2]));
                f->setHp(std::stoi(parts[3]));
                f->setMaxHp(std::stoi(parts[4]));
                f->setAttack(std::stoi(parts[5]));
                f->setDefense(std::stoi(parts[6]));
                f->setMp(std::stoi(parts[7]));
                f->setMaxMp(std::stoi(parts[8]));
                f->setCritRate(std::stod(parts[9]));
                recruitSystem->addFighterDirect(f);
            }
        } else if (key == "fighter_equip") {
            // 暂存，等角色创建完后处理
            pendingFighterEquips.push_back(value);
        } else if (key == "cleared_stages") {
            // 解析逗号分隔的关卡ID
            if (!value.empty()) {
                std::stringstream ss(value);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    if (!token.empty()) {
                        enemyManager->markStageCleared(std::stoi(token));
                    }
                }
            }
        } else if (key == "lastLoginTime") {
            lastLoginTime = std::stoll(value);
        } else if (key == "lastStaminaRecover") {
            lastStaminaRecover = std::stoll(value);
        } else if (key == "consecutiveDays") {
            consecutiveDays = std::stoi(value);
        } else if (key == "totalOnlineSeconds") {
            totalOnlineSeconds = std::stoll(value);
        } else if (key == "dailyOnlineSeconds") {
            dailyOnlineSeconds = std::stoll(value);
        }
    }
    file.close();

    // 恢复角色装备（角色已创建完毕）
    for (const std::string& equipLine : pendingFighterEquips) {
        // 解析 "name,college,id,name,type,level,attackBonus,defenseBonus,mpBonus,hpRestore"
        std::stringstream ss(equipLine);
        std::string token;
        std::vector<std::string> parts;
        while (std::getline(ss, token, ',')) parts.push_back(token);
        if (parts.size() >= 10) {
            // 找到对应角色
            for (int i = 0; i < recruitSystem->getFighterCount(); ++i) {
                Combatant* f = recruitSystem->getFighter(i);
                if (f->getName() == parts[0] && f->getCollege() == parts[1]) {
                    Equipment eq;
                    eq.id = parts[2];
                    eq.name = parts[3];
                    eq.type = parts[4];
                    eq.level = std::stoi(parts[5]);
                    eq.attackBonus = std::stod(parts[6]);
                    eq.defenseBonus = std::stod(parts[7]);
                    eq.mpBonus = std::stod(parts[8]);
                    eq.hpRestore = std::stoi(parts[9]);
                    f->equipItem(eq);
                    break;
                }
            }
        }
    }

    // 恢复会话开始时间
    sessionStartTime = time(nullptr);

    // 检查离线体力恢复
    checkStaminaRecover();

    // 基于真实日期判断连续登录
    {
        time_t now = time(nullptr);
        struct tm* lastTm = localtime(&lastLoginTime);
        struct tm* nowTm = localtime(&now);

        // 计算 lastLogin 日期的"天数序号"（从公元0年开始）
        auto toDays = [](const struct tm* t) -> int {
            int y = t->tm_year + 1900;
            int m = t->tm_mon + 1;
            int d = t->tm_mday;
            // 累加年份天数
            int days = (y - 1) * 365 + (y - 1) / 4 - (y - 1) / 100 + (y - 1) / 400;
            // 累加月份天数
            int monthDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            for (int i = 1; i < m; ++i) {
                days += monthDays[i];
                if (i == 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)))
                    days += 1; // 闰年2月多1天
            }
            days += d;
            return days;
        };

        int lastDays = toDays(lastTm);
        int nowDays = toDays(nowTm);
        int diff = nowDays - lastDays;

        if (diff == 0) {
            // 同一天登录，天数不变
            std::cout << "欢迎回来！今天是连续登录第 " << consecutiveDays << " 天" << std::endl;
        } else if (diff == 1) {
            // 连续第二天登录
            consecutiveDays++;
            std::cout << "连续登录 " << consecutiveDays << " 天！" << std::endl;
        } else {
            // 间隔超过1天，重置
            consecutiveDays = 1;
            std::cout << "重新开始计算登录天数" << std::endl;
        }

        lastLoginTime = now;
    }

    // 每日登录任务
    taskManager->updateProgress("login", 1);
    taskManager->updateProgress("login_days", 1);

    // 自动发放奖励
    autoClaimRewards();

    // 首次加载存档后初始化第1关（玩家进入地图时才会显示）
    if (enemyManager->getCurrentEnemies().empty()) {
        enemyManager->initFirstStage();
    }

    std::cout << "存档加载成功！" << std::endl;
}

// ============================================================
// saveGame — 保存游戏
// ============================================================
void GameManager::saveGame() {
    std::ofstream file("save.dat");
    if (!file) {
        std::cout << "存档保存失败！" << std::endl;
        return;
    }

    // 先更新在线时长（确保数据最新）
    updateOnlineTime();

    // ===== 主控数据 =====
    file << "main_name=" << mainChar->getName() << "\n";
    file << "main_birthday=" << mainChar->getBirthday() << "\n";
    file << "main_signature=" << mainChar->getSignature() << "\n";
    file << "main_isMale=" << mainChar->getIsMale() << "\n";
    file << "main_level=" << mainChar->getLevel() << "\n";
    file << "main_exp=" << mainChar->getExp() << "\n";
    file << "main_expToNext=" << mainChar->getExpToNext() << "\n";
    file << "main_gold=" << mainChar->getGold() << "\n";
    file << "main_diamond=" << mainChar->getDiamond() << "\n";
    file << "main_stamina=" << mainChar->getStamina() << "\n";
    file << "main_maxStamina=" << mainChar->getMaxStamina() << "\n";

    // ===== 背包物品 =====
    for (const auto& pair : inventory->getAllItems()) {
        file << "inv_item=" << pair.first << "," << pair.second << "\n";
    }

    // ===== 角色列表 =====
    for (int i = 0; i < recruitSystem->getFighterCount(); ++i) {
        Combatant* f = recruitSystem->getFighter(i);
        file << "fighter=" << f->getName() << "," << f->getCollege() << ","
             << f->getLevel() << "," << f->getHp() << "," << f->getMaxHp() << ","
             << f->getAttack() << "," << f->getDefense() << ","
             << f->getMp() << "," << f->getMaxMp() << ","
             << f->getCritRate() << "\n";

        // 保存装备
        std::vector<Equipment> eqs = f->getEquipped();
        for (const auto& eq : eqs) {
            file << "fighter_equip=" << f->getName() << "," << f->getCollege() << ","
                 << eq.id << "," << eq.name << "," << eq.type << ","
                 << eq.level << "," << eq.attackBonus << "," << eq.defenseBonus << ","
                 << eq.mpBonus << "," << eq.hpRestore << "\n";
        }
    }

    // ===== 已通关关卡 =====
    auto cleared = enemyManager->getClearedStages();
    file << "cleared_stages=";
    for (size_t i = 0; i < cleared.size(); ++i) {
        if (i > 0) file << ",";
        file << cleared[i];
    }
    file << "\n";

    // ===== 当前关卡 =====
    file << "current_stage=" << enemyManager->getCurrentStage() << "\n";

    // ===== 时间数据 =====
    file << "lastLoginTime=" << lastLoginTime << "\n";
    file << "lastStaminaRecover=" << lastStaminaRecover << "\n";
    file << "consecutiveDays=" << consecutiveDays << "\n";
    file << "totalOnlineSeconds=" << totalOnlineSeconds << "\n";
    file << "dailyOnlineSeconds=" << dailyOnlineSeconds << "\n";

    file.close();
}

// ============================================================
// autoSave — 自动存档 + 在线时长任务更新
// ============================================================
void GameManager::autoSave() {
    // 1. 更新在线时长（累计本次会话时间）
    updateOnlineTime();

    // 2. 在线时长任务进度（每60秒更新1次，基于当日在线时长）
    static long long lastDailyOnlineUpdate = 0;
    if (lastDailyOnlineUpdate == 0) {
        lastDailyOnlineUpdate = dailyOnlineSeconds;
    }
    // 每60秒累计1分钟，更新进度
    long long diff = dailyOnlineSeconds - lastDailyOnlineUpdate;
    if (diff >= 60) {
        int minutes = (int)(diff / 60);
        taskManager->updateProgress("online_time", minutes);
        lastDailyOnlineUpdate += minutes * 60;
        // 自动发放奖励
        autoClaimRewards();
    }

    // 3. 检查每日任务重置（跨天时重置）
    checkDailyReset();

    // 4. 检查离线体力恢复（每次操作时检查）
    checkStaminaRecover();

    // 5. 保存游戏
    saveGame();
}

// ============================================================
// updateOnlineTime — 更新在线时长
// ============================================================
void GameManager::updateOnlineTime() {
    if (sessionStartTime == 0) {
        sessionStartTime = time(nullptr);
        return;
    }
    time_t now = time(nullptr);
    long long elapsed = (long long)difftime(now, sessionStartTime);
    totalOnlineSeconds += elapsed;
    dailyOnlineSeconds += elapsed;
    sessionStartTime = now;
}

// ============================================================
// checkDailyReset — 检查每日任务重置
// ============================================================
void GameManager::checkDailyReset() {
    if (lastLoginTime == 0) return;
    
    time_t now = time(nullptr);
    struct tm* lastTm = localtime(&lastLoginTime);
    struct tm* nowTm = localtime(&now);
    
    if (lastTm->tm_mday != nowTm->tm_mday ||
        lastTm->tm_mon != nowTm->tm_mon ||
        lastTm->tm_year != nowTm->tm_year) {
        // 新的一天，重置每日任务
        if (taskManager) {
            taskManager->resetDailyTasks();
            std::cout << "每日任务已重置" << std::endl;
        }
        if (shop) {
            shop->resetDailyLimits();
        }
        // 重置当日在线时长
        dailyOnlineSeconds = 0;
    }
}

// ============================================================
// checkStaminaRecover — 检查离线体力恢复（每5点提示一次）
// ============================================================
void GameManager::checkStaminaRecover() {
    if (!mainChar) return;
    if (lastStaminaRecover == 0) {
        lastStaminaRecover = time(nullptr);
        return;
    }

    // 如果体力已满，不需要恢复
    if (mainChar->getStamina() >= mainChar->getMaxStamina()) {
        lastStaminaRecover = time(nullptr);
        return;
    }
    
    time_t now = time(nullptr);
    double seconds = difftime(now, lastStaminaRecover);
    // 每300秒（5分钟）恢复1体力
    int recover = (int)(seconds / 300);
    if (recover > 0) {
        int maxStamina = mainChar->getMaxStamina();
        int oldStamina = mainChar->getStamina();
        int newStamina = oldStamina + recover;
        if (newStamina > maxStamina) {
            recover = maxStamina - oldStamina;
        }
        if (recover > 0) {
            // 先恢复体力（分批次恢复，以便显示中间状态）
            int totalRecovered = 0;
            int currentStamina = oldStamina;
            int batchSize = 5;
            
            // 每5点恢复一次并提示
            while (totalRecovered < recover) {
                int thisBatch = batchSize;
                if (totalRecovered + thisBatch > recover) {
                    thisBatch = recover - totalRecovered;
                }
                currentStamina += thisBatch;
                totalRecovered += thisBatch;
                mainChar->setStamina(currentStamina);
                std::cout << "体力恢复 " << thisBatch << " 点（当前: " 
                          << currentStamina << "/" << maxStamina << "）" << std::endl;
            }
            
            // 更新恢复时间
            lastStaminaRecover += recover * 300;
        }
    }
}
// ============================================================
// showMainMenu — 主菜单
// ============================================================
void GameManager::showMainMenu() {
    std::cout << "\n========== 大英雄时代 ==========" << std::endl;
    std::cout << "[1] 查看主控信息" << std::endl;
    std::cout << "[2] 查看背包" << std::endl;
    std::cout << "[3] 外出采购" << std::endl;
    std::cout << "[4] 招募队友" << std::endl;
    std::cout << "[5] 我的伙伴" << std::endl;
    std::cout << "[6] 打开地图" << std::endl;
    std::cout << "[7] 查看任务" << std::endl;
    std::cout << "[8] 回忆" << std::endl;
    std::cout << "[0] 退出" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "选择: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1: showMainCharInfo(); break;
        case 2: showInventory(); break;
        case 3: showShop(); break;
        case 4: showRecruit(); break;
        case 5: showFighters(); break;
        case 6: showMap(); break;
        case 7: showTasks(); break;
        case 8: showMemory(); break;
        case 0:
            isRunning = false;
            saveGame();
            std::cout << "游戏已保存，再见！" << std::endl;
            break;
        default:
            std::cout << "无效选择" << std::endl;
            break;
    }
    
    if (isRunning) {
        autoSave();
    }
}

// ============================================================
// showMainCharInfo — 显示主控信息
// ============================================================
void GameManager::showMainCharInfo() {
    if (!mainChar) {
        std::cout << "请先创建角色" << std::endl;
        return;
    }
    mainChar->displayInfo();
    std::cout << "连续登录：" << consecutiveDays << " 天" << std::endl;
    
    long long hours = totalOnlineSeconds / 3600;
    long long minutes = (totalOnlineSeconds % 3600) / 60;
    std::cout << "总在线时长：" << hours << "小时" << minutes << "分钟" << std::endl;
}

// ============================================================
// showInventory — 显示背包
// ============================================================
void GameManager::showInventory() {
    if (!inventory) {
        std::cout << "背包未初始化" << std::endl;
        return;
    }

    // 先显示背包（已包含中文名称）
    inventory->displayAll();

    if (inventory->getTotalCount() == 0) {
        return;
    }

    // 获取有序物品ID列表（用于后续操作）
    std::vector<std::string> itemIds;
    for (const auto& pair : inventory->getAllItems()) {
        itemIds.push_back(pair.first);
    }

    // 第一步：选择物品
    std::cout << "输入物品编号（输入0返回）: ";
    int itemChoice;
    std::cin >> itemChoice;
    if (itemChoice == 0) return;
    if (itemChoice < 1 || itemChoice > (int)itemIds.size()) {
        std::cout << "无效编号" << std::endl;
        return;
    }

    const std::string& selectedId = itemIds[itemChoice - 1];
    int maxCount = inventory->getCount(selectedId);

    // 第二步：选择操作
    std::cout << "选择操作：" << std::endl;
    std::cout << "[1] 使用  [2] 丢弃  [3] 出售  [4] 取消" << std::endl;
    std::cout << "选择: ";
    int op;
    std::cin >> op;
    if (op == 4) return;

    std::cout << "输入数量（1-" << maxCount << "）: ";
    int count;
    std::cin >> count;
    if (count < 1 || count > maxCount) {
        std::cout << "数量无效" << std::endl;
        return;
    }

    switch (op) {
        case 1: {  // 使用
            if (recruitSystem->getFighterCount() == 0) {
                std::cout << "没有可使用的战斗角色" << std::endl;
                return;
            }
            std::cout << "选择目标角色编号：" << std::endl;
            recruitSystem->displayOwnedFighters();
            int targetIdx;
            std::cin >> targetIdx;
            Combatant* target = recruitSystem->getFighter(targetIdx - 1);
            if (!target) {
                std::cout << "无效目标" << std::endl;
                return;
            }
            for (int i = 0; i < count; ++i) {
                if (!inventory->hasItem(selectedId, 1)) break;
                inventory->useItem(selectedId, target);
            }

            // 装备类使用后，统计所有角色的装备数量，更新任务进度
            {
                int totalEquips = 0;
                for (int i = 0; i < recruitSystem->getFighterCount(); ++i) {
                    totalEquips += recruitSystem->getFighter(i)->getEquippedCount();
                }
                taskManager->updateProgress("equip", totalEquips);
            }
            break;
        }
        case 2:  // 丢弃
            if (inventory->removeItem(selectedId, count)) {
                std::cout << "已丢弃 " << count << " 个物品" << std::endl;
            }
            break;
        case 3:  // 出售
            if (inventory->sellItem(selectedId, count, mainChar)) {
                std::cout << "出售成功！" << std::endl;
            }
            break;
        default:
            std::cout << "无效操作" << std::endl;
            break;
    }
}
// ============================================================
// showShop — 商店
// ============================================================
void GameManager::showShop() {
    if (!shop || !mainChar || !inventory) {
        std::cout << "商店未初始化" << std::endl;
        return;
    }

    while (true) {
        shop->displayItems();
        std::cout << "选择商品编号购买（输入0返回）: ";
        int choice;
        std::cin >> choice;
        if (choice == 0) break;

        std::cout << "购买数量: ";
        int count;
        std::cin >> count;
        if (count <= 0) {
            std::cout << "数量必须大于0" << std::endl;
            continue;
        }

        std::vector<Combatant*> fighters;
        for (int i = 0; i < recruitSystem->getFighterCount(); ++i) {
            fighters.push_back(recruitSystem->getFighter(i));
        }

        // 购买失败时继续循环，不返回主菜单
        bool success = shop->buyItem(choice, count, mainChar, inventory, fighters);
        if (success) {
            taskManager->updateProgress("shop_buy", 1);
            // 自动发放奖励
            autoClaimRewards();
        } else {
            std::cout << "购买失败，请重试或输入0返回" << std::endl;
        }
    }
    std::cout << "离开商店" << std::endl;
}
/// ============================================================
// showRecruit — 招募
// ============================================================
void GameManager::showRecruit() {
    if (!recruitSystem || !inventory || !mainChar) {
        std::cout << "招募系统未初始化" << std::endl;
        return;
    }

    if (!inventory->hasItem("recruit_card", 1)) {
        std::cout << "没有招募卡，正在跳转到商店..." << std::endl;
        showShop();  // 跳转到商店
        return;
    }

    inventory->removeItem("recruit_card", 1);
    Combatant* result = recruitSystem->recruit(inventory, mainChar);
    if (result) {
        std::cout << "欢迎 " << result->getName() << " 加入队伍！" << std::endl;
        taskManager->updateProgress("collect_char", 1);
        taskManager->updateProgress("recruit", 1);
        autoClaimRewards();
    }
}

// ============================================================
// showFighters — 我的伙伴
// ============================================================
void GameManager::showFighters() {
    if (!recruitSystem) {
        std::cout << "请先创建角色" << std::endl;
        return;
    }

    while (true) {
        recruitSystem->displayOwnedFighters();
        if (recruitSystem->getFighterCount() == 0) return;

        std::cout << "选择角色编号（输入0返回）: ";
        int idx;
        std::cin >> idx;
        if (idx == 0) break;
        if (idx < 1 || idx > recruitSystem->getFighterCount()) {
            std::cout << "无效编号" << std::endl;
            continue;
        }

        std::cout << "\n[1] 查看详情  [2] 升级  [3] 返回" << std::endl;
        std::cout << "选择: ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            // 查看详情 — 显示后等待用户确认返回
            recruitSystem->displayFighterDetail(idx);
            std::cout << "输入任意键返回...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 2) {
            // 升级
            Combatant* fighter = recruitSystem->getFighter(idx - 1);
            int need = fighter->getUpgradeStoneCost();
            int have = inventory->getCount("upgrade_stone");

            std::cout << "\n=== 升级确认 ===" << std::endl;
            std::cout << "角色：" << fighter->getName() << "（Lv." << fighter->getLevel() << "）" << std::endl;
            std::cout << "需要升级石：" << need << " 个" << std::endl;
            std::cout << "当前升级石：" << have << " 个" << std::endl;

            if (have < need) {
                std::cout << "❌ 升级石不足！" << std::endl;
                std::cout << "输入任意键返回...";
                std::cin.ignore();
                std::cin.get();
                continue;
            }

            std::cout << "\n确认升级？[1] 是  [2] 否: ";
            int confirm;
            std::cin >> confirm;
            if (confirm != 1) continue;

            // 执行升级
            inventory->removeItem("upgrade_stone", need);
            fighter->levelUp();

            std::cout << "\n=== 升级成功！===" << std::endl;
            fighter->displayInfo();
            std::cout << "输入任意键返回...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 3) {
            continue;
        }
        else {
            std::cout << "无效选择" << std::endl;
        }
    }
}


// ============================================================
// showMap — 地图（关卡选择/战斗/扫荡）
// ============================================================
void GameManager::showMap() {
    if (!enemyManager || !recruitSystem || !mainChar) {
        std::cout << "地图未初始化" << std::endl;
        return;
    }

    // 首次进入地图时初始化第1关
    if (enemyManager->getCurrentEnemies().empty()) {
        enemyManager->initFirstStage();
    }

    std::cout << "\n========== 地图 ==========" << std::endl;
    std::cout << "体力：" << mainChar->getStamina() << "/" << mainChar->getMaxStamina() << std::endl;

    // 显示区域解锁状态
    std::cout << "\n[A区] " << (enemyManager->isZoneUnlocked("A") ? "🔓 已解锁" : "🔒 未解锁");
    if (enemyManager->isZoneCleared("A")) std::cout << " ✅ 已通关";
    std::cout << "\n[B区] " << (enemyManager->isZoneUnlocked("B") ? "🔓 已解锁" : "🔒 未解锁");
    if (enemyManager->isZoneCleared("B")) std::cout << " ✅ 已通关";
    std::cout << "\n[C区] " << (enemyManager->isZoneUnlocked("C") ? "🔓 已解锁" : "🔒 未解锁");
    if (enemyManager->isZoneCleared("C")) std::cout << " ✅ 已通关";

    // 显示关卡列表
    std::cout << "\n\n--- 关卡列表 ---" << std::endl;
    for (int i = 1; i <= 15; ++i) {
        const StageConfig* cfg = enemyManager->getStageConfig(i);
        if (!cfg) continue;
        bool cleared = enemyManager->isStageCleared(i);
        bool unlocked = enemyManager->isZoneUnlocked(cfg->zone);
        std::cout << "第" << i << "关 (" << cfg->zone << "区) "
                  << (cleared ? "✅ 已通关" : (unlocked ? "🔓 可挑战" : "🔒 未解锁"));
        if (cleared) std::cout << " [可扫荡]";
        std::cout << std::endl;
    }

    std::cout << "\n[1] 挑战/扫荡关卡  [2] 返回" << std::endl;
    int choice;
    std::cin >> choice;
    if (choice == 2) return;

    std::cout << "选择关卡编号（1-15）: ";
    int stageId;
    std::cin >> stageId;
    if (stageId < 1 || stageId > 15) {
        std::cout << "无效关卡编号" << std::endl;
        return;
    }

    const StageConfig* cfg = enemyManager->getStageConfig(stageId);
    if (!cfg) return;

    // 检查区域是否解锁
    if (!enemyManager->isZoneUnlocked(cfg->zone)) {
        std::cout << "该区域未解锁！请先通关前一区域" << std::endl;
        return;
    }

    bool cleared = enemyManager->isStageCleared(stageId);

    if (cleared) {
        // 已通关 → 扫荡
        std::cout << "\n[1] 扫荡  [2] 返回" << std::endl;
        int action;
        std::cin >> action;
        if (action == 1) {
            if (!mainChar->consumeStamina(10)) {
                std::cout << "体力不足！" << std::endl;
                return;
            }
            taskManager->updateProgress("stamina", 10);
            int exp = enemyManager->getSweepExp(stageId);
            int gold = enemyManager->getSweepGold(stageId);
            mainChar->addExp(exp);
            mainChar->addGold(gold);
            std::cout << "扫荡完成！获得 " << exp << " 经验，" << gold << " 金币" << std::endl;
            autoClaimRewards();
        }
    } else {
        // 未通关 → 挑战
        std::cout << "\n[1] 挑战  [2] 返回" << std::endl;
        int action;
        std::cin >> action;
        if (action == 1) {
            // 检查体力
            if (!mainChar->consumeStamina(10)) {
                std::cout << "体力不足！" << std::endl;
                return;
            }
            taskManager->updateProgress("stamina", 10);

            // 选择上场角色（最多3个）
            std::vector<Combatant*> fighters;
            std::cout << "选择上场角色（最多3个，输入0结束）:" << std::endl;
            recruitSystem->displayOwnedFighters();
            while (fighters.size() < 3) {
                int idx;
                std::cin >> idx;
                if (idx == 0) break;
                if (idx < 1 || idx > recruitSystem->getFighterCount()) {
                    std::cout << "无效编号" << std::endl;
                    continue;
                }
                Combatant* f = recruitSystem->getFighter(idx - 1);
                // 检查是否已选
                if (std::find(fighters.begin(), fighters.end(), f) != fighters.end()) {
                    std::cout << "该角色已选择" << std::endl;
                    continue;
                }
                fighters.push_back(f);
                std::cout << "已选择 " << f->getName() << "，还可选 " << (3 - fighters.size()) << " 个" << std::endl;
            }

            if (fighters.empty()) {
                std::cout << "至少选择1个角色" << std::endl;
                return;
            }

            // 构建关卡敌人
            enemyManager->buildStage(stageId);
            auto& enemies = enemyManager->getCurrentEnemies();
            std::vector<Enemy*> enemyPtrs;
            for (auto& e : enemies) {
                enemyPtrs.push_back(&e);
            }

            BattleSystem battle(mainChar, taskManager, enemyManager, inventory);
            battle.startBattle(fighters, enemyPtrs);

            if (battle.isVictory()) {
                // 发放首通奖励
                int gold = enemyManager->getFirstClearGold(stageId);
                int diamond = enemyManager->getFirstClearDiamond(stageId);
                int exp = enemyManager->getFirstClearExp(stageId);
                mainChar->addGold(gold);
                mainChar->addDiamond(diamond);
                mainChar->addExp(exp);
                enemyManager->markStageCleared(stageId);

                std::cout << "🎉 第 " << stageId << " 关通关！" << std::endl;
                std::cout << "奖励：" << gold << "金币，" << diamond << "钻石，" << exp << "经验" << std::endl;

                // 检查是否通关整个区域
                std::string zone = cfg->zone;
                bool zoneAllCleared = true;
                for (int i = 1; i <= 15; ++i) {
                    const StageConfig* c = enemyManager->getStageConfig(i);
                    if (c && c->zone == zone && !enemyManager->isStageCleared(i)) {
                        zoneAllCleared = false;
                        break;
                    }
                }
                if (zoneAllCleared) {
                    enemyManager->markZoneCleared(zone);
                    std::cout << "\n🎊 恭喜通关" << zone << "区！" << std::endl;
                    taskManager->updateProgress("clear_zone", 1);
                }

                // 检查经验升级
                while (mainChar->canLevelUp()) {
                    mainChar->levelUp();
                }
                std::cout << "\n当前主控信息：" << std::endl;
                mainChar->displayInfo();

                taskManager->updateProgress("clear_stage", 1);
                autoClaimRewards();

                // 如果下一关存在且区域已解锁，自动进入下一关
                if (stageId < 15) {
                    const StageConfig* nextCfg = enemyManager->getStageConfig(stageId + 1);
                    if (nextCfg && enemyManager->isZoneUnlocked(nextCfg->zone)) {
                        std::cout << "\n下一关已解锁，进入第 " << (stageId + 1) << " 关" << std::endl;
                    }
                }
            }
        }
    }
}

// ============================================================
// showTasks — 任务
// ============================================================
void GameManager::showTasks() {
    if (!taskManager) {
        std::cout << "任务系统未初始化" << std::endl;
        return;
    }
    
    taskManager->displayAllTasks();
    
    std::cout << "输入任意键返回...";
    std::cin.ignore();
    std::cin.get();
}

// ============================================================
// showMemory — 回忆（读取外部文档）
// ============================================================
void GameManager::showMemory() {
    std::cout << "\n========== 回忆 ==========" << std::endl;
    
    std::ifstream file("story.txt");
    if (!file) {
        std::cout << "（暂无回忆记录，请创建 story.txt 文件）" << std::endl;
        std::cout << "==============================\n" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    file.close();
    
    std::cout << "\n输入任意键返回...";
    std::cin.ignore();
    std::cin.get();
}

// ============================================================
// main 函数
// ============================================================
int main() {
    // 创建游戏对象，加载存档或新建游戏
    GameManager game;

    // 显示 raylib 图形界面（2500x1400），包含外出采购等功能
    ShowSplashScreen(&game);

    return 0;
}