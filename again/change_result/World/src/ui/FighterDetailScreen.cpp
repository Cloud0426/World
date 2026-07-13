#include "UIHelper.h"
#include "UIResource.h"
#include "UIShared.h"
#include "game/GameManager.h"
#include "role/combatant/Combatant.h"

extern void DrawImageCentered(Texture2D tex, int curW, int curH);
extern Rectangle GetImageDrawRect(Texture2D tex, int curW, int curH);
extern bool HitTestRect(Vector2 pos, float x, float y, float w, float h);

// ============================================================
// 伙伴详情全屏界面 — 绘制
// ============================================================
void RenderFighterDetailScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    // ---- 全屏背景：招募角色图片（ChenErYuan.png）铺满全屏 ----
    if (res.chenErYuanTex.id > 0) {
        DrawTexturePro(res.chenErYuanTex,
            { 0, 0, (float)res.chenErYuanTex.width, (float)res.chenErYuanTex.height },
            { 0, 0, (float)curW, (float)curH },
            { 0, 0 }, 0.0f, Color{ 255, 255, 255, 200 });
    } else {
        DrawRectangle(0, 0, curW, curH, Color{ 15, 15, 30, 255 });
    }

    std::vector<Combatant*> fighters = game->getFighters();
    if (fighters.empty()) {
        const char* msg = "没有伙伴";
        Vector2 sz = MeasureTextEx(res.staminaFont, msg, 40, 1);
        DrawTextEx(res.staminaFont, msg,
            { (curW - sz.x) / 2, (curH - sz.y) / 2 }, 40, 1, GRAY);
        return;
    }

    // 确保索引有效
    if (state.fighterSelectedIndex >= (int)fighters.size())
        state.fighterSelectedIndex = (int)fighters.size() - 1;
    if (state.fighterSelectedIndex < 0) state.fighterSelectedIndex = 0;

    Combatant* cur = fighters[state.fighterSelectedIndex];
    if (!cur) return;

    Vector2 mousePos = GetMousePosition();
    Inventory* inventory = game->getInventory();

    int oldAtk = cur->getAttack();
    int oldDef = cur->getDefense();
    int oldHp = cur->getMaxHp();
    int oldLv = cur->getLevel();

    // ---- 右侧伙伴详情面板（占屏幕右侧40%区域）----
    float panelX = curW * 0.58f;
    float panelY = 30;
    float panelW = curW * 0.38f;
    float panelH = curH - 100;

    // 半透明面板覆盖（在右侧显示属性）
    DrawRectangle((int)panelX, (int)panelY, (int)panelW, (int)panelH,
                  Color{ 30, 30, 50, 200 });

    // ---- 左上角返回按钮 ----
    float backBtnX = panelX + 10;
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

    // ---- 伙伴名称（不含索引）----
    Vector2 nameSz = MeasureTextEx(res.staminaFont, cur->getName().c_str(), 40, 1);
    DrawTextEx(res.staminaFont, cur->getName().c_str(),
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

    // ── 攻击力 ──
    DrawTextEx(res.staminaFont, "攻击力", { col1X, lineY }, 28, 1, WHITE);
    char atkBuf[48];
    snprintf(atkBuf, sizeof(atkBuf), "%d", cur->getAttack());
    Vector2 atkSz = MeasureTextEx(res.staminaFont, atkBuf, 28, 1);
    DrawTextEx(res.staminaFont, atkBuf, { panelX + panelW - 20 - atkSz.x, lineY }, 28, 1, ORANGE);
    lineY += lh;

    // ── 防御力 ──
    DrawTextEx(res.staminaFont, "防御力", { col1X, lineY }, 28, 1, WHITE);
    char defBuf[48];
    snprintf(defBuf, sizeof(defBuf), "%d", cur->getDefense());
    Vector2 defSz = MeasureTextEx(res.staminaFont, defBuf, 28, 1);
    DrawTextEx(res.staminaFont, defBuf, { panelX + panelW - 20 - defSz.x, lineY }, 28, 1, SKYBLUE);
    lineY += lh;

    // ── 暴击率 ──
    DrawTextEx(res.staminaFont, "暴击率", { col1X, lineY }, 28, 1, WHITE);
    char critBuf[48];
    snprintf(critBuf, sizeof(critBuf), "%.1f%%", cur->getCritRate() * 100.0);
    Vector2 critSz = MeasureTextEx(res.staminaFont, critBuf, 28, 1);
    DrawTextEx(res.staminaFont, critBuf, { panelX + panelW - 20 - critSz.x, lineY }, 28, 1, YELLOW);
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

    // ── 技能展示区 ──
    DrawTextEx(res.staminaFont, "--- 技能 ---", { col1X, lineY }, 26, 1, GOLD);
    lineY += 40;

    float skillIconSize = 65;
    for (int si = 1; si <= 3; ++si) {
        float sx = col1X;
        float sy = lineY;

        if (res.btnTex[si].id > 0) {
            DrawTexturePro(res.btnTex[si],
                { 0, 0, (float)res.btnTex[si].width, (float)res.btnTex[si].height },
                { sx, sy, skillIconSize, skillIconSize },
                { 0, 0 }, 0.0f, WHITE);
        } else {
            DrawRectangle((int)sx, (int)sy, (int)skillIconSize, (int)skillIconSize,
                          Color{ 60, 60, 100, 200 });
        }

        Skill* sk = cur->getSkill(si);
        const char* skName = sk ? sk->name.c_str() : "未知";
        float nameX = sx + skillIconSize + 12;
        DrawTextEx(res.staminaFont, skName, { nameX, sy }, 22, 1, GOLD);

        int mpCost = sk ? sk->mpCost : 0;
        char costBuf[64];
        snprintf(costBuf, sizeof(costBuf), "消耗能量: %d", mpCost);
        DrawTextEx(res.staminaFont, costBuf, { nameX, sy + 26 }, 20, 1, SKYBLUE);

        const char* descStr = sk ? sk->description.c_str() : "无";
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

    // ── 底部：下一个伙伴按钮 ──
    float bottomBtnY = panelY + panelH - 60;
    float bottomBtnH = 46;
    float nextBtnX = panelX + panelW - 200;
    float nextBtnW = 170;
    bool nextHover = HitTestRect(mousePos, nextBtnX, bottomBtnY, nextBtnW, bottomBtnH);
    Color nextColor = nextHover ? Color{ 60, 180, 220, 240 } : Color{ 40, 140, 180, 200 };
    DrawRectangleRounded({ nextBtnX, bottomBtnY, nextBtnW, bottomBtnH }, 0.2f, 8, nextColor);
    DrawRectangleRoundedLines({ nextBtnX, bottomBtnY, nextBtnW, bottomBtnH }, 0.2f, 8,
                              Color{ 100, 220, 255, 200 });
    Vector2 nextSz = MeasureTextEx(res.staminaFont, "下一个伙伴", 24, 1);
    DrawTextEx(res.staminaFont, "下一个伙伴",
        { nextBtnX + (nextBtnW - nextSz.x) / 2, bottomBtnY + (bottomBtnH - nextSz.y) / 2 },
        24, 1, WHITE);
}

// ============================================================
// 伙伴详情全屏界面 — 输入
// ============================================================
void HandleFighterDetailScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    float panelX = curW * 0.58f;
    float panelY = 30;
    float panelW = curW * 0.38f;
    float panelH = curH - 100;

    // ---- 返回按钮（左上角）----
    float backBtnX = panelX + 10;
    float backBtnY = panelY + 10;
    float backBtnW = 100;
    float backBtnH = 40;
    if (HitTestRect(mousePos, backBtnX, backBtnY, backBtnW, backBtnH)) {
        state.screenState = SCREEN_FRIENDS;
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

    // ---- 下一个伙伴按钮（底部右侧）----
    float bottomBtnY = panelY + panelH - 60;
    float bottomBtnH = 46;
    float nextBtnX = panelX + panelW - 200;
    float nextBtnW = 170;
    if (HitTestRect(mousePos, nextBtnX, bottomBtnY, nextBtnW, bottomBtnH)) {
        std::vector<Combatant*> fighters = game->getFighters();
        if (!fighters.empty()) {
            state.fighterSelectedIndex++;
            if (state.fighterSelectedIndex >= (int)fighters.size()) {
                state.fighterSelectedIndex = 0;
            }
        }
        return;
    }
}