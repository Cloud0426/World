#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderTaskListScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleTaskListScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
