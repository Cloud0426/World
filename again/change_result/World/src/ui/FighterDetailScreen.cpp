#include "UIHelper.h"
#include "UIResource.h"
#include "UIShared.h"
#include "game/GameManager.h"
#include "role/combatant/Combatant.h"
#include <cmath>

extern void DrawImageCentered(Texture2D tex, int curW, int curH);
extern void DrawImageFillScreen(Texture2D tex, int curW, int curH);
extern Rectangle GetImageDrawRect(Texture2D tex, int curW, int curH);
extern bool HitTestRect(Vector2 pos, float x, float y, float w, float h);

// ============================================================
// 陈尔愿界面 — 绘制（内容固定为能源学院属性）
// ============================================================
void RenderFighterDetailScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    // ---- 背景：ChenErYuan.png 按原比例缩放，不足用黑色补齐 ----
    if (res.chenErYuanTex.id > 0) {
        DrawImageFillScreen(res.chenErYuanTex, curW, curH);
    } else {
        DrawRectangle(0, 0, curW, curH, Color{ 15, 15, 30, 255 });
    }

    // ---- 信息面板：与"钟关白界面"(friends.png)一致的大小和位置 ----
    Rectangle br = GetImageDrawRect(res.chenErYuanTex, curW, curH);
    float panelX = br.x + br.width * 0.55f;
    float panelY = br.y + 30;
    float panelW = br.width * 0.38f;
    float panelH = br.height - 80;

    Vector2 mousePos = GetMousePosition();
    Inventory* inventory = game->getInventory();

    // ---- 战斗角色数据（用于升级逻辑）----
    std::vector<Combatant*> fighters = game->getFighters();
    if (state.fighterSelectedIndex >= (int)fighters.size())
        state.fighterSelectedIndex = (int)fighters.size() - 1;
    if (state.fighterSelectedIndex < 0) state.fighterSelectedIndex = 0;
    Combatant* curFighter = (!fighters.empty()) ? fighters[state.fighterSelectedIndex] : nullptr;
    int oldLv = curFighter ? curFighter->getLevel() : 1;
    int oldAtk = curFighter ? curFighter->getAttack() : 200;
    int oldDef = curFighter ? curFighter->getDefense() : 200;
    int oldHp = curFighter ? curFighter->getMaxHp() : 1000;

    // 半透明面板
    DrawRectangle((int)panelX, (int)panelY, (int)panelW, (int)panelH,
                  Color{ 30, 30, 50, 200 });

        // ---- 右上角返回按钮 ----
    float backBtnX = panelX + panelW - 110;
    float backBtnY = panelY + 10;
    float backBtnW = 100;
    float backBtnH = 40;
    bool backHover = HitTestRect(mousePos, backBtnX, backBtnY, backBtnW, backBtnH);
    Color backColor = backHover ? Color{ 200, 60, 60, 220 } : Color{ 180, 50, 50, 180 };
    DrawRectangleRounded({ backBtnX, backBtnY, backBtnW, backBtnH }, 0.2f, 8, backColor);
    DrawRectangleRoundedLines({ backBtnX, backBtnY, backBtnW, backBtnH }, 0.2f, 8,
                              Color{ 255, 100, 100, 200 });
    Vector2 backSz = MeasureTextEx(res.staminaFont, "返回", 22, 1);
    DrawTextEx(res.staminaFont, "返回",
        { backBtnX + (backBtnW - backSz.x) / 2, backBtnY + (backBtnH - backSz.y) / 2 },
        22, 1, WHITE);

    // ---- 名称：陈尔愿（固定）----
    const char* chenyName = "陈尔愿";
    Vector2 nameSz = MeasureTextEx(res.staminaFont, chenyName, 40, 1);
    DrawTextEx(res.staminaFont, chenyName,
        { panelX + (panelW - nameSz.x) / 2, panelY + 8 }, 40, 1, GOLD);

    float lineY = panelY + 70;
    float col1X = panelX + 25;
    float col2X = panelX + panelW * 0.45f;
    float lh = 44;

    // ── 等级 + 升级石 + 升级按钮 ──
    char lvBuf[64];
    snprintf(lvBuf, sizeof(lvBuf), "Lv.%d", oldLv);
    DrawTextEx(res.staminaFont, lvBuf, { col1X, lineY }, 36, 1, WHITE);

    int stoneCount = inventory ? inventory->getCount("upgrade_stone") : 0;
    char stoneBuf[64];
    snprintf(stoneBuf, sizeof(stoneBuf), "升级石 x%d", stoneCount);
    DrawTextEx(res.staminaFont, stoneBuf, { col2X, lineY }, 28, 1, SKYBLUE);

    int needStone = curFighter ? curFighter->getUpgradeStoneCost() : 3;
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

    // ══════════════════════════════════════════════════════════
    //  以下内容固定为"能源学院·陈尔愿"属性，不读取 Combatant
    // ══════════════════════════════════════════════════════════

    // ── 学院（固定）──
    DrawTextEx(res.staminaFont, "能源学院", { col1X, lineY }, 28, 1, LIGHTGRAY);
    lineY += lh;

    // ── 生命值：1000 + 血条 ──
    int hp = 1000;
    int maxHp = 1000;
    DrawTextEx(res.staminaFont, "生命值", { col1X, lineY }, 28, 1, WHITE);
    char hpBuf[48];
    snprintf(hpBuf, sizeof(hpBuf), "%d / %d", hp, maxHp);
    Vector2 hpSz = MeasureTextEx(res.staminaFont, hpBuf, 28, 1);
    DrawTextEx(res.staminaFont, hpBuf, { panelX + panelW - 20 - hpSz.x, lineY }, 28, 1, RED);
    lineY += 36;

    float barX = col1X;
    float barY = lineY;
    float barW = panelW - 50;
    float barH = 24;
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, Color{ 60, 30, 30, 200 });
    float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0.0f;
    Color hpColor = hpRatio > 0.5f ? GREEN : (hpRatio > 0.25f ? ORANGE : RED);
    DrawRectangle((int)barX, (int)barY, (int)(barW * hpRatio), (int)barH, hpColor);
    lineY += 36;

    // ── 攻击力：200（普通攻击）──
    DrawTextEx(res.staminaFont, "攻击力（普通攻击）", { col1X, lineY }, 28, 1, WHITE);
    char atkBuf[48];
    snprintf(atkBuf, sizeof(atkBuf), "%d", 200);
    Vector2 atkSz = MeasureTextEx(res.staminaFont, atkBuf, 28, 1);
    DrawTextEx(res.staminaFont, atkBuf, { panelX + panelW - 20 - atkSz.x, lineY }, 28, 1, ORANGE);
    lineY += lh;

    // ── 防御力：200 ──
    DrawTextEx(res.staminaFont, "防御力", { col1X, lineY }, 28, 1, WHITE);
    char defBuf[48];
    snprintf(defBuf, sizeof(defBuf), "%d", 200);
    Vector2 defSz = MeasureTextEx(res.staminaFont, defBuf, 28, 1);
    DrawTextEx(res.staminaFont, defBuf, { panelX + panelW - 20 - defSz.x, lineY }, 28, 1, SKYBLUE);
    lineY += lh;

    // ── 蓝条上限：100%（固定）──
    DrawTextEx(res.staminaFont, "蓝条上限", { col1X, lineY }, 28, 1, WHITE);
    const char* mpCap = "100%（溢出不存）";
    Vector2 mpSz = MeasureTextEx(res.staminaFont, mpCap, 24, 1);
    DrawTextEx(res.staminaFont, mpCap, { panelX + panelW - 20 - mpSz.x, lineY }, 24, 1, SKYBLUE);
    lineY += lh + 5;

    // ── 装备（固定：无）──
    DrawTextEx(res.staminaFont, "--- 装备 ---", { col1X, lineY }, 26, 1, GOLD);
    lineY += 34;
    DrawTextEx(res.staminaFont, "无", { col1X, lineY }, 24, 1, GRAY);
    lineY += 30 + 10;

    // ── 技能展示区（固定为能源学院技能，用 fire_button 图标）──
    DrawTextEx(res.staminaFont, "--- 技能 ---", { col1X, lineY }, 26, 1, GOLD);
    lineY += 40;

    struct SkillData {
        const char* name;
        const char* desc;
        const char* cost;
    };
    SkillData skillData[3] = {
        { "小技能1", "恢复20%蓝条，对敌方单体造成150%伤害", "消耗20%蓝条" },
        { "小技能2", "自身恢复10%生命值，恢复40%蓝条",       "消耗40%蓝条" },
        { "大招",    "本场战斗中我方全体角色群体恢复40%蓝条", "消耗40%蓝条" }
    };

    float skillIconSize = 65;
    for (int si = 1; si <= 3; ++si) {
        float sx = col1X;
        float sy = lineY;

        // 用能源学院 fire_button 图标
        int idx = si; // [1]=小技能1, [2]=小技能2, [3]=大招
        if (res.energyBtnTex[idx].id > 0) {
            DrawTexturePro(res.energyBtnTex[idx],
                { 0, 0, (float)res.energyBtnTex[idx].width, (float)res.energyBtnTex[idx].height },
                { sx, sy, skillIconSize, skillIconSize },
                { 0, 0 }, 0.0f, WHITE);
        } else {
            DrawRectangle((int)sx, (int)sy, (int)skillIconSize, (int)skillIconSize,
                          Color{ 60, 60, 100, 200 });
        }

        const char* skName = skillData[si - 1].name;
        float nameX = sx + skillIconSize + 12;
        DrawTextEx(res.staminaFont, skName, { nameX, sy }, 22, 1, GOLD);

        // 消耗蓝条
        DrawTextEx(res.staminaFont, skillData[si - 1].cost, { nameX, sy + 26 }, 20, 1, SKYBLUE);

        // 技能描述
        const char* descStr = skillData[si - 1].desc;
        float descX = nameX;
        float descY = sy + 52;
        float maxDescW = panelX + panelW - 20 - descX;
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

    // ── "查看升级条件"按钮（与钟关白界面位置一致）----
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
    state.fighterUpgradeInfoBtnRect = { infoBtnX, infoBtnY, infoBtnW, infoBtnH };

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

        int nextAtk = (int)(oldAtk * std::pow(1.2, oldLv));
        int nextDef = (int)(oldDef * std::pow(1.2, oldLv));
        int nextMaxHp = (int)(1000 * std::pow(1.2, oldLv));

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

    // ── "下一个伙伴"按钮（底部右侧，灰色不可用）----
    float bottomBtnY = panelY + panelH - 60;
    float bottomBtnH = 46;
    float nextBtnX = panelX + panelW - 200;
    float nextBtnW = 170;
    Color nextColor = Color{ 60, 60, 60, 160 };
    DrawRectangleRounded({ nextBtnX, bottomBtnY, nextBtnW, bottomBtnH }, 0.2f, 8, nextColor);
    DrawRectangleRoundedLines({ nextBtnX, bottomBtnY, nextBtnW, bottomBtnH }, 0.2f, 8,
                              Color{ 120, 120, 120, 160 });
    Vector2 nextSz = MeasureTextEx(res.staminaFont, "下一个伙伴", 24, 1);
    DrawTextEx(res.staminaFont, "下一个伙伴",
        { nextBtnX + (nextBtnW - nextSz.x) / 2, bottomBtnY + (bottomBtnH - nextSz.y) / 2 },
        24, 1, GRAY);
}

// ============================================================
// 陈尔愿界面 — 输入
// ============================================================
void HandleFighterDetailScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    Rectangle br = GetImageDrawRect(res.chenErYuanTex, curW, curH);
    float panelX = br.x + br.width * 0.55f;
    float panelY = br.y + 30;
    float panelW = br.width * 0.38f;
    float panelH = br.height - 80;

        // ---- 返回按钮（右上角）----
    float backBtnX = panelX + panelW - 110;
    float backBtnY = panelY + 10;
    float backBtnW = 100;
    float backBtnH = 40;
    if (HitTestRect(mousePos, backBtnX, backBtnY, backBtnW, backBtnH)) {
        state.screenState = SCREEN_FRIENDS;
        return;
    }

    // ---- 升级信息弹窗关闭（优先处理）----
    if (state.showFighterUpgradeInfo) {
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
        if (!HitTestRect(mousePos, dlgX, dlgY, dlgW, 450)) {
            state.showFighterUpgradeInfo = false;
            return;
        }
        return;
    }

    // ---- 升级按钮 ----
    float upBtnX = panelX + panelW - 160;
    float upBtnY = panelY + 68;
    float upBtnW = 140;
    float upBtnH = 42;
    if (HitTestRect(mousePos, upBtnX, upBtnY, upBtnW, upBtnH)) {
        std::vector<Combatant*> fighters = game->getFighters();
        Inventory* inventory = game->getInventory();
        if (!fighters.empty() && inventory) {
            Combatant* cur = fighters[state.fighterSelectedIndex];
            int need = cur->getUpgradeStoneCost();
            if (inventory->hasItem("upgrade_stone", need)) {
                inventory->removeItem("upgrade_stone", need);
                cur->levelUp();
            }
        }
        return;
    }

    // ---- 查看升级条件按钮 ----
    float infoBtnX = panelX + 15;
    float infoBtnY = panelY + panelH - 55;
    float infoBtnW = panelW - 30;
    float infoBtnH = 42;
    if (HitTestRect(mousePos, infoBtnX, infoBtnY, infoBtnW, infoBtnH)) {
        state.showFighterUpgradeInfo = !state.showFighterUpgradeInfo;
        return;
    }

    // ---- "下一个伙伴"按钮：点击无效 ----
    // 不处理任何点击事件
}