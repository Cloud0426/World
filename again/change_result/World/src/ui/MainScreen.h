#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderInitScreen(UIResource& res, int curW, int curH);
void HandleInitScreenInput(UIState& state);
void RenderMainScreen(UIResource& res, UIState& state, int curW, int curH);
void HandleMainScreenInput(UIResource& res, UIState& state, int curW, int curH, GameManager* game);
