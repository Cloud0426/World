#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderMapScreen(UIResource& res, int curW, int curH);
void HandleMapScreenInput(UIResource& res, UIState& state, int curW, int curH);
void RenderMapSubScreen(UIResource& res, int curW, int curH);
void RenderMapSub2Screen(UIResource& res, UIState& state, int curW, int curH);
void HandleMapSub2ScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void RenderMapSub3Screen(UIResource& res, int curW, int curH);
void HandleMapSub3ScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void InitCZoneBattle(UIState& state, GameManager* game);
