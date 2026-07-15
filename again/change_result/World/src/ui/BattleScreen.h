#pragma once
#include "UIResource.h"
#include "UIShared.h"

void RenderBattleScreen(UIResource& res, UIState& state, int curW, int curH);
void HandleBattleScreenInput(UIResource& res, UIState& state, int curW, int curH);
void HandleEnemyTurn(UIState& state);
void UpdateBattleAnimations(UIState& state, float dt);
