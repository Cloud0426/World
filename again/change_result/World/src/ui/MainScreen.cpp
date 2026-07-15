#include "SplashScreen.h"
#include "MainScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include <cmath>

// ============================================================
// 主界面按钮定义（设计坐标系 1440x960）
// ============================================================
static ImageButton btnSupply   = { 1000, 800, 1200, 900 };
static ImageButton btnTaskList = { 700, 700, 900, 800 };
static ImageButton btnStore    = { 100, 400, 300, 600 };
static ImageButton btnMap      = { 800, 200, 1000, 300 };
static ImageButton btnStory    = { 600, 400, 800, 500 };
static ImageButton btnRecruit  = { 400, 500, 600, 700 };
static ImageButton btnCharFile = { 0, 0, 100, 300 };

// ── 存档/读档按钮（设计坐标，右下角区域）──
// 存档：横1200-1320，竖850-900
// 读档：横1320-1440，竖850-900
static ImageButton btnSaveGame = { 1200, 850, 1320, 900 };
static ImageButton btnLoadGame = { 1320, 850, 1440, 900 };

// ============================================================
// 绘制初始界面
// ============================================================
void RenderInitScreen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.initTex, curW, curH);
    const char* hint = "Click anywhere to enter...";
    int hintSize = 20;
    int hintW = MeasureText(hint, hintSize);
    Color hintColor = { 180, 180, 180, 200 };
    DrawText(hint, (curW - hintW) / 2, curH - 40, hintSize, hintColor);
}

// ============================================================
// 处理初始界面输入
// ============================================================
void HandleInitScreenInput(UIState& state) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state.screenState = SCREEN_MAIN;
    }
}

// ============================================================
// 绘制主界面
// ============================================================
void RenderMainScreen(UIResource& res, UIState& state, int curW, int curH) {
        DrawImageCentered(res.mainTex, curW, curH);
    DrawStamina(res.staminaFont, state.stamina, state.maxStamina, res.mainTex, curW, curH);
    DrawCoins(res.staminaFont, state.coins, res.mainTex, curW, curH);
        DrawDiamonds(res.staminaFont, state.diamonds, res.mainTex, curW, curH);

        // ── 左上角显示角色姓名 ──
    {
        Vector2 nameP1 = DesignToScreen(200.0f, 10.0f, res.mainTex, curW, curH);
        Vector2 nameSz = MeasureTextEx(res.staminaFont, state.charName.c_str(), 32, 1);
        float bgW = nameSz.x + 24;
        float bgH = 50;
        DrawRectangleRounded({ nameP1.x, nameP1.y, bgW, bgH }, 0.15f, 8, Color{ 0, 0, 0, 140 });
        DrawTextEx(res.staminaFont, state.charName.c_str(),
            { nameP1.x + 12, nameP1.y + (bgH - nameSz.y) / 2 }, 32, 1, GOLD);
    }

    // 🆕 如果招募到新伙伴，在"我的伙伴"区域显示提示
    if (state.showNewFighterHint) {
        // 我的伙伴区域：设计坐标 1200,300 ~ 1400,400
        Vector2 hintP1 = DesignToScreen(1200.0f, 280.0f, res.mainTex, curW, curH);
        Vector2 hintP2 = DesignToScreen(1400.0f, 420.0f, res.mainTex, curW, curH);
        float hintX = hintP1.x;
        float hintY = hintP1.y;
        float hintW = hintP2.x - hintP1.x;
        float hintH = hintP2.y - hintP1.y;

        // 闪烁效果
        float blink = sinf(GetTime() * 3.0f) * 0.3f + 0.7f;
        Color hintBg = Color{ 255, 200, 50, (unsigned char)(160 * blink) };
        DrawRectangleRounded({ hintX, hintY, hintW, hintH }, 0.15f, 8, hintBg);
        DrawRectangleRoundedLines({ hintX, hintY, hintW, hintH }, 0.15f, 8,
            Color{ 255, 255, 100, (unsigned char)(200 * blink) });

        const char* hintText = "结识新的伙伴，点击查看";
        int hintFontSize = 28;
        Vector2 hintSz = MeasureTextEx(res.staminaFont, hintText, (float)hintFontSize, 1);
        DrawTextEx(res.staminaFont, hintText,
            { hintX + (hintW - hintSz.x) / 2, hintY + (hintH - hintSz.y) / 2 },
            (float)hintFontSize, 1, Color{ 50, 30, 0, (unsigned char)(255 * blink) });
    }

        // ── 存档/读档/新存档按钮 ──
    {
        Vector2 saveP1 = DesignToScreen((float)btnSaveGame.x1, (float)btnSaveGame.y1, res.mainTex, curW, curH);
        Vector2 saveP2 = DesignToScreen((float)btnSaveGame.x2, (float)btnSaveGame.y2, res.mainTex, curW, curH);
        float sx = saveP1.x, sy = saveP1.y, sw = saveP2.x - saveP1.x, sh = saveP2.y - saveP1.y;

        Vector2 loadP1 = DesignToScreen((float)btnLoadGame.x1, (float)btnLoadGame.y1, res.mainTex, curW, curH);
        Vector2 loadP2 = DesignToScreen((float)btnLoadGame.x2, (float)btnLoadGame.y2, res.mainTex, curW, curH);
        float lx = loadP1.x, ly = loadP1.y, lw = loadP2.x - loadP1.x, lh = loadP2.y - loadP1.y;

        Vector2 mousePos = GetMousePosition();
        bool saveHov = HitTestRect(mousePos, sx, sy, sw, sh);
        bool loadHov = HitTestRect(mousePos, lx, ly, lw, lh);

        Color saveColor = saveHov ? Color{ 80, 200, 80, 220 } : Color{ 60, 160, 60, 180 };
        Color loadColor = loadHov ? Color{ 80, 180, 220, 220 } : Color{ 60, 140, 180, 180 };

        DrawRectangleRounded({ sx, sy, sw, sh }, 0.2f, 8, saveColor);
        DrawRectangleRoundedLines({ sx, sy, sw, sh }, 0.2f, 8, Color{ 100, 255, 100, 200 });
        Vector2 saveSz = MeasureTextEx(res.staminaFont, "存档", 24, 1);
        DrawTextEx(res.staminaFont, "存档",
            { sx + (sw - saveSz.x) / 2, sy + (sh - saveSz.y) / 2 }, 24, 1, WHITE);

        DrawRectangleRounded({ lx, ly, lw, lh }, 0.2f, 8, loadColor);
        DrawRectangleRoundedLines({ lx, ly, lw, lh }, 0.2f, 8, Color{ 100, 220, 255, 200 });
        Vector2 loadSz = MeasureTextEx(res.staminaFont, "读档", 24, 1);
        DrawTextEx(res.staminaFont, "读档",
            { lx + (lw - loadSz.x) / 2, ly + (lh - loadSz.y) / 2 }, 24, 1, WHITE);

        // "新的存档"按钮：放在存档按钮上方
        float newBtnW = sw;
        float newBtnH = sh;
        float newBtnX = sx;
        float newBtnY = sy - newBtnH - 8;
        bool newHov = HitTestRect(mousePos, newBtnX, newBtnY, newBtnW, newBtnH);
        Color newColor = newHov ? Color{ 220, 80, 60, 220 } : Color{ 180, 60, 40, 180 };
        DrawRectangleRounded({ newBtnX, newBtnY, newBtnW, newBtnH }, 0.2f, 8, newColor);
        DrawRectangleRoundedLines({ newBtnX, newBtnY, newBtnW, newBtnH }, 0.2f, 8, Color{ 255, 120, 80, 200 });
        Vector2 newSz = MeasureTextEx(res.staminaFont, "新存档", 24, 1);
        DrawTextEx(res.staminaFont, "新存档",
            { newBtnX + (newBtnW - newSz.x) / 2, newBtnY + (newBtnH - newSz.y) / 2 }, 24, 1, WHITE);
        state.newGameBtnRect = { newBtnX, newBtnY, newBtnW, newBtnH };
    }

    // ── 存档提示弹窗 ──
    if (state.showSaveDlg) {
        state.saveDlgTimer -= GetFrameTime();
        // 半透明遮罩
        DrawRectangle(0, 0, curW, curH, Color{ 0, 0, 0, 140 });
        float dlgW = 400, dlgH = 180;
        float dlgX = (curW - dlgW) / 2, dlgY = (curH - dlgH) / 2;
        DrawRectangleRounded({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 40, 40, 70, 240 });
        DrawRectangleRoundedLines({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 200, 200, 255, 180 });
        Vector2 msgSz = MeasureTextEx(res.staminaFont, state.saveDlgMsg, 32, 1);
        DrawTextEx(res.staminaFont, state.saveDlgMsg,
            { dlgX + (dlgW - msgSz.x) / 2, dlgY + (dlgH - msgSz.y) / 2 },
            32, 1, GOLD);
        if (state.saveDlgTimer <= 0) {
            state.showSaveDlg = false;
        }
    }

        // ── 新建存档确认弹窗 ──
    if (state.showNewGameDlg) {
        DrawRectangle(0, 0, curW, curH, Color{ 0, 0, 0, 160 });
        float dlgW = 500, dlgH = 220;
        float dlgX = (curW - dlgW) / 2, dlgY = (curH - dlgH) / 2;

        DrawRectangleRounded({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 40, 40, 70, 245 });
        DrawRectangleRoundedLines({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 200, 200, 255, 180 });

        DrawTextEx(res.staminaFont, "开始新存档", { dlgX + 20, dlgY + 20 }, 30, 1, GOLD);
        DrawTextEx(res.staminaFont, "新建存档将覆盖当前进度，确认？",
            { dlgX + 20, dlgY + 70 }, 24, 1, LIGHTGRAY);

        // 确定按钮
        int bw = 130, bh = 44, by = (int)dlgY + 140;
        int bok = (int)dlgX + (int)dlgW / 2 - bw - 15;
        int bca = (int)dlgX + (int)dlgW / 2 + 15;
        Vector2 mp_new = GetMousePosition();

        bool okHov = HitTestRect(mp_new, (float)bok, (float)by, (float)bw, (float)bh);
        bool caHov = HitTestRect(mp_new, (float)bca, (float)by, (float)bw, (float)bh);

        DrawRectangle(bok, by, bw, bh, okHov ? Color{ 80, 200, 80, 255 } : Color{ 60, 180, 60, 230 });
        Vector2 okSz = MeasureTextEx(res.staminaFont, "确定", 24, 1);
        DrawTextEx(res.staminaFont, "确定",
            { (float)bok + ((float)bw - okSz.x) / 2, (float)by + ((float)bh - okSz.y) / 2 }, 24, 1, WHITE);

        DrawRectangle(bca, by, bw, bh, caHov ? Color{ 200, 80, 80, 255 } : Color{ 180, 60, 60, 230 });
        Vector2 caSz = MeasureTextEx(res.staminaFont, "取消", 24, 1);
        DrawTextEx(res.staminaFont, "取消",
            { (float)bca + ((float)bw - caSz.x) / 2, (float)by + ((float)bh - caSz.y) / 2 }, 24, 1, WHITE);
    }

    // ── 读档确认弹窗 ──
    if (state.showLoadDlg) {
        DrawRectangle(0, 0, curW, curH, Color{ 0, 0, 0, 160 });
        float dlgW = 500, dlgH = 220;
        float dlgX = (curW - dlgW) / 2, dlgY = (curH - dlgH) / 2;

        DrawRectangleRounded({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 40, 40, 70, 245 });
        DrawRectangleRoundedLines({ dlgX, dlgY, dlgW, dlgH }, 0.1f, 8, Color{ 200, 200, 255, 180 });

        DrawTextEx(res.staminaFont, "读档确认", { dlgX + 20, dlgY + 20 }, 30, 1, GOLD);
        DrawTextEx(res.staminaFont, "读档将丢失未保存的进度，确认？",
            { dlgX + 20, dlgY + 70 }, 24, 1, LIGHTGRAY);

        // 确定按钮
        int bw = 130, bh = 44, by = (int)dlgY + 140;
        int bok = (int)dlgX + (int)dlgW / 2 - bw - 15;
        int bca = (int)dlgX + (int)dlgW / 2 + 15;
        Vector2 mousePos = GetMousePosition();

        bool okHov = HitTestRect(mousePos, (float)bok, (float)by, (float)bw, (float)bh);
        bool caHov = HitTestRect(mousePos, (float)bca, (float)by, (float)bw, (float)bh);

        DrawRectangle(bok, by, bw, bh, okHov ? Color{ 80, 200, 80, 255 } : Color{ 60, 180, 60, 230 });
        Vector2 okSz = MeasureTextEx(res.staminaFont, "确定", 24, 1);
        DrawTextEx(res.staminaFont, "确定",
            { (float)bok + ((float)bw - okSz.x) / 2, (float)by + ((float)bh - okSz.y) / 2 }, 24, 1, WHITE);

        DrawRectangle(bca, by, bw, bh, caHov ? Color{ 200, 80, 80, 255 } : Color{ 180, 60, 60, 230 });
        Vector2 caSz = MeasureTextEx(res.staminaFont, "取消", 24, 1);
        DrawTextEx(res.staminaFont, "取消",
            { (float)bca + ((float)bw - caSz.x) / 2, (float)by + ((float)bh - caSz.y) / 2 }, 24, 1, WHITE);
    }
}

// ============================================================
// 处理主界面输入（鼠标点击）
// ============================================================
void HandleMainScreenInput(UIResource& res, UIState& state, int curW, int curH, GameManager* game) {
    Vector2 mousePos = GetMousePosition();

        // ── 如果新建存档确认弹窗打开，优先处理 ──
    if (state.showNewGameDlg) {
        float dlgW = 500, dlgH = 220;
        float dlgX = (curW - dlgW) / 2, dlgY = (curH - dlgH) / 2;
        int bw = 130, bh = 44, by = (int)dlgY + 140;
        int bok = (int)dlgX + (int)dlgW / 2 - bw - 15;
        int bca = (int)dlgX + (int)dlgW / 2 + 15;

        if (HitTestRect(mousePos, (float)bok, (float)by, (float)bw, (float)bh)) {
            // 确定新存档：重置游戏
            if (game) {
                game->newGame();
                // 同步新数据到UIState
                MainCharacter* p = game->getMainChar();
                if (p) {
                    state.coins = p->getGold();
                    state.diamonds = p->getDiamond();
                    state.stamina = p->getStamina();
                    state.maxStamina = p->getMaxStamina();
                }
                state.hasChenErYuan = false;
                state.showNewFighterHint = false;
                state.charName = p ? p->getName() : "英雄";
                // 显示成功提示
                snprintf(state.saveDlgMsg, sizeof(state.saveDlgMsg), "已开始新的存档！");
                state.showSaveDlg = true;
                state.saveDlgTimer = 1.5f;
            }
            state.showNewGameDlg = false;
            return;
        }
        if (HitTestRect(mousePos, (float)bca, (float)by, (float)bw, (float)bh)) {
            state.showNewGameDlg = false;
            return;
        }
        // 点击弹窗外部也关闭
        if (!HitTestRect(mousePos, dlgX, dlgY, dlgW, dlgH)) {
            state.showNewGameDlg = false;
        }
        return;
    }

    // ── 如果读档弹窗打开，优先处理 ──
    if (state.showLoadDlg) {
        float dlgW = 500, dlgH = 220;
        float dlgX = (curW - dlgW) / 2, dlgY = (curH - dlgH) / 2;
        int bw = 130, bh = 44, by = (int)dlgY + 140;
        int bok = (int)dlgX + (int)dlgW / 2 - bw - 15;
        int bca = (int)dlgX + (int)dlgW / 2 + 15;

        if (HitTestRect(mousePos, (float)bok, (float)by, (float)bw, (float)bh)) {
            // 确定读档
            if (game) {
                game->loadGame();
                // 读档后同步数据
                MainCharacter* p = game->getMainChar();
                if (p) {
                    state.coins = p->getGold();
                    state.diamonds = p->getDiamond();
                    state.stamina = p->getStamina();
                    state.maxStamina = p->getMaxStamina();
                }
                // 同步陈尔愿状态
                state.hasChenErYuan = false;
                auto fighters = game->getFighters();
                for (auto* f : fighters) {
                    if (f && f->getName() == "\u9648\u5c14\u613f" && f->getCollege() == "\u673a\u68b0\u5b66\u9662") {
                        state.hasChenErYuan = true;
                        break;
                    }
                }
                // 显示成功提示
                snprintf(state.saveDlgMsg, sizeof(state.saveDlgMsg), "\u8bfb\u6863\u6210\u529f\uff01");
                state.showSaveDlg = true;
                state.saveDlgTimer = 1.5f;
            }
            state.showLoadDlg = false;
            return;
        }
        if (HitTestRect(mousePos, (float)bca, (float)by, (float)bw, (float)bh)) {
            state.showLoadDlg = false;
            return;
        }
        // 点击弹窗外部也关闭
        if (!HitTestRect(mousePos, dlgX, dlgY, dlgW, dlgH)) {
            state.showLoadDlg = false;
        }
        return;
    }

        // ── 检测"新的存档"按钮 ──
    if (HitTestRect(mousePos, state.newGameBtnRect.x, state.newGameBtnRect.y,
                    state.newGameBtnRect.width, state.newGameBtnRect.height)) {
        state.showNewGameDlg = true;
        return;
    }

    // ── 检测存档按钮 ──
    {
        Vector2 saveP1 = DesignToScreen((float)btnSaveGame.x1, (float)btnSaveGame.y1, res.mainTex, curW, curH);
        Vector2 saveP2 = DesignToScreen((float)btnSaveGame.x2, (float)btnSaveGame.y2, res.mainTex, curW, curH);
        float sx = saveP1.x, sy = saveP1.y, sw = saveP2.x - saveP1.x, sh = saveP2.y - saveP1.y;
        if (HitTestRect(mousePos, sx, sy, sw, sh)) {
            if (game) {
                game->saveGame();
                snprintf(state.saveDlgMsg, sizeof(state.saveDlgMsg), "\u5b58\u6863\u6210\u529f\uff01");
                state.showSaveDlg = true;
                state.saveDlgTimer = 1.5f;
            }
            return;
        }
    }

    // ── 检测读档按钮 ──
    {
        Vector2 loadP1 = DesignToScreen((float)btnLoadGame.x1, (float)btnLoadGame.y1, res.mainTex, curW, curH);
        Vector2 loadP2 = DesignToScreen((float)btnLoadGame.x2, (float)btnLoadGame.y2, res.mainTex, curW, curH);
        float lx = loadP1.x, ly = loadP1.y, lw = loadP2.x - loadP1.x, lh = loadP2.y - loadP1.y;
        if (HitTestRect(mousePos, lx, ly, lw, lh)) {
            state.showLoadDlg = true;
            return;
        }
    }

    if (HitTestButton(mousePos, res.mainTex, curW, curH, btnSupply)) {
        state.screenState = SCREEN_BAG;
    } else if (HitTestButton(mousePos, res.mainTex, curW, curH, btnTaskList)) {
        state.screenState = SCREEN_TASK_LIST;
    } else if (HitTestButton(mousePos, res.mainTex, curW, curH, btnStore)) {
        state.screenState = SCREEN_STORE;
    } else if (HitTestButton(mousePos, res.mainTex, curW, curH, btnMap)) {
        state.screenState = SCREEN_MAP;
    } else if (HitTestButton(mousePos, res.mainTex, curW, curH, btnStory)) {
        state.screenState = SCREEN_STORY1;
    } else if (HitTestButton(mousePos, res.mainTex, curW, curH, btnRecruit)) {
        state.screenState = SCREEN_RECRUIT;
    } else if (HitTestButton(mousePos, res.mainTex, curW, curH, btnCharFile)) {
        state.screenState = SCREEN_CHARACTER_FILE;
    } else {
        // 检测"我的伙伴"点击区域：设计坐标竖300-400，横1200-1400
        Vector2 fr1 = DesignToScreen(1200.0f, 300.0f, res.mainTex, curW, curH);
        Vector2 fr2 = DesignToScreen(1400.0f, 400.0f, res.mainTex, curW, curH);
                if (HitTestRect(mousePos, fr1.x, fr1.y, fr2.x - fr1.x, fr2.y - fr1.y)) {
            state.screenState = SCREEN_FRIENDS;
            state.showNewFighterHint = false;  // 🆕 进入伙伴界面消除提示
        }
    }
}
