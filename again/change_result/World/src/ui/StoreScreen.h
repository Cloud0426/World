#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void InitStoreData(UIState& state, UIResource& res);
void RenderStoreScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleStoreScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
