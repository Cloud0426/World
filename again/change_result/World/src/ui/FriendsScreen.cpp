#include "SplashScreen.h"
#include "FriendsScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "system/backpack/Inventory.h"
#include "role/combatant/Combatant.h"
#include <cstdio>
#include <cmath>
#include <cstring>

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
    // 确保索引不越界
    if (state.fighterSelectedIndex >= (int)fighters.size()) {
        state.fighterSelectedIndex = (int)fighters.size() - 1;
    }
    if (state.fighterSelectedIndex < 0) state.fighterSelectedIndex = 0;

    if (!fighters.empty()) {
        Combatant* cur = fighters[state.fighterSelectedIndex];
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


            // ── 下一个伙伴按钮（查看升级条件上方）----
                        float nextBtnX = panelX + 15;
                        float nextBtnY = panelY + panelH - 105;
                        float nextBtnW = panelW - 30;
                        float nextBtnH = 42;
                        bool nextHover = HitTestRect(mousePos, nextBtnX, nextBtnY, nextBtnW, nextBtnH);
                        Color nextColor = nextHover ? Color{ 60, 180, 220, 240 } : Color{ 40, 140, 180, 200 };
                        DrawRectangleRounded({ nextBtnX, nextBtnY, nextBtnW, nextBtnH }, 0.2f, 8, nextColor);
                        DrawRectangleRoundedLines({ nextBtnX, nextBtnY, nextBtnW, nextBtnH }, 0.2f, 8,
                                                  Color{ 100, 220, 255, 200 });
                        char nextLabel[64];
                        int totalFighters = (int)fighters.size();
                        snprintf(nextLabel, sizeof(nextLabel), "下一个伙伴");
                        Vector2 nextSz = MeasureTextEx(res.staminaFont, nextLabel, 24, 1);
                        DrawTextEx(res.staminaFont, nextLabel,
                            { nextBtnX + (nextBtnW - nextSz.x) / 2, nextBtnY + (nextBtnH - nextSz.y) / 2 },
                            24, 1, WHITE);
                        state.friendNextBtnRect = { nextBtnX, nextBtnY, nextBtnW, nextBtnH };

                        // ── 查看等级提升条件按钮（在下一个按钮下方）----
                        float infoBtnX = panelX + 15;
                        float infoBtnY = nextBtnY + nextBtnH + 8;
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
    std::vector<Combatant*> fighters = game->getFighters();
    Inventory* inventory = game->getInventory();

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

                // ---- 检测"下一个伙伴"按钮：切换到全屏详情页（仅当有陈尔愿时）----
        if (HitTestRect(mousePos, state.friendNextBtnRect.x, state.friendNextBtnRect.y,
                        state.friendNextBtnRect.width, state.friendNextBtnRect.height)) {
            std::vector<Combatant*> fighters = game->getFighters();
            if (!fighters.empty() && state.hasChenErYuan) {
                // 切换到下一个伙伴
                state.fighterSelectedIndex++;
                if (state.fighterSelectedIndex >= (int)fighters.size()) {
                    state.fighterSelectedIndex = 0;  // 循环
                }
                // 切换到全屏详情页（陈尔愿界面）
                state.screenState = SCREEN_FIGHTER_DETAIL;
            }
            return;
        }

    // ---- 检测升级按钮（使用当前选中的伙伴）----
    if (HitTestRect(mousePos, state.fighterUpgradeBtnRect.x, state.fighterUpgradeBtnRect.y,
                    state.fighterUpgradeBtnRect.width, state.fighterUpgradeBtnRect.height)) {
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
