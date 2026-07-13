#include "SplashScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"

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
    DrawCoordGrid(res.mainTex, curW, curH, { 255, 255, 0, 100 });
    DrawStamina(res.staminaFont, state.stamina, state.maxStamina, res.mainTex, curW, curH);
    DrawCoins(res.staminaFont, state.coins, res.mainTex, curW, curH);
    DrawDiamonds(res.staminaFont, state.diamonds, res.mainTex, curW, curH);
}

// ============================================================
// 处理主界面输入（鼠标点击）
// ============================================================
void HandleMainScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();

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
        }
    }
}
