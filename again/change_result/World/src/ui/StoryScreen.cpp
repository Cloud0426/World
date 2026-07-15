#include "SplashScreen.h"
#include "StoryScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"

// ============================================================
void RenderStory1Screen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.story1Tex, curW, curH);
}

void HandleStory1ScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();
    Vector2 imgPt = DesignToScreen(800.0f, 0.0f, res.story1Tex, curW, curH);
    if (mousePos.x > imgPt.x) {
        state.screenState = SCREEN_STORY2;
    }
}

// ============================================================
// 绘制 & 输入：故事界面2
// ============================================================
void RenderStory2Screen(UIResource& res, int curW, int curH) {
    DrawImageCentered(res.story2Tex, curW, curH);
}

void HandleStory2ScreenInput(UIState& state) {
    state.screenState = SCREEN_MAIN;
}

