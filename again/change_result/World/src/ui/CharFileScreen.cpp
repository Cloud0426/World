#include "SplashScreen.h"
#include "CharFileScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include <cstdio>

// ============================================================
// 绘制 & 输入：角色档案界面
// ============================================================
void RenderCharacterFileScreen(UIResource& res, UIState& state, int curW, int curH) {
                DrawImageCentered(res.charFileTex, curW, curH);

    // ── 绘制绿色坐标系（左上角原点，每两条实线间四条虚线刻度） ──
    {
        Color solidColor = GREEN;
        Color dashColor = Color{ 0, 220, 0, 100 };

        int originX = 30;
        int originY = 30;
        int axisLen = 500;

        // X轴主轴
        DrawLine(originX, originY, originX + axisLen, originY, solidColor);
        // X轴实线刻度 + 间虚线刻度
        for (int x = originX; x <= originX + axisLen; x += 100) {
            DrawLine(x, originY, x, originY + 10, solidColor);
            if (x < originX + axisLen) {
                for (int d = 1; d <= 4; d++) {
                    int dx = x + d * 20;
                    for (int ly = originY + 1; ly < originY + 10; ly += 3) {
                        DrawLine(dx, ly, dx, ly + 1, dashColor);
                    }
                }
            }
        }

        // Y轴主轴
        DrawLine(originX, originY, originX, originY + axisLen, solidColor);
        // Y轴实线刻度 + 间虚线刻度
        for (int y = originY; y <= originY + axisLen; y += 100) {
            DrawLine(originX, y, originX + 10, y, solidColor);
            if (y < originY + axisLen) {
                for (int d = 1; d <= 4; d++) {
                    int dy = y + d * 20;
                    for (int lx = originX + 1; lx < originX + 10; lx += 3) {
                        DrawLine(lx, dy, lx + 1, dy, dashColor);
                    }
                }
            }
        }

        // 轴标签
        DrawTextEx(res.staminaFont, "O", { (float)originX - 18, (float)originY + 12 }, 18, 1, GREEN);
        DrawTextEx(res.staminaFont, "X", { (float)originX + axisLen - 8, (float)originY + 6 }, 18, 1, GREEN);
        DrawTextEx(res.staminaFont, "Y", { (float)originX - 22, (float)originY + 6 }, 18, 1, GREEN);
    }

    // 计算位置
    Vector2 inputP1 = DesignToScreen(1180.0f, 250.0f, res.charFileTex, curW, curH);
    Vector2 inputP2 = DesignToScreen(1300.0f, 280.0f, res.charFileTex, curW, curH);
    float boxW = inputP2.x - inputP1.x;
    float boxH = inputP2.y - inputP1.y;
    float btnW = boxH * 1.5f;
    float btnH = boxH;
    float btnX = inputP2.x + 10;
    float btnY = inputP1.y;

    Rectangle r = GetImageDrawRect(res.charFileTex, curW, curH);
    float scrScaleX = r.width / (float)res.charFileTex.width;
    float scrScaleY = r.height / (float)res.charFileTex.height;

    // 输入框
    Color inputBg = state.nameEditing ? Color{ 60, 60, 80, 220 } : Color{ 40, 40, 60, 200 };
    DrawRectangle((int)inputP1.x, (int)inputP1.y, (int)boxW, (int)boxH, inputBg);
    DrawRectangleLines((int)inputP1.x, (int)inputP1.y, (int)boxW, (int)boxH, Color{ 200, 200, 255, 220 });

    int inputFontSize = (int)(22 * ((scrScaleX + scrScaleY) / 2.0f));
    if (inputFontSize < 10) inputFontSize = 10;
    Vector2 textPos = { inputP1.x + 5, inputP1.y + (boxH - inputFontSize) / 2.0f };
    DrawTextEx(res.staminaFont, state.charName.c_str(), textPos, (float)inputFontSize, 1, WHITE);

    if (state.nameEditing) {
        Vector2 measured = MeasureTextEx(res.staminaFont, state.charName.c_str(), (float)inputFontSize, 1);
        float cursorX = textPos.x + measured.x + 2;
        if (cursorX < inputP2.x - 5) {
            int cursorY1 = (int)inputP1.y + 4;
            int cursorY2 = (int)inputP2.y - 4;
            DrawLine((int)cursorX, cursorY1, (int)cursorX, cursorY2, Color{ 255, 255, 255, 200 });
        }
    }

    // 保存按钮
    Color btnColor = { 80, 180, 80, 220 };
    DrawRectangle((int)btnX, (int)btnY, (int)btnW, (int)btnH, btnColor);
    DrawRectangleLines((int)btnX, (int)btnY, (int)btnW, (int)btnH, Color{ 150, 255, 150, 200 });

    int btnFontSize = (int)(20 * ((scrScaleX + scrScaleY) / 2.0f));
    if (btnFontSize < 10) btnFontSize = 10;
    const char* btnText = "保存";
    Vector2 btnTextSize = MeasureTextEx(res.staminaFont, btnText, (float)btnFontSize, 1);
    Vector2 btnTextPos = { btnX + (btnW - btnTextSize.x) / 2.0f, btnY + (btnH - btnTextSize.y) / 2.0f };
    DrawTextEx(res.staminaFont, btnText, btnTextPos, (float)btnFontSize, 1, WHITE);
}

// ============================================================
// 处理角色档案界面的键盘输入
// ============================================================
void HandleCharacterFileKeyboardInput(UIResource& res, UIState& state, int curW, int curH) {
    if (state.screenState != SCREEN_CHARACTER_FILE) return;
    if (!state.nameEditing) return;

    // 获取字符输入
    int codepoint = GetCharPressed();
    while (codepoint > 0) {
        if (state.charName.length() < 30) {
            int byteCount = 0;
            const char* utf8Bytes = CodepointToUTF8(codepoint, &byteCount);
            for (int i = 0; i < byteCount; i++) {
                state.charName += utf8Bytes[i];
            }
        }
        codepoint = GetCharPressed();
    }

    // 退格
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
        static int backspaceTimer = 0;
        backspaceTimer++;
        if (IsKeyPressed(KEY_BACKSPACE) || backspaceTimer % 6 == 0) {
            if (!state.charName.empty()) {
                state.charName.pop_back();
                while (!state.charName.empty() && ((unsigned char)state.charName.back() & 0xC0) == 0x80) {
                    state.charName.pop_back();
                }
            }
        }
        if (!IsKeyDown(KEY_BACKSPACE)) backspaceTimer = 0;
    }

    // 回车确认
    if (IsKeyPressed(KEY_ENTER)) {
        state.nameEditing = false;
    }
}

// ============================================================
// 处理角色档案界面的鼠标输入
// ============================================================
void HandleCharacterFileScreenInput(UIResource& res, UIState& state, int curW, int curH) {
    Vector2 mousePos = GetMousePosition();
    Vector2 inputP1 = DesignToScreen(1180.0f, 250.0f, res.charFileTex, curW, curH);
    Vector2 inputP2 = DesignToScreen(1300.0f, 280.0f, res.charFileTex, curW, curH);
    float boxW = inputP2.x - inputP1.x;
    float boxH = inputP2.y - inputP1.y;
    float btnW = boxH * 1.5f;
    float btnH = boxH;
    float btnX = inputP2.x + 10;
    float btnY = inputP1.y;

        if (HitTestRect(mousePos, inputP1.x, inputP1.y, boxW, boxH)) {
        state.nameEditing = true;
    } else if (HitTestRect(mousePos, btnX, btnY, btnW, btnH)) {
        state.nameEditing = false;
    } else {
        state.nameEditing = false;
        state.screenState = SCREEN_MAIN;
    }
}

