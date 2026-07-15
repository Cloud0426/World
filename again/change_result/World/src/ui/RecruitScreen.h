#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderRecruitScreen(UIResource& res, UIState& state, int curW, int curH);
void HandleRecruitScreenInput(UIState& state, GameManager* game, int curW, int curH);
