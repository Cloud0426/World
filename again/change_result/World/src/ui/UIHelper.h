#pragma once

#include "UIShared.h"

// ============================================================
// 坐标转换与绘图辅助工具
// ============================================================

// 计算图片在屏幕上的居中绘制区域（保持原比例）
Rectangle GetImageDrawRect(Texture2D tex, int screenW, int screenH);

// 在屏幕居中绘制图片（保持原比例）
void DrawImageCentered(Texture2D tex, int screenW, int screenH);

// 将设计坐标（1440x960）转换为屏幕坐标
Vector2 DesignToScreen(float dx, float dy, Texture2D tex, int screenW, int screenH);

// 检测鼠标是否点击了设计坐标系中的按钮区域
bool HitTestButton(Vector2 mousePos, Texture2D tex, int screenW, int screenH, ImageButton btn);

// 绘制设计坐标系网格（用于调试定位）
void DrawCoordGrid(Texture2D tex, int screenW, int screenH, Color gridColor);

// 绘制体力值（主界面）
void DrawStamina(Font font, int stamina, int maxStamina, Texture2D tex, int screenW, int screenH);

// 绘制金币（主界面）
void DrawCoins(Font font, int coins, Texture2D tex, int screenW, int screenH);

// 绘制钻石（主界面）
void DrawDiamonds(Font font, int diamonds, Texture2D tex, int screenW, int screenH);

// 检测鼠标点是否在矩形区域内
bool HitTestRect(Vector2 mousePos, float rx, float ry, float rw, float rh);

// 绘制角色档案界面的定位网格
void DrawCharFileGrid(Texture2D tex, int screenW, int screenH);

// 将图片按原比例缩放填充屏幕（不足用黑色补齐）
void DrawImageFillScreen(Texture2D tex, int screenW, int screenH);
