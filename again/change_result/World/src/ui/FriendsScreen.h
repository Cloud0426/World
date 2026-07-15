#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderFriendsScreen(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleFriendsScreenInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
