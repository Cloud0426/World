#include "SplashScreen.h"
#include "StoreScreen.h"
#include "UIShared.h"
#include "UIHelper.h"
#include "UIResource.h"
#include "game/GameManager.h"
#include "role/combatant/Combatant.h"
#include <cstdio>
#include <string>

// ============================================================
// 获取装备子菜单列表
// ============================================================
static std::vector<SubMenuItem> getSubMenu(const std::string& id) {
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
}

// ============================================================
// 加载商店商品图标
// ============================================================
void LoadStoreIcons(UIResource& res, std::vector<StoreEntry>& entries) {
    for (auto& e : entries) {
        std::string fp = std::string("All_resources/shop_and_bag/item/");
        if (!e.subDir.empty()) fp += e.subDir + "/";
        fp += e.file;
        Image simg = LoadImage(fp.c_str());
        if (!simg.data) {
            std::string alt = "resources/" + fp;
            simg = LoadImage(alt.c_str());
        }
        res.storeIcons.push_back(LoadTextureFromImage(simg));
        UnloadImage(simg);
    }
}

// ============================================================
// 初始化商店数据
// ============================================================
void InitStoreData(UIState& state, UIResource& res) {
    state.storeEntries = {
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
    LoadStoreIcons(res, state.storeEntries);
}

// ============================================================
// 绘制商店界面
// ============================================================
void RenderStoreScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    MainCharacter* player = game->getMainChar();
    if (!player) return;

    // 绘制store背景
    DrawImageCentered(res.storeTex, curW, curH);
    Rectangle br = GetImageDrawRect(res.storeTex, curW, curH);
    state.storeHitRects.clear();

    // ---- 横向中间纵向显示货币 ----
    char sbuf[64];
    float midX = br.x + br.width * 0.5f - 40;
    snprintf(sbuf, sizeof(sbuf), "%d", player->getGold());
    DrawTextEx(res.staminaFont, sbuf, {midX, br.y + 100}, 56, 1, GOLD);

    snprintf(sbuf, sizeof(sbuf), "%d", player->getDiamond());
    DrawTextEx(res.staminaFont, sbuf, {midX, br.y + 220}, 56, 1, SKYBLUE);

    // ---- 招募卡：左侧大图 ----
    {
        float rx = br.x + br.width * 0.06f;
        float ry = br.y + br.height * 0.25f;
        float rw = 340.0f, rh = 360.0f;
        state.storeHitRects.push_back({rx, ry, rw, rh, 0});
        if (res.storeIcons.size() > 0 && res.storeIcons[0].id > 0)
            DrawTexturePro(res.storeIcons[0], {0,0,(float)res.storeIcons[0].width,(float)res.storeIcons[0].height}, {rx,ry,rw,rh}, {0,0}, 0.0f, WHITE);

        DrawTextEx(res.staminaFont, "招募卡", {rx+30, ry+(float)rh+5}, 48, 1, WHITE);
        DrawTextEx(res.staminaFont, "150钻石", {rx+25, ry+(float)rh+50}, 36, 1, SKYBLUE);
    }

    float is = 300.0f, sx = 320.0f, sy = 520.0f;
    // ---- 货架 ----
    float sl = br.x + br.width * 0.28f, st = br.y + br.height * 0.28f;
    int row1[] = {1,2,3,4,5};
    for (int i = 0; i < 5; i++) {
        int idx = row1[i];
        if (idx >= (int)state.storeEntries.size()) continue;
        float ix = sl + i * sx, iy = st;
        state.storeHitRects.push_back({ix, iy, is, is, idx});
        if ((size_t)idx < res.storeIcons.size() && res.storeIcons[idx].id > 0)
            DrawTexturePro(res.storeIcons[idx], {0,0,(float)res.storeIcons[idx].width,(float)res.storeIcons[idx].height}, {ix,iy,is,is}, {0,0}, 0.0f, WHITE);
        auto& e = state.storeEntries[idx];
        DrawTextEx(res.staminaFont, e.name.c_str(),
                   {(float)ix+((float)is-MeasureTextEx(res.staminaFont,e.name.c_str(),36,1).x)/2, (float)iy+(float)is+5},
                   36, 1, WHITE);
        char pb[32];
        snprintf(pb, sizeof(pb), "%d", e.price);
        Vector2 pbsz = MeasureTextEx(res.staminaFont, pb, 30, 1);
        Color pc = (e.cur == "gold") ? GOLD : SKYBLUE;
        DrawTextEx(res.staminaFont, pb, {(float)ix+((float)is-pbsz.x)/2, (float)iy+(float)is+48}, 30, 1, pc);
    }

    int row2[] = {6,7,8,9,10};
    float r2t = st + sy;
    for (int i = 0; i < 5; i++) {
        int idx = row2[i];
        if (idx >= (int)state.storeEntries.size()) continue;
        float ix = sl + i * sx, iy = r2t;
        state.storeHitRects.push_back({ix, iy, is, is, idx});
        if ((size_t)idx < res.storeIcons.size() && res.storeIcons[idx].id > 0)
            DrawTexturePro(res.storeIcons[idx], {0,0,(float)res.storeIcons[idx].width,(float)res.storeIcons[idx].height}, {ix,iy,is,is}, {0,0}, 0.0f, WHITE);
        auto& e = state.storeEntries[idx];
        DrawTextEx(res.staminaFont, e.name.c_str(),
                   {(float)ix+((float)is-MeasureTextEx(res.staminaFont,e.name.c_str(),36,1).x)/2, (float)iy+(float)is+5},
                   36, 1, WHITE);
        char pb[32];
        snprintf(pb, sizeof(pb), "%d", e.price);
        Vector2 pbsz = MeasureTextEx(res.staminaFont, pb, 30, 1);
        Color pc = (e.cur == "gold") ? GOLD : SKYBLUE;
        DrawTextEx(res.staminaFont, pb, {(float)ix+((float)is-pbsz.x)/2, (float)iy+(float)is+48}, 30, 1, pc);
    }

    Vector2 mousePos = GetMousePosition();

    // ---- 鼠标悬停提示 ----
    if (!state.storeShowBuyDlg && !state.storeShowSubMenu) {
        int hi = -1;
        for (auto& hr : state.storeHitRects) {
            if (HitTestRect(mousePos, hr.x, hr.y, hr.w, hr.h)) { hi = hr.idx; break; }
        }
        if (hi >= 0 && hi < (int)state.storeEntries.size()) {
            const char* tip = state.storeEntries[hi].desc.c_str();
            Vector2 tsz = MeasureTextEx(res.staminaFont, tip, 24, 1);
            float tw = tsz.x + 20, th = tsz.y + 12;
            float tx = mousePos.x + 15, ty = mousePos.y - 10;
            if (tx + tw > curW) tx = curW - tw - 10;
            if (ty < 0) ty = 0;
            DrawRectangle((int)tx, (int)ty, (int)tw, (int)th, {20, 20, 40, 220});
            DrawRectangleLines((int)tx, (int)ty, (int)tw, (int)th, {200, 200, 240, 200});
            DrawTextEx(res.staminaFont, tip, {tx+8, ty+6}, 24, 1, WHITE);
        }
        DrawTextEx(res.staminaFont, "点击商品购买 | ESC返回", {br.x+20, br.y+br.height-40}, 18, 1, GRAY);
    }

    // ---- 购买确认对话框 ----
    if (state.storeShowBuyDlg) {
        int dw = 500, dh = 220, dx = (curW - dw) / 2, dy = (curH - dh) / 2;
        DrawRectangle(0, 0, curW, curH, {0, 0, 0, 150});
        DrawRectangle(dx, dy, dw, dh, {40, 40, 60, 240});
        DrawRectangleLines(dx, dy, dw, dh, {180, 180, 220, 255});
        DrawTextEx(res.staminaFont, state.storeDlgTitle.c_str(), {(float)dx+20, (float)dy+20}, 28, 1, WHITE);
        DrawTextEx(res.staminaFont, state.storeDlgMsg.c_str(), {(float)dx+20, (float)dy+65}, 22, 1, LIGHTGRAY);
        int bw = 120, bh = 45, by = dy + 140, bok = dx + dw/2 - bw - 15, bca = dx + dw/2 + 15;
        bool okHov = HitTestRect(mousePos, (float)bok, (float)by, (float)bw, (float)bh);
        bool caHov = HitTestRect(mousePos, (float)bca, (float)by, (float)bw, (float)bh);
        DrawRectangle(bok, by, bw, bh, okHov ? Color{80,200,80,255} : Color{60,180,60,230});
        DrawTextEx(res.staminaFont, "确定", {(float)bok+((float)bw-MeasureTextEx(res.staminaFont,"确定",24,1).x)/2, (float)by+((float)bh-24)/2}, 24, 1, WHITE);
        DrawRectangle(bca, by, bw, bh, caHov ? Color{200,80,80,255} : Color{180,60,60,230});
        DrawTextEx(res.staminaFont, "取消", {(float)bca+((float)bw-MeasureTextEx(res.staminaFont,"取消",24,1).x)/2, (float)by+((float)bh-24)/2}, 24, 1, WHITE);
    }

    // ---- 装备等级子菜单 ----
    if (state.storeShowSubMenu) {
        int pw = 420, ph = 60 + (int)state.storeSubItems.size() * 55;
        int px = (curW - pw) / 2, py = (curH - ph) / 2;
        DrawRectangle(0, 0, curW, curH, {0, 0, 0, 150});
        DrawRectangle(px, py, pw, ph, {30, 30, 50, 240});
        DrawRectangleLines(px, py, pw, ph, {200, 200, 240, 255});
        DrawText("选择等级", px+20, py+15, 26, WHITE);
        int iy = py + 55;
        for (size_t i = 0; i < state.storeSubItems.size(); i++) {
            auto& it = state.storeSubItems[i];
            int ih = 50;
            bool hv = HitTestRect(mousePos, (float)px+10, (float)iy, (float)pw-20, (float)ih-5);
            DrawRectangle(px+10, iy, pw-20, ih-5, hv ? Color{80,80,110,240} : Color{50,50,70,200});
            char lb[80];
            snprintf(lb, sizeof(lb), "%s - %d金币", it.name.c_str(), it.price);
            DrawTextEx(res.staminaFont, lb, {(float)px+20, (float)iy+4}, 20, 1, WHITE);
            DrawTextEx(res.staminaFont, it.description.c_str(), {(float)px+20, (float)iy+28}, 16, 1, LIGHTGRAY);
            // 点击处理
            if (hv && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                state.storeBuyEntryIdx = state.storeSubEntryIdx;
                state.storeBuyId = it.id;
                state.storeBuyPrice = it.price;
                state.storeBuyCur = "gold";
                                char buf[128];
                snprintf(buf, sizeof(buf), "确定购买 %s？ 价格: %d金币 (%s)", it.name.c_str(), it.price, it.description.c_str());
                state.storeDlgTitle = "购买确认";
                state.storeDlgMsg = buf;
                state.storeShowSubMenu = false;
                state.storeShowBuyDlg = true;
            }
            iy += ih;
        }
        // 关闭按钮
        int cx = px + pw - 35, cy = py + 10;
        bool ch = HitTestRect(mousePos, (float)cx-5, (float)cy-5, 30, 30);
        DrawText("X", cx, cy, 22, ch ? RED : GRAY);
        if (ch && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) state.storeShowSubMenu = false;
    }
}

// ============================================================
// 处理商店界面输入
// ============================================================
void HandleStoreScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH) {
    MainCharacter* player = game->getMainChar();
    Inventory* inventory = game->getInventory();
    std::vector<Combatant*> fighters = game->getFighters();
    if (!player || !inventory) return;

    Vector2 mousePos = GetMousePosition();

    bool storeClickedItem = false;
    bool storeHitItemOrDlg = false;

    if (state.storeShowBuyDlg) {
        storeHitItemOrDlg = true;
        int dw = 500, dh = 200, dx = (curW - dw) / 2, dy = (curH - dh) / 2;
        int bw = 120, bh = 45, by = dy + 130, bok = dx + dw/2 - bw - 15, bca = dx + dw/2 + 15;
        bool okHit = HitTestRect(mousePos, (float)bok, (float)by, (float)bw, (float)bh);
        bool caHit = HitTestRect(mousePos, (float)bca, (float)by, (float)bw, (float)bh);
        if (okHit) {
            bool enough = (state.storeBuyCur == "gold") ? player->hasEnoughGold(state.storeBuyPrice) : player->hasEnoughDiamond(state.storeBuyPrice);
            if (!enough) {
                state.storeDlgTitle = "提示";
                state.storeDlgMsg = (state.storeBuyCur == "gold") ? "金币不足" : "钻石不足";
            } else {
                if (state.storeBuyCur == "gold") player->addGold(-state.storeBuyPrice);
                else player->addDiamond(-state.storeBuyPrice);
                // 装备类处理
                StoreEntry& be = state.storeEntries[state.storeBuyEntryIdx];
                                if (be.cat == "weapon") {
                    // 🆕 改为：直接把装备放入背包，不直接装备
                    inventory->addItem(state.storeBuyId, 1);
                } else {
                    inventory->addItem(state.storeBuyId, 1);
                }
                // 同步招募卡数量（如果购买的是招募卡）
                if (state.storeBuyId == "recruit_card") {
                    state.recruitCardCount = inventory->getCount("recruit_card");
                }
                state.storeDlgTitle = "购买成功";
                state.storeDlgMsg = "商品已添加到背包/装备";
            }
        } else if (caHit) {
            state.storeShowBuyDlg = false;
            state.storeShowSubMenu = false;
        }
    } else if (state.storeShowSubMenu) {
        storeHitItemOrDlg = true;
        // 子菜单点击在绘制时处理
    } else {
        // 点击商品
        for (auto& hr : state.storeHitRects) {
            if (HitTestRect(mousePos, hr.x, hr.y, hr.w, hr.h)) {
                storeClickedItem = true;
                StoreEntry& e = state.storeEntries[hr.idx];
                if (e.id.find("s1") != std::string::npos || e.id.find("b1") != std::string::npos || e.id.find("h1") != std::string::npos) {
                    state.storeSubItems = getSubMenu(e.id);
                    state.storeSubEntryIdx = hr.idx;
                    state.storeShowSubMenu = true;
                } else {
                    state.storeBuyEntryIdx = hr.idx;
                    state.storeBuyId = e.id;
                    state.storeBuyPrice = e.price;
                    state.storeBuyCur = e.cur;
                                        const char* cs = (e.cur == "gold") ? "金币" : "钻石";
                    char buf[128];
                    snprintf(buf, sizeof(buf), "确定购买 %s？ 价格: %d %s", e.name.c_str(), e.price, cs);
                    state.storeDlgTitle = "购买确认";
                    state.storeDlgMsg = buf;
                    state.storeShowBuyDlg = true;
                }
                break;
            }
        }
    }

    // 点击空白区域返回
    if (!storeHitItemOrDlg && !storeClickedItem) {
        state.screenState = SCREEN_MAIN;
    }
}
