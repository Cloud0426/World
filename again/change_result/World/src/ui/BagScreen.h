#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderBagScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleBagScreenInput(UIResource& res, UIState& state, int curW, int curH);
