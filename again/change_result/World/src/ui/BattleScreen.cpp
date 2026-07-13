#include "SplashScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <deque>

// ============================================================
// 更新技能精灵动画帧
// ============================================================
void UpdateSkillAnimation(UIState& state, float dt) {
    if (!state.skillAnimActive) return;

    state.skillAnimTimer += dt;
    int totalFrames = UIResource::SKILL_FRAMES;
    float frameDuration = state.skillAnimDuration / (float)totalFrames;
    state.skillAnimFrame = (int)(state.skillAnimTimer / frameDuration);
    if (state.skillAnimFrame >= totalFrames) {
        // 动画结束，执行伤害结算
        state.skillAnimActive = false;
        state.skillAnimFrame = 0;
        state.skillAnimTimer = 0.0f;

        if (state.battleEnemy && state.battleEnemy->isAlive()) {
            state.battleEnemy->takeDamage(state.skillPayload.finalDmg);
            state.battlePlayer->restoreMp(25);
            if (state.battlePlayer->getMp() > state.battlePlayer->getMaxMp())
                state.battlePlayer->setMp(state.battlePlayer->getMaxMp());
            state.battleEnemy->addMp(25);
            if (state.battleEnemy->getMp() > state.battleEnemy->getMaxMp())
                state.battleEnemy->setMp(state.battleEnemy->getMaxMp());
            state.battleLogLines.push_back(std::string(state.skillPayload.logBuf));
            if (!state.battleEnemy->isAlive()) {
                state.battleFinished = true;
                state.battleVictory = true;
                state.battleLogLines.push_back("战斗胜利!");
            } else {
                state.battlePlayerTurn = false;
            }
        }
    }
}

// ============================================================
// 更新敌人（老鼠）攻击精灵动画帧
// ============================================================
void UpdateMouseAnimation(UIState& state, float dt) {
    if (!state.mouseAnimActive) return;

    state.mouseAnimTimer += dt;
    int totalFrames = UIResource::MOUSE_FRAMES;
    float frameDuration = state.mouseAnimDuration / (float)totalFrames;
    state.mouseAnimFrame = (int)(state.mouseAnimTimer / frameDuration);
    if (state.mouseAnimFrame >= totalFrames) {
        state.mouseAnimActive = false;
        state.mouseAnimFrame = 0;
        state.mouseAnimTimer = 0.0f;

        if (state.battlePlayer && state.battlePlayer->isAlive()) {
            state.battlePlayer->takeDamage(state.mousePayload.finalDmg);
            state.battleLogLines.push_back(std::string(state.mousePayload.logBuf));
            if (!state.battlePlayer->isAlive()) {
                state.battleFinished = true;
                state.battleVictory = false;
                state.battleLogLines.push_back("战斗失败...");
            }
            state.battlePlayerTurn = true;
        }
    }
}

// ============================================================
// 更新所有动画
// ============================================================
void UpdateBattleAnimations(UIState& state, float dt) {
    UpdateSkillAnimation(state, dt);
    UpdateMouseAnimation(state, dt);
}

// ============================================================
// 处理敌人回合（每帧检测）
// ============================================================
void HandleEnemyTurn(UIState& state) {
    if (state.screenState != SCREEN_BATTLE) return;
    if (state.battlePlayerTurn) return;
    if (state.battleFinished) return;
    if (state.mouseAnimActive || state.skillAnimActive) return;
    if (!state.battlePlayer || !state.battleEnemy) return;
    if (!state.battleEnemy->isAlive() || !state.battlePlayer->isAlive()) return;

    int enemyAtk = state.battleEnemy->getAttack();
    int playerDef = state.battlePlayer->getDefense();
    int rawDmg = enemyAtk;

    if (state.battleEnemy->isMpFull()) {
        rawDmg = (int)(enemyAtk * 1.5);
        state.battleEnemy->resetMp();
    } else {
        state.battleEnemy->addMp(25);
        if (state.battleEnemy->getMp() > state.battleEnemy->getMaxMp())
            state.battleEnemy->setMp(state.battleEnemy->getMaxMp());
    }

    int finalDmg = (int)(rawDmg * (2000.0 / (playerDef + 2000)));
    if (finalDmg < 1) finalDmg = 1;

    state.mousePayload.finalDmg = finalDmg;
    snprintf(state.mousePayload.logBuf, sizeof(state.mousePayload.logBuf), "%s 造成 %d 伤害 (防御减伤后)",
             state.battleEnemy->getName().c_str(), finalDmg);

    // 触发老鼠攻击动画
    state.mouseAnimActive = true;
    state.mouseAnimFrame = 0;
    state.mouseAnimTimer = 0.0f;
    state.mouseAnimDuration = 1.5f;
    state.battleLogLines.push_back(std::string(state.battleEnemy->getName()) + "攻击!老鼠精灵动画播放中...");
}

// ============================================================
// 绘制战斗界面
// ============================================================
void RenderBattleScreen(UIResource& res, UIState& state, int curW, int curH) {
    ClearBackground(BLACK);

    // ---- 计算玩家和敌人绘制区域 ----
    float playerImgW = (float)res.playerTex.width;
    float playerImgH = (float)res.playerTex.height;
    float playerTargetH = (float)curH * 0.5f;
    float playerScale = playerTargetH / playerImgH;
    float playerDrawW = playerImgW * playerScale;
    float playerDrawH = playerImgH * playerScale;
    float playerPosX = 30.0f;
    float playerPosY = (float)curH - playerDrawH - 20.0f;

    float enemyImgW = (float)res.enemyTex.width;
    float enemyImgH = (float)res.enemyTex.height;
    float enemyTargetH = (float)curH * 0.5f;
    float enemyScale = enemyTargetH / enemyImgH;
    float enemyDrawW = enemyImgW * enemyScale;
    float enemyDrawH = enemyImgH * enemyScale;
    float enemyPosX = (float)curW - enemyDrawW - 30.0f;
    float enemyPosY = (float)curH - enemyDrawH - 20.0f;

    // ---- 绘制玩家角色（左下角），动画激活时用动画帧替换 ----
    if (state.skillAnimActive) {
        int si = state.skillAnimSkillIdx;
        int fi = state.skillAnimFrame;
        if (si >= 0 && si < UIResource::SKILL_COUNT && fi >= 0 && fi < UIResource::SKILL_FRAMES &&
            res.skillAnimFrames[si][fi].id > 0) {
            float fw = (float)res.skillAnimFrames[si][fi].width * state.skillAnimScale;
            float fh = (float)res.skillAnimFrames[si][fi].height * state.skillAnimScale;
            Rectangle src = { 0, 0, (float)res.skillAnimFrames[si][fi].width, (float)res.skillAnimFrames[si][fi].height };
            Rectangle dst = { playerPosX, playerPosY, fw, fh };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(res.skillAnimFrames[si][fi], src, dst, origin, 0.0f, WHITE);
        }
    } else {
        Rectangle srcRect = { 0, 0, playerImgW, playerImgH };
        Rectangle dstRect = { playerPosX, playerPosY, playerDrawW, playerDrawH };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(res.playerTex, srcRect, dstRect, origin, 0, WHITE);
    }

    // ---- 绘制敌人角色（右下角），攻击动画激活时用动画帧替换 ----
    if (state.mouseAnimActive) {
        int fi = state.mouseAnimFrame;
        if (fi >= 0 && fi < UIResource::MOUSE_FRAMES && res.mouseFrames[fi].id > 0) {
            float baseW = enemyDrawW;
            float baseH = (float)res.mouseFrames[fi].height * (enemyDrawW / (float)res.mouseFrames[fi].width);
            float fw = baseW * 1.5f;
            float fh = baseH * 1.5f;
            float offsetX = -20.0f;
            float offsetY = 20.0f;
            Rectangle src = { 0, 0, (float)res.mouseFrames[fi].width, (float)res.mouseFrames[fi].height };
            Rectangle dst = { enemyPosX + enemyDrawW - fw + offsetX, enemyPosY + enemyDrawH - fh + offsetY, fw, fh };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(res.mouseFrames[fi], src, dst, origin, 0.0f, WHITE);
        }
    } else {
        Rectangle srcRect = { 0, 0, enemyImgW, enemyImgH };
        Rectangle dstRect = { enemyPosX, enemyPosY, enemyDrawW, enemyDrawH };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(res.enemyTex, srcRect, dstRect, origin, 0, WHITE);
    }

    // ---- 玩家HP/MP条（设计坐标：横100-300，竖400-500） ----
    {
        Vector2 p1 = DesignToScreen(100.0f, 400.0f, res.battleBgTex, curW, curH);
        Vector2 p2 = DesignToScreen(300.0f, 500.0f, res.battleBgTex, curW, curH);

        if (state.battlePlayer) {
            float uiX = p1.x;
            float uiY = p1.y;
            float boxW = p2.x - p1.x;
            float barW = boxW * 0.9f;
            float barH = (p2.y - p1.y) * 0.35f;

            DrawTextEx(res.staminaFont, state.battlePlayer->getName().c_str(), { uiX + 4, uiY }, 24, 1, WHITE);
            float curY = uiY + 28;

            float hpRatio = (float)state.battlePlayer->getHp() / (float)state.battlePlayer->getMaxHp();
            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * hpRatio), (int)barH, Color{ 210, 60, 60, 240 });
            char hpText[64];
            snprintf(hpText, sizeof(hpText), "HP: %d/%d", state.battlePlayer->getHp(), state.battlePlayer->getMaxHp());
            DrawTextEx(res.staminaFont, hpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
            curY += barH + 4;

            float mpRatio = (float)state.battlePlayer->getMp() / (float)state.battlePlayer->getMaxMp();
            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * mpRatio), (int)barH, Color{ 60, 60, 230, 240 });
            char mpText[64];
            snprintf(mpText, sizeof(mpText), "MP: %d/%d", state.battlePlayer->getMp(), state.battlePlayer->getMaxMp());
            DrawTextEx(res.staminaFont, mpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
        }
    }

    // ---- 四个技能按钮 1行4列：普攻、小技能一、小技能二、大招 ----
    {
        Vector2 sp1 = DesignToScreen(300.0f, 800.0f, res.battleBgTex, curW, curH);
        Vector2 sp2 = DesignToScreen(700.0f, 900.0f, res.battleBgTex, curW, curH);

        if (!state.battleFinished && state.battlePlayer && state.battlePlayer->isAlive()) {
            float boxX = sp1.x;
            float boxY = sp1.y;
            float boxW = sp2.x - sp1.x;
            float boxH = sp2.y - sp1.y;

            static const char* skillNames[] = { "普攻", "小技能一", "小技能二", "大招" };
            static const char* skillDescs[] = {
                "100%单体攻击，不消耗MP",
                "[爆发打击] 单体200%伤害，消耗50MP",
                "[精准打击] 暴击率+25%，全体120%伤害，消耗50MP",
                "[火力压制] 全体150%伤害，消耗100MP"
            };

            float btnW = boxW / 4.0f;
            float btnH = boxH;
            Vector2 mousePos = GetMousePosition();
            int hoveredIdx = -1;

            for (int i = 0; i < 4; i++) {
                float bx = boxX + i * btnW;
                float by = boxY;

                if (res.btnTex[i].id > 0) {
                    float imgW = (float)res.btnTex[i].width;
                    float imgH = (float)res.btnTex[i].height;
                    float scale = (btnW / imgW < btnH / imgH) ? (btnW / imgW) : (btnH / imgH);
                    float drawW = imgW * scale;
                    float drawH = imgH * scale;
                    float drawX = bx + (btnW - drawW) / 2.0f;
                    float drawY = by + (btnH - drawH) / 2.0f;

                    Rectangle src = { 0, 0, imgW, imgH };
                    Rectangle dst = { drawX, drawY, drawW, drawH };
                    Vector2 origin = { 0, 0 };
                    bool hover = HitTestRect(mousePos, bx, by, btnW, btnH);
                    if (hover) hoveredIdx = i;
                    Color tint = hover ? WHITE : (Color){ 200, 200, 200, 255 };

                    Skill* sk = nullptr;
                    if (i == 0) sk = state.battlePlayer->getNormalAttack();
                    else if (i == 1) sk = state.battlePlayer->getSkill1();
                    else if (i == 2) sk = state.battlePlayer->getSkill2();
                    else if (i == 3) sk = state.battlePlayer->getUltimate();
                    bool canUse = true;
                    if (sk && sk->mpCost > state.battlePlayer->getMp()) canUse = false;
                    if (!canUse) tint = (Color){ 80, 80, 80, 200 };

                    DrawTexturePro(res.btnTex[i], src, dst, origin, 0.0f, tint);
                }
            }

            // ---- 鼠标悬停显示技能详情 ----
            if (hoveredIdx >= 0) {
                float hx = boxX + hoveredIdx * btnW;
                float hy = boxY;
                float tipW = 380.0f;
                float tipH = 100.0f;
                float tipX = hx + btnW / 2.0f - tipW / 2.0f;
                float tipY = hy - tipH - 10.0f;
                if (tipX < 10) tipX = 10;
                if (tipX + tipW > curW - 10) tipX = curW - 10 - tipW;
                if (tipY < 10) tipY = hy + btnH + 10;

                DrawRectangle((int)tipX, (int)tipY, (int)tipW, (int)tipH, { 20, 20, 50, 230 });
                DrawRectangleLines((int)tipX, (int)tipY, (int)tipW, (int)tipH, { 220, 220, 255, 200 });

                DrawTextEx(res.staminaFont, skillNames[hoveredIdx], { tipX + 12, tipY + 8 }, 22, 1, GOLD);
                DrawTextEx(res.staminaFont, skillDescs[hoveredIdx], { tipX + 12, tipY + 38 }, 18, 1, WHITE);

                int mpCost = (hoveredIdx == 0) ? 0 : ((hoveredIdx == 3) ? 100 : 50);
                char mpBuf[64];
                snprintf(mpBuf, sizeof(mpBuf), "MP消耗: %d  当前MP: %d/%d",
                         mpCost, state.battlePlayer->getMp(), state.battlePlayer->getMaxMp());
                Color mpColor = (state.battlePlayer->getMp() >= mpCost) ? SKYBLUE : RED;
                DrawTextEx(res.staminaFont, mpBuf, { tipX + 12, tipY + 66 }, 16, 1, mpColor);
            }
        }
    }

    // ---- 敌人HP/MP条（设计坐标：横1000-1200，竖400-500） ----
    {
        Vector2 ep1 = DesignToScreen(1000.0f, 400.0f, res.battleBgTex, curW, curH);
        Vector2 ep2 = DesignToScreen(1200.0f, 500.0f, res.battleBgTex, curW, curH);

        if (state.battleEnemy) {
            float uiX = ep1.x;
            float uiY = ep1.y;
            float boxW = ep2.x - ep1.x;
            float barW = boxW * 0.9f;
            float barH = (ep2.y - ep1.y) * 0.35f;

            DrawTextEx(res.staminaFont, state.battleEnemy->getName().c_str(), { uiX + 4, uiY }, 24, 1, RED);
            float curY = uiY + 28;

            float hpRatio = (float)state.battleEnemy->getHp() / (float)state.battleEnemy->getMaxHp();
            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * hpRatio), (int)barH, Color{ 210, 60, 60, 240 });
            char hpText[64];
            snprintf(hpText, sizeof(hpText), "HP: %d/%d", state.battleEnemy->getHp(), state.battleEnemy->getMaxHp());
            DrawTextEx(res.staminaFont, hpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
            curY += barH + 4;

            float mpRatio = (float)state.battleEnemy->getMp() / (float)state.battleEnemy->getMaxMp();
            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * mpRatio), (int)barH, Color{ 60, 60, 230, 240 });
            char mpText[64];
            snprintf(mpText, sizeof(mpText), "MP: %d/%d", state.battleEnemy->getMp(), state.battleEnemy->getMaxMp());
            DrawTextEx(res.staminaFont, mpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
        }
    }

    // ---- 右上角返回按钮 ----
    {
        float btnW = 140.0f;
        float btnH = 50.0f;
        float btnX = (float)curW - btnW - 20.0f;
        float btnY = 20.0f;
        state.battleBackBtnRect = { btnX, btnY, btnW, btnH };
        Vector2 mousePos = GetMousePosition();
        bool hover = HitTestRect(mousePos, btnX, btnY, btnW, btnH);
        Color btnColor = hover ? Color{ 200, 60, 60, 240 } : Color{ 180, 50, 50, 200 };
        DrawRectangleRounded({ btnX, btnY, btnW, btnH }, 0.3f, 8, btnColor);
        DrawRectangleRoundedLines({ btnX, btnY, btnW, btnH }, 0.3f, 8, Color{ 255, 200, 200, 200 });
        const char* label = "返回 B区";
        Vector2 txtSz = MeasureTextEx(res.staminaFont, label, 24, 1);
        DrawTextEx(res.staminaFont, label, { btnX + (btnW - txtSz.x) / 2, btnY + (btnH - txtSz.y) / 2 }, 24, 1, WHITE);
    }

    // ---- 战斗日志（右上方，可滚动） ----
    {
        float logFontSize = 22.0f;
        float lineSpacing = 30.0f;
        int visibleLines = 5;
        int totalLines = (int)state.battleLogLines.size();

        if (state.battleLogScrollOffset > totalLines - visibleLines) state.battleLogScrollOffset = totalLines - visibleLines;
        if (state.battleLogScrollOffset < 0) state.battleLogScrollOffset = 0;
        if (totalLines == 0) state.battleLogScrollOffset = 0;

        float maxW = 0;
        for (auto& ln : state.battleLogLines) {
            Vector2 sz = MeasureTextEx(res.staminaFont, ln.c_str(), logFontSize, 1);
            if (sz.x > maxW) maxW = sz.x;
        }
        if (maxW < 300) maxW = 300;
        maxW += 40.0f;

        float panelH = (float)visibleLines * lineSpacing + 20.0f;
        float lx = (float)curW - maxW - 20.0f;
        float ly = 80.0f;

        // 鼠标滚轮滚动
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            state.battleLogScrollOffset -= (int)wheel;
            if (state.battleLogScrollOffset > totalLines - visibleLines) state.battleLogScrollOffset = totalLines - visibleLines;
            if (state.battleLogScrollOffset < 0) state.battleLogScrollOffset = 0;
        }

        DrawRectangle((int)lx, (int)ly, (int)maxW, (int)panelH, { 0, 0, 0, 200 });
        DrawRectangleLines((int)lx, (int)ly, (int)maxW, (int)panelH, { 200, 200, 200, 100 });

        DrawTextEx(res.staminaFont, "-- 战斗日志 --", { lx + 8, ly + 2 }, 20, 1, GOLD);

        int startIdx = state.battleLogScrollOffset;
        int endIdx = startIdx + visibleLines;
        if (endIdx > totalLines) endIdx = totalLines;
        float textY = ly + 26;
        for (int i = startIdx; i < endIdx; i++) {
            DrawTextEx(res.staminaFont, state.battleLogLines[i].c_str(), { lx + 10, textY }, logFontSize, 1, YELLOW);
            textY += lineSpacing;
        }

        if (totalLines > visibleLines) {
            char scrollHint[64];
            snprintf(scrollHint, sizeof(scrollHint), "滚轮滚动 (%d/%d)", endIdx, totalLines);
            DrawTextEx(res.staminaFont, scrollHint, { lx + 8, ly + panelH - 20 }, 14, 1, LIGHTGRAY);
        }
    }

    // ---- 战斗结果 ----
    if (state.battleFinished) {
        const char* resultText = state.battleVictory ? "战斗胜利!" : "战斗失败...";
        Color resultColor = state.battleVictory ? Color{ 255, 215, 0, 255 } : Color{ 255, 80, 80, 255 };
        Vector2 sz = MeasureTextEx(res.staminaFont, resultText, 64, 1);
        float rx = (curW - sz.x) / 2.0f;
        float ry = (float)curH * 0.3f;
        DrawRectangle((int)rx - 24, (int)ry - 12, (int)sz.x + 48, (int)sz.y + 24, { 0, 0, 0, 210 });
        DrawRectangleLines((int)rx - 24, (int)ry - 12, (int)sz.x + 48, (int)sz.y + 24, { 255, 255, 200, 150 });
        DrawTextEx(res.staminaFont, resultText, { rx, ry }, 64, 1, resultColor);
        const char* tip = "点击返回按钮或按ESC退出";
        Vector2 tipSz = MeasureTextEx(res.staminaFont, tip, 24, 1);
        DrawTextEx(res.staminaFont, tip, { (curW - tipSz.x) / 2, ry + sz.y + 24 }, 24, 1, LIGHTGRAY);
    }
}

// ============================================================
// 处理战斗界面输入（技能按钮点击）
// ============================================================
void HandleBattleScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    // 检测返回按钮
    if (state.battleBackBtnRect.width > 0 && state.battleBackBtnRect.height > 0 &&
        HitTestRect(mousePos, state.battleBackBtnRect.x, state.battleBackBtnRect.y, state.battleBackBtnRect.width, state.battleBackBtnRect.height)) {
        state.screenState = SCREEN_MAP_SUB2;
        state.showPeaceInB = false;
        state.battleFinished = false;
        return;
    }

    // 检测四个技能按钮
    if (!state.battleFinished && state.battlePlayer && state.battleEnemy &&
        state.battleEnemy->isAlive() && state.battlePlayer->isAlive()) {

        float btnDWidth = 100.0f;
        float btnDHeight = 100.0f;
        float baseDX = 300.0f;
        float baseDY = 800.0f;

        for (int i = 0; i < 4; i++) {
            float dx = baseDX + i * btnDWidth;
            float dy = baseDY;
            Vector2 p1 = DesignToScreen(dx, dy, res.battleBgTex, curW, curH);
            Vector2 p2 = DesignToScreen(dx + btnDWidth, dy + btnDHeight, res.battleBgTex, curW, curH);
            float bx = p1.x;
            float by = p1.y;
            float bw = p2.x - p1.x;
            float bh = p2.y - p1.y;

            if (HitTestRect(mousePos, bx, by, bw, bh)) {
                // 动画播放中禁止点击技能
                if (state.skillAnimActive || state.mouseAnimActive) break;

                int requiredMp = (i == 0) ? 0 : ((i == 3) ? 100 : 50);
                if (state.battlePlayer->getMp() < requiredMp) {
                    state.battleLogLines.push_back("蓝量不足!需要 " + std::to_string(requiredMp) + " MP");
                    break;
                }

                state.battlePlayer->consumeMp(requiredMp);

                int baseAtk = state.battlePlayer->getAttack();
                double critRate = state.battlePlayer->getCritRate();
                int skillMult = 100;
                const char* skillName = "普攻";
                double effCrit = critRate;

                if (i == 1) { skillMult = 200; skillName = "爆发打击"; }
                else if (i == 2) { skillMult = 120; skillName = "精准打击"; effCrit = critRate + 0.25; }
                else if (i == 3) { skillMult = 150; skillName = "火力压制"; }

                bool isCrit = ((double)std::rand() / RAND_MAX) < effCrit;
                int skillBase = isCrit ? (int)(baseAtk * 1.5) : baseAtk;
                int rawDmg = skillBase * skillMult / 100;
                int tmpFinal = (int)(rawDmg * (2000.0 / (state.battleEnemy->getDefense() + 2000)));
                if (tmpFinal < 1) tmpFinal = 1;

                state.skillPayload.isCrit = isCrit;
                state.skillPayload.rawDmg = rawDmg;
                state.skillPayload.finalDmg = tmpFinal;
                snprintf(state.skillPayload.logBuf, sizeof(state.skillPayload.logBuf), "%s%s造成 %d 伤害 (最终 %d)",
                         skillName, isCrit ? "暴击!" : "", rawDmg, tmpFinal);

                {
                    Vector2 pp = DesignToScreen(0.0f, 500.0f, res.battleBgTex, curW, curH);
                    state.skillAnimPos = { pp.x, pp.y };
                }
                float targetH = (float)curH * 0.5f;
                state.skillAnimScale = targetH / (float)(res.skillAnimFrames[i][0].height > 0 ? res.skillAnimFrames[i][0].height : 1);
                state.skillAnimActive = true;
                state.skillAnimSkillIdx = i;
                state.skillAnimFrame = 0;
                state.skillAnimTimer = 0.0f;
                state.skillAnimDuration = (i == 0) ? 1.0f : 1.5f;
                state.battleLogLines.push_back(std::string(skillName) + "!精灵动画播放中...");

                break;
            }
        }
    }
}
