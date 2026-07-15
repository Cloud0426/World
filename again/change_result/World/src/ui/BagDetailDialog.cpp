#include "SplashScreen.h"
#include "BagDetailDialog.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "system/backpack/Inventory.h"
#include "item/ItemTemplate.h"
#include "role/character/MainCharacter.h"
#include "role/combatant/Combatant.h"
#include <cstdio>
#include <cmath>
#include <cstring>

// ============================================================
// 背包物品详情弹窗 — 绘制
// ============================================================
void RenderBagDetailDialog(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    if (!state.bagShowDetail) return;

    Vector2 mousePos = GetMousePosition();

    // ---- 半透明遮罩 ----
    DrawRectangle(0, 0, curW, curH, Color{ 0, 0, 0, 160 });

    // ---- 弹窗尺寸 ----
    float dlgW = 520;
    float dlgH = 380;
    float dlgX = (curW - dlgW) / 2;
    float dlgY = (curH - dlgH) / 2;

    // ---- 弹窗背景 ----
    DrawRectangleRounded({ dlgX, dlgY, dlgW, dlgH }, 0.08f, 8, Color{ 35, 35, 55, 240 });
    DrawRectangleRoundedLines({ dlgX, dlgY, dlgW, dlgH }, 0.08f, 8, Color{ 180, 180, 220, 200 });

    // ---- 物品图标 ----
    float iconSize = 100;
    float iconX = dlgX + (dlgW - iconSize) / 2;
    float iconY = dlgY + 20;
    int iconIdx = state.getStoreIconIdx(state.bagDetailItemId);
    if (iconIdx >= 0 && (size_t)iconIdx < res.storeIcons.size() && res.storeIcons[iconIdx].id > 0) {
        DrawTexturePro(res.storeIcons[iconIdx],
            {0, 0, (float)res.storeIcons[iconIdx].width, (float)res.storeIcons[iconIdx].height},
            {iconX, iconY, iconSize, iconSize}, {0, 0}, 0.0f, WHITE);
    }

    // ---- 物品名称 ----
    float nameY = iconY + iconSize + 10;
    Vector2 nameSz = MeasureTextEx(res.staminaFont, state.bagDetailItemName.c_str(), 30, 1);
    DrawTextEx(res.staminaFont, state.bagDetailItemName.c_str(),
        { dlgX + (dlgW - nameSz.x) / 2, nameY }, 30, 1, GOLD);

    // ---- 物品作用描述 ----
    float descY = nameY + 45;
    Vector2 descSz = MeasureTextEx(res.staminaFont, state.bagDetailDesc.c_str(), 22, 1);
    DrawTextEx(res.staminaFont, state.bagDetailDesc.c_str(),
        { dlgX + (dlgW - descSz.x) / 2, descY }, 22, 1, WHITE);

    // ---- 分类标签 ----
    float catY = descY + 35;
    const char* catLabel = "";
    if (state.bagDetailCategory == "food") catLabel = "食品类";
    else if (state.bagDetailCategory == "potion") catLabel = "药品类";
    else if (state.bagDetailCategory == "weapon") catLabel = "装备类（全队生效）";
    else if (state.bagDetailCategory == "material") catLabel = "材料类";
    else if (state.bagDetailCategory == "special") catLabel = "特殊物品";
    if (catLabel[0] != '\0') {
        Vector2 catSz = MeasureTextEx(res.staminaFont, catLabel, 18, 1);
        DrawTextEx(res.staminaFont, catLabel,
            { dlgX + (dlgW - catSz.x) / 2, catY }, 18, 1, SKYBLUE);
    }

    // ---- 三个按钮 ----
    float btnY = dlgY + dlgH - 70;
    float btnW = 130;
    float btnH = 46;
    float gap = 20;
    float totalW = btnW * 3 + gap * 2;
    float btnStartX = dlgX + (dlgW - totalW) / 2;

        // 使用按钮 — 药品和升级石不显示使用按钮
    const std::string& useItemId = state.bagDetailItemId;
    const std::string& useCat = state.bagDetailCategory;
    bool showUseBtn = (useCat != "potion" && useItemId.find("upgrade_stone") == std::string::npos);
    
    if (showUseBtn) {
        float useBtnX = btnStartX;
        bool useHover = HitTestRect(mousePos, useBtnX, btnY, btnW, btnH);
        Color useColor = useHover ? Color{ 60, 180, 60, 240 } : Color{ 40, 140, 40, 200 };
        DrawRectangleRounded({ useBtnX, btnY, btnW, btnH }, 0.2f, 8, useColor);
        DrawRectangleRoundedLines({ useBtnX, btnY, btnW, btnH }, 0.2f, 8, Color{ 100, 255, 100, 200 });
        Vector2 useSz = MeasureTextEx(res.staminaFont, "使用", 26, 1);
        DrawTextEx(res.staminaFont, "使用",
            { useBtnX + (btnW - useSz.x) / 2, btnY + (btnH - useSz.y) / 2 }, 26, 1, WHITE);
        state.bagDetailUseBtnRect = { useBtnX, btnY, btnW, btnH };
        btnStartX += btnW + gap;
    } else {
        state.bagDetailUseBtnRect = {0, 0, 0, 0};
    }

        // 取消按钮
    float cancelBtnX = btnStartX;
    bool cancelHover = HitTestRect(mousePos, cancelBtnX, btnY, btnW, btnH);
    Color cancelColor = cancelHover ? Color{ 100, 100, 100, 220 } : Color{ 70, 70, 70, 180 };
    DrawRectangleRounded({ cancelBtnX, btnY, btnW, btnH }, 0.2f, 8, cancelColor);
    DrawRectangleRoundedLines({ cancelBtnX, btnY, btnW, btnH }, 0.2f, 8, Color{ 180, 180, 180, 200 });
    Vector2 cancelSz = MeasureTextEx(res.staminaFont, "取消", 26, 1);
    DrawTextEx(res.staminaFont, "取消",
        { cancelBtnX + (btnW - cancelSz.x) / 2, btnY + (btnH - cancelSz.y) / 2 }, 26, 1, WHITE);
    state.bagDetailCancelBtnRect = { cancelBtnX, btnY, btnW, btnH };

    // 出售按钮
    float sellBtnX = cancelBtnX + btnW + gap;
    bool sellHover = HitTestRect(mousePos, sellBtnX, btnY, btnW, btnH);
    Color sellColor = sellHover ? Color{ 200, 160, 40, 240 } : Color{ 180, 130, 30, 200 };
    DrawRectangleRounded({ sellBtnX, btnY, btnW, btnH }, 0.2f, 8, sellColor);
    DrawRectangleRoundedLines({ sellBtnX, btnY, btnW, btnH }, 0.2f, 8, Color{ 255, 220, 80, 200 });

    int sellPrice = (int)(state.bagDetailPrice * 0.5f);
    char sellLabel[32];
    snprintf(sellLabel, sizeof(sellLabel), "出售(%d金)", sellPrice);
    Vector2 sellSz = MeasureTextEx(res.staminaFont, sellLabel, 22, 1);
    DrawTextEx(res.staminaFont, sellLabel,
        { sellBtnX + (btnW - sellSz.x) / 2, btnY + (btnH - sellSz.y) / 2 }, 22, 1, WHITE);
    state.bagDetailSellBtnRect = { sellBtnX, btnY, btnW, btnH };
}

// ============================================================
// 背包物品详情弹窗 — 输入处理
// ============================================================
void HandleBagDetailDialogInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) 
{
    if (!state.bagShowDetail) return;

    Vector2 mousePos = GetMousePosition();

        // ---- 点击使用按钮 ----
    if (HitTestRect(mousePos, state.bagDetailUseBtnRect.x, state.bagDetailUseBtnRect.y,
                    state.bagDetailUseBtnRect.width, state.bagDetailUseBtnRect.height)) {
        Inventory* inv = game->getInventory();
        MainCharacter* player = game->getMainChar();
                const std::string& itemId = state.bagDetailItemId;
        const std::string& cat = state.bagDetailCategory;
                if (!inv || !player || !inv->hasItem(itemId, 1)) {
            // 没有该物品或指针无效，关闭弹窗
            state.bagShowDetail = false;
            return;
        }

                                                                        if (cat == "food") {
                // 食物：恢复体力
                auto allTemplates = ItemTemplate::loadAllTemplates();
                int restoreAmt = 0;
                for (const auto& t : allTemplates) {
                    if (t.getId() == itemId) {
                        restoreAmt = t.getEffectValue();
                        break;
                    }
                }
                player->restoreStamina(restoreAmt);
                inv->removeItem(itemId, 1);
                state.bagShowDetail = false;
            }
                        else if (cat == "weapon" || cat == "equipment") {
                // 🆕 装备：打开角色选择弹窗
                state.bagEquipItemId = itemId;
                state.bagShowDetail = false;
                state.bagShowEquipSelect = true;
            }
                        else {
                            state.bagShowDetail = false;
                        }
        }


    // ---- 点击取消按钮 ----
    if (HitTestRect(mousePos, state.bagDetailCancelBtnRect.x, state.bagDetailCancelBtnRect.y,
                    state.bagDetailCancelBtnRect.width, state.bagDetailCancelBtnRect.height)) {
        state.bagShowDetail = false;
        return;
    }

    // ---- 点击出售按钮 ----
    if (HitTestRect(mousePos, state.bagDetailSellBtnRect.x, state.bagDetailSellBtnRect.y,
                    state.bagDetailSellBtnRect.width, state.bagDetailSellBtnRect.height)) {
        Inventory* inv = game->getInventory();
        MainCharacter* player = game->getMainChar();
        if (inv && player && inv->hasItem(state.bagDetailItemId, 1)) {
            int sellPrice = (int)(state.bagDetailPrice * 0.5f);
            if (sellPrice > 0) {
                inv->removeItem(state.bagDetailItemId, 1);
                player->addGold(sellPrice);
            }
            state.bagShowDetail = false;
        }
        return;
    }

    // ---- 点击弹窗外部关闭 ----
    float dlgW = 520;
    float dlgH = 380;
    float dlgX = (curW - dlgW) / 2;
    float dlgY = (curH - dlgH) / 2;
    if (!HitTestRect(mousePos, dlgX, dlgY, dlgW, dlgH)) {
        state.bagShowDetail = false;
    }
}


// ============================================================
// 背包装备选择角色弹窗 — 绘制
// ============================================================
void RenderBagEquipSelectDialog(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    if (!state.bagShowEquipSelect) return;

    Vector2 mousePos = GetMousePosition();
    std::vector<Combatant*> fighters = game->getFighters();

    float cardH = 160.0f;
    float cardGap = 12.0f;
    float dlgW = 560;
    float titleH = 60.0f;
    float bottomPadding = 30.0f;
    float totalH = titleH + fighters.size() * (cardH + cardGap) + bottomPadding;
    if (totalH > curH * 0.8f) totalH = curH * 0.8f;
    float dlgX = (curW - dlgW) / 2;
    float dlgY = (curH - totalH) / 2;

    DrawRectangle(0, 0, curW, curH, Color{ 0, 0, 0, 180 });

    DrawRectangleRounded({ dlgX, dlgY, dlgW, totalH }, 0.08f, 8, Color{ 30, 30, 55, 245 });
    DrawRectangleRoundedLines({ dlgX, dlgY, dlgW, totalH }, 0.08f, 8, Color{ 180, 180, 220, 200 });

    const char* title = u8"选择装备角色";
    Vector2 titleSz = MeasureTextEx(res.staminaFont, title, 32, 1);
    DrawTextEx(res.staminaFont, title,
        { dlgX + (dlgW - titleSz.x) / 2, dlgY + 15 }, 32, 1, GOLD);

    float cardStartY = dlgY + titleH;
    for (size_t i = 0; i < fighters.size(); ++i) {
        Combatant* f = fighters[i];
        if (!f) continue;
        float cy = cardStartY + i * (cardH + cardGap);
        if (cy + cardH > dlgY + totalH - 10) break;

        bool hovered = HitTestRect(mousePos, dlgX + 15, cy, dlgW - 30, cardH);
        Color cardBg = hovered ? Color{ 60, 60, 100, 220 } : Color{ 40, 40, 70, 200 };
        DrawRectangleRounded({ dlgX + 15, cy, dlgW - 30, cardH }, 0.1f, 8, cardBg);
        if (hovered) {
            DrawRectangleRoundedLines({ dlgX + 15, cy, dlgW - 30, cardH }, 0.1f, 8, Color{ 100, 200, 255, 220 });
        }

        // Q版图
        float qx = dlgX + 25;
        float qy = cy + 15;
        float qSize = cardH - 30;
        Texture2D* qTex = nullptr;
        std::string college = f->getCollege();
        if (college.find(u8"能源") != std::string::npos || f->getName() == u8"陈尔愿") {
            if (res.energyCharTex.id > 0) qTex = &res.energyCharTex;
        } else {
            if (res.machineCharTex.id > 0) qTex = &res.machineCharTex;
        }
        if (qTex) {
            float txW = (float)qTex->width;
            float txH = (float)qTex->height;
            float scale = qSize / std::max(txW, txH);
            float drawW = txW * scale;
            float drawH = txH * scale;
            float drawX = qx + (qSize - drawW) / 2;
            float drawY = qy + (qSize - drawH) / 2;
            DrawTexturePro(*qTex, { 0, 0, txW, txH }, { drawX, drawY, drawW, drawH }, { 0, 0 }, 0.0f, WHITE);
        } else {
            DrawRectangle((int)qx, (int)qy, (int)qSize, (int)qSize, Color{ 60, 60, 80, 180 });
        }

        float infoX = qx + qSize + 20;
        float infoY = cy + 18;
        DrawTextEx(res.staminaFont, f->getName().c_str(), { infoX, infoY }, 30, 1, GOLD);
        DrawTextEx(res.staminaFont, f->getCollege().c_str(), { infoX, infoY + 36 }, 22, 1, LIGHTGRAY);
        char lvBuf[32];
        snprintf(lvBuf, sizeof(lvBuf), "Lv.%d", f->getLevel());
        DrawTextEx(res.staminaFont, lvBuf, { infoX, infoY + 66 }, 22, 1, WHITE);
        int eqCnt = f->getEquippedCount();
        char eqBuf[64];
        snprintf(eqBuf, sizeof(eqBuf), u8"装备: %d/2", eqCnt);
        DrawTextEx(res.staminaFont, eqBuf, { infoX, infoY + 96 }, 20, 1, eqCnt < 2 ? GREEN : RED);

        // 装备按钮
        float btnX = dlgX + dlgW - 160;
        float btnY = cy + cardH - 48;
        float btnW2 = 130;
        float btnH2 = 36;
        bool btnHover = HitTestRect(mousePos, btnX, btnY, btnW2, btnH2);
        Color btnColor = btnHover ? Color{ 60, 200, 60, 240 } : Color{ 40, 160, 40, 200 };
        DrawRectangleRounded({ btnX, btnY, btnW2, btnH2 }, 0.2f, 8, btnColor);
        const char* btnLabel = u8"装备到此角色";
        Vector2 btnSz = MeasureTextEx(res.staminaFont, btnLabel, 18, 1);
        DrawTextEx(res.staminaFont, btnLabel,
            { btnX + (btnW2 - btnSz.x) / 2, btnY + (btnH2 - btnSz.y) / 2 }, 18, 1, WHITE);
    }
}

// ============================================================
// 背包装备选择角色弹窗 — 输入处理
// ============================================================
void HandleBagEquipSelectDialogInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    if (!state.bagShowEquipSelect) return;

    Vector2 mousePos = GetMousePosition();
    Inventory* inv = game->getInventory();
    std::vector<Combatant*> fighters = game->getFighters();

    if (!inv || fighters.empty()) {
        state.bagShowEquipSelect = false;
        return;
    }

    float cardH = 160.0f;
    float cardGap = 12.0f;
    float dlgW = 560;
    float titleH = 60.0f;
    float bottomPadding = 30.0f;
    float totalH = titleH + fighters.size() * (cardH + cardGap) + bottomPadding;
    if (totalH > curH * 0.8f) totalH = curH * 0.8f;
    float dlgX = (curW - dlgW) / 2;
    float dlgY = (curH - totalH) / 2;

    float cardStartY = dlgY + titleH;
    for (size_t i = 0; i < fighters.size(); ++i) {
        Combatant* f = fighters[i];
        if (!f) continue;
        float cy = cardStartY + i * (cardH + cardGap);
        float btnX = dlgX + dlgW - 160;
        float btnY = cy + cardH - 48;
        float btnW = 130;
        float btnH = 36;

        if (HitTestRect(mousePos, btnX, btnY, btnW, btnH)) {
            const std::string& itemId = state.bagEquipItemId;
            if (f->getEquippedCount() >= 2) {
                state.bagShowEquipSelect = false;
                return;
            }

            Equipment eq;
            eq.id = itemId;
            auto allTemplates = ItemTemplate::loadAllTemplates();
            const ItemTemplate* tmpl = nullptr;
            for (const auto& t : allTemplates) {
                if (t.getId() == itemId) { tmpl = &t; break; }
            }
            if (tmpl) {
                eq.name = tmpl->getName();
                eq.level = tmpl->getLevel();
                if (itemId.find("sword") != std::string::npos || itemId.find("weapon_s") != std::string::npos) {
                    eq.type = "weapon";
                    eq.attackBonus = tmpl->getEffectValue() / 100.0;
                } else if (itemId.find("book") != std::string::npos || itemId.find("weapon_b") != std::string::npos) {
                    eq.type = "book";
                    eq.mpBonus = tmpl->getEffectValue() / 100.0;
                } else if (itemId.find("shield") != std::string::npos || itemId.find("weapon_h") != std::string::npos) {
                    eq.type = "shield";
                    eq.defenseBonus = tmpl->getEffectValue() / 100.0;
                }
            }
            // 处理纯ID（没有模板的情况）
            if (itemId == "weapon_s1") { eq.name=u8"初级剑"; eq.type="weapon"; eq.level=1; eq.attackBonus=0.05; }
            else if (itemId == "weapon_s2") { eq.name=u8"中级剑"; eq.type="weapon"; eq.level=2; eq.attackBonus=0.10; }
            else if (itemId == "weapon_s3") { eq.name=u8"高级剑"; eq.type="weapon"; eq.level=3; eq.attackBonus=0.15; }
            else if (itemId == "weapon_b1") { eq.name=u8"初级魔法书"; eq.type="book"; eq.level=1; eq.mpBonus=0.05; eq.hpRestore=10; }
            else if (itemId == "weapon_b2") { eq.name=u8"中级魔法书"; eq.type="book"; eq.level=2; eq.mpBonus=0.10; eq.hpRestore=15; }
            else if (itemId == "weapon_b3") { eq.name=u8"高级魔法书"; eq.type="book"; eq.level=3; eq.mpBonus=0.15; eq.hpRestore=20; }
            else if (itemId == "weapon_h1") { eq.name=u8"初级护盾"; eq.type="shield"; eq.level=1; eq.defenseBonus=0.05; }
            else if (itemId == "weapon_h2") { eq.name=u8"中级护盾"; eq.type="shield"; eq.level=2; eq.defenseBonus=0.10; }
            else if (itemId == "weapon_h3") { eq.name=u8"高级护盾"; eq.type="shield"; eq.level=3; eq.defenseBonus=0.15; }
            else if (itemId == "weapon_shield_1") { eq.name=u8"初级护盾"; eq.type="shield"; eq.level=1; eq.defenseBonus=0.05; }

            f->equipItem(eq);
            inv->removeItem(itemId, 1);
            state.bagShowEquipSelect = false;
            return;
        }
    }

    if (!HitTestRect(mousePos, dlgX, dlgY, dlgW, totalH)) {
        state.bagShowEquipSelect = false;
    }
}
