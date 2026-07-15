#include "SplashScreen.h"
#include "TaskListScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "system/mission/TaskManager.h"
#include "role/character/MainCharacter.h"
#include <cstdio>
#include <cstring>

// ============================================================
void RenderTaskListScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    DrawImageCentered(res.taskListTex, curW, curH);
    Rectangle br = GetImageDrawRect(res.taskListTex, curW, curH);

    TaskManager* tm = game->getTaskManager();
    if (!tm) return;

    Vector2 mousePos = GetMousePosition();

    // ---- 左上角退出按钮 ----
    float exitBtnX = br.x + 10;
    float exitBtnY = br.y + 10;
    float exitBtnW = 100.0f;
    float exitBtnH = 40.0f;
    bool exitHover = HitTestRect(mousePos, exitBtnX, exitBtnY, exitBtnW, exitBtnH);
    Color exitColor = exitHover ? Color{ 200, 60, 60, 220 } : Color{ 180, 50, 50, 180 };
    DrawRectangleRounded({ exitBtnX, exitBtnY, exitBtnW, exitBtnH }, 0.2f, 8, exitColor);
    DrawRectangleRoundedLines({ exitBtnX, exitBtnY, exitBtnW, exitBtnH }, 0.2f, 8, Color{ 255, 100, 100, 200 });
    const char* exitLabel = "退出";
    Vector2 exitSz = MeasureTextEx(res.staminaFont, exitLabel, 24, 1);
    DrawTextEx(res.staminaFont, exitLabel,
        { exitBtnX + (exitBtnW - exitSz.x) / 2, exitBtnY + (exitBtnH - exitSz.y) / 2 },
        24, 1, WHITE);
    state.taskListExitBtnRect = { exitBtnX, exitBtnY, exitBtnW, exitBtnH };

    // ---- 顶部在线时长统计 ----
    long long totalSec = game->getTotalOnlineSeconds();
    long long dailySec = game->getDailyOnlineSeconds();
    int totalHh = (int)(totalSec / 3600);
    int totalMm = (int)((totalSec % 3600) / 60);
    int dailyHh = (int)(dailySec / 3600);
    int dailyMm = (int)((dailySec % 3600) / 60);

    // 更新在线时长（在 SplashScreen 主循环里会自动调用 updateOnlineTime）
    // 这里只做显示
    char timeBuf[128];
    snprintf(timeBuf, sizeof(timeBuf), "今日在线: %d小时%d分    累计在线: %d小时%d分",
             dailyHh, dailyMm, totalHh, totalMm);
    Vector2 timeSz = MeasureTextEx(res.staminaFont, timeBuf, 26, 1);
    float timeX = br.x + (br.width - timeSz.x) / 2;
    float timeY = br.y + 12;
    // 半透明背景
    DrawRectangle((int)timeX - 10, (int)timeY - 6, (int)timeSz.x + 20, (int)timeSz.y + 12,
                  { 40, 40, 60, 160 });
    DrawTextEx(res.staminaFont, timeBuf, { timeX, timeY }, 26, 1, GREEN);

    // ---- 绘制滚动条区域（右侧）----
    float scrollBarX = br.x + br.width - 30;
    float scrollBarY = br.y + 50;
    float scrollBarH = br.height - 100;
    float scrollBarW = 20;

    // ---- 收集所有任务并按类型分组显示 ----
    // 将任务按 daily / achievement / novice 分组
    struct DisplayTask {
        std::string id;
        std::string name;
        std::string desc;
        std::string type;
        int progress, target;
        int rewardGold, rewardDiamond, rewardExp;
        bool completed, claimed;
        bool isExtra; // 额外奖励标记
    };
    std::vector<DisplayTask> dispTasks;
    for (int i = 0; i < tm->getTaskCountByType("daily"); ++i) {
        // 无法直接用索引获取，需通过 getClaimableTasks + 遍历；改用全遍历
    }

    // 用 TM 的所有任务填充 dispTasks（按 daily/achievement/novice 顺序）
    // 由于 TaskManager 没有 getTask 索引方法，我们用 id 列表遍历
    std::vector<std::string> taskIds = {
        "daily_stamina", "daily_shop_buy", "daily_clear_stage", "daily_online", "daily_login",
        "achieve_collect_5", "achieve_clear_C", "achieve_recruit_3", "achieve_login_5",
        "novice_login_3", "novice_shop_buy_5", "novice_clear_A", "novice_equip_4"
    };
    for (const auto& tid : taskIds) {
        Task* t = tm->getTask(tid);
        if (!t) continue;
        DisplayTask dt;
        dt.id = t->id;
        dt.name = t->name;
        dt.desc = t->description;
        dt.type = t->type;
        dt.progress = t->currentProgress;
        dt.target = t->targetValue;
        dt.rewardGold = t->rewardGold;
        dt.rewardDiamond = t->rewardDiamond;
        dt.rewardExp = t->rewardExp;
        dt.completed = t->isCompleted;
        dt.claimed = t->isClaimed;
        dt.isExtra = false;
        dispTasks.push_back(dt);
    }

    // 计算总高度
    float taskAreaX = br.x + 30;
    float taskAreaY = br.y + 50;
    float taskAreaW = scrollBarX - taskAreaX - 20;
    float taskH = 135.0f;
    float taskSpacing = 10.0f;
    float totalH = dispTasks.size() * (taskH + taskSpacing);

    // 可见区域高度
    float viewH = br.height - 100;
    float maxScroll = std::max(0.0f, totalH - viewH);

    // 处理鼠标滚轮
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        state.taskListScrollOffset -= (int)(wheel * 40);
        if (state.taskListScrollOffset < 0) state.taskListScrollOffset = 0;
        if (state.taskListScrollOffset > maxScroll) state.taskListScrollOffset = (int)maxScroll;
    }

    // 绘制类型标题
    state.taskClaimBtns.clear();
    float curY = taskAreaY - state.taskListScrollOffset;
    std::string lastType = "";
    for (size_t i = 0; i < dispTasks.size(); ++i) {
        auto& dt = dispTasks[i];

        // 类型分组标题
        if (dt.type != lastType) {
            lastType = dt.type;
            const char* typeLabel = "";
            if (dt.type == "daily") typeLabel = "== 每日任务 ==";
            else if (dt.type == "achievement") typeLabel = "== 成就任务 ==";
            else if (dt.type == "novice") typeLabel = "== 新手任务 ==";
            Vector2 typeSz = MeasureTextEx(res.staminaFont, typeLabel, 36, 1);
            if (curY + 30 > taskAreaY - 20 && curY < taskAreaY + viewH) {
                DrawTextEx(res.staminaFont, typeLabel, {taskAreaX + 10, curY}, 36, 1, DARKGREEN);
            }
            curY += 50;
        }

        float ty = curY;
        // 如果可见才绘制
        if (ty + taskH > taskAreaY - 20 && ty < taskAreaY + viewH) {
            // 透明底板
            bool hovered = HitTestRect(mousePos, taskAreaX, ty, taskAreaW, taskH);
            Color bgColor = hovered ? Color{80, 80, 120, 160} : Color{50, 50, 80, 80};
            DrawRectangle((int)taskAreaX, (int)ty, (int)taskAreaW, (int)taskH, bgColor);

            // 任务名称（中文，加粗大号）
            const char* cnName = dt.name.c_str();
            // 英文→中文映射
            auto taskCN = [](const std::string& id) -> const char* {
                if (id == "daily_stamina") return "消耗体力";
                if (id == "daily_shop_buy") return "商店购物";
                if (id == "daily_clear_stage") return "通关关卡";
                if (id == "daily_online") return "在线时长";
                if (id == "daily_login") return "每日登录";
                if (id == "achieve_collect_5") return "收集5名角色";
                if (id == "achieve_clear_C") return "通关C区";
                if (id == "achieve_recruit_3") return "招募3次";
                if (id == "achieve_login_5") return "累计登录5天";
                if (id == "novice_login_3") return "登录3天";
                if (id == "novice_shop_buy_5") return "商店购买5次";
                if (id == "novice_clear_A") return "通关A区";
                if (id == "novice_equip_4") return "装备4件物品";
                return nullptr;
            };
            const char* cname = taskCN(dt.id);
            if (!cname) cname = dt.name.c_str();
            DrawTextEx(res.staminaFont, cname, {taskAreaX + 15, ty + 6}, 28, 1, WHITE);

            // 任务描述 + 进度（中文，加大）
            auto descCN = [](const std::string& id) -> const char* {
                if (id == "daily_stamina") return "今日消耗50点体力";
                if (id == "daily_shop_buy") return "在商店购买1次物品";
                if (id == "daily_clear_stage") return "通关2个关卡";
                if (id == "daily_online") return "在线30分钟";
                if (id == "daily_login") return "登录游戏";
                if (id == "achieve_collect_5") return "获得5个战斗角色";
                if (id == "achieve_clear_C") return "清除C区所有关卡";
                if (id == "achieve_recruit_3") return "累计招募3次";
                if (id == "achieve_login_5") return "累计登录5天";
                if (id == "novice_login_3") return "累计登录3天";
                if (id == "novice_shop_buy_5") return "在商店购买5次物品";
                if (id == "novice_clear_A") return "清除A区所有关卡";
                if (id == "novice_equip_4") return "为2名角色装备4件物品";
                return nullptr;
            };
            const char* cdesc = descCN(dt.id);
            if (!cdesc) cdesc = dt.desc.c_str();
            char descBuf[128];
            snprintf(descBuf, sizeof(descBuf), "%s  (%d/%d)", cdesc, dt.progress, dt.target);
            DrawTextEx(res.staminaFont, descBuf, {taskAreaX + 15, ty + 42}, 22, 1, WHITE);

            // 奖励信息
            char rewBuf[128];
            rewBuf[0] = '\0';
            if (dt.rewardGold > 0) { char tmp[32]; snprintf(tmp, sizeof(tmp), "金币+%d ", dt.rewardGold); strcat(rewBuf, tmp); }
            if (dt.rewardDiamond > 0) { char tmp[32]; snprintf(tmp, sizeof(tmp), "钻石+%d ", dt.rewardDiamond); strcat(rewBuf, tmp); }
            if (dt.rewardExp > 0) { char tmp[32]; snprintf(tmp, sizeof(tmp), "经验+%d ", dt.rewardExp); strcat(rewBuf, tmp); }
            if (rewBuf[0] != '\0') {
                DrawTextEx(res.staminaFont, rewBuf, {taskAreaX + 15, ty + 74}, 20, 1, YELLOW);
            }

            // 右侧状态
            float btnX = taskAreaX + taskAreaW - 150;
            float btnY = ty + (taskH - 40) / 2;
            float btnW = 130;
            float btnH = 44;

            if (dt.claimed) {
                DrawTextEx(res.staminaFont, "已领取", {btnX, btnY + 6}, 26, 1, GRAY);
            } else if (dt.completed) {
                bool btnHover = HitTestRect(mousePos, btnX, btnY, btnW, btnH);
                Color btnClr = btnHover ? Color{60, 200, 60, 240} : Color{40, 160, 40, 200};
                DrawRectangleRounded({btnX, btnY, btnW, btnH}, 0.2f, 8, btnClr);
                DrawRectangleRoundedLines({btnX, btnY, btnW, btnH}, 0.2f, 8, Color{100, 255, 100, 200});
                Vector2 lblSz = MeasureTextEx(res.staminaFont, "领取奖励", 24, 1);
                DrawTextEx(res.staminaFont, "领取奖励",
                    {btnX + (btnW - lblSz.x) / 2, btnY + (btnH - lblSz.y) / 2}, 24, 1, WHITE);
                state.taskClaimBtns.push_back({btnX, btnY, btnW, btnH, (int)i});
            } else {
                DrawTextEx(res.staminaFont, "进行中", {btnX, btnY + 8}, 26, 1, YELLOW);
            }
        }

        curY += taskH + taskSpacing;
    }

    // ---- 滚动条 ----
    if (maxScroll > 0) {
        float thumbH = viewH * (viewH / totalH);
        float thumbY = scrollBarY + (state.taskListScrollOffset / maxScroll) * (scrollBarH - thumbH);
        DrawRectangle((int)scrollBarX, (int)scrollBarY, (int)scrollBarW, (int)scrollBarH, {60, 60, 80, 100});
        DrawRectangleRounded({scrollBarX + 2, thumbY, scrollBarW - 4, thumbH}, 0.5f, 6, {180, 180, 200, 180});
    }

    // ---- ESC 提示 ----
    Vector2 tipSz = MeasureTextEx(res.staminaFont, "按 ESC 返回", 18, 1);
    DrawTextEx(res.staminaFont, "按 ESC 返回", {br.x + br.width - tipSz.x - 15, br.y + br.height - 30}, 18, 1, GRAY);
}

void HandleTaskListScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    // 检测退出按钮
    if (HitTestRect(mousePos, state.taskListExitBtnRect.x, state.taskListExitBtnRect.y,
                    state.taskListExitBtnRect.width, state.taskListExitBtnRect.height)) {
        state.screenState = SCREEN_MAIN;
        return;
    }

    // 检查是否点击了某个"领取奖励"按钮
    for (auto& btn : state.taskClaimBtns) {
        if (HitTestRect(mousePos, btn.x, btn.y, btn.w, btn.h)) {
            // 获取对应 Task
            TaskManager* tm = game->getTaskManager();
            if (tm) {
                MainCharacter* player = game->getMainChar();
                if (player) {
                    std::vector<std::string> taskIds = {
                        "daily_stamina", "daily_shop_buy", "daily_clear_stage", "daily_online", "daily_login",
                        "achieve_collect_5", "achieve_clear_C", "achieve_recruit_3", "achieve_login_5",
                        "novice_login_3", "novice_shop_buy_5", "novice_clear_A", "novice_equip_4"
                    };
                    if (btn.taskIdx >= 0 && btn.taskIdx < (int)taskIds.size()) {
                        const std::string& tid = taskIds[btn.taskIdx];
                        Task* t = tm->getTask(tid);
                        if (t && t->isCompleted && !t->isClaimed) {
                            // 先领取奖励
                            tm->claimTask(tid, player);
                            // 构建弹窗消息
                            char tmp[256];
                            snprintf(state.taskRewardMsg, sizeof(state.taskRewardMsg),
                                "任务奖励已领取！\n\n%s", t->name.c_str());
                            if (t->rewardGold > 0) {
                                snprintf(tmp, sizeof(tmp), "\n金币 +%d", t->rewardGold);
                                strcat(state.taskRewardMsg, tmp);
                            }
                            if (t->rewardDiamond > 0) {
                                snprintf(tmp, sizeof(tmp), "\n钻石 +%d", t->rewardDiamond);
                                strcat(state.taskRewardMsg, tmp);
                            }
                            if (t->rewardExp > 0) {
                                snprintf(tmp, sizeof(tmp), "\n经验 +%d", t->rewardExp);
                                strcat(state.taskRewardMsg, tmp);
                            }
                            state.showTaskRewardDlg = true;
                        }
                    }
                }
            }
            return;
        }
    }

    // 点击空白区域不返回（由 ESC 处理）
}

// ============================================================
// 绘制 & 输入：招募界面
