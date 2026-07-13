#include "SplashScreen.h"
#include "role/combatant/Combatant.h"
#include "role/enemy/EnemyManager.h"

// SubMenuItem definition (replaces removed ShopUI.h)
struct SubMenuItem {
    std::string id;
    std::string name;
    int price;
    std::string description;
};
#include "raylib.h"
#include <string>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <deque>

// raylib v6.0 header has obfuscated names (BeginDraWing), but the DLL exports correct names (BeginDrawing)
extern "C" {
    __declspec(dllimport) void BeginDrawing(void);
    __declspec(dllimport) void EndDrawing(void);
}

// ============================================================
// Design canvas: 1440x960
// ============================================================
static const float DESIGN_W = 1440.0f;
static const float DESIGN_H = 960.0f;

struct ImageButton {
    int x1, y1;
    int x2, y2;
};

// ============================================================
static Rectangle GetImageDrawRect(Texture2D tex, int screenW, int screenH) {
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

static void DrawImageCentered(Texture2D tex, int screenW, int screenH) {
    Rectangle r = GetImageDrawRect(tex, screenW, screenH);
    Rectangle srcRect = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(tex, srcRect, r, origin, 0.0f, WHITE);
}

static Vector2 DesignToScreen(float dx, float dy, Texture2D tex, int screenW, int screenH) {
    Rectangle r = GetImageDrawRect(tex, screenW, screenH);
    float texScaleX = (float)tex.width / DESIGN_W;
    float texScaleY = (float)tex.height / DESIGN_H;
    float texX = dx * texScaleX;
    float texY = dy * texScaleY;
    float scrScaleX = r.width / (float)tex.width;
    float scrScaleY = r.height / (float)tex.height;
    return { r.x + texX * scrScaleX, r.y + texY * scrScaleY };
}

static bool HitTestButton(Vector2 mousePos, Texture2D tex, int screenW, int screenH, ImageButton btn) {
    Vector2 p1 = DesignToScreen((float)btn.x1, (float)btn.y1, tex, screenW, screenH);
    Vector2 p2 = DesignToScreen((float)btn.x2, (float)btn.y2, tex, screenW, screenH);
    return (mousePos.x >= p1.x && mousePos.x <= p2.x &&
            mousePos.y >= p1.y && mousePos.y <= p2.y);
}

// ============================================================
static void DrawCoordGrid(Texture2D tex, int screenW, int screenH, Color gridColor = { 255, 255, 0, 100 }) {
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
static void DrawStamina(Font font, int stamina, int maxStamina, Texture2D tex, int screenW, int screenH) {
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
static void DrawCoins(Font font, int coins, Texture2D tex, int screenW, int screenH) {
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
static void DrawDiamonds(Font font, int diamonds, Texture2D tex, int screenW, int screenH) {
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
// Helper: hit test on screen rectangle
static bool HitTestRect(Vector2 mousePos, float rx, float ry, float rw, float rh) {
    return (mousePos.x >= rx && mousePos.x <= rx + rw &&
            mousePos.y >= ry && mousePos.y <= ry + rh);
}

// ============================================================
// Helper: draw a rounded rectangle (simple version)
static void DrawCharFileGrid(Texture2D tex, int screenW, int screenH) {
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
// 将图片按原比例缩放到 2500x1400，不足用黑色补齐
static void DrawImageFillScreen(Texture2D tex, int screenW, int screenH) {
    float imgW = (float)tex.width;
    float imgH = (float)tex.height;
    float scaleX = (float)screenW / imgW;
    float scaleY = (float)screenH / imgH;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    float drawW = imgW * scale;
    float drawH = imgH * scale;
    float posX = (screenW - drawW) / 2.0f;
    float posY = (screenH - drawH) / 2.0f;

    // 用黑色填充全屏（ClearBackground已做），再绘制图片居中
    Rectangle srcRect = { 0.0f, 0.0f, imgW, imgH };
    Rectangle dstRect = { posX, posY, drawW, drawH };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(tex, srcRect, dstRect, origin, 0.0f, WHITE);
}

// ============================================================
enum ScreenState {
    SCREEN_INIT,
    SCREEN_MAIN,
    SCREEN_BAG,
    SCREEN_TASK_LIST,
    SCREEN_STORE,
    SCREEN_MAP,
        SCREEN_STORY1,
    SCREEN_STORY2,
    SCREEN_RECRUIT,
        SCREEN_CHARACTER_FILE,
        SCREEN_MAP_SUB,      // 广播站子地图 (A区)
        SCREEN_MAP_SUB2,     // B区子地图
        SCREEN_MAP_SUB3,     // C区子地图
        SCREEN_BATTLE,       // 战斗界面
        SCREEN_FRIENDS       // 我的伙伴界面
};

// ============================================================
void ShowSplashScreen(GameManager* game) {
    if (!game) return;
    MainCharacter* player = game->getMainChar();
    Inventory* inventory = game->getInventory();
    Shop* shop = game->getShop();
    std::vector<Combatant*> fighters = game->getFighters();
    if (!player || !inventory || !shop) return;
    const int screenWidth = 2500;
    const int screenHeight = 1400;

    InitWindow(screenWidth, screenHeight, "Da Ying Xiong Shi Dai - HEROES ERA");
    SetTargetFPS(60);

        // Load font supporting CJK characters and full-width punctuation
    int codepointsCount = 0x9FFF - 0x4E00 + 1 + 0xFFEF - 0xFF00 + 1 + 128;
    int* codepoints = new int[codepointsCount];
    for (int i = 0; i < 128; i++) codepoints[i] = i;           // ASCII 0-127
    for (int i = 0; i <= 0x9FFF - 0x4E00; i++) codepoints[128 + i] = 0x4E00 + i; // CJK
    int offset = 128 + (0x9FFF - 0x4E00 + 1);
    for (int i = 0; i <= 0xFFEF - 0xFF00; i++) codepoints[offset + i] = 0xFF00 + i; // 全角标点
    Font staminaFont = LoadFontEx("resources/Typeface/simhei.ttf", 64, codepoints, codepointsCount);
    delete[] codepoints;
    SetTextureFilter(staminaFont.texture, TEXTURE_FILTER_BILINEAR);

    // Load images
    Image initImg = LoadImage("resources/interface/Initial_game_interface.png");
    Texture2D initTex = LoadTextureFromImage(initImg);
    UnloadImage(initImg);

    Image mainImg = LoadImage("resources/interface/main_interface.png");
    Texture2D mainTex = LoadTextureFromImage(mainImg);
    UnloadImage(mainImg);

    Image bagImg = LoadImage("resources/interface/bag.png");
    Texture2D bagTex = LoadTextureFromImage(bagImg);
    UnloadImage(bagImg);

    Image taskListImg = LoadImage("resources/interface/task_list.png");
    Texture2D taskListTex = LoadTextureFromImage(taskListImg);
    UnloadImage(taskListImg);

    Image storeImg = LoadImage("resources/interface/store.png");
    Texture2D storeTex = LoadTextureFromImage(storeImg);
    UnloadImage(storeImg);

    Image mapImg = LoadImage("resources/interface/map.png");
    Texture2D mapTex = LoadTextureFromImage(mapImg);
    UnloadImage(mapImg);

    Image story1Img = LoadImage("resources/interface/story1.png");
    Texture2D story1Tex = LoadTextureFromImage(story1Img);
    UnloadImage(story1Img);

    Image story2Img = LoadImage("resources/interface/story2.png");
    Texture2D story2Tex = LoadTextureFromImage(story2Img);
    UnloadImage(story2Img);

        Image recruitImg = LoadImage("resources/interface/Recruiting_teammates.png");
    Texture2D recruitTex = LoadTextureFromImage(recruitImg);
    UnloadImage(recruitImg);

        Image friendsImg = LoadImage("resources/interface/friends.png");
        Texture2D friendsTex = LoadTextureFromImage(friendsImg);
        UnloadImage(friendsImg);

        Image charFileImg = LoadImage("resources/interface/character_file.png");
    Texture2D charFileTex = LoadTextureFromImage(charFileImg);
    UnloadImage(charFileImg);

        // 加载广播站地图（直接使用 All_resources 路径）
        Image mapSubImg = LoadImage("All_resources/map/A.png");
        if (!mapSubImg.data) { Image alt = LoadImage("resources/All_resources/map/A.png"); mapSubImg = alt; }
        Texture2D mapSubTex = LoadTextureFromImage(mapSubImg);
        UnloadImage(mapSubImg);

        // 加载B区地图
        Image mapSubBImg = LoadImage("All_resources/map/B.png");
        if (!mapSubBImg.data) { Image alt = LoadImage("resources/All_resources/map/B.png"); mapSubBImg = alt; }
        Texture2D mapSubBTex = LoadTextureFromImage(mapSubBImg);
        UnloadImage(mapSubBImg);

        // 加载C区地图
        Image mapSubCImg = LoadImage("All_resources/map/C.png");
        if (!mapSubCImg.data) { Image alt = LoadImage("resources/All_resources/map/C.png"); mapSubCImg = alt; }
        Texture2D mapSubCTex = LoadTextureFromImage(mapSubCImg);
        UnloadImage(mapSubCImg);

        // 加载peace图片（在B区点击显示）
        Image peaceImg = LoadImage("All_resources/map/peace.png");
        if (!peaceImg.data) { Image alt = LoadImage("resources/All_resources/map/peace.png"); peaceImg = alt; }
        Texture2D peaceTex = LoadTextureFromImage(peaceImg);
        UnloadImage(peaceImg);

        // 加载战斗背景图片
        Image battleBgImg = LoadImage("All_resources/battle/battle_background.png");
        if (!battleBgImg.data) { Image alt = LoadImage("resources/All_resources/battle/battle_background.png"); battleBgImg = alt; }
        Texture2D battleBgTex = LoadTextureFromImage(battleBgImg);
        UnloadImage(battleBgImg);

        // 加载战斗角色图片
        Image playerImg = LoadImage("All_resources/role/machine_character/Q03.png");
        if (!playerImg.data) { Image alt = LoadImage("resources/All_resources/role/machine_character/Q03.png"); playerImg = alt; }
        Texture2D playerTex = LoadTextureFromImage(playerImg);
        UnloadImage(playerImg);

                Image enemyImg = LoadImage("All_resources/role/enemy/boss_mouse.png");
        if (!enemyImg.data) { Image alt = LoadImage("resources/All_resources/role/enemy/boss_mouse.png"); enemyImg = alt; }
        Texture2D enemyTex = LoadTextureFromImage(enemyImg);
        UnloadImage(enemyImg);

        // 加载四个技能按钮图片
        Image btnImg[4];
        const char* btnFiles[] = { "normal_attack.png", "1.png", "2.png", "big.png" };
        for (int i = 0; i < 4; i++) {
            std::string fp = std::string("All_resources/battle/Mech/mech_button/") + btnFiles[i];
            btnImg[i] = LoadImage(fp.c_str());
            if (!btnImg[i].data) { std::string alt = "resources/" + fp; btnImg[i] = LoadImage(alt.c_str()); }
        }
                Texture2D btnTex[4];
        for (int i = 0; i < 4; i++) {
            btnTex[i] = LoadTextureFromImage(btnImg[i]);
            UnloadImage(btnImg[i]);
        }

                // 加载普攻精灵动画帧 (pu文件夹10张)
        static const int PU_FRAMES = 10;
        Texture2D puFrames[PU_FRAMES];
        for (int i = 0; i < PU_FRAMES; i++) {
            char path[128];
            snprintf(path, sizeof(path), "All_resources/battle/Mech/03/pu/%03d.png", i + 1);
            Image img = LoadImage(path);
            if (!img.data) { std::string alt = "resources/" + std::string(path); img = LoadImage(alt.c_str()); }
            puFrames[i] = LoadTextureFromImage(img);
            UnloadImage(img);
        }

                // 加载四个技能的精灵动画帧（01~04文件夹，每个10帧）
        static const int SKILL_FRAMES = 10;
        static const int SKILL_COUNT = 4;
        Texture2D skillAnimFrames[SKILL_COUNT][SKILL_FRAMES];
        const char* skillAnimDirs[] = { "pu", "1", "2", "da" };
        for (int s = 0; s < SKILL_COUNT; s++) {
            for (int f = 0; f < SKILL_FRAMES; f++) {
                char path[128];
                snprintf(path, sizeof(path), "All_resources/battle/Mech/03/%s/%03d.png", skillAnimDirs[s], f + 1);
                Image img = LoadImage(path);
                if (!img.data) { std::string alt = "resources/" + std::string(path); img = LoadImage(alt.c_str()); }
                skillAnimFrames[s][f] = LoadTextureFromImage(img);
                UnloadImage(img);
            }
        }

        // 加载敌人（老鼠）攻击精灵动画帧（monster/mouse 10帧）
        static const int MOUSE_FRAMES = 10;
        Texture2D mouseFrames[MOUSE_FRAMES];
        for (int i = 0; i < MOUSE_FRAMES; i++) {
            char path[128];
            snprintf(path, sizeof(path), "All_resources/battle/monster/mouse/%03d.png", i + 1);
            Image img = LoadImage(path);
            if (!img.data) { std::string alt = "resources/" + std::string(path); img = LoadImage(alt.c_str()); }
            mouseFrames[i] = LoadTextureFromImage(img);
            UnloadImage(img);
        }

    Color hintColor = { 180, 180, 180, 200 };

    ScreenState state = SCREEN_INIT;

    // --- Buttons on design canvas (1440x960) ---
    ImageButton btnSupply   = { 1000, 800, 1200, 900 };
    ImageButton btnTaskList = { 700, 700, 900, 800 };
    ImageButton btnStore    = { 100, 400, 300, 600 };
    ImageButton btnMap      = { 800, 200, 1000, 300 };
    ImageButton btnStory    = { 600, 400, 800, 500 };
    ImageButton btnRecruit  = { 400, 500, 600, 700 };
    ImageButton btnCharFile = { 0, 0, 100, 300 };

    int stamina = 100;
    int maxStamina = 100;
    int coins = 99999;
    int diamonds = 8888;

    // Character name input on character file screen
    // Position: 竖着250-280, 横着1180-1300
    // Design coords: dx=1180~1300 (水平), dy=250~280 (垂直)
        std::string charName = "英雄";
    bool nameEditing = false;

    // ---- 商店相关状态 ----
    bool storeShowBuyDlg = false;       // 是否显示购买确认对话框
    bool storeShowSubMenu = false;      // 是否显示装备等级子菜单
    std::string storeDlgTitle;
    std::string storeDlgMsg;
    int storeBuyEntryIdx = -1;           // 购买目标商品索引
    std::string storeBuyId;              // 购买目标商品ID
    int storeBuyPrice = 0;
    std::string storeBuyCur;             // "gold" 或 "diamond"
    std::vector<SubMenuItem> storeSubItems;
    int storeSubEntryIdx = -1;

    // 商店商品条目结构
    struct StoreEntry {
        std::string id, name, cat, subDir, file;
        int price;
        std::string cur, desc;
    };
    // 商店商品数据
    std::vector<StoreEntry> storeEntries = {
        {"recruit_card","招募卡","special","","recruit_card.png",150,"diamond","消耗1张招募卡随机获得1个战斗角色"},
        {"food_001","罐头","food","food","can.png",50,"diamond","恢复体力50点"},
        {"food_002","压缩饼干","food","food","biscuit.png",20,"diamond","恢复体力30点"},
        {"food_003","瓶装水","food","food","water.png",10,"diamond","恢复体力20点"},
        {"potion_001","急救包","potion","medicine","first_aid_kit.png",500,"gold","恢复生命80点"},
        {"potion_002","外伤药包","potion","medicine","medicine_bag.png",350,"gold","恢复生命40点"},
        {"potion_003","止痛片","potion","medicine","painkiller.png",250,"gold","恢复生命25点"},
        {"weapon_s1","剑","weapon","equipment","sword.png",500,"gold","点击选择初级/中级/高级剑"},
        {"weapon_b1","魔法书","weapon","equipment","magic_book.png",500,"gold","点击选择初级/中级/高级魔法书"},
        {"weapon_h1","护盾","weapon","equipment","shield.png",500,"gold","点击选择初级/中级/高级护盾"},
        {"upgrade_stone","升级石","material","","upgrate_stone.png",200,"gold","用于战斗角色升级"},
    };
    // 加载商店商品图标
    std::vector<Texture2D> storeIcons;
    for (auto& e : storeEntries) {
        std::string fp = std::string("All_resources/shop_and_bag/item/");
        if (!e.subDir.empty()) fp += e.subDir + "/";
        fp += e.file;
        Image simg = LoadImage(fp.c_str());
        if (!simg.data) { std::string alt = "resources/" + fp; simg = LoadImage(alt.c_str()); }
        storeIcons.push_back(LoadTextureFromImage(simg));
        UnloadImage(simg);
    }
    // 子菜单辅助函数
    auto getSubMenu = [](const std::string& id) -> std::vector<SubMenuItem> {
        std::vector<SubMenuItem> r;
        if (id.find("s1") != std::string::npos) {
            r.push_back({"weapon_s1","初级剑",500,"攻击+5%"});
            r.push_back({"weapon_s2","中级剑",1000,"攻击+10%"});
            r.push_back({"weapon_s3","高级剑",1500,"攻击+15%"});
        } else if (id.find("b1") != std::string::npos) {
            r.push_back({"weapon_b1","初级魔法书",500,"蓝条+5%，生命恢复+10"});
            r.push_back({"weapon_b2","中级魔法书",1000,"蓝条+10%，生命恢复+15"});
            r.push_back({"weapon_b3","高级魔法书",1500,"蓝条+15%，生命恢复+20"});
        } else if (id.find("h1") != std::string::npos) {
            r.push_back({"weapon_h1","初级护盾",500,"防御+5%"});
            r.push_back({"weapon_h2","中级护盾",1000,"防御+10%"});
            r.push_back({"weapon_h3","高级护盾",1500,"防御+15%"});
        }
        return r;
    };
        // 存储商品点击区域
    struct StoreHitRect { float x,y,w,h; int idx; };
    std::vector<StoreHitRect> storeHitRects;

        // B区子地图 - peace图片显示标志
        bool showPeaceInB = false;
        // 记录peace图片在屏幕上的绘制区域（用于点击检测）
        Rectangle peaceScreenRect = { 0, 0, 0, 0 };
        // 战斗界面返回按钮区域
        Rectangle battleBackBtnRect = { 0, 0, 0, 0 };

                // ---- 战斗状态管理 ----
        Combatant* battlePlayer = nullptr;
        Enemy* battleEnemy = nullptr;
        bool battlePlayerTurn = true;
        bool battleFinished = false;
        bool battleVictory = false;
                                std::deque<std::string> battleLogLines; // 保留所有战斗日志
        int battleLogScrollOffset = 0; // 滚动偏移，0=最新行在最下面
        auto addBattleLog = [&](const std::string& line) {
            battleLogLines.push_back(line);
        };

                // ---- 技能精灵动画状态（通用） ----
        bool skillAnimActive = false;
        int skillAnimSkillIdx = 0;    // 0=普攻(pu), 1=小技能1(1), 2=小技能2(2), 3=大招(big)
        int skillAnimFrame = 0;       // 当前帧索引 0~9
        float skillAnimTimer = 0.0f;
        float skillAnimDuration = 0.5f; // 默认0.5秒，可根据技能单独设置
        // 存储点击时记录的伤害计算所需数据（动画结束时执行伤害）
        struct SkillAnimPayload {
            bool isCrit;
            int rawDmg;
            int finalDmg;
            char logBuf[256];
        } skillPayload;
                Vector2 skillAnimPos = {0,0};
        float skillAnimScale = 1.0f;

        // ---- 敌人（老鼠）攻击动画状态 ----
        bool mouseAnimActive = false;
        int mouseAnimFrame = 0;
        float mouseAnimTimer = 0.0f;
        float mouseAnimDuration = 1.5f;
        // 存储敌人攻击数据（动画结束时执行）
        struct MouseAnimPayload {
            int finalDmg;
            char logBuf[256];
        } mousePayload;

    while (!WindowShouldClose()) {
        int curW = GetScreenWidth();
        int curH = GetScreenHeight();
        Vector2 mousePos = GetMousePosition();

        // Pre-calculate name input and save button screen positions (for current frame)
        Rectangle charFileRect = GetImageDrawRect(charFileTex, curW, curH);
        Vector2 inputP1 = DesignToScreen(1180.0f, 250.0f, charFileTex, curW, curH);
        Vector2 inputP2 = DesignToScreen(1300.0f, 280.0f, charFileTex, curW, curH);
        float boxW = inputP2.x - inputP1.x;
        float boxH = inputP2.y - inputP1.y;
        float btnW = boxH * 1.5f;
        float btnH = boxH;
        float btnX = inputP2.x + 10;
        float btnY = inputP1.y;

        // --- Input handling ---
        if (state == SCREEN_CHARACTER_FILE && nameEditing) {
            // Get character input (GetCharPressed returns Unicode codepoint)
            int codepoint = GetCharPressed();
            while (codepoint > 0) {
                if (charName.length() < 30) {
                    // Use raylib's CodepointToUTF8 to convert codepoint to UTF-8
                    int byteCount = 0;
                    const char* utf8Bytes = CodepointToUTF8(codepoint, &byteCount);
                    for (int i = 0; i < byteCount; i++) {
                        charName += utf8Bytes[i];
                    }
                }
                codepoint = GetCharPressed();
            }

            // Backspace
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
                static int backspaceTimer = 0;
                backspaceTimer++;
                if (IsKeyPressed(KEY_BACKSPACE) || backspaceTimer % 6 == 0) {
                    if (!charName.empty()) {
                        // Remove last UTF-8 character: pop trailing continuation bytes (10xxxxxx) then lead byte
                        charName.pop_back();
                        while (!charName.empty() && ((unsigned char)charName.back() & 0xC0) == 0x80) {
                            charName.pop_back();
                        }
                    }
                }
                if (!IsKeyDown(KEY_BACKSPACE)) backspaceTimer = 0;
            }

            // Confirm with Enter
            if (IsKeyPressed(KEY_ENTER)) {
                nameEditing = false;
            }
        }

        // --- Mouse click handling ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (state == SCREEN_INIT) {
                state = SCREEN_MAIN;
            } else if (state == SCREEN_MAIN) {
                if (HitTestButton(mousePos, mainTex, curW, curH, btnSupply)) {
                    state = SCREEN_BAG;
                } else if (HitTestButton(mousePos, mainTex, curW, curH, btnTaskList)) {
                    state = SCREEN_TASK_LIST;
                } else if (HitTestButton(mousePos, mainTex, curW, curH, btnStore)) {
                    state = SCREEN_STORE;
                } else if (HitTestButton(mousePos, mainTex, curW, curH, btnMap)) {
                    state = SCREEN_MAP;
                } else if (HitTestButton(mousePos, mainTex, curW, curH, btnStory)) {
                    state = SCREEN_STORY1;
                } else if (HitTestButton(mousePos, mainTex, curW, curH, btnRecruit)) {
                    state = SCREEN_RECRUIT;
                                } else if (HitTestButton(mousePos, mainTex, curW, curH, btnCharFile)) {
                    state = SCREEN_CHARACTER_FILE;
                } else {
                    // 检测"我的伙伴"点击区域：设计坐标竖300-400，横1200-1400
                    Vector2 fr1 = DesignToScreen(1200.0f, 300.0f, mainTex, curW, curH);
                    Vector2 fr2 = DesignToScreen(1400.0f, 400.0f, mainTex, curW, curH);
                    if (HitTestRect(mousePos, fr1.x, fr1.y, fr2.x - fr1.x, fr2.y - fr1.y)) {
                        state = SCREEN_FRIENDS;
                    }
                }
                                                } else if (state == SCREEN_BAG || state == SCREEN_TASK_LIST ||
                       state == SCREEN_RECRUIT) {
                state = SCREEN_MAIN;
                                                                                                } else if (state == SCREEN_MAP) {
                // 检测广播站(A区)点击区域：设计坐标竖着200-500，横着100-500
                Vector2 a1 = DesignToScreen(100.0f, 200.0f, mapTex, curW, curH);
                Vector2 a2 = DesignToScreen(500.0f, 500.0f, mapTex, curW, curH);
                // 检测B区点击区域：设计坐标横着500-700，竖着400-700
                Vector2 b1 = DesignToScreen(500.0f, 400.0f, mapTex, curW, curH);
                Vector2 b2 = DesignToScreen(700.0f, 700.0f, mapTex, curW, curH);
                                // 检测C区点击区域：设计坐标竖着600-800，横着800-1000
                Vector2 c1 = DesignToScreen(800.0f, 600.0f, mapTex, curW, curH);
                Vector2 c2 = DesignToScreen(1000.0f, 800.0f, mapTex, curW, curH);

                if (HitTestRect(mousePos, a1.x, a1.y, a2.x - a1.x, a2.y - a1.y)) {
                    state = SCREEN_MAP_SUB;
                } else if (HitTestRect(mousePos, b1.x, b1.y, b2.x - b1.x, b2.y - b1.y)) {
                    state = SCREEN_MAP_SUB2;
                } else if (HitTestRect(mousePos, c1.x, c1.y, c2.x - c1.x, c2.y - c1.y)) {
                    state = SCREEN_MAP_SUB3;
                } else {
                    state = SCREEN_MAIN;
                }
                                                                                                } else if (state == SCREEN_MAP_SUB) {
                state = SCREEN_MAP;
                                                                                                } else if (state == SCREEN_MAP_SUB2) {
                // 检测peace点击区域（设计坐标）：竖着100-300，横着700-1100
                Vector2 peace1 = DesignToScreen(700.0f, 100.0f, mapSubBTex, curW, curH);
                Vector2 peace2 = DesignToScreen(1100.0f, 300.0f, mapSubBTex, curW, curH);
                // 如果peace图片正在显示，检测是否点击了peace图片本身
                bool hitPeacePic = false;
                if (showPeaceInB && peaceScreenRect.width > 0 && peaceScreenRect.height > 0) {
                                        if (HitTestRect(mousePos, peaceScreenRect.x, peaceScreenRect.y, peaceScreenRect.width, peaceScreenRect.height)) {
                        hitPeacePic = true;
                        state = SCREEN_BATTLE;
                                                // 初始化战斗 - 钟关白（机械学院）
                        std::vector<Combatant*> allFighters = game->getFighters();
                        if (!allFighters.empty()) {
                            battlePlayer = allFighters[0];
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
                            pDefaultFighter->setMp(0);
                            pDefaultFighter->setCritRate(0.10);
                            battlePlayer = pDefaultFighter;
                        }
                        // 获取敌人管理器并创建敌人
                        EnemyManager* em = game->getEnemyManager();
                        if (em) {
                            em->initFirstStage();
                            auto& enemies = em->getCurrentEnemies();
                            if (!enemies.empty()) {
                                battleEnemy = &enemies[0];
                            } else {
                                battleEnemy = nullptr;
                            }
                        } else {
                            battleEnemy = nullptr;
                        }
                                                battlePlayerTurn = true;
                        battleFinished = false;
                        battleVictory = false;
                        battleLogLines.clear();
                        addBattleLog("战斗开始!请选择行动");
                    }
                }
                if (!hitPeacePic) {
                    if (HitTestRect(mousePos, peace1.x, peace1.y, peace2.x - peace1.x, peace2.y - peace1.y)) {
                        // 点击peace区域（设计坐标范围），切换显示peace图片
                        showPeaceInB = !showPeaceInB;
                    } else {
                        // 点击其他区域，返回地图界面
                        state = SCREEN_MAP;
                        showPeaceInB = false;
                    }
                }
                                                                                                } else if (state == SCREEN_MAP_SUB3) {
                state = SCREEN_MAP;
                        } else if (state == SCREEN_STORE) {
                            // 商店点击处理：先检查对话框
                            bool storeClickedItem = false;
                            bool storeHitItemOrDlg = false;
                            if (storeShowBuyDlg) {
                                storeHitItemOrDlg = true;
                                int dw=500,dh=200,dx=(curW-dw)/2,dy=(curH-dh)/2;
                                int bw=120,bh=45,by=dy+130,bok=dx+dw/2-bw-15,bca=dx+dw/2+15;
                                bool okHit=HitTestRect(mousePos,(float)bok,(float)by,(float)bw,(float)bh);
                                bool caHit=HitTestRect(mousePos,(float)bca,(float)by,(float)bw,(float)bh);
                                if (okHit) {
                        // 检查库存
                        bool enough = (storeBuyCur=="gold") ? player->hasEnoughGold(storeBuyPrice) : player->hasEnoughDiamond(storeBuyPrice);
                        if (!enough) {
                            storeDlgTitle="提示";
                            storeDlgMsg=(storeBuyCur=="gold")?"金币不足":"钻石不足";
                        } else {
                            if (storeBuyCur=="gold") player->addGold(-storeBuyPrice);
                            else player->addDiamond(-storeBuyPrice);
                            // 装备类处理
                            StoreEntry& be = storeEntries[storeBuyEntryIdx];
                            if (be.cat=="weapon") {
                                Equipment eq; eq.id=storeBuyId;
                                if (storeBuyId.find("s1")!=std::string::npos){eq.name="初级剑";eq.type="weapon";eq.level=1;eq.attackBonus=0.05;}
                                else if (storeBuyId.find("s2")!=std::string::npos){eq.name="中级剑";eq.type="weapon";eq.level=2;eq.attackBonus=0.10;}
                                else if (storeBuyId.find("s3")!=std::string::npos){eq.name="高级剑";eq.type="weapon";eq.level=3;eq.attackBonus=0.15;}
                                else if (storeBuyId.find("b1")!=std::string::npos){eq.name="初级魔法书";eq.type="book";eq.level=1;eq.mpBonus=0.05;eq.hpRestore=10;}
                                else if (storeBuyId.find("b2")!=std::string::npos){eq.name="中级魔法书";eq.type="book";eq.level=2;eq.mpBonus=0.10;eq.hpRestore=15;}
                                else if (storeBuyId.find("b3")!=std::string::npos){eq.name="高级魔法书";eq.type="book";eq.level=3;eq.mpBonus=0.15;eq.hpRestore=20;}
                                else if (storeBuyId.find("h1")!=std::string::npos){eq.name="初级护盾";eq.type="shield";eq.level=1;eq.defenseBonus=0.05;}
                                else if (storeBuyId.find("h2")!=std::string::npos){eq.name="中级护盾";eq.type="shield";eq.level=2;eq.defenseBonus=0.10;}
                                else if (storeBuyId.find("h3")!=std::string::npos){eq.name="高级护盾";eq.type="shield";eq.level=3;eq.defenseBonus=0.15;}
                                bool equipped = false;
                                for (auto* f : fighters) {
                                    if (f->getEquippedCount()<2) { if (f->equipItem(eq)) { equipped=true; break; } }
                                }
                                if (!equipped) inventory->addItem(storeBuyId,1);
                            } else {
                                inventory->addItem(storeBuyId,1);
                            }
                            storeDlgTitle="购买成功";
                            storeDlgMsg="商品已添加到背包/装备";
                        }
                    } else if (caHit) {
                        storeShowBuyDlg = false;
                        storeShowSubMenu = false;
                    }
                                } else if (storeShowSubMenu) {
                    storeHitItemOrDlg = true;
                    // 子菜单点击由绘制时的循环处理
                } else {
                                    // 点击商品
                                    for (auto& hr : storeHitRects) {
                        if (HitTestRect(mousePos, hr.x, hr.y, hr.w, hr.h)) {
                            storeClickedItem = true;
                            StoreEntry& e = storeEntries[hr.idx];
                            if (e.id.find("s1")!=std::string::npos || e.id.find("b1")!=std::string::npos || e.id.find("h1")!=std::string::npos) {
                                storeSubItems = getSubMenu(e.id);
                                storeSubEntryIdx = hr.idx;
                                storeShowSubMenu = true;
                            } else {
                                storeBuyEntryIdx = hr.idx;
                                storeBuyId = e.id;
                                storeBuyPrice = e.price;
                                storeBuyCur = e.cur;
                                const char* cs = (e.cur=="gold")?"金币":"钻石";
                                char buf[128];
                                snprintf(buf,sizeof(buf),"确定购买 %s？\n价格: %d %s",e.name.c_str(),e.price,cs);
                                storeDlgTitle="购买确认";
                                storeDlgMsg=buf;
                                storeShowBuyDlg = true;
                    }
                            break;
                        }
                }
                }
                                // 点击后，如果没点到任何商品/对话框/子菜单，返回主界面
                if (!storeHitItemOrDlg && !storeClickedItem) state = SCREEN_MAIN;
                        } else if (state == SCREEN_CHARACTER_FILE) {
                // Check if clicked on name input box
                if (HitTestRect(mousePos, inputP1.x, inputP1.y, boxW, boxH)) {
                    nameEditing = true;
                }
                // Check if clicked on save button
                else if (HitTestRect(mousePos, btnX, btnY, btnW, btnH)) {
                    nameEditing = false;
                    // Save would call mainChar->setName(charName) in future
                    // For now, just show feedback
                }
                // Check if clicked on save button
                else {
                    nameEditing = false;
                    state = SCREEN_MAIN;
                }
                        } else if (state == SCREEN_FRIENDS) {
                // 检测右边"点击查看更多伙伴"按钮区域
                Vector2 btnP1 = DesignToScreen(900.0f, 0.0f, friendsTex, curW, curH);
                Vector2 btnP2 = DesignToScreen(1400.0f, 960.0f, friendsTex, curW, curH);
                float btnX = btnP1.x, btnY = btnP1.y, btnW = btnP2.x - btnP1.x, btnH = btnP2.y - btnP1.y;
                if (HitTestRect(mousePos, btnX, btnY, btnW, btnH)) {
                    state = SCREEN_RECRUIT;
                } else {
                    state = SCREEN_MAIN;
                }
                        } else if (state == SCREEN_STORY1) {
                Vector2 imgPt = DesignToScreen(800.0f, 0.0f, story1Tex, curW, curH);
                if (mousePos.x > imgPt.x) {
                    state = SCREEN_STORY2;
                }
            } else if (state == SCREEN_STORY2) {
                state = SCREEN_MAIN;
                        } else if (state == SCREEN_BATTLE) {
                // 检测是否点击了返回按钮（右上角）
                if (battleBackBtnRect.width > 0 && battleBackBtnRect.height > 0 &&
                    HitTestRect(mousePos, battleBackBtnRect.x, battleBackBtnRect.y, battleBackBtnRect.width, battleBackBtnRect.height)) {
                    state = SCREEN_MAP_SUB2;
                    showPeaceInB = false;
                    battleFinished = false;
                }
                                                                                                                                // ---- 战斗：检测四个技能按钮 1行4列（设计坐标：横300-700，竖800-900）----
                                if (!battleFinished && battlePlayer && battleEnemy && battleEnemy->isAlive() && battlePlayer->isAlive()) {
                                    // 1行4列，从左到右：普攻、小技能一、小技能二、大招
                                    float btnDWidth = 100.0f; // 设计宽度 (400/4)
                                    float btnDHeight = 100.0f; // 设计高度
                                    float baseDX = 300.0f;
                                    float baseDY = 800.0f;

                                    for (int i = 0; i < 4; i++) {
                                        float dx = baseDX + i * btnDWidth;
                                        float dy = baseDY;
                                        Vector2 p1 = DesignToScreen(dx, dy, battleBgTex, curW, curH);
                                        Vector2 p2 = DesignToScreen(dx + btnDWidth, dy + btnDHeight, battleBgTex, curW, curH);
                                        float bx = p1.x;
                                        float by = p1.y;
                                        float bw = p2.x - p1.x;
                                        float bh = p2.y - p1.y;
                                        if (HitTestRect(mousePos, bx, by, bw, bh)) {
                                                                                        // ---- 动画播放中禁止点击技能 ----
                                                                                        if (skillAnimActive || mouseAnimActive) { break; }
                                            // ---- 战斗规则 ----
                                            // 暴击：基础10%，暴击后基础攻击力变为150%
                                            // 承伤 = 2000 / (敌方防御 + 2000)
                                            // 小技能消耗50%蓝，大招消耗100%蓝
                                                                                        int requiredMp = (i == 0) ? 0 : ((i == 3) ? 100 : 50);
                                                                                                                                    if (battlePlayer->getMp() < requiredMp) {
                                                                                            addBattleLog("蓝量不足!需要 " + std::to_string(requiredMp) + " MP");
                                                                                            break;
                                                                                        }
                                                                                        battlePlayer->consumeMp(requiredMp);
                                                                                        int baseAtk = battlePlayer->getAttack();
                                                                                        double critRate = battlePlayer->getCritRate();
                                                                                        int rawDmg = 0;
                                                                                        bool isCrit = false;
                                                                                        char logBuf[256];
                                                                                        // 所有技能都使用精灵动画，延迟伤害
                                                                                        int skillMult = 100;
                                                                                        const char* skillName = "普攻";
                                                                                        double effCrit = critRate;
                                                                                        if (i == 1) {
                                                                                            skillMult = 200;
                                                                                            skillName = "爆发打击";
                                                                                        } else if (i == 2) {
                                                                                            skillMult = 120;
                                                                                            skillName = "精准打击";
                                                                                            effCrit = critRate + 0.25;
                                                                                        } else if (i == 3) {
                                                                                            skillMult = 150;
                                                                                            skillName = "火力压制";
                                                                                        }
                                                                                        isCrit = ((double)std::rand() / RAND_MAX) < effCrit;
                                                                                        int skillBase = isCrit ? (int)(baseAtk * 1.5) : baseAtk;
                                                                                        rawDmg = skillBase * skillMult / 100;
                                                                                        int tmpFinal = (int)(rawDmg * (2000.0 / (battleEnemy->getDefense() + 2000)));
                                                                                        if (tmpFinal < 1) tmpFinal = 1;
                                                                                        skillPayload.isCrit = isCrit;
                                                                                        skillPayload.rawDmg = rawDmg;
                                                                                        skillPayload.finalDmg = tmpFinal;
                                                                                        snprintf(skillPayload.logBuf, sizeof(skillPayload.logBuf), "%s%s造成 %d 伤害 (最终 %d)",
                                                                                                 skillName, isCrit ? "暴击!" : "", rawDmg, tmpFinal);
                                                                                        // 设置动画位置：玩家角色位置
                                                                                        {
                                                                                            Vector2 pp = DesignToScreen(0.0f, 500.0f, battleBgTex, curW, curH);
                                                                                            skillAnimPos = { pp.x, pp.y };
                                                                                        }
                                                                                        float targetH = (float)curH * 0.5f;
                                                                                        skillAnimScale = targetH / (float)(skillAnimFrames[i][0].height > 0 ? skillAnimFrames[i][0].height : 1);
                                                                                        skillAnimActive = true;
                                                                                        skillAnimSkillIdx = i;
                                                                                        skillAnimFrame = 0;
                                                                                        skillAnimTimer = 0.0f;
                                                                                        // 普攻1秒，小技能1.5秒，大招1.5秒
                                                                                        skillAnimDuration = (i == 0) ? 1.0f : 1.5f;
                                                                                        addBattleLog(std::string(skillName) + "!精灵动画播放中...");
                                                                                        break;
                                        }
                                    }
                                }
                                                                            }
        }

        // ---- 敌人回合（每帧检测，不在点击块内） ----
        if (state == SCREEN_BATTLE && !battlePlayerTurn && !battleFinished && !mouseAnimActive && !skillAnimActive && battlePlayer && battleEnemy && battleEnemy->isAlive() && battlePlayer->isAlive()) {
            int enemyAtk = battleEnemy->getAttack();
            int playerDef = battlePlayer->getDefense();
            int rawDmg = enemyAtk;
            if (battleEnemy->isMpFull()) {
                rawDmg = (int)(enemyAtk * 1.5);
                battleEnemy->resetMp();
            } else {
                battleEnemy->addMp(25);
                if (battleEnemy->getMp() > battleEnemy->getMaxMp())
                    battleEnemy->setMp(battleEnemy->getMaxMp());
            }
            int finalDmg = (int)(rawDmg * (2000.0 / (playerDef + 2000)));
            if (finalDmg < 1) finalDmg = 1;
            // 保存伤害数据，动画结束时执行
            mousePayload.finalDmg = finalDmg;
            snprintf(mousePayload.logBuf, sizeof(mousePayload.logBuf), "%s 造成 %d 伤害 (防御减伤后)", battleEnemy->getName().c_str(), finalDmg);
            // 触发老鼠攻击动画
            mouseAnimActive = true;
            mouseAnimFrame = 0;
            mouseAnimTimer = 0.0f;
            mouseAnimDuration = 1.5f;
            addBattleLog(std::string(battleEnemy->getName()) + "攻击!老鼠精灵动画播放中...");
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (state == SCREEN_INIT) break;
                        else if (state == SCREEN_STORE) {
                if (storeShowBuyDlg || storeShowSubMenu) {
                    storeShowBuyDlg = false;
                    storeShowSubMenu = false;
                } else {
                    state = SCREEN_MAIN;
                }
            }             else if (state == SCREEN_STORY1) state = SCREEN_MAIN;
            else if (state == SCREEN_STORY2) state = SCREEN_MAIN;
                        else if (state == SCREEN_MAP_SUB) state = SCREEN_MAP;
                        else if (state == SCREEN_MAP_SUB2) { showPeaceInB = false; state = SCREEN_MAP; }
                        else if (state == SCREEN_MAP_SUB3) state = SCREEN_MAP;
                        else if (state == SCREEN_BATTLE) { showPeaceInB = false; state = SCREEN_MAP_SUB2; }
                        else if (state == SCREEN_FRIENDS) state = SCREEN_MAIN;
                        else if (state == SCREEN_RECRUIT) state = SCREEN_MAIN;
            else if (state == SCREEN_CHARACTER_FILE) {
                nameEditing = false;
                state = SCREEN_MAIN;
            }
                                                else if (state != SCREEN_MAIN) state = SCREEN_MAIN;
        }

                // ---- 技能精灵动画帧更新 ----
                if (state == SCREEN_BATTLE && skillAnimActive) {
                    float dt = GetFrameTime();
                    skillAnimTimer += dt;
                    int totalFrames = SKILL_FRAMES;
                    float frameDuration = skillAnimDuration / (float)totalFrames;
                    skillAnimFrame = (int)(skillAnimTimer / frameDuration);
                    if (skillAnimFrame >= totalFrames) {
                        // 动画结束，执行伤害结算
                        skillAnimActive = false;
                        skillAnimFrame = 0;
                        skillAnimTimer = 0.0f;
                        // 执行之前保存的伤害
                        if (battleEnemy && battleEnemy->isAlive()) {
                            battleEnemy->takeDamage(skillPayload.finalDmg);
                            battlePlayer->restoreMp(25);
                            if (battlePlayer->getMp() > battlePlayer->getMaxMp())
                                battlePlayer->setMp(battlePlayer->getMaxMp());
                            battleEnemy->addMp(25);
                            if (battleEnemy->getMp() > battleEnemy->getMaxMp())
                                battleEnemy->setMp(battleEnemy->getMaxMp());
                            addBattleLog(std::string(skillPayload.logBuf));
                            if (!battleEnemy->isAlive()) {
                                battleFinished = true;
                                battleVictory = true;
                                addBattleLog("战斗胜利!");
                            } else {
                                battlePlayerTurn = false;
                            }
                        }
                    }
                }

                // ---- 敌人（老鼠）攻击精灵动画帧更新 ----
                if (state == SCREEN_BATTLE && mouseAnimActive) {
                    float dt = GetFrameTime();
                    mouseAnimTimer += dt;
                    int totalFrames = MOUSE_FRAMES;
                    float frameDuration = mouseAnimDuration / (float)totalFrames;
                    mouseAnimFrame = (int)(mouseAnimTimer / frameDuration);
                    if (mouseAnimFrame >= totalFrames) {
                        // 动画结束，执行伤害结算
                        mouseAnimActive = false;
                        mouseAnimFrame = 0;
                        mouseAnimTimer = 0.0f;
                        if (battlePlayer && battlePlayer->isAlive()) {
                            battlePlayer->takeDamage(mousePayload.finalDmg);
                            addBattleLog(std::string(mousePayload.logBuf));
                            if (!battlePlayer->isAlive()) {
                                battleFinished = true;
                                battleVictory = false;
                                addBattleLog("战斗失败...");
                            }
                            battlePlayerTurn = true;
                        }
                    }
                }

        BeginDrawing();
        ClearBackground(BLACK);

        if (state == SCREEN_INIT) {
            DrawImageCentered(initTex, curW, curH);
            const char* hint = "Click anywhere to enter...";
            int hintSize = 20;
            int hintW = MeasureText(hint, hintSize);
            DrawText(hint, (curW - hintW) / 2, curH - 40, hintSize, hintColor);
        } else if (state == SCREEN_MAIN) {
            DrawImageCentered(mainTex, curW, curH);
            DrawCoordGrid(mainTex, curW, curH);
            DrawStamina(staminaFont, stamina, maxStamina, mainTex, curW, curH);
            DrawCoins(staminaFont, coins, mainTex, curW, curH);
            DrawDiamonds(staminaFont, diamonds, mainTex, curW, curH);
                } else if (state == SCREEN_BAG) {
                DrawImageCentered(bagTex, curW, curH);
                // 用黄色绘制定位坐标系（调高亮度确保可见）
                DrawCoordGrid(bagTex, curW, curH, { 255, 255, 0, 220 });
        } else if (state == SCREEN_TASK_LIST) {
            DrawImageCentered(taskListTex, curW, curH);
                } else if (state == SCREEN_STORE) {
            // 绘制store背景
            DrawImageCentered(storeTex, curW, curH);
            Rectangle br = GetImageDrawRect(storeTex, curW, curH);
            storeHitRects.clear();

                        // ---- 横向中间纵向显示货币（仅数字） ----
                                    char sbuf[64];
                                    float midX = br.x + br.width * 0.5f - 40;
                                    snprintf(sbuf,sizeof(sbuf),"%d",player->getGold());
                                    DrawTextEx(staminaFont,sbuf,{midX,br.y+100},56,1,GOLD);


                                    snprintf(sbuf,sizeof(sbuf),"%d",player->getDiamond());
                                                                        DrawTextEx(staminaFont,sbuf,{midX,br.y+220},56,1,SKYBLUE);


            // ---- 招募卡：左侧大图 ----
            {
                float rx=br.x+br.width*0.06f, ry=br.y+br.height*0.25f;
                                float rw=340.0f, rh=360.0f;
                                storeHitRects.push_back({rx,ry,rw,rh,0});
                if(storeIcons[0].id>0) DrawTexturePro(storeIcons[0],{0,0,(float)storeIcons[0].width,(float)storeIcons[0].height},{rx,ry,rw,rh},{0,0},0.0f,WHITE);

                                DrawTextEx(staminaFont,"招募卡",{rx+30,ry+(float)rh+5},48,1,WHITE);
                                                DrawTextEx(staminaFont,"150钻石",{rx+25,ry+(float)rh+50},36,1,SKYBLUE);
            }

                        float is=300.0f, sx=320.0f, sy=520.0f;
                        // ---- 货架 ----
            float sl=br.x+br.width*0.28f, st=br.y+br.height*0.28f;
            int row1[]={1,2,3,4,5};
            for(int i=0;i<5;i++){
                int idx=row1[i]; if(idx>=(int)storeEntries.size()) continue;
                float ix=sl+i*sx, iy=st;
                storeHitRects.push_back({ix,iy,is,is,idx});
                auto& t=storeIcons[idx];
                if(t.id>0) DrawTexturePro(t,{0,0,(float)t.width,(float)t.height},{ix,iy,is,is},{0,0},0.0f,WHITE);
                auto& e=storeEntries[idx];
                int nw=MeasureText(e.name.c_str(),16);
                DrawTextEx(staminaFont,e.name.c_str(),{(float)ix+((float)is-MeasureTextEx(staminaFont,e.name.c_str(),36,1).x)/2,(float)iy+(float)is+5},36,1,WHITE);
                                                char pb[32];
                                                snprintf(pb,sizeof(pb),"%d",e.price);
                                                Vector2 pbsz=MeasureTextEx(staminaFont,pb,30,1);
                                                Color pc=(e.cur=="gold")?GOLD:SKYBLUE;
                                                DrawTextEx(staminaFont,pb,{(float)ix+((float)is-pbsz.x)/2,(float)iy+(float)is+48},30,1,pc);
            }

            int row2[]={6,7,8,9,10};
            float r2t = st + sy;
            for(int i=0;i<5;i++){
                int idx=row2[i]; if(idx>=(int)storeEntries.size()) continue;
                float ix=sl+i*sx, iy=r2t;
                storeHitRects.push_back({ix,iy,is,is,idx});
                auto& t=storeIcons[idx];
                if(t.id>0) DrawTexturePro(t,{0,0,(float)t.width,(float)t.height},{ix,iy,is,is},{0,0},0.0f,WHITE);
                auto& e=storeEntries[idx];
                int nw=MeasureText(e.name.c_str(),16);
                DrawTextEx(staminaFont,e.name.c_str(),{(float)ix+((float)is-MeasureTextEx(staminaFont,e.name.c_str(),36,1).x)/2,(float)iy+(float)is+5},36,1,WHITE);
                                                char pb[32];
                                                snprintf(pb,sizeof(pb),"%d",e.price);
                                                Vector2 pbsz=MeasureTextEx(staminaFont,pb,30,1);
                                                Color pc=(e.cur=="gold")?GOLD:SKYBLUE;
                                                DrawTextEx(staminaFont,pb,{(float)ix+((float)is-pbsz.x)/2,(float)iy+(float)is+48},30,1,pc);
            }

            // ---- 鼠标悬停提示 ----
            if (!storeShowBuyDlg && !storeShowSubMenu) {
                                int hi=-1;
                for(auto& hr:storeHitRects){
                    if(HitTestRect(mousePos,hr.x,hr.y,hr.w,hr.h)){hi=hr.idx;break;}
                }
                                if(hi>=0&&hi<(int)storeEntries.size()){
                    const char* tip=storeEntries[hi].desc.c_str();
                    Vector2 tsz=MeasureTextEx(staminaFont,tip,24,1);
                    float tw=tsz.x+20, th=tsz.y+12;
                    float tx=mousePos.x+15, ty=mousePos.y-10;
                    if(tx+tw>curW)tx=curW-tw-10;
                    if(ty<0)ty=0;
                    DrawRectangle((int)tx,(int)ty,(int)tw,(int)th,{20,20,40,220});
                    DrawRectangleLines((int)tx,(int)ty,(int)tw,(int)th,{200,200,240,200});

                    DrawTextEx(staminaFont,tip,{tx+8,ty+6},24,1,WHITE);
                                }
                DrawTextEx(staminaFont,"点击商品购买 | ESC返回",{br.x+20,br.y+br.height-40},18,1,GRAY);
            }

            // ---- 购买确认对话框 ----
            if (storeShowBuyDlg) {
                int dw=500,dh=220,dx=(curW-dw)/2,dy=(curH-dh)/2;
                DrawRectangle(0,0,curW,curH,{0,0,0,150});
                DrawRectangle(dx,dy,dw,dh,{40,40,60,240});
                DrawRectangleLines(dx,dy,dw,dh,{180,180,220,255});
                DrawTextEx(staminaFont,storeDlgTitle.c_str(),{(float)dx+20,(float)dy+20},28,1,WHITE);
                                DrawTextEx(staminaFont,storeDlgMsg.c_str(),{(float)dx+20,(float)dy+65},22,1,LIGHTGRAY);
                int bw=120,bh=45,by=dy+140,bok=dx+dw/2-bw-15,bca=dx+dw/2+15;
                bool okHov=HitTestRect(mousePos,(float)bok,(float)by,(float)bw,(float)bh);
                bool caHov=HitTestRect(mousePos,(float)bca,(float)by,(float)bw,(float)bh);
                DrawRectangle(bok,by,bw,bh,okHov?Color{80,200,80,255}:Color{60,180,60,230});
                DrawTextEx(staminaFont,"确定",{(float)bok+((float)bw-MeasureTextEx(staminaFont,"确定",24,1).x)/2,(float)by+((float)bh-24)/2},24,1,WHITE);
                DrawRectangle(bca,by,bw,bh,caHov?Color{200,80,80,255}:Color{180,60,60,230});
                DrawTextEx(staminaFont,"取消",{(float)bca+((float)bw-MeasureTextEx(staminaFont,"取消",24,1).x)/2,(float)by+((float)bh-24)/2},24,1,WHITE);
            }

            // ---- 装备等级子菜单 ----
            if (storeShowSubMenu) {
                int pw=420,ph=60+(int)storeSubItems.size()*55,px=(curW-pw)/2,py=(curH-ph)/2;
                DrawRectangle(0,0,curW,curH,{0,0,0,150});
                DrawRectangle(px,py,pw,ph,{30,30,50,240});
                DrawRectangleLines(px,py,pw,ph,{200,200,240,255});
                DrawText("选择等级",px+20,py+15,26,WHITE);
                int iy=py+55;
                for(size_t i=0;i<storeSubItems.size();i++){
                    auto& it=storeSubItems[i];
                    int ih=50;
                    bool hv=HitTestRect(mousePos,(float)px+10,(float)iy,(float)pw-20,(float)ih-5);
                    DrawRectangle(px+10,iy,pw-20,ih-5,hv?Color{80,80,110,240}:Color{50,50,70,200});
                    char lb[80];
                    snprintf(lb,sizeof(lb),"%s - %d金币",it.name.c_str(),it.price);
                    DrawTextEx(staminaFont,lb,{(float)px+20,(float)iy+4},20,1,WHITE);
                                        DrawTextEx(staminaFont,it.description.c_str(),{(float)px+20,(float)iy+28},16,1,LIGHTGRAY);
                    // 点击处理
                    if(hv&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        storeBuyEntryIdx = storeSubEntryIdx;
                        storeBuyId = it.id;
                        storeBuyPrice = it.price;
                        storeBuyCur = "gold";
                        char buf[128];
                        snprintf(buf,sizeof(buf),"确定购买 %s？\n价格: %d金币\n%s",it.name.c_str(),it.price,it.description.c_str());
                        storeDlgTitle="购买确认";
                        storeDlgMsg=buf;
                        storeShowSubMenu = false;
                        storeShowBuyDlg = true;
                    }
                    iy+=ih;
                }
                // 关闭按钮
                int cx=px+pw-35,cy=py+10;
                bool ch=HitTestRect(mousePos,(float)cx-5,(float)cy-5,30,30);
                DrawText("X",cx,cy,22,ch?RED:GRAY);
                if(ch&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) storeShowSubMenu = false;
            }
                                } else if (state == SCREEN_MAP) {
            DrawImageCentered(mapTex, curW, curH);
            // 紫色坐标系，方便定位
            Color purpleGrid = { 180, 0, 255, 120 };
            DrawCoordGrid(mapTex, curW, curH, purpleGrid);
                } else if (state == SCREEN_MAP_SUB) {
            ClearBackground(BLACK);
            DrawImageFillScreen(mapSubTex, curW, curH);
                } else if (state == SCREEN_MAP_SUB2) {
                    ClearBackground(BLACK);
                    DrawImageFillScreen(mapSubBTex, curW, curH);
                    // 在B区子地图上绘制黄色坐标系
                    Color yellowGrid = { 255, 255, 0, 150 };
                    DrawCoordGrid(mapSubBTex, curW, curH, yellowGrid);
                    // 如果showPeaceInB为true，在屏幕下方显示peace图片（原比例缩放）
                    if (showPeaceInB) {
                        float imgW = (float)peaceTex.width;
                        float imgH = (float)peaceTex.height;
                        float maxW = (float)curW;
                        float maxH = (float)curH * 0.3f; // 占屏幕下方30%高度
                        float scale = (maxW / imgW < maxH / imgH) ? (maxW / imgW) : (maxH / imgH);
                        float drawW = imgW * scale;
                        float drawH = imgH * scale;
                        float posX = (curW - drawW) / 2.0f;
                        float posY = curH - drawH - 10;
                        peaceScreenRect = { posX, posY, drawW, drawH };
                        Rectangle srcRect = { 0, 0, imgW, imgH };
                        Rectangle dstRect = { posX, posY, drawW, drawH };
                        Vector2 origin = { 0, 0 };
                        DrawTexturePro(peaceTex, srcRect, dstRect, origin, 0, WHITE);
                    } else {
                        peaceScreenRect = { 0, 0, 0, 0 };
                    }
                } else if (state == SCREEN_MAP_SUB3) {
            ClearBackground(BLACK);
            DrawImageFillScreen(mapSubCTex, curW, curH);
                                                                } else if (state == SCREEN_BATTLE) {
                                    // 纯黑色背景
                                    ClearBackground(BLACK);

                                    // ---- 计算玩家和敌人绘制区域 ----
                    float playerImgW = (float)playerTex.width;
                    float playerImgH = (float)playerTex.height;
                    float playerTargetH = (float)curH * 0.5f;
                    float playerScale = playerTargetH / playerImgH;
                    float playerDrawW = playerImgW * playerScale;
                    float playerDrawH = playerImgH * playerScale;
                    float playerPosX = 30.0f;
                    float playerPosY = (float)curH - playerDrawH - 20.0f;

                    float enemyImgW = (float)enemyTex.width;
                    float enemyImgH = (float)enemyTex.height;
                    float enemyTargetH = (float)curH * 0.5f;
                    float enemyScale = enemyTargetH / enemyImgH;
                    float enemyDrawW = enemyImgW * enemyScale;
                    float enemyDrawH = enemyImgH * enemyScale;
                    float enemyPosX = (float)curW - enemyDrawW - 30.0f;
                    float enemyPosY = (float)curH - enemyDrawH - 20.0f;

                                        // ---- 绘制玩家角色（左下角），动画激活时用动画帧替换 ----
                                        if (skillAnimActive) {
                                            // 动画播放中，用对应技能的动画帧替代玩家角色
                                            int si = skillAnimSkillIdx;
                                            int fi = skillAnimFrame;
                                            if (si >= 0 && si < SKILL_COUNT && fi >= 0 && fi < SKILL_FRAMES && skillAnimFrames[si][fi].id > 0) {
                                                float fw = (float)skillAnimFrames[si][fi].width * skillAnimScale;
                                                float fh = (float)skillAnimFrames[si][fi].height * skillAnimScale;
                                                Rectangle src = { 0, 0, (float)skillAnimFrames[si][fi].width, (float)skillAnimFrames[si][fi].height };
                                                Rectangle dst = { playerPosX, playerPosY, fw, fh };
                                                Vector2 origin = { 0, 0 };
                                                DrawTexturePro(skillAnimFrames[si][fi], src, dst, origin, 0.0f, WHITE);
                                            }
                                        } else {
                                            Rectangle srcRect = { 0, 0, playerImgW, playerImgH };
                                            Rectangle dstRect = { playerPosX, playerPosY, playerDrawW, playerDrawH };
                                            Vector2 origin = { 0, 0 };
                                            DrawTexturePro(playerTex, srcRect, dstRect, origin, 0, WHITE);
                                        }

                                        // ---- 绘制敌人角色（右下角），攻击动画激活时用动画帧替换 ----
                                                                                if (mouseAnimActive) {
                                                                                    int fi = mouseAnimFrame;
                                                                                    if (fi >= 0 && fi < MOUSE_FRAMES && mouseFrames[fi].id > 0) {
                                                                                        // 原比例缩放到与待机图片同宽，再扩大到1.5倍，并向左下方移动
                                                                                        float baseW = enemyDrawW;
                                                                                        float baseH = (float)mouseFrames[fi].height * (enemyDrawW / (float)mouseFrames[fi].width);
                                                                                        float fw = baseW * 1.5f;
                                                                                        float fh = baseH * 1.5f;
                                                                                        // 向右下偏移：左移 20 像素，下移 20 像素
                                                                                        float offsetX = -20.0f;
                                                                                        float offsetY = 20.0f;
                                                                                        Rectangle src = { 0, 0, (float)mouseFrames[fi].width, (float)mouseFrames[fi].height };
                                                                                        Rectangle dst = { enemyPosX + enemyDrawW - fw + offsetX, enemyPosY + enemyDrawH - fh + offsetY, fw, fh };
                                                                                        Vector2 origin = { 0, 0 };
                                                                                        DrawTexturePro(mouseFrames[fi], src, dst, origin, 0.0f, WHITE);
                                                                                    }
                                                                                } else {
                        Rectangle srcRect = { 0, 0, enemyImgW, enemyImgH };
                        Rectangle dstRect = { enemyPosX, enemyPosY, enemyDrawW, enemyDrawH };
                        Vector2 origin = { 0, 0 };
                        DrawTexturePro(enemyTex, srcRect, dstRect, origin, 0, WHITE);
                    }

                                        // ---- 玩家HP/MP条（设计坐标：横100-300，竖400-500） ----
                                        {
                                            Vector2 p1 = DesignToScreen(100.0f, 400.0f, battleBgTex, curW, curH);
                                            Vector2 p2 = DesignToScreen(300.0f, 500.0f, battleBgTex, curW, curH);

                                            if (battlePlayer) {
                            float uiX = p1.x;
                            float uiY = p1.y;
                            float boxW = p2.x - p1.x;
                            float barW = boxW * 0.9f;
                            float barH = (p2.y - p1.y) * 0.35f;

                            DrawTextEx(staminaFont, battlePlayer->getName().c_str(), { uiX + 4, uiY }, 24, 1, WHITE);
                            float curY = uiY + 28;

                                                        float hpRatio = (float)battlePlayer->getHp() / (float)battlePlayer->getMaxHp();
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * hpRatio), (int)barH, Color{ 210, 60, 60, 240 });
                            char hpText[64];
                            snprintf(hpText, sizeof(hpText), "HP: %d/%d", battlePlayer->getHp(), battlePlayer->getMaxHp());
                            DrawTextEx(staminaFont, hpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
                            curY += barH + 4;

                            float mpRatio = (float)battlePlayer->getMp() / (float)battlePlayer->getMaxMp();
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * mpRatio), (int)barH, Color{ 60, 60, 230, 240 });
                            char mpText[64];
                            snprintf(mpText, sizeof(mpText), "MP: %d/%d", battlePlayer->getMp(), battlePlayer->getMaxMp());
                            DrawTextEx(staminaFont, mpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
                        }
                    }

                                                                                                                                                                // ---- 四个技能按钮 1行4列：普攻、小技能一、小技能二、大招（设计坐标：横300-700，竖800-900）用图片原比例显示 ----
                                                                                                                                                                {
                                                                                                                                                                    Vector2 sp1 = DesignToScreen(300.0f, 800.0f, battleBgTex, curW, curH);
                                                                                                                                                                    Vector2 sp2 = DesignToScreen(700.0f, 900.0f, battleBgTex, curW, curH);

                                                                                                                                                                    if (!battleFinished && battlePlayer && battlePlayer->isAlive()) {
                            float boxX = sp1.x;
                            float boxY = sp1.y;
                            float boxW = sp2.x - sp1.x;
                            float boxH = sp2.y - sp1.y;

                            // 技能名称和描述
                            static const char* skillNames[] = { "普攻", "小技能一", "小技能二", "大招" };
                            static const char* skillDescs[] = {
                                "100%单体攻击，不消耗MP",
                                "[爆发打击] 单体200%伤害，消耗50MP",
                                "[精准打击] 暴击率+25%，全体120%伤害，消耗50MP",
                                "[火力压制] 全体150%伤害，消耗100MP"
                            };

                            // 1行4列
                            float btnW = boxW / 4.0f;
                            float btnH = boxH;

                            int hoveredIdx = -1;

                            for (int i = 0; i < 4; i++) {
                                float bx = boxX + i * btnW;
                                float by = boxY;

                                // 用对应按钮图片绘制（保持原比例，居中放置）
                                if (btnTex[i].id > 0) {
                                    float imgW = (float)btnTex[i].width;
                                    float imgH = (float)btnTex[i].height;
                                    // 在原比例下缩放到填满格子，取较小缩放比例
                                    float scale = (btnW / imgW < btnH / imgH) ? (btnW / imgW) : (btnH / imgH);
                                    float drawW = imgW * scale;
                                    float drawH = imgH * scale;
                                    float drawX = bx + (btnW - drawW) / 2.0f;
                                    float drawY = by + (btnH - drawH) / 2.0f;

                                    Rectangle src = { 0, 0, imgW, imgH };
                                    Rectangle dst = { drawX, drawY, drawW, drawH };
                                    Vector2 origin = { 0, 0 };
                                    bool hover = HitTestRect(GetMousePosition(), bx, by, btnW, btnH);
                                    if (hover) hoveredIdx = i;
                                    Color tint = hover ? WHITE : (Color){ 200, 200, 200, 255 };
                                    // 检查能否使用
                                    Skill* sk = nullptr;
                                    if (i == 0) sk = battlePlayer->getNormalAttack();
                                    else if (i == 1) sk = battlePlayer->getSkill1();
                                    else if (i == 2) sk = battlePlayer->getSkill2();
                                    else if (i == 3) sk = battlePlayer->getUltimate();
                                    bool canUse = true;
                                    if (sk && sk->mpCost > battlePlayer->getMp()) canUse = false;
                                    if (!canUse) tint = (Color){ 80, 80, 80, 200 };
                                    DrawTexturePro(btnTex[i], src, dst, origin, 0.0f, tint);
                                }
                            }

                            // ---- 鼠标悬停显示技能详情 ----
                            if (hoveredIdx >= 0) {
                                float hx = boxX + hoveredIdx * btnW;
                                float hy = boxY;
                                // 浮窗位置：在按钮上方
                                float tipW = 380.0f;
                                float tipH = 100.0f;
                                float tipX = hx + btnW / 2.0f - tipW / 2.0f;
                                float tipY = hy - tipH - 10.0f;
                                // 确保不超出左右边界
                                if (tipX < 10) tipX = 10;
                                if (tipX + tipW > curW - 10) tipX = curW - 10 - tipW;
                                if (tipY < 10) tipY = hy + btnH + 10; // 如果上方不够，放到下方

                                DrawRectangle((int)tipX, (int)tipY, (int)tipW, (int)tipH, { 20, 20, 50, 230 });
                                DrawRectangleLines((int)tipX, (int)tipY, (int)tipW, (int)tipH, { 220, 220, 255, 200 });

                                // 技能名称
                                DrawTextEx(staminaFont, skillNames[hoveredIdx], { tipX + 12, tipY + 8 }, 22, 1, GOLD);
                                // 技能描述
                                DrawTextEx(staminaFont, skillDescs[hoveredIdx], { tipX + 12, tipY + 38 }, 18, 1, WHITE);

                                // 蓝耗信息
                                int mpCost = (hoveredIdx == 0) ? 0 : ((hoveredIdx == 3) ? 100 : 50);
                                char mpBuf[64];
                                snprintf(mpBuf, sizeof(mpBuf), "MP消耗: %d  当前MP: %d/%d",
                                         mpCost, battlePlayer->getMp(), battlePlayer->getMaxMp());
                                Color mpColor = (battlePlayer->getMp() >= mpCost) ? SKYBLUE : RED;
                                DrawTextEx(staminaFont, mpBuf, { tipX + 12, tipY + 66 }, 16, 1, mpColor);
                            }
                        }
                    }

                                        // ---- 敌人HP/MP条（设计坐标：横1000-1200，竖400-500） ----
                    {
                        Vector2 ep1 = DesignToScreen(1000.0f, 400.0f, battleBgTex, curW, curH);
                        Vector2 ep2 = DesignToScreen(1200.0f, 500.0f, battleBgTex, curW, curH);

                        if (battleEnemy) {
                            float uiX = ep1.x;
                            float uiY = ep1.y;
                            float boxW = ep2.x - ep1.x;
                            float barW = boxW * 0.9f;
                            float barH = (ep2.y - ep1.y) * 0.35f;

                            DrawTextEx(staminaFont, battleEnemy->getName().c_str(), { uiX + 4, uiY }, 24, 1, RED);
                            float curY = uiY + 28;

                            float hpRatio = (float)battleEnemy->getHp() / (float)battleEnemy->getMaxHp();
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * hpRatio), (int)barH, Color{ 210, 60, 60, 240 });
                            char hpText[64];
                            snprintf(hpText, sizeof(hpText), "HP: %d/%d", battleEnemy->getHp(), battleEnemy->getMaxHp());
                            DrawTextEx(staminaFont, hpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
                            curY += barH + 4;

                                                        float mpRatio = (float)battleEnemy->getMp() / (float)battleEnemy->getMaxMp();
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)barW, (int)barH, Color{ 40, 40, 40, 220 });
                            DrawRectangle((int)(uiX + 4), (int)curY, (int)(barW * mpRatio), (int)barH, Color{ 60, 60, 230, 240 });
                            char mpText[64];
                            snprintf(mpText, sizeof(mpText), "MP: %d/%d", battleEnemy->getMp(), battleEnemy->getMaxMp());
                            DrawTextEx(staminaFont, mpText, { uiX + 6, curY + 2 }, 16, 1, WHITE);
                        }
                    }

                    // ---- 右上角返回按钮 ----
                    {
                        float btnW = 140.0f;
                        float btnH = 50.0f;
                        float btnX = (float)curW - btnW - 20.0f;
                        float btnY = 20.0f;
                        battleBackBtnRect = { btnX, btnY, btnW, btnH };
                        bool hover = HitTestRect(GetMousePosition(), btnX, btnY, btnW, btnH);
                        Color btnColor = hover ? Color{ 200, 60, 60, 240 } : Color{ 180, 50, 50, 200 };
                        DrawRectangleRounded({ btnX, btnY, btnW, btnH }, 0.3f, 8, btnColor);
                        DrawRectangleRoundedLines({ btnX, btnY, btnW, btnH }, 0.3f, 8, Color{ 255, 200, 200, 200 });
                        const char* label = "返回 B区";
                        Vector2 txtSz = MeasureTextEx(staminaFont, label, 24, 1);
                        DrawTextEx(staminaFont, label, { btnX + (btnW - txtSz.x) / 2, btnY + (btnH - txtSz.y) / 2 }, 24, 1, WHITE);
                    }

                                                            // ---- 战斗日志（右上方，可滚动，保留所有行，只显示最近5行） ----
                    {
                        float logFontSize = 22.0f;
                        float lineSpacing = 30.0f;
                        int visibleLines = 5;
                        int totalLines = (int)battleLogLines.size();
                        // 限制滚动范围
                        if (battleLogScrollOffset > totalLines - visibleLines) battleLogScrollOffset = totalLines - visibleLines;
                        if (battleLogScrollOffset < 0) battleLogScrollOffset = 0;
                        if (totalLines == 0) battleLogScrollOffset = 0;

                        // 计算日志面板宽度（取最宽行）
                        float maxW = 0;
                        for (auto& ln : battleLogLines) {
                            Vector2 sz = MeasureTextEx(staminaFont, ln.c_str(), logFontSize, 1);
                            if (sz.x > maxW) maxW = sz.x;
                        }
                        if (maxW < 300) maxW = 300;
                        maxW += 40.0f;

                        float panelH = (float)visibleLines * lineSpacing + 20.0f;
                        float lx = (float)curW - maxW - 20.0f;
                        float ly = 80.0f;

                        // 鼠标滚轮滚动
                        float wheel = GetMouseWheelMove();
                        if (wheel != 0) {
                            battleLogScrollOffset -= (int)wheel;
                            if (battleLogScrollOffset > totalLines - visibleLines) battleLogScrollOffset = totalLines - visibleLines;
                            if (battleLogScrollOffset < 0) battleLogScrollOffset = 0;
                        }

                        // 半透明背景
                        DrawRectangle((int)lx, (int)ly, (int)maxW, (int)panelH, { 0, 0, 0, 200 });
                        DrawRectangleLines((int)lx, (int)ly, (int)maxW, (int)panelH, { 200, 200, 200, 100 });

                        // 标题
                        DrawTextEx(staminaFont, "-- 战斗日志 --", { lx + 8, ly + 2 }, 20, 1, GOLD);

                        // 显示 visibleLines 行，从 battleLogScrollOffset 开始
                        int startIdx = battleLogScrollOffset;
                        int endIdx = startIdx + visibleLines;
                        if (endIdx > totalLines) endIdx = totalLines;
                        float textY = ly + 26;
                        for (int i = startIdx; i < endIdx; i++) {
                            DrawTextEx(staminaFont, battleLogLines[i].c_str(), { lx + 10, textY }, logFontSize, 1, YELLOW);
                            textY += lineSpacing;
                        }

                        // 如果有更多行，显示滚动提示
                        if (totalLines > visibleLines) {
                            char scrollHint[64];
                            snprintf(scrollHint, sizeof(scrollHint), "滚轮滚动 (%d/%d)", endIdx, totalLines);
                            DrawTextEx(staminaFont, scrollHint, { lx + 8, ly + panelH - 20 }, 14, 1, LIGHTGRAY);
                        }
                    }

                    // ---- 战斗结果 ----
                    if (battleFinished) {
                        const char* resultText = battleVictory ? "战斗胜利!" : "战斗失败...";
                        Color resultColor = battleVictory ? Color{ 255, 215, 0, 255 } : Color{ 255, 80, 80, 255 };
                        Vector2 sz = MeasureTextEx(staminaFont, resultText, 64, 1);
                        float rx = (curW - sz.x) / 2.0f;
                        float ry = (float)curH * 0.3f;
                        DrawRectangle((int)rx - 24, (int)ry - 12, (int)sz.x + 48, (int)sz.y + 24, { 0, 0, 0, 210 });
                        DrawRectangleLines((int)rx - 24, (int)ry - 12, (int)sz.x + 48, (int)sz.y + 24, { 255, 255, 200, 150 });
                        DrawTextEx(staminaFont, resultText, { rx, ry }, 64, 1, resultColor);
                        const char* tip = "点击返回按钮或按ESC退出";
                        Vector2 tipSz = MeasureTextEx(staminaFont, tip, 24, 1);
                        DrawTextEx(staminaFont, tip, { (curW - tipSz.x) / 2, ry + sz.y + 24 }, 24, 1, LIGHTGRAY);
                    }
                } else if (state == SCREEN_STORY1) {
            DrawImageCentered(story1Tex, curW, curH);
        } else if (state == SCREEN_STORY2) {
            DrawImageCentered(story2Tex, curW, curH);
                } else if (state == SCREEN_RECRUIT) {
            DrawImageCentered(recruitTex, curW, curH);
                } else if (state == SCREEN_FRIENDS) {
            DrawImageCentered(friendsTex, curW, curH);
            // 右边"点击查看更多伙伴"按钮：设计坐标 横900-1400，竖0-960（右侧区域）
            Vector2 btnP1 = DesignToScreen(900.0f, 0.0f, friendsTex, curW, curH);
            Vector2 btnP2 = DesignToScreen(1400.0f, 960.0f, friendsTex, curW, curH);
            float btnX = btnP1.x, btnY = btnP1.y, btnW = btnP2.x - btnP1.x, btnH = btnP2.y - btnP1.y;
            bool hover = HitTestRect(mousePos, btnX, btnY, btnW, btnH);
            Color btnColor = hover ? Color{ 80, 180, 80, 160 } : Color{ 60, 150, 60, 120 };
            DrawRectangleRounded({ btnX, btnY, btnW, btnH }, 0.2f, 8, btnColor);
            DrawRectangleRoundedLines({ btnX, btnY, btnW, btnH }, 0.2f, 8, Color{ 100, 255, 100, 200 });
            const char* label = "点击查看更多伙伴";
            Vector2 txtSz = MeasureTextEx(staminaFont, label, 32, 1);
            DrawTextEx(staminaFont, label, { btnX + (btnW - txtSz.x) / 2, btnY + (btnH - txtSz.y) / 2 }, 32, 1, WHITE);
            // 点击检测在鼠标点击块内处理
        } else if (state == SCREEN_CHARACTER_FILE) {
            DrawImageCentered(charFileTex, curW, curH);
            DrawCharFileGrid(charFileTex, curW, curH);

            // Calculate scale for font sizes
            Rectangle r = GetImageDrawRect(charFileTex, curW, curH);
            float scrScaleX = r.width / (float)charFileTex.width;
            float scrScaleY = r.height / (float)charFileTex.height;

            // --- Name input box ---
            Color inputBg = nameEditing ? Color{ 60, 60, 80, 220 } : Color{ 40, 40, 60, 200 };
            DrawRectangle((int)inputP1.x, (int)inputP1.y, (int)boxW, (int)boxH, inputBg);
            DrawRectangleLines((int)inputP1.x, (int)inputP1.y, (int)boxW, (int)boxH, Color{ 200, 200, 255, 220 });

            int inputFontSize = (int)(22 * ((scrScaleX + scrScaleY) / 2.0f));
            if (inputFontSize < 10) inputFontSize = 10;
            Vector2 textPos = { inputP1.x + 5, inputP1.y + (boxH - inputFontSize) / 2.0f };
            DrawTextEx(staminaFont, charName.c_str(), textPos, (float)inputFontSize, 1, WHITE);

            // Cursor blink if editing
            if (nameEditing) {
                Vector2 measured = MeasureTextEx(staminaFont, charName.c_str(), (float)inputFontSize, 1);
                float cursorX = textPos.x + measured.x + 2;
                if (cursorX < inputP2.x - 5) {
                    int cursorY1 = (int)inputP1.y + 4;
                    int cursorY2 = (int)inputP2.y - 4;
                    DrawLine((int)cursorX, cursorY1, (int)cursorX, cursorY2, Color{ 255, 255, 255, 200 });
                }
            }

            // --- Save button ---
            Color btnColor = { 80, 180, 80, 220 };
            DrawRectangle((int)btnX, (int)btnY, (int)btnW, (int)btnH, btnColor);
            DrawRectangleLines((int)btnX, (int)btnY, (int)btnW, (int)btnH, Color{ 150, 255, 150, 200 });

            int btnFontSize = (int)(20 * ((scrScaleX + scrScaleY) / 2.0f));
            if (btnFontSize < 10) btnFontSize = 10;
            const char* btnText = "保存";
            Vector2 btnTextSize = MeasureTextEx(staminaFont, btnText, (float)btnFontSize, 1);
            Vector2 btnTextPos = { btnX + (btnW - btnTextSize.x) / 2.0f, btnY + (btnH - btnTextSize.y) / 2.0f };
            DrawTextEx(staminaFont, btnText, btnTextPos, (float)btnFontSize, 1, WHITE);
        }

        EndDrawing();
    }

    UnloadFont(staminaFont);
        for (auto& t : storeIcons) if (t.id > 0) UnloadTexture(t);
    UnloadTexture(initTex);
    UnloadTexture(mainTex);
    UnloadTexture(bagTex);
    UnloadTexture(taskListTex);
    UnloadTexture(storeTex);
    UnloadTexture(mapTex);
    UnloadTexture(story1Tex);
    UnloadTexture(story2Tex);
        UnloadTexture(recruitTex);
    UnloadTexture(friendsTex);
    UnloadTexture(charFileTex);
    UnloadTexture(mapSubTex);
    UnloadTexture(mapSubBTex);
    UnloadTexture(mapSubCTex);
    UnloadTexture(peaceTex);
    UnloadTexture(battleBgTex);
        UnloadTexture(playerTex);
    UnloadTexture(enemyTex);
        for (int i = 0; i < 4; i++) if (btnTex[i].id > 0) UnloadTexture(btnTex[i]);
        for (int s = 0; s < SKILL_COUNT; s++)
        for (int f = 0; f < SKILL_FRAMES; f++)
            if (skillAnimFrames[s][f].id > 0) UnloadTexture(skillAnimFrames[s][f]);
    for (int i = 0; i < MOUSE_FRAMES; i++) if (mouseFrames[i].id > 0) UnloadTexture(mouseFrames[i]);
    CloseWindow();
}
