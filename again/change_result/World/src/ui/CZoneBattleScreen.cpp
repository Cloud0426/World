#include "CZoneBattleScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "role/enemy/EnemyManager.h"
#include <cstdio>
#include <string>
#include <deque>
#include <cmath>

// ============================================================
// 获取当前上场玩家
// ============================================================
static Combatant* GetActivePlayer(UIState& state) {
    if (state.cZone.currentPlayerIdx < 0 || state.cZone.currentPlayerIdx >= 2)
        return nullptr;
    return state.cZone.playerSlots[state.cZone.currentPlayerIdx].fighter;
}

// ============================================================
// 检查战斗是否结束
// ============================================================
static void CheckBattleEnd(UIState& state) {
    if (state.cZone.finished) return;
    bool allEnemiesDead = true;
    for (int i = 0; i < 3; i++) {
        if (state.cZone.enemies[i].enemy && state.cZone.enemies[i].enemy->isAlive()) { allEnemiesDead = false; break; }
    }
    if (allEnemiesDead) { state.cZone.finished = true; state.cZone.victory = true; state.cZone.logLines.push_back("战斗胜利!"); return; }
    bool allPlayersDead = true;
    for (int i = 0; i < 2; i++) {
        if (state.cZone.playerSlots[i].fighter && state.cZone.playerSlots[i].fighter->isAlive()) allPlayersDead = false;
    }
    if (allPlayersDead) { state.cZone.finished = true; state.cZone.victory = false; state.cZone.logLines.push_back("战斗失败..."); }
}

// ============================================================
// 初始化C区战斗
// ============================================================
void InitCZoneBattle(UIState& state, GameManager* game) {
    auto& cz = state.cZone;
    cz.active = true;
    cz.playerTurn = true;
    cz.finished = false;
    cz.victory = false;
    cz.currentPlayerIdx = -1;
    cz.skillAnimActive = false;
    cz.selectingTarget = false;
    cz.enemyActedThisTurn = false;
    cz.logLines.clear();
    cz.logScrollOffset = 0;
    cz.logLines.push_back("C区战斗开始！请选择上场角色");

    for (int i = 0; i < 3; i++) {
        cz.enemies[i].energy = 0;
        cz.enemies[i].animActive = false;
        cz.enemies[i].animFrame = 0;
        cz.enemies[i].animTimer = 0.0f;
    }
    cz.playerSlots[0].fighter = nullptr; cz.playerSlots[0].inBattle = false;
    cz.playerSlots[1].fighter = nullptr; cz.playerSlots[1].inBattle = false;

    auto fighters = game->getFighters();
    for (auto* f : fighters) {
        if (f && f->getName() == "钟关白")
            cz.playerSlots[0].fighter = f;
        if (f && f->getName() == "陈尔愿" && state.hasChenErYuan)
            cz.playerSlots[1].fighter = f;
    }
    if (!cz.playerSlots[0].fighter) {
        static Combatant zhong("钟关白","机械学院");
        zhong.setHp(900); zhong.setMaxHp(900); zhong.setAttack(300);
        zhong.setDefense(200); zhong.setMaxMp(100); zhong.setMp(100);
        zhong.setCritRate(0.10); cz.playerSlots[0].fighter = &zhong;
    }
    if (state.hasChenErYuan && !cz.playerSlots[1].fighter) {
        static Combatant chen("陈尔愿","能源学院");
        chen.setHp(800); chen.setMaxHp(800); chen.setAttack(250);
        chen.setDefense(150); chen.setMaxMp(80); chen.setMp(80);
        chen.setCritRate(0.15); cz.playerSlots[1].fighter = &chen;
    }

    EnemyManager* em = game->getEnemyManager();
    if (em) {
        em->initFirstStage();
        auto& enemies = em->getCurrentEnemies();
        for (int i = 0; i < 3; i++) {
            if (i < (int)enemies.size())
                cz.enemies[i].enemy = &enemies[i];
        }
    }
    for (int i = 0; i < 3; i++) {
        if (!cz.enemies[i].enemy) {
            static Enemy def[3] = {
                Enemy("c_rat_1","C区老鼠1","","mechanical",600,80,60,10,5,0),
                Enemy("c_rat_2","C区老鼠2","","mechanical",600,80,60,10,5,0),
                Enemy("c_rat_3","C区老鼠3","","mechanical",600,80,60,10,5,0)
            };
            cz.enemies[i].enemy = &def[i];
        }
    }

    // 自动选中第一个可用角色
    if (cz.playerSlots[0].fighter && cz.playerSlots[0].fighter->isAlive()) {
        cz.playerSlots[0].inBattle = true;
        cz.currentPlayerIdx = 0;
    } else if (cz.playerSlots[1].fighter && cz.playerSlots[1].fighter->isAlive()) {
        cz.playerSlots[1].inBattle = true;
        cz.currentPlayerIdx = 1;
    }
}

// ============================================================
// 技能动画更新（单体/群体攻击伤害结算）
// ============================================================
static void UpdateCZoneSkillAnim(UIState& state, float dt) {
    if (!state.cZone.skillAnimActive) return;
    state.cZone.skillAnimTimer += dt;
    int totalFrames = UIResource::SKILL_FRAMES;
    float fd = state.cZone.skillAnimDuration / (float)totalFrames;
    state.cZone.skillAnimFrame = (int)(state.cZone.skillAnimTimer / fd);
    if (state.cZone.skillAnimFrame >= totalFrames) {
        state.cZone.skillAnimActive = false;
        state.cZone.skillAnimFrame = 0;
        state.cZone.skillAnimTimer = 0.0f;
        Combatant* p = GetActivePlayer(state);
        if (!p) return;

        if (state.cZone.skillPayload.isAoe) {
            // 群体攻击：对所有存活的敌人造成伤害
            for (int i = 0; i < 3; i++) {
                auto& eu = state.cZone.enemies[i];
                if (!eu.enemy || !eu.enemy->isAlive()) continue;
                int def = eu.enemy->getDefense();
                int d = (int)(state.cZone.skillPayload.rawDmg * (2000.0 / (def + 2000)));
                if (d < 1) d = 1;
                eu.enemy->takeDamage(d);
                eu.energy += 25; if (eu.energy > 100) eu.energy = 100;
                char buf[128]; snprintf(buf, sizeof(buf), "群体命中 %s, %d伤害", eu.enemy->getName().c_str(), d);
                state.cZone.logLines.push_back(std::string(buf));
                if (!eu.enemy->isAlive()) {
                    char db[64]; snprintf(db, sizeof(db), "%s 被击杀!", eu.enemy->getName().c_str());
                    state.cZone.logLines.push_back(std::string(db));
                }
            }
        } else {
            // 单体攻击：只对选中的目标敌人造成伤害
            int targetIdx = state.cZone.skillPayload.targetIdx;
            if (targetIdx >= 0 && targetIdx < 3) {
                auto& eu = state.cZone.enemies[targetIdx];
                if (eu.enemy && eu.enemy->isAlive()) {
                    int def = eu.enemy->getDefense();
                    int d = (int)(state.cZone.skillPayload.rawDmg * (2000.0 / (def + 2000)));
                    if (d < 1) d = 1;
                    eu.enemy->takeDamage(d);
                    eu.energy += 25; if (eu.energy > 100) eu.energy = 100;
                    char buf[128]; snprintf(buf, sizeof(buf), "技能命中 %s, %d伤害", eu.enemy->getName().c_str(), d);
                    state.cZone.logLines.push_back(std::string(buf));
                    if (!eu.enemy->isAlive()) {
                        char db[64]; snprintf(db, sizeof(db), "%s 被击杀!", eu.enemy->getName().c_str());
                        state.cZone.logLines.push_back(std::string(db));
                    }
                }
            }
        }
        p->restoreMp(25);
        if (p->getMp() > p->getMaxMp()) p->setMp(p->getMaxMp());
        CheckBattleEnd(state);
        if (!state.cZone.finished) state.cZone.playerTurn = false;
    }
}

// ============================================================
// 敌人攻击动画更新
// ============================================================
static void UpdateCZoneEnemyAnim(UIState& state, float dt) {
    for (int i = 0; i < 3; i++) {
        auto& eu = state.cZone.enemies[i];
        if (!eu.animActive) continue;
        eu.animTimer += dt;
        // 移除对鼠标动画帧（mouseFrames / MOUSE_FRAMES）的依赖
        // 改为使用固定动画持续时间逻辑
        const int ENEMY_ANIM_TOTAL_FRAMES = 10;
        float fd = eu.animDuration / (float)ENEMY_ANIM_TOTAL_FRAMES;
        if (fd <= 0.0f) fd = 0.1f;
        eu.animFrame = (int)(eu.animTimer / fd);
        if (eu.animFrame >= ENEMY_ANIM_TOTAL_FRAMES) {
            eu.animActive = false; eu.animFrame = 0; eu.animTimer = 0;
            Combatant* p = GetActivePlayer(state);
            if (!p || !p->isAlive()) {
                for (int j = 0; j < 2; j++) {
                    if (state.cZone.playerSlots[j].fighter && state.cZone.playerSlots[j].fighter->isAlive()) {
                        state.cZone.currentPlayerIdx = j;
                        state.cZone.playerSlots[j].inBattle = true;
                        p = state.cZone.playerSlots[j].fighter;
                        char buf[64]; snprintf(buf, sizeof(buf), "自动切换为 %s", p->getName().c_str());
                        state.cZone.logLines.push_back(std::string(buf));
                        break;
                    }
                }
                if (!p) { CheckBattleEnd(state); continue; }
            }
            if (eu.enemy && eu.enemy->isAlive()) {
                p->takeDamage(eu.animFinalDmg);
                char buf[256]; snprintf(buf, sizeof(buf), "%s %d伤害", eu.enemy->getName().c_str(), eu.animFinalDmg);
                state.cZone.logLines.push_back(std::string(buf));
                if (!p->isAlive()) {
                    char db[64]; snprintf(db, sizeof(db), "%s 被击倒!", p->getName().c_str());
                    state.cZone.logLines.push_back(std::string(db));
                    state.cZone.playerSlots[state.cZone.currentPlayerIdx].inBattle = false;
                    state.cZone.currentPlayerIdx = -1;
                    CheckBattleEnd(state);
                    if (state.cZone.finished) continue;
                    for (int j = 0; j < 2; j++) {
                        if (state.cZone.playerSlots[j].fighter && state.cZone.playerSlots[j].fighter->isAlive()) {
                            state.cZone.currentPlayerIdx = j;
                            state.cZone.playerSlots[j].inBattle = true;
                            p = state.cZone.playerSlots[j].fighter;
                            char buf2[64]; snprintf(buf2, sizeof(buf2), "切换为 %s", p->getName().c_str());
                            state.cZone.logLines.push_back(std::string(buf2));
                            break;
                        }
                    }
                }
                CheckBattleEnd(state);
            }
            // 敌人攻击动画结束后：切换为玩家回合，重置敌人回合标记
            if (!state.cZone.finished) {
                state.cZone.playerTurn = true;
                state.cZone.enemyActedThisTurn = false;
            }
        }
    }
}

// ============================================================
// 主动画更新
// ============================================================
void UpdateCZoneBattleAnimations(UIState& state, float dt) {
    UpdateCZoneSkillAnim(state, dt);
    UpdateCZoneEnemyAnim(state, dt);
}

// ============================================================
// 敌人回合（每回合只执行一次能量累积/攻击）
// ============================================================
void HandleCZoneEnemyTurn(UIState& state) {
    if (!state.cZone.active || state.cZone.playerTurn || state.cZone.finished || state.cZone.currentPlayerIdx < 0)
        return;
    // 有动画正在播放时，等待动画结束
    bool anyAnim = state.cZone.skillAnimActive;
    for (int i = 0; i < 3; i++) if (state.cZone.enemies[i].animActive) anyAnim = true;
    if (anyAnim) return;
    // 本回合已经执行过敌人行动，不再重复执行
    if (state.cZone.enemyActedThisTurn) return;
    Combatant* p = GetActivePlayer(state);
    if (!p || !p->isAlive()) return;
    // 标记本回合已执行，保证只执行一次
    state.cZone.enemyActedThisTurn = true;
    for (int i = 0; i < 3; i++) {
        auto& eu = state.cZone.enemies[i];
        if (!eu.enemy || !eu.enemy->isAlive()) continue;
        if (eu.energy >= 100) {
            int atk = eu.enemy->getAttack();
            int pdef = p->getDefense();
            int rawDmg = (int)(atk * 1.5);
            int finalDmg = (int)(rawDmg * (2000.0 / (pdef + 2000)));
            if (finalDmg < 1) finalDmg = 1;
            eu.animFinalDmg = finalDmg;
            eu.energy = 0;
            eu.animActive = true; eu.animFrame = 0; eu.animTimer = 0; eu.animDuration = 1.5f;
            char buf[128]; snprintf(buf, sizeof(buf), "%s 发动攻击!", eu.enemy->getName().c_str());
            state.cZone.logLines.push_back(std::string(buf));
            return;
        }
    }
    // 没有能量满的敌人，给第一个存活的敌人加25能量，然后切换回玩家回合
    for (int i = 0; i < 3; i++) {
        auto& eu = state.cZone.enemies[i];
        if (eu.enemy && eu.enemy->isAlive()) {
            eu.energy += 25; if (eu.energy > 100) eu.energy = 100;
            break;
        }
    }
    // 没有敌人攻击，直接切回玩家回合
    state.cZone.playerTurn = true;
    state.cZone.enemyActedThisTurn = false;
}

// ============================================================
// 渲染C区战斗
// ============================================================
void RenderCZoneBattle(UIResource& res, UIState& state, int curW, int curH) {
    ClearBackground(BLACK);
    float sf = ((float)curW / DESIGN_W < (float)curH / DESIGN_H) ? (float)curW / DESIGN_W : (float)curH / DESIGN_H;
    float leftW = 200 * sf;

    // 左侧角色选择面板
    DrawRectangle(0, 0, (int)leftW, curH, Color{ 20,20,40,230 });
    DrawTextEx(res.staminaFont, "上场角色", { 10,10 }, 22, 1, GOLD);

    Vector2 mousePos = GetMousePosition();
    for (int i = 0; i < 2; i++) {
        float sy = 50 + i * 90;
        if (!state.cZone.playerSlots[i].fighter) {
            DrawRectangle(8, (int)sy, (int)(leftW - 16), 80, Color{ 60,60,60,200 });
            DrawTextEx(res.staminaFont, i == 0 ? "钟关白" : "陈尔愿", { 14,sy + 4 }, 18, 1, DARKGRAY);
            DrawTextEx(res.staminaFont, "未招募", { 14,sy + 30 }, 14, 1, DARKGRAY);
            continue;
        }
        bool isActive = (state.cZone.currentPlayerIdx == i);
        bool hover = HitTestRect(mousePos, 8.0f, sy, leftW - 16, 80);
        Color bg = isActive ? Color{ 60,120,60,240 } : (hover ? Color{ 50,50,80,240 } : Color{ 40,40,60,220 });
        DrawRectangle(8, (int)sy, (int)(leftW - 16), 80, bg);
        if (isActive) DrawRectangleLines(8, (int)sy, (int)(leftW - 16), 80, GREEN);
        Combatant* f = state.cZone.playerSlots[i].fighter;
        DrawTextEx(res.staminaFont, i == 0 ? "钟关白" : "陈尔愿", { 14,sy + 2 }, 20, 1, WHITE);
        char hbuf[32]; snprintf(hbuf, sizeof(hbuf), "HP:%d/%d", f->getHp(), f->getMaxHp());
        DrawTextEx(res.staminaFont, hbuf, { 14,sy + 28 }, 16, 1, RED);
        float hpBW = leftW - 28;
        float hpR = (float)f->getHp() / (float)f->getMaxHp();
        DrawRectangle(14, (int)(sy + 52), (int)hpBW, 8, Color{ 40,40,40,220 });
        DrawRectangle(14, (int)(sy + 52), (int)(hpBW * hpR), 8, Color{ 210,60,60,240 });
        if (!f->isAlive()) DrawTextEx(res.staminaFont, "战斗不能", { 14,sy + 64 }, 14, 1, RED);
    }

    Combatant* player = GetActivePlayer(state);
    float pImgW = (float)res.playerTex.width, pImgH = (float)res.playerTex.height;
    float pTargetH = curH * 0.45f, pScale = pTargetH / pImgH;
    float pDW = pImgW * pScale, pDH = pImgH * pScale;
    float pX = leftW + 40, pY = curH - pDH - 100;

    // 血量/蓝量条放在角色上方（在角色绘制之前绘制）
    if (player) {
        float bx = pX, by = pY - 48, bw = pDW, bh = 18;
        float hr = (float)player->getHp() / (float)player->getMaxHp();
        DrawRectangle((int)bx, (int)by, (int)bw, (int)bh, Color{ 40,40,40,220 });
        DrawRectangle((int)bx, (int)by, (int)(bw * hr), (int)bh, Color{ 210,60,60,240 });
        char ht[64]; snprintf(ht, sizeof(ht), "HP:%d/%d", player->getHp(), player->getMaxHp());
        Vector2 hsz = MeasureTextEx(res.staminaFont, ht, 18, 1);
        DrawTextEx(res.staminaFont, ht, { bx + (bw - hsz.x) / 2,by + 1 }, 18, 1, WHITE);
        float my = by + bh + 4, mr = (float)player->getMp() / (float)player->getMaxMp();
        DrawRectangle((int)bx, (int)my, (int)bw, (int)bh, Color{ 40,40,40,220 });
        DrawRectangle((int)bx, (int)my, (int)(bw * mr), (int)bh, Color{ 60,60,230,240 });
        char mt[64]; snprintf(mt, sizeof(mt), "MP:%d/%d", player->getMp(), player->getMaxMp());
        Vector2 msz = MeasureTextEx(res.staminaFont, mt, 18, 1);
        DrawTextEx(res.staminaFont, mt, { bx + (bw - msz.x) / 2,my + 1 }, 18, 1, WHITE);
    }

    // 渲染角色和技能动画
    if (state.cZone.skillAnimActive && player) {
        // 技能动画期间：绘制闪烁的圆形特效
        float blinkAlpha = 0.6f + sinf(GetTime() * 20.0f) * 0.4f;
        DrawCircle((int)(pX + pDW / 2), (int)(pY + pDH / 2), 60.0f * state.cZone.skillAnimScale, Color{ 255, 200, 50, (unsigned char)(blinkAlpha * 255) });
        DrawCircleLines((int)(pX + pDW / 2), (int)(pY + pDH / 2), 60.0f * state.cZone.skillAnimScale, Color{ 255, 255, 100, (unsigned char)(blinkAlpha * 200) });
        Rectangle sr{ 0,0,pImgW,pImgH }, dr{ pX,pY,pDW,pDH };
        DrawTexturePro(res.playerTex, sr, dr, { 0,0 }, 0, Color{ 255,255,255,128 });
        char nb[64]; snprintf(nb, sizeof(nb), "%s(Lv.%d)", player->getName().c_str(), player->getLevel());
        DrawTextEx(res.staminaFont, nb, { pX,pY - 30 }, 24, 1, WHITE);
    } else if (player) {
        Rectangle sr{ 0,0,pImgW,pImgH }, dr{ pX,pY,pDW,pDH };
        DrawTexturePro(res.playerTex, sr, dr, { 0,0 }, 0, WHITE);
        char nb[64]; snprintf(nb, sizeof(nb), "%s(Lv.%d)", player->getName().c_str(), player->getLevel());
        DrawTextEx(res.staminaFont, nb, { pX,pY - 30 }, 24, 1, WHITE);
    }

    float rX = curW * 0.55f, eAW = curW - rX - 20;
    float eImgW = (float)res.enemyTex.width, eImgH = (float)res.enemyTex.height;
    float eTH = curH * 0.90f, eSH = eTH / 3.0f, eDS = (eSH * 0.55f) / eImgH;
    float eDW = eImgW * eDS, eDH = eImgH * eDS;

    for (int i = 0; i < 3; i++) {
        auto& eu = state.cZone.enemies[i];
        if (!eu.enemy) continue;
        float sY = i * eSH + eSH * 0.05f, eX = rX, eY = sY;
        if (eu.animActive) {
            // 移除对 mouseFrames 的依赖：用闪烁红圈替代敌人攻击动画
            float blinkAlpha = 0.5f + sinf(GetTime() * 15.0f) * 0.5f;
            DrawCircle((int)(eX + eDW / 2), (int)(eY + eDH / 2), 40.0f, Color{ 255, 0, 0, (unsigned char)(blinkAlpha * 200) });
            DrawCircleLines((int)(eX + eDW / 2), (int)(eY + eDH / 2), 40.0f, Color{ 255, 100, 0, (unsigned char)(blinkAlpha * 180) });
            // 同样绘制半透明敌人底图
            Rectangle sr{ 0,0,eImgW,eImgH }, dr{ eX,eY,eDW,eDH };
            DrawTexturePro(res.enemyTex, sr, dr, { 0,0 }, 0, Color{ 255,255,255,128 });
        } else {
            Rectangle sr{ 0,0,eImgW,eImgH }, dr{ eX,eY,eDW,eDH };
            Color tint = eu.enemy->isAlive() ? WHITE : Color{ 80,80,80,200 };
            DrawTexturePro(res.enemyTex, sr, dr, { 0,0 }, 0, tint);
        }
        DrawTextEx(res.staminaFont, eu.enemy->getName().c_str(), { eX + eDW + 10,eY }, 18, 1, RED);
        float iX = eX + eDW + 10, iY = eY + 24, iW = eAW - eDW - 20, bH = 16;
        float hR2 = (float)eu.enemy->getHp() / (float)eu.enemy->getMaxHp();
        DrawRectangle((int)iX, (int)iY, (int)iW, (int)bH, Color{ 40,40,40,220 });
        DrawRectangle((int)iX, (int)iY, (int)(iW * hR2), (int)bH, Color{ 210,60,60,240 });
        char ht2[64]; snprintf(ht2, sizeof(ht2), "HP:%d/%d", eu.enemy->getHp(), eu.enemy->getMaxHp());
        Vector2 hsz2 = MeasureTextEx(res.staminaFont, ht2, 14, 1);
        DrawTextEx(res.staminaFont, ht2, { iX + (iW - hsz2.x) / 2,iY + 1 }, 14, 1, WHITE);
        float enY = iY + bH + 2, enR = (float)eu.energy / 100.0f;
        DrawRectangle((int)iX, (int)enY, (int)iW, (int)bH, Color{ 40,40,40,220 });
        DrawRectangle((int)iX, (int)enY, (int)(iW * enR), (int)bH, Color{ 255,200,50,240 });
        char et[32]; snprintf(et, sizeof(et), "能量:%d/100", eu.energy);
        Vector2 esz = MeasureTextEx(res.staminaFont, et, 14, 1);
        DrawTextEx(res.staminaFont, et, { iX + (iW - esz.x) / 2,enY + 1 }, 14, 1, WHITE);
        if (eu.energy >= 100 && eu.enemy->isAlive()) {
            float blink = sinf(GetTime() * 6.0f) * 0.3f + 0.7f;
            DrawRectangle((int)iX, (int)(enY + bH + 2), (int)iW, 20, Color{ 255,200,50,(unsigned char)(180 * blink) });
            DrawTextEx(res.staminaFont, "能量满!攻击待发...", { iX + 4,enY + bH + 4 }, 14, 1, Color{ 80,30,0,255 });
        }
    }

    // 目标选择模式：在敌人周围绘制高亮框和提示文字
    if (!state.cZone.finished && state.cZone.selectingTarget) {
        for (int i = 0; i < 3; i++) {
            auto& eu = state.cZone.enemies[i];
            if (!eu.enemy || !eu.enemy->isAlive()) continue;
            float sY = i * eSH + eSH * 0.05f, eX = rX, eY = sY;
            float blink2 = sinf(GetTime() * 8.0f) * 0.4f + 0.6f;
            DrawRectangleLinesEx({ eX - 4, eY - 4, eDW + 8, eDH + 8 }, 4,
                Color{ 255, 255, 100, (unsigned char)(blink2 * 255) });
        }
        const char* hint = "点击敌人选择攻击目标 (点击空白取消)";
        Vector2 hintSz = MeasureTextEx(res.staminaFont, hint, 24, 1);
        DrawTextEx(res.staminaFont, hint,
            { (curW - hintSz.x) / 2, curH - 140 * sf }, 24, 1, Color{ 255, 255, 100, 255 });
    }

    // 退出战斗按钮（右上角）
    float exitBtnX = curW - 130, exitBtnY = 10, exitBtnW = 110, exitBtnH = 40;
    bool exitHover = HitTestRect(mousePos, exitBtnX, exitBtnY, exitBtnW, exitBtnH);
    Color exitColor = exitHover ? Color{ 200, 60, 60, 220 } : Color{ 180, 40, 40, 180 };
    DrawRectangleRounded({ exitBtnX, exitBtnY, exitBtnW, exitBtnH }, 0.2f, 8, exitColor);
    DrawRectangleRoundedLines({ exitBtnX, exitBtnY, exitBtnW, exitBtnH }, 0.2f, 8, Color{ 255, 100, 100, 200 });
    Vector2 exitSz = MeasureTextEx(res.staminaFont, "退出战斗", 20, 1);
    DrawTextEx(res.staminaFont, "退出战斗",
        { exitBtnX + (exitBtnW - exitSz.x) / 2, exitBtnY + (exitBtnH - exitSz.y) / 2 },
        20, 1, WHITE);
    state.cZone.exitBtnRect = { exitBtnX, exitBtnY, exitBtnW, exitBtnH };

    // 技能按钮贴图（按原比例缩放，不足用黑色补齐）
    float bAX = leftW, bAW = curW - leftW, bAH = 100 * sf, bAY = curH - bAH - 5;
    if (!state.cZone.finished && player && player->isAlive() && state.cZone.playerTurn && !state.cZone.selectingTarget) {
        const char* sn[] = { "普攻","小技能一","小技能二","大招" };
        int bc = 4; float bW = bAW / bc, bH2 = bAH;
        for (int i = 0; i < bc; i++) {
            float bx = bAX + i * bW, by = bAY;
            bool hover = HitTestRect(mousePos, bx, by, bW, bH2);
            // 黑色背景
            Color bg = hover ? Color{ 80,80,100,255 } : Color{ 0,0,0,255 };
            DrawRectangle((int)bx, (int)by, (int)bW, (int)bH2, bg);
            // 技能图标贴图（按原比例缩放居中绘制）
            Texture2D& tex = res.btnTex[i];
            if (tex.id > 0) {
                float texW = (float)tex.width, texH = (float)tex.height;
                float scaleX = bW / texW, scaleY = bH2 / texH;
                float scale = (scaleX < scaleY) ? scaleX : scaleY;
                float drawW = texW * scale, drawH = texH * scale;
                float drawX = bx + (bW - drawW) / 2, drawY = by + (bH2 - drawH) / 2;
                DrawTexturePro(tex,
                    { 0, 0, texW, texH },
                    { drawX, drawY, drawW, drawH },
                    { 0, 0 }, 0.0f, WHITE);
            }
            // 技能名称和消耗（在图标下方）
            DrawTextEx(res.staminaFont, sn[i], { bx + 4,by + bH2 - 40 }, 14, 1, WHITE);
            Color mpColor = (i == 0) ? GREEN : ((i == 3) ? ORANGE : GOLD);
            char mpInfo[32];
            if (i == 0) snprintf(mpInfo, sizeof(mpInfo), "0MP");
            else if (i == 3) snprintf(mpInfo, sizeof(mpInfo), "100MP");
            else snprintf(mpInfo, sizeof(mpInfo), "50MP");
            DrawTextEx(res.staminaFont, mpInfo, { bx + 4,by + bH2 - 22 }, 14, 1, mpColor);
        }
    }

    if (state.cZone.finished) {
        Color fc = state.cZone.victory ? Color{ 60,200,60,200 } : Color{ 200,60,60,200 };
        DrawRectangle(0, 0, curW, curH, fc);
        const char* msg = state.cZone.victory ? "战斗胜利!" : "战斗失败";
        Vector2 msz = MeasureTextEx(res.staminaFont, msg, 60, 1);
        DrawTextEx(res.staminaFont, msg, { (curW - msz.x) / 2,(curH - msz.y) / 2 }, 60, 1, WHITE);
        DrawTextEx(res.staminaFont, "点击返回", { (curW - msz.x) / 2,(curH - msz.y) / 2 + 70 }, 24, 1, WHITE);
    }

    float logY = curH - 300 * sf, logH = 190 * sf;
    DrawTextEx(res.staminaFont, "战斗日志:", { 10,logY + 4 }, 18, 1, GRAY);
    int maxLines = (int)((logH - 30) / 20);
    int start = (int)state.cZone.logLines.size() - maxLines;
    if (start < 0) start = 0;
    for (size_t j = start; j < state.cZone.logLines.size(); j++) {
        DrawTextEx(res.staminaFont, state.cZone.logLines[j].c_str(),
            { 10,logY + 28 + (float)(j - start) * 20 }, 16, 1, WHITE);
    }
}

// ============================================================
// 选择角色槽位
// ============================================================
static void SelectPlayerSlot(UIState& state, int idx) {
    if (idx < 0 || idx >= 2) return;
    if (!state.cZone.playerSlots[idx].fighter || !state.cZone.playerSlots[idx].fighter->isAlive()) return;
    if (state.cZone.currentPlayerIdx == idx) return;
    for (int i = 0; i < 2; i++) state.cZone.playerSlots[i].inBattle = false;
    state.cZone.playerSlots[idx].inBattle = true;
    state.cZone.currentPlayerIdx = idx;
    char buf[64]; snprintf(buf, sizeof(buf), "已切换为:%s", state.cZone.playerSlots[idx].fighter->getName().c_str());
    state.cZone.logLines.push_back(std::string(buf));
}

// ============================================================
// 目标选择：点击敌人触发技能（群体攻击无需选择目标）
// ============================================================
static void ExecuteSkillOnTarget(UIState& state, int targetIdx) {
    if (targetIdx < 0 || targetIdx >= 3) return;
    if (!state.cZone.enemies[targetIdx].enemy || !state.cZone.enemies[targetIdx].enemy->isAlive()) return;

    Combatant* player = GetActivePlayer(state);
    if (!player) return;

    int rawDmg = state.cZone.pendingRawDmg;
    bool crit = state.cZone.pendingCrit;

    state.cZone.skillPayload.rawDmg = rawDmg;
    state.cZone.skillPayload.finalDmg = rawDmg;
    state.cZone.skillPayload.isCrit = crit;
    state.cZone.skillPayload.targetIdx = targetIdx;
    state.cZone.skillPayload.isAoe = false;  // 单体攻击

    state.cZone.skillAnimActive = true;
    state.cZone.skillAnimFrame = 0;
    state.cZone.skillAnimTimer = 0.0f;
    state.cZone.skillAnimDuration = 0.5f;
    state.cZone.skillAnimScale = 1.5f;
    state.cZone.skillAnimPos = { (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f };

    state.cZone.selectingTarget = false;
}

// ============================================================
// 群体攻击：无需选择目标，直接对所有敌人释放
// ============================================================
static void ExecuteAoeSkill(UIState& state) {
    Combatant* player = GetActivePlayer(state);
    if (!player) return;

    int rawDmg = state.cZone.pendingRawDmg;
    bool crit = state.cZone.pendingCrit;

    state.cZone.skillPayload.rawDmg = rawDmg;
    state.cZone.skillPayload.finalDmg = rawDmg;
    state.cZone.skillPayload.isCrit = crit;
    state.cZone.skillPayload.targetIdx = -1;  // 无效索引，触发群体逻辑
    state.cZone.skillPayload.isAoe = true;     // 群体攻击

    state.cZone.skillAnimActive = true;
    state.cZone.skillAnimFrame = 0;
    state.cZone.skillAnimTimer = 0.0f;
    state.cZone.skillAnimDuration = 0.5f;
    state.cZone.skillAnimScale = 1.5f;
    state.cZone.skillAnimPos = { (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f };

    state.cZone.selectingTarget = false;
}

// ============================================================
// C区战斗输入处理
// ============================================================
void HandleCZoneBattleInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mp = GetMousePosition();
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    if (state.cZone.finished) {
        state.screenState = SCREEN_MAP;
        state.cZone.active = false;
        return;
    }

    float sf = ((float)curW / DESIGN_W < (float)curH / DESIGN_H) ? (float)curW / DESIGN_W : (float)curH / DESIGN_H;
    float leftW = 200 * sf;

    // 如果处于目标选择模式，点击敌人来释放技能
    if (state.cZone.selectingTarget) {
        float rX = curW * 0.55f;
        float eImgW = (float)res.enemyTex.width, eImgH = (float)res.enemyTex.height;
        float eTH = curH * 0.90f, eSH = eTH / 3.0f, eDS = (eSH * 0.55f) / eImgH;
        float eDW = eImgW * eDS, eDH = eImgH * eDS;
        for (int i = 0; i < 3; i++) {
            auto& eu = state.cZone.enemies[i];
            if (!eu.enemy || !eu.enemy->isAlive()) continue;
            float sY = i * eSH + eSH * 0.05f, eX = rX, eY = sY;
            if (HitTestRect(mp, eX, eY, eDW, eDH)) {
                ExecuteSkillOnTarget(state, i);
                return;
            }
        }
        // 点击空白区域取消目标选择
        state.cZone.selectingTarget = false;
        state.cZone.logLines.push_back("已取消选择目标");
        return;
    }

    // 退出按钮
    if (HitTestRect(mp, state.cZone.exitBtnRect.x, state.cZone.exitBtnRect.y,
                    state.cZone.exitBtnRect.width, state.cZone.exitBtnRect.height)) {
        state.cZone.active = false;
        state.cZone.finished = false;
        state.screenState = SCREEN_MAP;
        return;
    }

    // 左侧角色选择
    for (int i = 0; i < 2; i++) {
        float sy = 50 + i * 90;
        if (!state.cZone.playerSlots[i].fighter || !state.cZone.playerSlots[i].fighter->isAlive()) continue;
        if (HitTestRect(mp, 8.0f, sy, leftW - 16, 80)) {
            SelectPlayerSlot(state, i);
            return;
        }
    }

    if (!state.cZone.playerTurn || state.cZone.skillAnimActive) return;
    Combatant* player = GetActivePlayer(state);
    if (!player || !player->isAlive()) return;

    // 技能按钮点击 → 使用角色技能数据判断单体/群体
    float bAX = leftW, bAW = curW - leftW, bAH = 100 * sf, bAY = curH - bAH - 5;
    int bc = 4; float bW = bAW / bc, bH2 = bAH;
    for (int i = 0; i < bc; i++) {
        float bx = bAX + i * bW, by = bAY;
        if (HitTestRect(mp, bx, by, bW, bH2)) {
            if (state.cZone.skillAnimActive) return;

            // 从角色技能数据获取信息
            Skill* sk = (i == 0) ? player->getNormalAttack()
                      : (i == 1) ? player->getSkill1()
                      : (i == 2) ? player->getSkill2()
                      : player->getUltimate();
            if (!sk) return;

            int rawDmg = 0;
            int pAtk = player->getAttack();
            int pCrit = (int)(player->getCritRate() * 100.0);
            int roll = rand() % 100;
            bool crit = false;
            int mpCost = sk->mpCost;

            // 检查MP消耗（普攻0消耗不检查）
            if (mpCost > 0 && !player->hasEnoughMp(mpCost)) {
                state.cZone.logLines.push_back("MP不足!");
                return;
            }
            if (mpCost > 0) {
                player->consumeMp(mpCost);
            }

            // 根据技能 effect 计算伤害
            if (sk->effect == "attack") {
                rawDmg = (int)(pAtk * sk->effectValue / 100.0);
            }

            // 暴击判定（根据技能 subEffect 和 subEffectValue）
            int critChance = pCrit;
            if (sk->subEffect == "buff_crit") critChance += sk->subEffectValue;
            crit = (roll < critChance);
            if (crit) {
                rawDmg = (int)(rawDmg * 1.5);
                state.cZone.logLines.push_back("暴击!");
            }

            state.cZone.logLines.push_back(std::string("选择") + sk->name + "目标");

            // 暂存技能数据
            state.cZone.pendingSkillIdx = i;
            state.cZone.pendingRawDmg = rawDmg;
            state.cZone.pendingCrit = crit;
            state.cZone.pendingMpCost = mpCost;

            // 判断是否群体攻击
            bool isAoe = (sk->target == "enemy_all");
            if (isAoe) {
                // 群体攻击：直接释放，无需选择目标
                state.cZone.logLines.back() = "释放 " + sk->name + "（群体攻击）";
                ExecuteAoeSkill(state);
            } else {
                // 单体攻击：进入目标选择模式
                state.cZone.selectingTarget = true;
            }
            return;
        }
    }
}
