#include "UIHelper.h"
#include "UIShared.h"
#include <cstdio>
#include <cmath>

// ============================================================
Rectangle GetImageDrawRect(Texture2D tex, int screenW, int screenH) {
    float imgW = (float)tex.width;
    float imgH = (float)tex.height;
    float scaleX = (float)screenW / imgW;
    float scaleY = (float)screenH / imgH;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    float drawW = imgW * scale;
    float drawH = imgH * scale;
    float posX = (screenW - drawW) / 2.0f;
    float posY = (screenH - drawH) / 2.0f;

    return { posX, posY, drawW, drawH };
}

// ============================================================
void DrawImageCentered(Texture2D tex, int screenW, int screenH) {
    Rectangle r = GetImageDrawRect(tex, screenW, screenH);
    Rectangle srcRect = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(tex, srcRect, r, origin, 0.0f, WHITE);
}

// ============================================================
Vector2 DesignToScreen(float dx, float dy, Texture2D tex, int screenW, int screenH) {
    Rectangle r = GetImageDrawRect(tex, screenW, screenH);
    float texScaleX = (float)tex.width / DESIGN_W;
    float texScaleY = (float)tex.height / DESIGN_H;
    float texX = dx * texScaleX;
    float texY = dy * texScaleY;
    float scrScaleX = r.width / (float)tex.width;
    float scrScaleY = r.height / (float)tex.height;
    return { r.x + texX * scrScaleX, r.y + texY * scrScaleY };
}

// ============================================================
bool HitTestButton(Vector2 mousePos, Texture2D tex, int screenW, int screenH, ImageButton btn) {
    Vector2 p1 = DesignToScreen((float)btn.x1, (float)btn.y1, tex, screenW, screenH);
    Vector2 p2 = DesignToScreen((float)btn.x2, (float)btn.y2, tex, screenW, screenH);
    return (mousePos.x >= p1.x && mousePos.x <= p2.x &&
            mousePos.y >= p1.y && mousePos.y <= p2.y);
}

// ============================================================
void DrawCoordGrid(Texture2D tex, int screenW, int screenH, Color gridColor) {
    Rectangle r = GetImageDrawRect(tex, screenW, screenH);
    float texScaleX = (float)tex.width / DESIGN_W;
    float texScaleY = (float)tex.height / DESIGN_H;
    float scrScaleX = r.width / (float)tex.width;
    float scrScaleY = r.height / (float)tex.height;

    int step = 100;

    for (int x = 0; x <= (int)DESIGN_W; x += step) {
        float texX = x * texScaleX;
        float sx = r.x + texX * scrScaleX;
        DrawLine((int)sx, (int)r.y, (int)sx, (int)(r.y + r.height), gridColor);

        char label[16];
        snprintf(label, sizeof(label), "%d", x);
        int fontSize = (int)(14 * ((scrScaleX + scrScaleY) / 2.0f));
        if (fontSize < 8) fontSize = 8;
        DrawText(label, (int)sx + 2, (int)r.y + 2, fontSize, gridColor);
    }

    for (int y = 0; y <= (int)DESIGN_H; y += step) {
        float texY = y * texScaleY;
        float sy = r.y + texY * scrScaleY;
        DrawLine((int)r.x, (int)sy, (int)(r.x + r.width), (int)sy, gridColor);

        char label[16];
        snprintf(label, sizeof(label), "%d", y);
        int fontSize = (int)(14 * ((scrScaleX + scrScaleY) / 2.0f));
        if (fontSize < 8) fontSize = 8;
        DrawText(label, (int)r.x + 2, (int)sy + 2, fontSize, gridColor);
    }
}

// ============================================================
void DrawStamina(Font font, int stamina, int maxStamina, Texture2D tex, int screenW, int screenH) {
    float dx = 900.0f;
    float dy = 35.0f;
    Vector2 center = DesignToScreen(dx, dy, tex, screenW, screenH);
    float textOffsetX = -30.0f;

    float screenR = 60.0f;
    Color circleColor = { 255, 255, 0, 220 };
    DrawCircleLines((int)center.x, (int)center.y, screenR, circleColor);

    int fontSize = 36;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d/%d", stamina, maxStamina);

    Color shadowColor = { 0, 0, 0, 200 };
    Vector2 shadowPos = { center.x + 2 + textOffsetX, center.y + 2 };
    DrawTextEx(font, buf, shadowPos, (float)fontSize, 1, shadowColor);
    Vector2 textPos = { center.x + textOffsetX, center.y };
    DrawTextEx(font, buf, textPos, (float)fontSize, 1, WHITE);
}

// ============================================================
void DrawCoins(Font font, int coins, Texture2D tex, int screenW, int screenH) {
    float dx = 1100.0f;
    float dy = 35.0f;
    Vector2 pos = DesignToScreen(dx, dy, tex, screenW, screenH);
    float textOffsetX = -30.0f;

    int fontSize = 36;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", coins);

    Color shadowColor = { 0, 0, 0, 200 };
    Vector2 shadowPos = { pos.x + 2 + textOffsetX, pos.y + 2 };
    DrawTextEx(font, buf, shadowPos, (float)fontSize, 1, shadowColor);
    Vector2 textPos = { pos.x + textOffsetX, pos.y };
    DrawTextEx(font, buf, textPos, (float)fontSize, 1, WHITE);
}

// ============================================================
void DrawDiamonds(Font font, int diamonds, Texture2D tex, int screenW, int screenH) {
    float dx = 1300.0f;
    float dy = 35.0f;
    Vector2 pos = DesignToScreen(dx, dy, tex, screenW, screenH);
    float textOffsetX = -90.0f;

    int fontSize = 36;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", diamonds);

    Color shadowColor = { 0, 0, 0, 200 };
    Vector2 shadowPos = { pos.x + 2 + textOffsetX, pos.y + 2 };
    DrawTextEx(font, buf, shadowPos, (float)fontSize, 1, shadowColor);
    Vector2 textPos = { pos.x + textOffsetX, pos.y };
    DrawTextEx(font, buf, textPos, (float)fontSize, 1, WHITE);
}

// ============================================================
bool HitTestRect(Vector2 mousePos, float rx, float ry, float rw, float rh) {
    return (mousePos.x >= rx && mousePos.x <= rx + rw &&
            mousePos.y >= ry && mousePos.y <= ry + rh);
}

// ============================================================
void DrawCharFileGrid(Texture2D tex, int screenW, int screenH) {
    Rectangle r = GetImageDrawRect(tex, screenW, screenH);
    float texScaleX = (float)tex.width / DESIGN_W;
    float texScaleY = (float)tex.height / DESIGN_H;
    float scrScaleX = r.width / (float)tex.width;
    float scrScaleY = r.height / (float)tex.height;

    Color gridColor = { 0, 255, 0, 120 };
    Color purpleColor = { 180, 0, 255, 150 };
    int step = 100;

    for (int x = 0; x <= (int)DESIGN_W; x += step) {
        float texX = x * texScaleX;
        float sx = r.x + texX * scrScaleX;
        Color lineColor = (x > 800) ? purpleColor : gridColor;
        DrawLine((int)sx, (int)r.y, (int)sx, (int)(r.y + r.height), lineColor);

        char label[16];
        snprintf(label, sizeof(label), "%d", x);
        int fontSize = (int)(14 * ((scrScaleX + scrScaleY) / 2.0f));
        if (fontSize < 8) fontSize = 8;
        DrawText(label, (int)sx + 2, (int)r.y + 2, fontSize, gridColor);
    }

    for (int y = 0; y <= (int)DESIGN_H; y += step) {
        float texY = y * texScaleY;
        float sy = r.y + texY * scrScaleY;
        float splitX = r.x + (float)tex.width * scrScaleX * (800.0f / DESIGN_W);
        DrawLine((int)r.x, (int)sy, (int)splitX, (int)sy, gridColor);
        DrawLine((int)splitX, (int)sy, (int)(r.x + r.width), (int)sy, purpleColor);
    }
}

// ============================================================
void DrawImageFillScreen(Texture2D tex, int screenW, int screenH) {
    float imgW = (float)tex.width;
    float imgH = (float)tex.height;
    float scaleX = (float)screenW / imgW;
    float scaleY = (float)screenH / imgH;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    float drawW = imgW * scale;
    float drawH = imgH * scale;
    float posX = (screenW - drawW) / 2.0f;
    float posY = (screenH - drawH) / 2.0f;

    Rectangle srcRect = { 0.0f, 0.0f, imgW, imgH };
    Rectangle dstRect = { posX, posY, drawW, drawH };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(tex, srcRect, dstRect, origin, 0.0f, WHITE);
}
