#include "SplashScreen.h"
#include "RecruitScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "system/backpack/Inventory.h"
#include "system/recruit/RecruitSystem.h"
#include "system/mission/TaskManager.h"
#include <cstdio>

// ============================================================
void RenderRecruitScreen(UIResource& res, UIState& state, int curW, int curH) {
        DrawImageCentered(res.recruitTex, curW, curH);

        // ---- 招募按钮（x600-1100，y1200-1300）----
        float btnX = 600.0f;
        float btnY = 1200.0f;
        float btnW = 500.0f;  // 1100-600=500
        float btnH = 100.0f;  // 1300-1200=100

        // 透明按钮（仅保留点击区域）

                // ---- 招募结果全屏显示（原比例缩放+黑色补齐）----
    if (state.recruitShowResult) {
        // 全屏黑色
        DrawRectangle(0, 0, curW, curH, BLACK);
        // 图片按原比例缩放填充（不足用黑色补齐）
        if (res.getCHENTex.id > 0) {
            DrawImageFillScreen(res.getCHENTex, curW, curH);
        } else if (res.chenErYuanTex.id > 0) {
            DrawImageFillScreen(res.chenErYuanTex, curW, curH);
        }
    }

        // ---- 招募卡数量显示（横2200-2350，竖50-100）----
    // 使用游戏管理器获取招募卡数量
    // 在UIState中暂存，由HandleRecruitScreenInput更新
    float cardX = 2200.0f;
    float cardY = 50.0f;
    float cardW = 150.0f;  // 2350-2200=150
    float cardH = 50.0f;   // 100-50=50
    // 黑色背景
    DrawRectangle((int)cardX, (int)cardY, (int)cardW, (int)cardH, BLACK);

    char cardBuf[32];
    snprintf(cardBuf, sizeof(cardBuf), "x%d", state.recruitCardCount);
    Vector2 cardSz = MeasureTextEx(res.staminaFont, cardBuf, 28, 1);
    DrawTextEx(res.staminaFont, cardBuf,
        { cardX + (cardW - cardSz.x) / 2, cardY + (cardH - cardSz.y) / 2 },
        28, 1, WHITE);
}

void HandleRecruitScreenInput(UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    // ---- 每帧同步招募卡数量 ----
    Inventory* inv = game->getInventory();
    state.recruitCardCount = (inv) ? inv->getCount("recruit_card") : 0;

    // ---- 如果正在显示招募结果，点击任意位置关闭 ----
    if (state.recruitShowResult) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state.recruitShowResult = false;
        }
        return;
    }

        // ---- 点击招募按钮（局部变量，与 RenderRecruitScreen 中一致）----
        Rectangle localRecruitBtn = { 600.0f, 1200.0f, 500.0f, 100.0f };
        if (HitTestRect(mousePos, localRecruitBtn.x, localRecruitBtn.y,
                        localRecruitBtn.width, localRecruitBtn.height)) {
            if (inv && inv->hasItem("recruit_card", 1)) {
                inv->removeItem("recruit_card", 1);
                // 🆕 执行招募逻辑
                RecruitSystem* rs = game->getRecruitSystem();
                MainCharacter* player = game->getMainChar();
                if (rs && player) {
                    rs->recruit(inv, player);
                    // 更新任务进度
                    TaskManager* tm = game->getTaskManager();
                    if (tm) {
                        tm->updateProgress("collect_char", 1);
                        tm->updateProgress("recruit", 1);
                    }
                }
                state.recruitShowResult = true;
                // 🆕 检查是否招募到了陈尔愿
                std::vector<Combatant*> fighters = game->getFighters();
                for (auto* f : fighters) {
                    if (f && f->getName() == "陈尔愿" && f->getCollege() == "机械学院") {
                        state.hasChenErYuan = true;
                        state.showNewFighterHint = true;
                        break;
                    }
                }
            }
            return;
        }

    // ---- 点击空白区域返回主界面 ----
    state.screenState = SCREEN_MAIN;
}

// ============================================================
// 绘制 & 输入：伙伴界面
