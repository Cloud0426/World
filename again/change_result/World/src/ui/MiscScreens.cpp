#include "SplashScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "system/mission/TaskManager.h"
#include <cstdio>
#include <cmath>
#include <cstring>

// ============================================================
// 绘制 & 输入：背包界面
// ============================================================
void RenderBagScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    DrawImageCentered(res.bagTex, curW, curH);
    Rectangle br = GetImageDrawRect(res.bagTex, curW, curH);

    Inventory* inv = game->getInventory();
    if (!inv) return;

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

    // 保存退出按钮点击区域
    state.bagExitBtnRect = { exitBtnX, exitBtnY, exitBtnW, exitBtnH };

    // 获取背包物品列表
    const auto& allItems = inv->getAllItems();
    int idx = 0;

    float startX = br.x + br.width * 0.08f;
    float startY = br.y + br.height * 0.22f;
    float itemW = br.width * 0.12f;
    float itemH = itemW;
    float spacingX = br.width * 0.17f;
    float spacingY = br.height * 0.24f;
    int itemsPerRow = 4;

    // 物品ID → 显示名称映射
    auto getItemName = [](const std::string& id) -> const char* {
        if (id == "potion_003") return "止痛片";
        if (id == "potion_002") return "外伤药包";
        if (id == "potion_001") return "急救包";
        if (id == "food_001")   return "罐头";
        if (id == "food_002")   return "压缩饼干";
        if (id == "food_003")   return "瓶装水";
        if (id == "upgrade_stone") return "升级石";
        if (id == "recruit_card")  return "招募卡";
        if (id == "weapon_shield_1") return "初级护盾";
        if (id.find("weapon_s") != std::string::npos) return "剑";
        if (id.find("weapon_b") != std::string::npos) return "魔法书";
        if (id.find("weapon_h") != std::string::npos) return "护盾";
        return id.c_str();
    };

    // 物品ID → 作用描述映射
    auto getItemDesc = [](const std::string& id) -> const char* {
        if (id == "potion_003") return "恢复生命25点";
        if (id == "potion_002") return "恢复生命40点";
        if (id == "potion_001") return "恢复生命80点";
        if (id == "food_001")   return "恢复体力50点";
        if (id == "food_002")   return "恢复体力30点";
        if (id == "food_003")   return "恢复体力20点";
        if (id == "upgrade_stone") return "用于战斗角色升级";
        if (id == "recruit_card")  return "消耗1张招募卡随机获得1个战斗角色";
        if (id == "weapon_shield_1") return "初级护盾，防御+5%";
        if (id.find("weapon_s") != std::string::npos) return "剑类装备，提升攻击力";
        if (id.find("weapon_b") != std::string::npos) return "魔法书装备，提升蓝条和生命恢复";
        if (id.find("weapon_h") != std::string::npos) return "护盾装备，提升防御力";
        return "";
    };

    int hoveredIdx = -1;
    float hoveredDescX = 0, hoveredDescY = 0;

    for (const auto& pair : allItems) {
        const std::string& itemId = pair.first;
        int count = pair.second;
        if (count <= 0) continue;

        int row = idx / itemsPerRow;
        int col = idx % itemsPerRow;
        float ix = startX + col * spacingX;
        float iy = startY + row * spacingY;

        // 判断鼠标是否悬停在该物品上（包括图标和名称区域）
        bool itemHovered = HitTestRect(mousePos, ix - 8, iy - 8, itemW + 16, itemH + 40);

        // 鼠标悬停检测（用于后续提示框）
        if (itemHovered) {
            hoveredIdx = idx;
            hoveredDescX = ix;
            hoveredDescY = iy + itemH + 30;
        }

        // 绘制物品底板（透明度50%，悬停时加深）
        Color bgColor = itemHovered ? Color{ 60, 60, 80, 160 } : Color{ 40, 40, 60, 80 };
        DrawRectangle((int)(ix - 8), (int)(iy - 8), (int)(itemW + 16), (int)(itemH + 40), bgColor);

        // 查找对应的商店图标
        int iconIdx = state.getStoreIconIdx(itemId);
        if (iconIdx >= 0 && (size_t)iconIdx < res.storeIcons.size() && res.storeIcons[iconIdx].id > 0) {
            DrawTexturePro(res.storeIcons[iconIdx],
                {0, 0, (float)res.storeIcons[iconIdx].width, (float)res.storeIcons[iconIdx].height},
                {ix, iy, itemW, itemH}, {0, 0}, 0.0f, WHITE);
        }

        // 显示物品名称 + 数量（数量在名称右边）
        const char* dName = getItemName(itemId);
        Vector2 nameSz = MeasureTextEx(res.staminaFont, dName, 22, 1);
        DrawTextEx(res.staminaFont, dName,
            {ix + (itemW - nameSz.x) / 2, iy + itemH + 2}, 22, 1, BLACK);

        char cntBuf[16];
        snprintf(cntBuf, sizeof(cntBuf), "x%d", count);
        Vector2 cntSz = MeasureTextEx(res.staminaFont, cntBuf, 18, 1);
        // 数量显示在名称右边，间隔4像素
        float cntX = ix + (itemW - nameSz.x) / 2 + nameSz.x + 4;
        float cntY = iy + itemH + 4;
        DrawTextEx(res.staminaFont, cntBuf, {cntX, cntY}, 18, 1, BLACK);

        idx++;
    }

    if (idx == 0) {
        const char* emptyMsg = "背包为空";
        Vector2 emptySz = MeasureTextEx(res.staminaFont, emptyMsg, 36, 1);
        DrawTextEx(res.staminaFont, emptyMsg,
            {(curW - emptySz.x) / 2, (curH - emptySz.y) / 2}, 36, 1, GRAY);
    }

    // ---- 鼠标悬停显示物品作用介绍 ----
    if (hoveredIdx >= 0) {
        // 获取对应的 itemId
        std::string hoveredItemId;
        int tmpIdx = 0;
        for (const auto& pair : allItems) {
            if (pair.second <= 0) continue;
            if (tmpIdx == hoveredIdx) { hoveredItemId = pair.first; break; }
            tmpIdx++;
        }
        if (!hoveredItemId.empty()) {
            const char* desc = getItemDesc(hoveredItemId);
            if (desc && desc[0] != '\0') {
                Vector2 descSz = MeasureTextEx(res.staminaFont, desc, 20, 1);
                float tipW = descSz.x + 20;
                float tipH = descSz.y + 12;
                float tipX = hoveredDescX;
                float tipY = hoveredDescY + 4;
                // 确保不超出屏幕
                if (tipX + tipW > curW) tipX = curW - tipW - 10;
                if (tipY + tipH > curH) tipY = hoveredDescY - tipH - 50;
                DrawRectangle((int)tipX, (int)tipY, (int)tipW, (int)tipH, { 20, 20, 40, 220 });
                DrawRectangleLines((int)tipX, (int)tipY, (int)tipW, (int)tipH, { 200, 200, 240, 200 });
                DrawTextEx(res.staminaFont, desc, { tipX + 10, tipY + 6 }, 20, 1, WHITE);
            }
        }
    }
}

void HandleBagScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();
    // 只有点击退出按钮才返回
    if (HitTestRect(mousePos, state.bagExitBtnRect.x, state.bagExitBtnRect.y,
                    state.bagExitBtnRect.width, state.bagExitBtnRect.height)) {
        state.screenState = SCREEN_MAIN;
    }
}

// ============================================================
// 绘制 & 输入：任务列表界面
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
// ============================================================
void RenderRecruitScreen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.recruitTex, curW, curH);
}

void HandleRecruitScreenInput(UIState& state) {
    state.screenState = SCREEN_MAIN;
}

// ============================================================
// 绘制 & 输入：伙伴界面
// ============================================================
void RenderFriendsScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    DrawImageCentered(res.friendsTex, curW, curH);
    Rectangle br = GetImageDrawRect(res.friendsTex, curW, curH);

    std::vector<Combatant*> fighters = game->getFighters();
    Vector2 mousePos = GetMousePosition();
    Inventory* inventory = game->getInventory();

    // ---- 右上角退出按钮 ----
    float exitBtnX = br.x + br.width - 130;
    float exitBtnY = br.y + 12;
    float exitBtnW = 110;
    float exitBtnH = 42;
    bool exitHover = HitTestRect(mousePos, exitBtnX, exitBtnY, exitBtnW, exitBtnH);
    Color exitColor = exitHover ? Color{ 200, 60, 60, 220 } : Color{ 180, 50, 50, 180 };
    DrawRectangleRounded({ exitBtnX, exitBtnY, exitBtnW, exitBtnH }, 0.2f, 8, exitColor);
    DrawRectangleRoundedLines({ exitBtnX, exitBtnY, exitBtnW, exitBtnH }, 0.2f, 8, Color{ 255, 100, 100, 200 });
    Vector2 exitSz = MeasureTextEx(res.staminaFont, "退出", 22, 1);
    DrawTextEx(res.staminaFont, "退出",
        { exitBtnX + (exitBtnW - exitSz.x) / 2, exitBtnY + (exitBtnH - exitSz.y) / 2 },
        22, 1, WHITE);
    state.friendExitBtnRect = { exitBtnX, exitBtnY, exitBtnW, exitBtnH };

    // ---- 左侧"查看更多伙伴"按钮 ----
    Vector2 btnP1 = DesignToScreen(1100.0f, 600.0f, res.friendsTex, curW, curH);
    Vector2 btnP2 = DesignToScreen(1350.0f, 800.0f, res.friendsTex, curW, curH);
    float btnX = btnP1.x, btnY = btnP1.y, btnW = btnP2.x - btnP1.x, btnH = btnP2.y - btnP1.y;
    bool hover = HitTestRect(mousePos, btnX, btnY, btnW, btnH);
    Color btnColor = hover ? Color{ 80, 180, 80, 200 } : Color{ 60, 150, 60, 160 };
    DrawRectangleRounded({ btnX, btnY, btnW, btnH }, 0.2f, 8, btnColor);
    DrawRectangleRoundedLines({ btnX, btnY, btnW, btnH }, 0.2f, 8, Color{ 100, 255, 100, 200 });
    Vector2 txtSz = MeasureTextEx(res.staminaFont, "查看更多伙伴>", 28, 1);
    DrawTextEx(res.staminaFont, "查看更多伙伴>",
        { btnX + (btnW - txtSz.x) / 2, btnY + (btnH - txtSz.y) / 2 }, 28, 1, WHITE);

    // ---- 右侧：当前选中角色的详细面板 ----
    if (!fighters.empty()) {
        Combatant* cur = fighters[0];
        if (cur) {
            int oldAtk = cur->getAttack();
            int oldDef = cur->getDefense();
            int oldHp = cur->getMaxHp();
            int oldLv = cur->getLevel();

            float panelX = br.x + br.width * 0.55f;
            float panelY = br.y + 30;
            float panelW = br.width * 0.38f;
            float panelH = br.height - 80;

            // 半透明面板背景
            DrawRectangle((int)panelX, (int)panelY, (int)panelW, (int)panelH,
                          Color{ 30, 30, 50, 200 });

            float lineY = panelY + 15;
            float col1X = panelX + 15;
            float col2X = panelX + panelW * 0.50f;
            float lh = 44;

            // ── 名称 ──
            const char* cname = cur->getName().c_str();
            Vector2 nsz = MeasureTextEx(res.staminaFont, cname, 40, 1);
            DrawTextEx(res.staminaFont, cname,
                { panelX + (panelW - nsz.x) / 2, lineY }, 40, 1, GOLD);
            lineY += 60;

            // ── 等级 + 升级石 + 升级按钮 ──
            char lvBuf[64];
            snprintf(lvBuf, sizeof(lvBuf), "Lv.%d", oldLv);
            DrawTextEx(res.staminaFont, lvBuf, { col1X, lineY }, 36, 1, WHITE);

            int stoneCount = inventory ? inventory->getCount("upgrade_stone") : 0;
            char stoneBuf[64];
            snprintf(stoneBuf, sizeof(stoneBuf), "升级石 x%d", stoneCount);
            DrawTextEx(res.staminaFont, stoneBuf, { col2X, lineY }, 28, 1, SKYBLUE);

            int needStone = cur->getUpgradeStoneCost();
            float upBtnX = panelX + panelW - 160;
            float upBtnY = lineY - 2;
            float upBtnW = 140;
            float upBtnH = 42;
            bool canUpgrade = (inventory && inventory->hasItem("upgrade_stone", needStone));
            bool upHover = HitTestRect(mousePos, upBtnX, upBtnY, upBtnW, upBtnH);
            Color upColor;
            if (canUpgrade) {
                upColor = upHover ? Color{ 60, 200, 60, 240 } : Color{ 40, 160, 40, 200 };
            } else {
                upColor = upHover ? Color{ 80, 80, 80, 200 } : Color{ 60, 60, 60, 160 };
            }
            DrawRectangleRounded({ upBtnX, upBtnY, upBtnW, upBtnH }, 0.2f, 8, upColor);
            DrawRectangleRoundedLines({ upBtnX, upBtnY, upBtnW, upBtnH }, 0.2f, 8,
                canUpgrade ? Color{ 100, 255, 100, 200 } : Color{ 120, 120, 120, 200 });
            const char* upLabel = canUpgrade ? "升  级" : "升级石不足";
            Vector2 upSz = MeasureTextEx(res.staminaFont, upLabel, 26, 1);
            DrawTextEx(res.staminaFont, upLabel,
                { upBtnX + (upBtnW - upSz.x) / 2, upBtnY + (upBtnH - upSz.y) / 2 }, 26, 1, WHITE);
            state.fighterUpgradeBtnRect = { upBtnX, upBtnY, upBtnW, upBtnH };
            lineY += 58;

            // ── 学院 ──
            DrawTextEx(res.staminaFont, cur->getCollege().c_str(), { col1X, lineY }, 28, 1, LIGHTGRAY);
            lineY += lh;

            // ── 生命值 + 血条 ──
            DrawTextEx(res.staminaFont, "生命值", { col1X, lineY }, 28, 1, WHITE);
            int hp = cur->getHp();
            int maxHp = cur->getMaxHp();
            char hpBuf[48];
            snprintf(hpBuf, sizeof(hpBuf), "%d / %d", hp, maxHp);
            Vector2 hpSz = MeasureTextEx(res.staminaFont, hpBuf, 28, 1);
            DrawTextEx(res.staminaFont, hpBuf, { panelX + panelW - 15 - hpSz.x, lineY }, 28, 1, RED);
            lineY += 36;

            // 血条
            float barX = col1X;
            float barY = lineY;
            float barW = panelW - 30;
            float barH = 24;
            DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, Color{ 60, 30, 30, 200 });
            float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0.0f;
            Color hpColor = hpRatio > 0.5f ? GREEN : (hpRatio > 0.25f ? ORANGE : RED);
            DrawRectangle((int)barX, (int)barY, (int)(barW * hpRatio), (int)barH, hpColor);
            lineY += 36;

            // ── 攻击力 ──
            DrawTextEx(res.staminaFont, "攻击力", { col1X, lineY }, 28, 1, WHITE);
            char atkBuf[48];
            snprintf(atkBuf, sizeof(atkBuf), "%d", cur->getAttack());
            Vector2 atkSz = MeasureTextEx(res.staminaFont, atkBuf, 28, 1);
            DrawTextEx(res.staminaFont, atkBuf, { panelX + panelW - 15 - atkSz.x, lineY }, 28, 1, ORANGE);
            lineY += lh;

            // ── 防御力 ──
            DrawTextEx(res.staminaFont, "防御力", { col1X, lineY }, 28, 1, WHITE);
            char defBuf[48];
            snprintf(defBuf, sizeof(defBuf), "%d", cur->getDefense());
            Vector2 defSz = MeasureTextEx(res.staminaFont, defBuf, 28, 1);
            DrawTextEx(res.staminaFont, defBuf, { panelX + panelW - 15 - defSz.x, lineY }, 28, 1, SKYBLUE);
            lineY += lh;

            // ── 暴击率 ──
            DrawTextEx(res.staminaFont, "暴击率", { col1X, lineY }, 28, 1, WHITE);
            char critBuf[48];
            snprintf(critBuf, sizeof(critBuf), "%.1f%%", cur->getCritRate() * 100.0);
            Vector2 critSz = MeasureTextEx(res.staminaFont, critBuf, 28, 1);
            DrawTextEx(res.staminaFont, critBuf, { panelX + panelW - 15 - critSz.x, lineY }, 28, 1, YELLOW);
            lineY += lh + 10;

            // ── 装备 ──
            DrawTextEx(res.staminaFont, "--- 装备 ---", { col1X, lineY }, 26, 1, GOLD);
            lineY += 34;
            auto equipped = cur->getEquipped();
            if (equipped.empty()) {
                DrawTextEx(res.staminaFont, "无", { col1X, lineY }, 24, 1, GRAY);
                lineY += 30;
            } else {
                for (size_t ei = 0; ei < equipped.size(); ++ei) {
                    auto& eq = equipped[ei];
                    char eqBuf[64];
                    snprintf(eqBuf, sizeof(eqBuf), "[%zu] %s Lv.%d", ei, eq.name.c_str(), eq.level);
                    DrawTextEx(res.staminaFont, eqBuf, { col1X, lineY }, 24, 1, WHITE);
                    lineY += 30;
                }
            }
            lineY += 10;

            // ── 技能展示区（竖排显示，直接展示技能描述，无需悬停） ──
            DrawTextEx(res.staminaFont, "--- 技能 ---", { col1X, lineY }, 26, 1, GOLD);
            lineY += 40;

            // btnTex[1]=小技能1、[2]=小技能2、[3]=大招
            float skillIconSize = 65;
            float skillLeftX = col1X;

            for (int si = 1; si <= 3; ++si) {
                float sx = skillLeftX;
                float sy = lineY;

                // 技能图标
                if (res.btnTex[si].id > 0) {
                    DrawTexturePro(res.btnTex[si],
                        { 0, 0, (float)res.btnTex[si].width, (float)res.btnTex[si].height },
                        { sx, sy, skillIconSize, skillIconSize },
                        { 0, 0 }, 0.0f, WHITE);
                } else {
                    DrawRectangle((int)sx, (int)sy, (int)skillIconSize, (int)skillIconSize,
                                  Color{ 60, 60, 100, 200 });
                }

                // 技能名称（图标右侧）
                Skill* sk = cur->getSkill(si);
                const char* skName = sk ? sk->name.c_str() : "未知";
                float nameX = sx + skillIconSize + 12;
                DrawTextEx(res.staminaFont, skName, { nameX, sy }, 22, 1, GOLD);

                // 消耗能量
                int mpCost = sk ? sk->mpCost : 0;
                char costBuf[64];
                snprintf(costBuf, sizeof(costBuf), "消耗能量: %d", mpCost);
                DrawTextEx(res.staminaFont, costBuf, { nameX, sy + 26 }, 20, 1, SKYBLUE);

                // 技能描述（换行处理）
                const char* descStr = sk ? sk->description.c_str() : "无";
                float descX = nameX;
                float descY = sy + 52;
                float maxDescW = panelX + panelW - 15 - descX;
                if (maxDescW < 80) maxDescW = 80;

                std::string remaining = descStr;
                while (!remaining.empty()) {
                    int cut = (int)remaining.length();
                    for (int ci = 0; ci <= (int)remaining.length(); ++ci) {
                        std::string sub = remaining.substr(0, ci);
                        Vector2 subSz = MeasureTextEx(res.staminaFont, sub.c_str(), 20, 1);
                        if (subSz.x > maxDescW - 10) {
                            cut = ci - 1;
                            break;
                        }
                    }
                    if (cut <= 0) cut = 1;
                    std::string line = remaining.substr(0, cut);
                    DrawTextEx(res.staminaFont, line.c_str(), { descX, descY }, 20, 1, WHITE);
                    descY += 26;
                    remaining = remaining.substr(cut);
                    if (descY > panelY + panelH - 80) break;
                }

                lineY = descY + 8;
            }

            // ── 查看等级提升条件按钮 ──
            float infoBtnX = panelX + 15;
            float infoBtnY = panelY + panelH - 55;
            float infoBtnW = panelW - 30;
            float infoBtnH = 42;
            bool infoHover = HitTestRect(mousePos, infoBtnX, infoBtnY, infoBtnW, infoBtnH);
            Color infoColor = infoHover ? Color{ 80, 80, 200, 220 } : Color{ 60, 60, 140, 180 };
            DrawRectangleRounded({ infoBtnX, infoBtnY, infoBtnW, infoBtnH }, 0.2f, 8, infoColor);
            DrawRectangleRoundedLines({ infoBtnX, infoBtnY, infoBtnW, infoBtnH }, 0.2f, 8,
                                      Color{ 120, 120, 255, 200 });
            const char* infoLabel = "查看升级条件";
            Vector2 infoSz = MeasureTextEx(res.staminaFont, infoLabel, 26, 1);
            DrawTextEx(res.staminaFont, infoLabel,
                { infoBtnX + (infoBtnW - infoSz.x) / 2, infoBtnY + (infoBtnH - infoSz.y) / 2 },
                26, 1, WHITE);

            // ── 升级条件弹窗 ──
            if (state.showFighterUpgradeInfo) {
                float dlgX = panelX + 20;
                float dlgY = panelY + 50;
                float dlgW = panelW - 40;
                float dlgH = panelH - 110;
                DrawRectangle((int)dlgX, (int)dlgY, (int)dlgW, (int)dlgH,
                              Color{ 20, 20, 40, 235 });
                DrawRectangleLines((int)dlgX, (int)dlgY, (int)dlgW, (int)dlgH,
                                   Color{ 200, 200, 255, 180 });

                float dlineY = dlgY + 18;
                DrawTextEx(res.staminaFont, "升级条件", { dlgX + 15, dlineY }, 30, 1, GOLD);
                dlineY += 46;

                int nextAtk = (int)(cur->getBaseAttack() * std::pow(1.2, oldLv));
                int nextDef = (int)(cur->getBaseDefense() * std::pow(1.2, oldLv));
                int nextMaxHp = (int)(1000 * std::pow(1.2, oldLv));

                // 用 > 替代箭头符号，避免乱码
                char condBuf[320];
                snprintf(condBuf, sizeof(condBuf),
                    "当前等级: %d\n"
                    "升级所需: 升级石 x%d\n\n"
                    "下一级属性预览:\n"
                    "  生命值: %d > %d\n"
                    "  攻击力: %d > %d\n"
                    "  防御力: %d > %d\n\n"
                    "（升级后生命值按比例保留）",
                    oldLv, needStone,
                    oldHp, nextMaxHp,
                    oldAtk, nextAtk,
                    oldDef, nextDef);

                const char* lineStart = condBuf;
                const char* p = condBuf;
                while (*p) {
                    if (*p == '\n') {
                        std::string line(lineStart, p - lineStart);
                        Vector2 lsz = MeasureTextEx(res.staminaFont, line.c_str(), 24, 1);
                        DrawTextEx(res.staminaFont, line.c_str(),
                            { dlgX + 15, dlineY }, 24, 1, WHITE);
                        dlineY += 32;
                        lineStart = p + 1;
                    }
                    ++p;
                }
                if (*lineStart) {
                    Vector2 lsz = MeasureTextEx(res.staminaFont, lineStart, 24, 1);
                    DrawTextEx(res.staminaFont, lineStart,
                        { dlgX + 15, dlineY }, 24, 1, WHITE);
                }

                float closeBtnX = dlgX + dlgW - 120;
                float closeBtnY = dlgY + 12;
                float closeBtnW = 100;
                float closeBtnH = 36;
                bool closeHover = HitTestRect(mousePos, closeBtnX, closeBtnY, closeBtnW, closeBtnH);
                Color closeColor = closeHover ? Color{ 200, 60, 60, 220 } : Color{ 160, 40, 40, 180 };
                DrawRectangleRounded({ closeBtnX, closeBtnY, closeBtnW, closeBtnH }, 0.2f, 8, closeColor);
                const char* closeLbl = "关闭";
                Vector2 closeSz = MeasureTextEx(res.staminaFont, closeLbl, 22, 1);
                DrawTextEx(res.staminaFont, closeLbl,
                    { closeBtnX + (closeBtnW - closeSz.x) / 2, closeBtnY + (closeBtnH - closeSz.y) / 2 },
                    22, 1, WHITE);
            }
        }
    }
}

// ============================================================
// 处理伙伴界面鼠标输入
// ============================================================
void HandleFriendsScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();
    Rectangle br = GetImageDrawRect(res.friendsTex, curW, curH);

    // ---- 只有点击右上角退出按钮才能退出 ----
    if (HitTestRect(mousePos, state.friendExitBtnRect.x, state.friendExitBtnRect.y,
                    state.friendExitBtnRect.width, state.friendExitBtnRect.height)) {
        state.screenState = SCREEN_MAIN;
        return;
    }

    // ---- 如果有升级信息弹窗打开，优先处理弹窗关闭 ----
    if (state.showFighterUpgradeInfo) {
        float panelX = br.x + br.width * 0.55f;
        float panelY = br.y + 30;
        float panelW = br.width * 0.38f;
        float dlgX = panelX + 20;
        float dlgY = panelY + 50;
        float dlgW = panelW - 40;
        float closeBtnX = dlgX + dlgW - 120;
        float closeBtnY = dlgY + 12;
        float closeBtnW = 100;
        float closeBtnH = 36;
        if (HitTestRect(mousePos, closeBtnX, closeBtnY, closeBtnW, closeBtnH)) {
            state.showFighterUpgradeInfo = false;
            return;
        }
        // 点击弹窗外部也关闭
        if (!HitTestRect(mousePos, dlgX, dlgY, dlgW, 450)) {
            state.showFighterUpgradeInfo = false;
            return;
        }
        return;
    }

    // ---- 检测升级按钮 ----
    if (HitTestRect(mousePos, state.fighterUpgradeBtnRect.x, state.fighterUpgradeBtnRect.y,
                    state.fighterUpgradeBtnRect.width, state.fighterUpgradeBtnRect.height)) {
        std::vector<Combatant*> fighters = game->getFighters();
        Inventory* inventory = game->getInventory();
        if (!fighters.empty() && inventory) {
            Combatant* cur = fighters[0];
            int need = cur->getUpgradeStoneCost();
            if (inventory->hasItem("upgrade_stone", need)) {
                inventory->removeItem("upgrade_stone", need);
                cur->levelUp();
            }
        }
        return;
    }

    // ---- 检测"查看升级条件"按钮 ----
    float panelX = br.x + br.width * 0.55f;
    float panelY = br.y + 30;
    float panelW = br.width * 0.38f;
    float panelH = br.height - 80;
    float infoBtnX = panelX + 15;
    float infoBtnY = panelY + panelH - 55;
    float infoBtnW = panelW - 30;
    float infoBtnH = 42;
    if (HitTestRect(mousePos, infoBtnX, infoBtnY, infoBtnW, infoBtnH)) {
        state.showFighterUpgradeInfo = !state.showFighterUpgradeInfo;
        return;
    }

    // ---- 检测"查看更多伙伴"按钮 ----
    Vector2 btnP1 = DesignToScreen(1100.0f, 600.0f, res.friendsTex, curW, curH);
    Vector2 btnP2 = DesignToScreen(1350.0f, 800.0f, res.friendsTex, curW, curH);
    float btnX = btnP1.x, btnY = btnP1.y, btnW = btnP2.x - btnP1.x, btnH = btnP2.y - btnP1.y;
    if (HitTestRect(mousePos, btnX, btnY, btnW, btnH)) {
        state.screenState = SCREEN_RECRUIT;
        return;
    }
}

// ============================================================
// 绘制 & 输入：故事界面1
// ============================================================
void RenderStory1Screen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.story1Tex, curW, curH);
}

void HandleStory1ScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();
    Vector2 imgPt = DesignToScreen(800.0f, 0.0f, res.story1Tex, curW, curH);
    if (mousePos.x > imgPt.x) {
        state.screenState = SCREEN_STORY2;
    }
}

// ============================================================
// 绘制 & 输入：故事界面2
// ============================================================
void RenderStory2Screen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.story2Tex, curW, curH);
}

void HandleStory2ScreenInput(UIState& state) {
    state.screenState = SCREEN_MAIN;
}

// ============================================================
// 绘制 & 输入：角色档案界面
// ============================================================
void RenderCharacterFileScreen(UIResource& res, UIState& state, int curW, int curH) {
    DrawImageCentered(res.charFileTex, curW, curH);
    DrawCharFileGrid(res.charFileTex, curW, curH);

    // 计算位置
    Vector2 inputP1 = DesignToScreen(1180.0f, 250.0f, res.charFileTex, curW, curH);
    Vector2 inputP2 = DesignToScreen(1300.0f, 280.0f, res.charFileTex, curW, curH);
    float boxW = inputP2.x - inputP1.x;
    float boxH = inputP2.y - inputP1.y;
    float btnW = boxH * 1.5f;
    float btnH = boxH;
    float btnX = inputP2.x + 10;
    float btnY = inputP1.y;

    Rectangle r = GetImageDrawRect(res.charFileTex, curW, curH);
    float scrScaleX = r.width / (float)res.charFileTex.width;
    float scrScaleY = r.height / (float)res.charFileTex.height;

    // 输入框
    Color inputBg = state.nameEditing ? Color{ 60, 60, 80, 220 } : Color{ 40, 40, 60, 200 };
    DrawRectangle((int)inputP1.x, (int)inputP1.y, (int)boxW, (int)boxH, inputBg);
    DrawRectangleLines((int)inputP1.x, (int)inputP1.y, (int)boxW, (int)boxH, Color{ 200, 200, 255, 220 });

    int inputFontSize = (int)(22 * ((scrScaleX + scrScaleY) / 2.0f));
    if (inputFontSize < 10) inputFontSize = 10;
    Vector2 textPos = { inputP1.x + 5, inputP1.y + (boxH - inputFontSize) / 2.0f };
    DrawTextEx(res.staminaFont, state.charName.c_str(), textPos, (float)inputFontSize, 1, WHITE);

    if (state.nameEditing) {
        Vector2 measured = MeasureTextEx(res.staminaFont, state.charName.c_str(), (float)inputFontSize, 1);
        float cursorX = textPos.x + measured.x + 2;
        if (cursorX < inputP2.x - 5) {
            int cursorY1 = (int)inputP1.y + 4;
            int cursorY2 = (int)inputP2.y - 4;
            DrawLine((int)cursorX, cursorY1, (int)cursorX, cursorY2, Color{ 255, 255, 255, 200 });
        }
    }

    // 保存按钮
    Color btnColor = { 80, 180, 80, 220 };
    DrawRectangle((int)btnX, (int)btnY, (int)btnW, (int)btnH, btnColor);
    DrawRectangleLines((int)btnX, (int)btnY, (int)btnW, (int)btnH, Color{ 150, 255, 150, 200 });

    int btnFontSize = (int)(20 * ((scrScaleX + scrScaleY) / 2.0f));
    if (btnFontSize < 10) btnFontSize = 10;
    const char* btnText = "保存";
    Vector2 btnTextSize = MeasureTextEx(res.staminaFont, btnText, (float)btnFontSize, 1);
    Vector2 btnTextPos = { btnX + (btnW - btnTextSize.x) / 2.0f, btnY + (btnH - btnTextSize.y) / 2.0f };
    DrawTextEx(res.staminaFont, btnText, btnTextPos, (float)btnFontSize, 1, WHITE);
}

// ============================================================
// 处理角色档案界面的键盘输入
// ============================================================
void HandleCharacterFileKeyboardInput(UIResource& res, UIState& state, int curW, int curH) {
    if (state.screenState != SCREEN_CHARACTER_FILE) return;
    if (!state.nameEditing) return;

    // 获取字符输入
    int codepoint = GetCharPressed();
    while (codepoint > 0) {
        if (state.charName.length() < 30) {
            int byteCount = 0;
            const char* utf8Bytes = CodepointToUTF8(codepoint, &byteCount);
            for (int i = 0; i < byteCount; i++) {
                state.charName += utf8Bytes[i];
            }
        }
        codepoint = GetCharPressed();
    }

    // 退格
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
        static int backspaceTimer = 0;
        backspaceTimer++;
        if (IsKeyPressed(KEY_BACKSPACE) || backspaceTimer % 6 == 0) {
            if (!state.charName.empty()) {
                state.charName.pop_back();
                while (!state.charName.empty() && ((unsigned char)state.charName.back() & 0xC0) == 0x80) {
                    state.charName.pop_back();
                }
            }
        }
        if (!IsKeyDown(KEY_BACKSPACE)) backspaceTimer = 0;
    }

    // 回车确认
    if (IsKeyPressed(KEY_ENTER)) {
        state.nameEditing = false;
    }
}

// ============================================================
// 处理角色档案界面的鼠标输入
// ============================================================
void HandleCharacterFileScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();
    Vector2 inputP1 = DesignToScreen(1180.0f, 250.0f, res.charFileTex, curW, curH);
    Vector2 inputP2 = DesignToScreen(1300.0f, 280.0f, res.charFileTex, curW, curH);
    float boxW = inputP2.x - inputP1.x;
    float boxH = inputP2.y - inputP1.y;
    float btnW = boxH * 1.5f;
    float btnH = boxH;
    float btnX = inputP2.x + 10;
    float btnY = inputP1.y;

    if (HitTestRect(mousePos, inputP1.x, inputP1.y, boxW, boxH)) {
        state.nameEditing = true;
    } else if (HitTestRect(mousePos, btnX, btnY, btnW, btnH)) {
        state.nameEditing = false;
    } else {
        state.nameEditing = false;
        state.screenState = SCREEN_MAIN;
    }
}
