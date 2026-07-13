#pragma once

#include "raylib.h"
#include <vector>

// ============================================================
// UI 资源管理器：封装所有纹理和字体的加载/卸载
// ============================================================
struct UIResource {
    Font staminaFont;

    // 界面背景
    Texture2D initTex;
    Texture2D mainTex;
    Texture2D bagTex;
    Texture2D taskListTex;
    Texture2D storeTex;
    Texture2D mapTex;
    Texture2D story1Tex;
    Texture2D story2Tex;
    Texture2D recruitTex;
    Texture2D friendsTex;
    Texture2D charFileTex;

    // 子地图
    Texture2D mapSubTex;    // A区
    Texture2D mapSubBTex;   // B区
    Texture2D mapSubCTex;   // C区
    Texture2D peaceTex;

    // 战斗
    Texture2D battleBgTex;
    Texture2D playerTex;
    Texture2D enemyTex;
    Texture2D btnTex[4];    // 4个技能按钮

    // 精灵动画帧
    static const int SKILL_FRAMES = 10;
    static const int SKILL_COUNT = 4;
    Texture2D skillAnimFrames[SKILL_COUNT][SKILL_FRAMES];
    static const int MOUSE_FRAMES = 10;
    Texture2D mouseFrames[MOUSE_FRAMES];

    // 商店图标
    std::vector<Texture2D> storeIcons;

    // 加载所有资源
    void loadAll();

    // 卸载所有资源
    void unloadAll();
};
