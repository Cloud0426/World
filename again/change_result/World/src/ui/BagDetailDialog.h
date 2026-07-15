#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderBagDetailDialog(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleBagDetailDialogInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void RenderBagEquipSelectDialog(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void HandleBagEquipSelectDialogInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
