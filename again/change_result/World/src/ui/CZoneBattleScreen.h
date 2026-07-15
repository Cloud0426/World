#pragma once
#include "UIResource.h"
#include "UIShared.h"

class GameManager;

void RenderCZoneBattle(UIResource& res, UIState& state, int curW, int curH);
void HandleCZoneBattleInput(UIResource& res, UIState& state, GameManager* game, int curW, int curH);
void UpdateCZoneBattleAnimations(UIState& state, float dt);
void HandleCZoneEnemyTurn(UIState& state);
void InitCZoneBattle(UIState& state, GameManager* game);
