#include "SplashScreen.h"
#include "MapScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "role/enemy/EnemyManager.h"
#include <cstdio>

// ============================================================
// 绘制主地图界面
// ============================================================
void RenderMapScreen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.mapTex, curW, curH);
}

// ============================================================
// 处理主地图输入
// ============================================================
void HandleMapScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    // 检测广播站(A区)点击区域：设计坐标竖着200-500，横着100-500
    Vector2 a1 = DesignToScreen(100.0f, 200.0f, res.mapTex, curW, curH);
    Vector2 a2 = DesignToScreen(500.0f, 500.0f, res.mapTex, curW, curH);
    // 检测B区点击区域：设计坐标横着500-700，竖着400-700
    Vector2 b1 = DesignToScreen(500.0f, 400.0f, res.mapTex, curW, curH);
    Vector2 b2 = DesignToScreen(700.0f, 700.0f, res.mapTex, curW, curH);
    // 检测C区点击区域：设计坐标竖着600-800，横着800-1000
    Vector2 c1 = DesignToScreen(800.0f, 600.0f, res.mapTex, curW, curH);
    Vector2 c2 = DesignToScreen(1000.0f, 800.0f, res.mapTex, curW, curH);

    if (HitTestRect(mousePos, a1.x, a1.y, a2.x - a1.x, a2.y - a1.y)) {
        state.screenState = SCREEN_MAP_SUB;
    } else if (HitTestRect(mousePos, b1.x, b1.y, b2.x - b1.x, b2.y - b1.y)) {
        state.screenState = SCREEN_MAP_SUB2;
    } else if (HitTestRect(mousePos, c1.x, c1.y, c2.x - c1.x, c2.y - c1.y)) {
        state.screenState = SCREEN_MAP_SUB3;
    } else {
        state.screenState = SCREEN_MAIN;
    }
}

// ============================================================
// 绘制 A 区子地图
// ============================================================
void RenderMapSubScreen(UIResource& res, int curW, int curH) {
    ClearBackground(BLACK);
    DrawImageFillScreen(res.mapSubTex, curW, curH);
}

// ============================================================
// 绘制 B 区子地图
// ============================================================
void RenderMapSub2Screen(UIResource& res, UIState& state, int curW, int curH) {
        ClearBackground(BLACK);
    DrawImageFillScreen(res.mapSubBTex, curW, curH);
    // 如果showPeaceInB为true，在屏幕下方显示peace图片
    if (state.showPeaceInB) {
        float imgW = (float)res.peaceTex.width;
        float imgH = (float)res.peaceTex.height;
        float maxW = (float)curW;
        float maxH = (float)curH * 0.3f;
        float scale = (maxW / imgW < maxH / imgH) ? (maxW / imgW) : (maxH / imgH);
        float drawW = imgW * scale;
        float drawH = imgH * scale;
        float posX = (curW - drawW) / 2.0f;
        float posY = curH - drawH - 10;
        state.peaceScreenRect = { posX, posY, drawW, drawH };
        Rectangle srcRect = { 0, 0, imgW, imgH };
        Rectangle dstRect = { posX, posY, drawW, drawH };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(res.peaceTex, srcRect, dstRect, origin, 0, WHITE);
    } else {
        state.peaceScreenRect = { 0, 0, 0, 0 };
    }
}

// ============================================================
// 处理 B 区子地图输入
// ============================================================
void HandleMapSub2ScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    // 检测peace点击区域（设计坐标）：竖着100-300，横着700-1100
    Vector2 peace1 = DesignToScreen(700.0f, 100.0f, res.mapSubBTex, curW, curH);
    Vector2 peace2 = DesignToScreen(1100.0f, 300.0f, res.mapSubBTex, curW, curH);

    // 如果peace图片正在显示，检测是否点击了peace图片本身
    bool hitPeacePic = false;
    if (state.showPeaceInB && state.peaceScreenRect.width > 0 && state.peaceScreenRect.height > 0) {
        if (HitTestRect(mousePos, state.peaceScreenRect.x, state.peaceScreenRect.y, state.peaceScreenRect.width, state.peaceScreenRect.height)) {
            hitPeacePic = true;
            state.screenState = SCREEN_BATTLE;

            // 初始化战斗
            std::vector<Combatant*> allFighters = game->getFighters();
            if (!allFighters.empty()) {
                state.battlePlayer = allFighters[0];
            } else {
                static Combatant* pDefaultFighter = nullptr;
                if (!pDefaultFighter) {
                    pDefaultFighter = new Combatant("钟关白", "机械学院");
                }
                                pDefaultFighter->setHp(900);
                pDefaultFighter->setMaxHp(900);
                pDefaultFighter->setAttack(300);
                pDefaultFighter->setDefense(200);
                pDefaultFighter->setMaxMp(100);
                pDefaultFighter->setMp(100);  // 满蓝开局
                pDefaultFighter->setCritRate(0.10);
                state.battlePlayer = pDefaultFighter;
            }

            EnemyManager* em = game->getEnemyManager();
            if (em) {
                em->initFirstStage();
                auto& enemies = em->getCurrentEnemies();
                if (!enemies.empty()) {
                    state.battleEnemy = &enemies[0];
                } else {
                    state.battleEnemy = nullptr;
                }
            } else {
                state.battleEnemy = nullptr;
            }

            state.battlePlayerTurn = true;
            state.battleFinished = false;
            state.battleVictory = false;
            state.battleLogLines.clear();
            state.battleLogLines.push_back("战斗开始!请选择行动");
        }
    }

    if (!hitPeacePic) {
        if (HitTestRect(mousePos, peace1.x, peace1.y, peace2.x - peace1.x, peace2.y - peace1.y)) {
            // 点击peace区域，切换显示peace图片
            state.showPeaceInB = !state.showPeaceInB;
        } else {
            state.screenState = SCREEN_MAP;
            state.showPeaceInB = false;
        }
    }
}

// ============================================================
// 绘制 C 区子地图
// ============================================================
void RenderMapSub3Screen(UIResource& res, int curW, int curH) {
        ClearBackground(BLACK);
    DrawImageFillScreen(res.mapSubCTex, curW, curH);
}

// ============================================================
// 处理 C 区子地图输入
// ============================================================
void HandleMapSub3ScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

    // 点击区域：设计坐标 竖着100-300，横着700-900
    Vector2 c1 = DesignToScreen(700.0f, 100.0f, res.mapSubCTex, curW, curH);
    Vector2 c2 = DesignToScreen(900.0f, 300.0f, res.mapSubCTex, curW, curH);

    if (HitTestRect(mousePos, c1.x, c1.y, c2.x - c1.x, c2.y - c1.y)) {
        InitCZoneBattle(state, game);
        state.screenState = SCREEN_CZONE_BATTLE;
    } else {
        state.screenState = SCREEN_MAP;
    }
}
