#include "pch.h"

int levelID = LevelIDs_CityEscape;
float yLevel = -50.0f;

extern "C" {
	__declspec(dllexport) void __cdecl OnFrame() {
		if (GameState == GameStates_Ingame && CurrentLevel == levelID) {
			if (MainCharObj1[0]->Position.y <= yLevel) {
				GameState = GameStates_NormalRestart;
			}
		}
	}
}
