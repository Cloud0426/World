#include "UIResource.h"
#include "UIShared.h"
#include <cstdio>
#include <string>

// ============================================================
// loadAll — 加载字体 + 全部图片
// ============================================================
void UIResource::loadAll() {
    // 加载字体（支持 CJK 和全角标点）
    int codepointsCount = 0x9FFF - 0x4E00 + 1 + 0xFFEF - 0xFF00 + 1 + 128;
    int* codepoints = new int[codepointsCount];
    for (int i = 0; i < 128; i++) codepoints[i] = i;                    // ASCII 0-127
    for (int i = 0; i <= 0x9FFF - 0x4E00; i++) codepoints[128 + i] = 0x4E00 + i; // CJK
    int offset = 128 + (0x9FFF - 0x4E00 + 1);
    for (int i = 0; i <= 0xFFEF - 0xFF00; i++) codepoints[offset + i] = 0xFF00 + i; // 全角标点
    staminaFont = LoadFontEx("resources/Typeface/simhei.ttf", 64, codepoints, codepointsCount);
    delete[] codepoints;
    SetTextureFilter(staminaFont.texture, TEXTURE_FILTER_BILINEAR);

    // 加载界面背景图片
    auto loadTex = [](const char* path) -> Texture2D {
        Image img = LoadImage(path);
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        return tex;
    };

    initTex     = loadTex("resources/interface/Initial_game_interface.png");
    mainTex     = loadTex("resources/interface/main_interface.png");
    bagTex      = loadTex("resources/interface/bag.png");
    taskListTex = loadTex("resources/interface/task_list.png");
    storeTex    = loadTex("resources/interface/store.png");
    mapTex      = loadTex("resources/interface/map.png");
    story1Tex   = loadTex("resources/interface/story1.png");
    story2Tex   = loadTex("resources/interface/story2.png");
    recruitTex  = loadTex("resources/interface/Recruiting_teammates.png");
    friendsTex  = loadTex("resources/interface/friends.png");
    charFileTex = loadTex("resources/interface/character_file.png");

    // 加载子地图（带 fallback 路径）
    auto loadTexWithFallback = [](const char* path, const char* fallback) -> Texture2D {
        Image img = LoadImage(path);
        if (!img.data) {
            img = LoadImage(fallback);
        }
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        return tex;
    };

    mapSubTex  = loadTexWithFallback("All_resources/map/A.png", "resources/All_resources/map/A.png");
    mapSubBTex = loadTexWithFallback("All_resources/map/B.png", "resources/All_resources/map/B.png");
    mapSubCTex = loadTexWithFallback("All_resources/map/C.png", "resources/All_resources/map/C.png");
    peaceTex   = loadTexWithFallback("All_resources/map/peace.png", "resources/All_resources/map/peace.png");

    // 战斗背景
    battleBgTex = loadTexWithFallback("All_resources/battle/battle_background.png", "resources/All_resources/battle/battle_background.png");

    // 战斗角色
    playerTex = loadTexWithFallback("All_resources/role/machine_character/Q03.png", "resources/All_resources/role/machine_character/Q03.png");
    enemyTex  = loadTexWithFallback("All_resources/role/enemy/boss_mouse.png", "resources/All_resources/role/enemy/boss_mouse.png");

    // 四个技能按钮图片
    const char* btnFiles[] = { "normal_attack.png", "1.png", "2.png", "big.png" };
    for (int i = 0; i < 4; i++) {
        std::string fp = std::string("All_resources/battle/Mech/mech_button/") + btnFiles[i];
        Image img = LoadImage(fp.c_str());
        if (!img.data) {
            std::string alt = "resources/" + fp;
            img = LoadImage(alt.c_str());
        }
        btnTex[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // 加载四个技能的精灵动画帧（每个10帧）
    const char* skillAnimDirs[] = { "pu", "1", "2", "da" };
    for (int s = 0; s < SKILL_COUNT; s++) {
        for (int f = 0; f < SKILL_FRAMES; f++) {
            char path[128];
            snprintf(path, sizeof(path), "All_resources/battle/Mech/03/%s/%03d.png", skillAnimDirs[s], f + 1);
            Image img = LoadImage(path);
            if (!img.data) {
                std::string alt = "resources/" + std::string(path);
                img = LoadImage(alt.c_str());
            }
            skillAnimFrames[s][f] = LoadTextureFromImage(img);
            UnloadImage(img);
        }
    }

    // 加载敌人（老鼠）攻击精灵动画帧
    for (int i = 0; i < MOUSE_FRAMES; i++) {
        char path[128];
        snprintf(path, sizeof(path), "All_resources/battle/monster/mouse/%03d.png", i + 1);
        Image img = LoadImage(path);
        if (!img.data) {
            std::string alt = "resources/" + std::string(path);
            img = LoadImage(alt.c_str());
        }
        mouseFrames[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

// ============================================================
// unloadAll — 卸载所有资源
// ============================================================
void UIResource::unloadAll() {
    UnloadFont(staminaFont);

    for (auto& t : storeIcons) {
        if (t.id > 0) UnloadTexture(t);
    }
    storeIcons.clear();

    auto unloadTex = [](Texture2D& t) { if (t.id > 0) UnloadTexture(t); };

    unloadTex(initTex);
    unloadTex(mainTex);
    unloadTex(bagTex);
    unloadTex(taskListTex);
    unloadTex(storeTex);
    unloadTex(mapTex);
    unloadTex(story1Tex);
    unloadTex(story2Tex);
    unloadTex(recruitTex);
    unloadTex(friendsTex);
    unloadTex(charFileTex);
    unloadTex(mapSubTex);
    unloadTex(mapSubBTex);
    unloadTex(mapSubCTex);
    unloadTex(peaceTex);
    unloadTex(battleBgTex);
    unloadTex(playerTex);
    unloadTex(enemyTex);

    for (int i = 0; i < 4; i++) unloadTex(btnTex[i]);

    for (int s = 0; s < SKILL_COUNT; s++)
        for (int f = 0; f < SKILL_FRAMES; f++)
            unloadTex(skillAnimFrames[s][f]);

    for (int i = 0; i < MOUSE_FRAMES; i++) unloadTex(mouseFrames[i]);
}
