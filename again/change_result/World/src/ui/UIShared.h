#pragma once

#include <string>
#include <vector>
#include <deque>
#include "raylib.h"
#include "role/combatant/Combatant.h"
#include "role/enemy/Enemy.h"

// ============================================================
// Design canvas: 1440x960
// ============================================================
static const float DESIGN_W = 1440.0f;
static const float DESIGN_H = 960.0f;

// ============================================================
// 按钮区域（设计坐标系）
// ============================================================
struct ImageButton {
    int x1, y1;
    int x2, y2;
};

// ============================================================
// 子菜单项
// ============================================================
struct SubMenuItem {
    std::string id;
    std::string name;
    int price;
    std::string description;
};

// ============================================================
// 商店商品条目
// ============================================================
struct StoreEntry {
    std::string id, name, cat, subDir, file;
    int price;
    std::string cur, desc;
};

// ============================================================
// 商店商品点击区域
// ============================================================
struct StoreHitRect {
    float x, y, w, h;
    int idx;
};

// ============================================================
// 技能精灵动画载荷（动画结束时执行伤害）
// ============================================================
struct SkillAnimPayload {
    bool isCrit;
    int rawDmg;
    int finalDmg;
    char logBuf[256];
};

// ============================================================
// 敌人攻击动画载荷
// ============================================================
struct MouseAnimPayload {
    int finalDmg;
    char logBuf[256];
};

// ============================================================
// 屏幕状态枚举
// ============================================================
enum ScreenState {
    SCREEN_INIT,
    SCREEN_MAIN,
    SCREEN_BAG,
    SCREEN_TASK_LIST,
    SCREEN_STORE,
    SCREEN_MAP,
    SCREEN_STORY1,
    SCREEN_STORY2,
    SCREEN_RECRUIT,
    SCREEN_CHARACTER_FILE,
    SCREEN_MAP_SUB,      // 广播站子地图 (A区)
    SCREEN_MAP_SUB2,     // B区子地图
    SCREEN_MAP_SUB3,     // C区子地图
    SCREEN_BATTLE,       // 战斗界面
        SCREEN_FRIENDS,      // 我的伙伴界面
        SCREEN_FIGHTER_DETAIL // 伙伴详情全屏界面
};

// ============================================================
// UI 全局状态
// ============================================================
struct UIState {
    ScreenState screenState = SCREEN_INIT;

    // ---- 主界面 ----
    int stamina = 100;
    int maxStamina = 100;
    int coins = 99999;
    int diamonds = 8888;

    // ---- 角色档案 ----
    std::string charName = "英雄";
    bool nameEditing = false;

    // ---- 商店 ----
    bool storeShowBuyDlg = false;
    bool storeShowSubMenu = false;
    std::string storeDlgTitle;
    std::string storeDlgMsg;
    int storeBuyEntryIdx = -1;
    std::string storeBuyId;
    int storeBuyPrice = 0;
    std::string storeBuyCur;       // "gold" 或 "diamond"
    std::vector<SubMenuItem> storeSubItems;
    int storeSubEntryIdx = -1;
    std::vector<StoreEntry> storeEntries;
    std::vector<StoreHitRect> storeHitRects;

    // ---- 背包 ----
    // 左上角退出按钮区域
    Rectangle bagExitBtnRect = { 0, 0, 0, 0 };
    // 物品ID → 商店图标索引映射（-1表示无图标）
    int getStoreIconIdx(const std::string& itemId) const {
        // storeIcons顺序与storeEntries一致：
        // 0=recruit_card, 1=can, 2=biscuit, 3=water,
        // 4=first_aid_kit, 5=medicine_bag, 6=painkiller,
        // 7=sword, 8=magic_book, 9=shield, 10=upgrate_stone
        if (itemId == "recruit_card") return 0;
        if (itemId == "food_001") return 1;  // can
        if (itemId == "food_002") return 2;  // biscuit
        if (itemId == "food_003") return 3;  // water
        if (itemId == "potion_001") return 4;  // first_aid_kit
        if (itemId == "potion_002") return 5;  // medicine_bag
        if (itemId == "potion_003") return 6;  // painkiller
        if (itemId == "weapon_s1" || itemId == "weapon_s2" || itemId == "weapon_s3") return 7;  // sword
        if (itemId == "weapon_b1" || itemId == "weapon_b2" || itemId == "weapon_b3") return 8;  // magic_book
        if (itemId == "weapon_h1" || itemId == "weapon_h2" || itemId == "weapon_h3") return 9;  // shield
        if (itemId == "weapon_shield_1") return 9; // shield（初始护盾）
        if (itemId == "upgrade_stone") return 10;   // upgrate_stone
        return -1;
    }

    // ---- 地图 ----
    bool showPeaceInB = false;
    Rectangle peaceScreenRect = { 0, 0, 0, 0 };

    // ---- 战斗 ----
    Combatant* battlePlayer = nullptr;
    Enemy* battleEnemy = nullptr;
    bool battlePlayerTurn = true;
    bool battleFinished = false;
    bool battleVictory = false;
    std::deque<std::string> battleLogLines;
    int battleLogScrollOffset = 0;
    Rectangle battleBackBtnRect = { 0, 0, 0, 0 };

        // ---- 背包物品点击区域 ----
    struct BagHitRect {
        float x, y, w, h;
        int count;
        std::string itemId;
    };
    std::vector<BagHitRect> bagItemHitRects;

    // ---- 背包物品详情弹窗 ----
    bool bagShowDetail = false;          // 是否显示物品详情弹窗
    std::string bagDetailItemId;         // 被点击的物品ID
    std::string bagDetailItemName;       // 物品名称
    std::string bagDetailDesc;           // 物品描述
    std::string bagDetailCategory;       // 物品类别
    int bagDetailPrice = 0;              // 商店价格（用于计算出售价）
        Rectangle bagDetailUseBtnRect = {0,0,0,0};
    Rectangle bagDetailCancelBtnRect = {0,0,0,0};
    Rectangle bagDetailSellBtnRect = {0,0,0,0};

        // ---- 任务列表 ----
        int taskListScrollOffset = 0;
        Rectangle taskListExitBtnRect = { 0, 0, 0, 0 };
    // 记录每个任务的"领取奖励"按钮区域和索引，用于鼠标点击检测
    struct TaskClaimBtn { float x, y, w, h; int taskIdx; };
    std::vector<TaskClaimBtn> taskClaimBtns;
    // 任务奖励弹窗
    bool showTaskRewardDlg = false;
    char taskRewardMsg[512] = {};

        // ---- 伙伴界面 ----
        bool showFighterUpgradeInfo = false;
        int fighterSelectedIndex = 0;  // 当前选中的伙伴索引
        Rectangle fighterUpgradeBtnRect = { 0, 0, 0, 0 };
        Rectangle friendExitBtnRect = { 0, 0, 0, 0 };
        Rectangle friendNextBtnRect = { 0, 0, 0, 0 };  // 下一个伙伴按钮
    bool skillAnimActive = false;
    int skillAnimSkillIdx = 0;       // 0=普攻, 1=小技能1, 2=小技能2, 3=大招
    int skillAnimFrame = 0;
    float skillAnimTimer = 0.0f;
    float skillAnimDuration = 0.5f;
    SkillAnimPayload skillPayload;
    Vector2 skillAnimPos = { 0, 0 };
    float skillAnimScale = 1.0f;

        // ---- 招募界面 ----
    bool recruitShowResult = false;   // 是否全屏显示招募结果
    float recruitResultTimer = 0.0f;  // 结果展示计时
    int recruitCardCount = 0;         // 招募卡数量

    // ---- 敌人（老鼠）攻击动画 ----
    bool mouseAnimActive = false;
    int mouseAnimFrame = 0;
    float mouseAnimTimer = 0.0f;
    float mouseAnimDuration = 1.5f;
    MouseAnimPayload mousePayload;
};
