#include "SplashScreen.h"
#include "BagScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "system/backpack/Inventory.h"
#include "item/ItemTemplate.h"
#include "role/character/MainCharacter.h"
#include <cstdio>
#include <cmath>
#include <cstring>

// ============================================================
// 绘制 & 输入：背包界面
// ============================================================
void RenderBagScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    DrawImageCentered(res.bagTex, curW, curH);
    Rectangle br = GetImageDrawRect(res.bagTex, curW, curH);

    // 每帧清空并重新构建点击区域
    state.bagItemHitRects.clear();

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

        // 保存物品点击区域（用于打开详情弹窗）
        state.bagItemHitRects.push_back({ix - 8, iy - 8, itemW + 16, itemH + 40, count, itemId});

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

    // 点击退出按钮返回主界面
    if (HitTestRect(mousePos, state.bagExitBtnRect.x, state.bagExitBtnRect.y,
                    state.bagExitBtnRect.width, state.bagExitBtnRect.height)) {
        state.screenState = SCREEN_MAIN;
        return;
    }

    // 点击物品打开详情弹窗
    for (auto& h : state.bagItemHitRects) {
        if (HitTestRect(mousePos, h.x, h.y, h.w, h.h)) {
            const std::string& itemId = h.itemId;

            // 查找模板获取详细信息
            auto allTemplates = ItemTemplate::loadAllTemplates();
            const ItemTemplate* tmpl = nullptr;
            for (const auto& t : allTemplates) {
                if (t.getId() == itemId) {
                    tmpl = &t;
                    break;
                }
            }

            state.bagDetailItemId = itemId;
            state.bagDetailPrice = tmpl ? tmpl->getPrice() : 0;

            // 设置物品名称
            auto getItemName = [](const std::string& id) -> std::string {
                if (id == "potion_003") return "止痛片";
                if (id == "potion_002") return "外伤药包";
                if (id == "potion_001") return "急救包";
                if (id == "food_001")   return "罐头";
                if (id == "food_002")   return "压缩饼干";
                if (id == "food_003")   return "瓶装水";
                if (id == "upgrade_stone") return "升级石";
                if (id == "recruit_card")  return "招募卡";
                if (id.find("weapon_s") != std::string::npos || id.find("sword") != std::string::npos) return "剑";
                if (id.find("weapon_b") != std::string::npos || id.find("book") != std::string::npos) return "魔法书";
                if (id.find("weapon_h") != std::string::npos || id.find("shield") != std::string::npos) return "护盾";
                return id;
            };
            state.bagDetailItemName = tmpl ? tmpl->getName() : getItemName(itemId);

            // 设置物品分类
            state.bagDetailCategory = tmpl ? tmpl->getCategory() : "";

            // 设置物品描述
            auto getItemDesc = [](const std::string& id) -> std::string {
                if (id == "potion_003") return "恢复生命25点";
                if (id == "potion_002") return "恢复生命40点";
                if (id == "potion_001") return "恢复生命80点";
                if (id == "food_001")   return "恢复体力50点";
                if (id == "food_002")   return "恢复体力30点";
                if (id == "food_003")   return "恢复体力20点";
                if (id == "upgrade_stone") return "用于战斗角色升级";
                if (id == "recruit_card")  return "消耗1张招募卡随机获得1个战斗角色";
                if (id.find("sword") != std::string::npos) return "剑类装备：攻击加成，装备后全队生效";
                if (id.find("book") != std::string::npos) return "魔法书装备：蓝条和生命恢复，装备后全队生效";
                if (id.find("shield") != std::string::npos) return "护盾装备：防御加成，装备后全队生效";
                return "";
            };
            state.bagDetailDesc = tmpl ? tmpl->getDescription() : getItemDesc(itemId);

            // 打开详情弹窗
            state.bagShowDetail = true;
            return;
        }
    }
}

// ============================================================
// 绘制 & 输入：任务列表界面
