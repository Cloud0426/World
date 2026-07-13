#include "SplashScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "role/combatant/Combatant.h"
#include "role/enemy/EnemyManager.h"

#include "raylib.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <deque>

// raylib v6.0 header has obfuscated names (BeginDraWing), but the DLL exports correct names (BeginDrawing)
extern "C" {
    __declspec(dllimport) void BeginDrawing(void);
    __declspec(dllimport) void EndDrawing(void);
}

// ============================================================
// 前置声明：各模块函数
// ============================================================
// MainScreen
void RenderInitScreen(UIResource& res, int curW, int curH);
void HandleInitScreenInput(UIState& state);
void RenderMainScreen(UIResource& res, UIState& state, int curW, int curH);
void HandleMainScreenInput(UIResource& res, UIState& state, int curW, int curH);

// StoreScreen
void InitStoreData(UIState& state, UIResource& res);
void RenderStoreScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleStoreScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);

// MapScreen
void RenderMapScreen(UIResource& res, int curW, int curH);
void HandleMapScreenInput(UIResource& res, UIState& state, int curW, int curH);
void RenderMapSubScreen(UIResource& res, int curW, int curH);
void RenderMapSub2Screen(UIResource& res, UIState& state, int curW, int curH);
void HandleMapSub2ScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void RenderMapSub3Screen(UIResource& res, int curW, int curH);

// BattleScreen
void RenderBattleScreen(UIResource& res, UIState& state, int curW, int curH);
void HandleBattleScreenInput(UIResource& res, UIState& state, int curW, int curH);
void HandleEnemyTurn(UIState& state);
void UpdateBattleAnimations(UIState& state, float dt);

// MiscScreens
void RenderBagScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleBagScreenInput(UIResource& res, UIState& state, int curW, int curH);
void RenderTaskListScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleTaskListScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void RenderRecruitScreen(UIResource& res, int curW, int curH);
void HandleRecruitScreenInput(UIState& state);
void RenderFriendsScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleFriendsScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void RenderStory1Screen(UIResource& res, int curW, int curH);
void HandleStory1ScreenInput(UIResource& res, UIState& state, int curW, int curH);
void RenderStory2Screen(UIResource& res, int curW, int curH);
void HandleStory2ScreenInput(UIState& state);
void RenderCharacterFileScreen(UIResource& res, UIState& state, int curW, int curH);
void HandleCharacterFileScreenInput(UIResource& res, UIState& state, int curW, int curH);
void HandleCharacterFileKeyboardInput(UIResource& res, UIState& state, int curW, int curH);

// ============================================================
// 统一输入处理（委托各界面模块）
// ============================================================
static void HandleAllInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    // 角色档案键盘输入（提前处理，不依赖鼠标点击）
    HandleCharacterFileKeyboardInput(res, state, curW, curH);

    // 鼠标点击处理
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // ---- 如果奖励弹窗打开，先处理弹窗 ----
        if (state.showTaskRewardDlg) {
            int curW = GetScreenWidth();
            int curH = GetScreenHeight();
            float dlgW = 500, dlgH = 300;
            float dlgX = (curW - dlgW) / 2;
            float dlgY = (curH - dlgH) / 2;
            float closeBtnX = dlgX + (dlgW - 140) / 2;
            float closeBtnY = dlgY + dlgH - 65;
            float closeBtnW = 140;
            float closeBtnH = 44;
            Vector2 mousePos = GetMousePosition();
            // 点击确定按钮或弹窗外空白区域关闭
            if (HitTestRect(mousePos, closeBtnX, closeBtnY, closeBtnW, closeBtnH) ||
                !HitTestRect(mousePos, dlgX, dlgY, dlgW, dlgH)) {
                state.showTaskRewardDlg = false;
            }
            return;
        }

        switch (state.screenState) {
            case SCREEN_INIT:
                HandleInitScreenInput(state);
                break;
            case SCREEN_MAIN:
                HandleMainScreenInput(res, state, curW, curH);
                break;
            case SCREEN_BAG:
                HandleBagScreenInput(res, state, curW, curH);
                break;
            case SCREEN_TASK_LIST:
                HandleTaskListScreenInput(res, state, game, curW, curH);
                break;
            case SCREEN_STORE:
                HandleStoreScreenInput(res, state, game, curW, curH);
                break;
            case SCREEN_MAP:
                HandleMapScreenInput(res, state, curW, curH);
                break;
            case SCREEN_MAP_SUB:
                state.screenState = SCREEN_MAP;
                break;
            case SCREEN_MAP_SUB2:
                HandleMapSub2ScreenInput(res, state, game, curW, curH);
                break;
            case SCREEN_MAP_SUB3:
                state.screenState = SCREEN_MAP;
                break;
            case SCREEN_BATTLE:
                HandleBattleScreenInput(res, state, curW, curH);
                break;
            case SCREEN_STORY1:
                HandleStory1ScreenInput(res, state, curW, curH);
                break;
            case SCREEN_STORY2:
                HandleStory2ScreenInput(state);
                break;
            case SCREEN_RECRUIT:
                HandleRecruitScreenInput(state);
                break;
            case SCREEN_FRIENDS:
                HandleFriendsScreenInput(res, state, game, curW, curH);
                break;
            case SCREEN_CHARACTER_FILE:
                HandleCharacterFileScreenInput(res, state, curW, curH);
                break;
        }
    }

    // ESC 键处理
    if (IsKeyPressed(KEY_ESCAPE)) {
        // 如果奖励弹窗打开，按 ESC 关闭
        if (state.showTaskRewardDlg) {
            state.showTaskRewardDlg = false;
            return;
        }
        switch (state.screenState) {
            case SCREEN_INIT:
                // 不处理，让主循环自然退出
                break;
            case SCREEN_STORE:
                if (state.storeShowBuyDlg || state.storeShowSubMenu) {
                    state.storeShowBuyDlg = false;
                    state.storeShowSubMenu = false;
                } else {
                    state.screenState = SCREEN_MAIN;
                }
                break;
            case SCREEN_STORY1:
                state.screenState = SCREEN_MAIN;
                break;
            case SCREEN_STORY2:
                state.screenState = SCREEN_MAIN;
                break;
            case SCREEN_MAP_SUB:
                state.screenState = SCREEN_MAP;
                break;
            case SCREEN_MAP_SUB2:
                state.showPeaceInB = false;
                state.screenState = SCREEN_MAP;
                break;
            case SCREEN_MAP_SUB3:
                state.screenState = SCREEN_MAP;
                break;
            case SCREEN_BATTLE:
                state.showPeaceInB = false;
                state.screenState = SCREEN_MAP_SUB2;
                break;
            case SCREEN_FRIENDS:
                state.screenState = SCREEN_MAIN;
                break;
            case SCREEN_RECRUIT:
                state.screenState = SCREEN_MAIN;
                break;
            case SCREEN_CHARACTER_FILE:
                state.nameEditing = false;
                state.screenState = SCREEN_MAIN;
                break;
            default:
                if (state.screenState != SCREEN_MAIN)
                    state.screenState = SCREEN_MAIN;
                break;
        }
    }
}

// ============================================================
// 统一绘制分发
// ============================================================
static void RenderCurrentScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    switch (state.screenState) {
        case SCREEN_INIT:
            RenderInitScreen(res, curW, curH);
            break;
        case SCREEN_MAIN:
            RenderMainScreen(res, state, curW, curH);
            break;
        case SCREEN_BAG:
            RenderBagScreen(res, state, game, curW, curH);
            break;
        case SCREEN_TASK_LIST:
            RenderTaskListScreen(res, state, game, curW, curH);
            break;
        case SCREEN_STORE:
            RenderStoreScreen(res, state, game, curW, curH);
            break;
        case SCREEN_MAP:
            RenderMapScreen(res, curW, curH);
            break;
        case SCREEN_MAP_SUB:
            RenderMapSubScreen(res, curW, curH);
            break;
        case SCREEN_MAP_SUB2:
            RenderMapSub2Screen(res, state, curW, curH);
            break;
        case SCREEN_MAP_SUB3:
            RenderMapSub3Screen(res, curW, curH);
            break;
        case SCREEN_BATTLE:
            RenderBattleScreen(res, state, curW, curH);
            break;
        case SCREEN_STORY1:
            RenderStory1Screen(res, curW, curH);
            break;
        case SCREEN_STORY2:
            RenderStory2Screen(res, curW, curH);
            break;
        case SCREEN_RECRUIT:
            RenderRecruitScreen(res, curW, curH);
            break;
        case SCREEN_FRIENDS:
            RenderFriendsScreen(res, state, game, curW, curH);
            break;
        case SCREEN_CHARACTER_FILE:
            RenderCharacterFileScreen(res, state, curW, curH);
            break;
    }
}

// ============================================================
void ShowSplashScreen(GameManager* game) {
    if (!game) return;
    MainCharacter* player = game->getMainChar();
    Inventory* inventory = game->getInventory();
    Shop* shop = game->getShop();
    if (!player || !inventory || !shop) return;
    const int screenWidth = 2500;
    const int screenHeight = 1400;

    InitWindow(screenWidth, screenHeight, "Da Ying Xiong Shi Dai - HEROES ERA");
    SetTargetFPS(60);

    // 加载全部资源
    UIResource res;
    res.loadAll();

    // 初始化UI状态
    UIState state;
    state.stamina = player->getStamina();
    state.maxStamina = player->getMaxStamina();
    state.coins = player->getGold();
    state.diamonds = player->getDiamond();
    state.charName = "英雄";
    state.nameEditing = false;

    // 初始化商店数据（含商品图标加载）
    InitStoreData(state, res);

    // ---- 标记"每日登录"任务为完成状态（但不领取，等待玩家手动操作）----
    {
        TaskManager* tm = game->getTaskManager();
        if (tm) {
            Task* loginTask = tm->getTask("daily_login");
            if (loginTask && !loginTask->isCompleted && !loginTask->isClaimed) {
                loginTask->currentProgress = loginTask->targetValue;
                loginTask->isCompleted = true;
            }
        }
    }

    while (!WindowShouldClose()) {
        int curW = GetScreenWidth();
        int curH = GetScreenHeight();

        // ---- 输入处理 ----
        HandleAllInput(res, state, game, curW, curH);

        // ---- 同步 UI 状态与 GameManager 数据 ----
        MainCharacter* p = game->getMainChar();
        if (p) {
            state.coins = p->getGold();
            state.diamonds = p->getDiamond();
            state.stamina = p->getStamina();
            state.maxStamina = p->getMaxStamina();
        }

        // ---- 战斗逻辑更新（敌人回合 + 动画帧更新） ----
        if (state.screenState == SCREEN_BATTLE) {
            HandleEnemyTurn(state);
            UpdateBattleAnimations(state, GetFrameTime());
        }

        // ---- 绘制 ----
        BeginDrawing();
        ClearBackground(BLACK);
        RenderCurrentScreen(res, state, game, curW, curH);

        // ---- 任务奖励弹窗（覆盖在所有界面之上）----
        if (state.showTaskRewardDlg) {
            float dlgW = 500, dlgH = 300;
            float dlgX = (curW - dlgW) / 2;
            float dlgY = (curH - dlgH) / 2;

            // 半透明遮罩
            DrawRectangle(0, 0, curW, curH, Color{ 0, 0, 0, 160 });

            // 弹窗背景
            DrawRectangleRounded({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 40, 40, 70, 240 });
            DrawRectangleRoundedLines({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 200, 200, 255, 180 });

            // 标题
            Vector2 titleSz = MeasureTextEx(res.staminaFont, "任务奖励", 36, 1);
            DrawTextEx(res.staminaFont, "任务奖励",
                { dlgX + (dlgW - titleSz.x) / 2, dlgY + 20 }, 36, 1, GOLD);

            // 奖励内容（逐行绘制，处理换行）
            float msgY = dlgY + 80;
            const char* pMsg = state.taskRewardMsg;
            const char* lineStart = pMsg;
            while (*pMsg) {
                if (*pMsg == '\n') {
                    std::string line(lineStart, pMsg - lineStart);
                    Vector2 lsz = MeasureTextEx(res.staminaFont, line.c_str(), 28, 1);
                    DrawTextEx(res.staminaFont, line.c_str(),
                        { dlgX + (dlgW - lsz.x) / 2, msgY }, 28, 1, WHITE);
                    msgY += 40;
                    lineStart = pMsg + 1;
                }
                ++pMsg;
            }
            if (*lineStart) {
                Vector2 lsz = MeasureTextEx(res.staminaFont, lineStart, 28, 1);
                DrawTextEx(res.staminaFont, lineStart,
                    { dlgX + (dlgW - lsz.x) / 2, msgY }, 28, 1, WHITE);
            }

            // 关闭按钮
            float closeBtnX = dlgX + (dlgW - 140) / 2;
            float closeBtnY = dlgY + dlgH - 65;
            float closeBtnW = 140;
            float closeBtnH = 44;
            Vector2 mousePos = GetMousePosition();
            bool closeHover = HitTestRect(mousePos, closeBtnX, closeBtnY, closeBtnW, closeBtnH);
            Color closeColor = closeHover ? Color{ 80, 80, 200, 240 } : Color{ 60, 60, 140, 200 };
            DrawRectangleRounded({ closeBtnX, closeBtnY, closeBtnW, closeBtnH }, 0.2f, 8, closeColor);
            Vector2 closeSz = MeasureTextEx(res.staminaFont, "确  定", 26, 1);
            DrawTextEx(res.staminaFont, "确  定",
                { closeBtnX + (closeBtnW - closeSz.x) / 2, closeBtnY + (closeBtnH - closeSz.y) / 2 },
                26, 1, WHITE);
        }
        EndDrawing();
    }

    // 卸载全部资源
    res.unloadAll();
    CloseWindow();
}
