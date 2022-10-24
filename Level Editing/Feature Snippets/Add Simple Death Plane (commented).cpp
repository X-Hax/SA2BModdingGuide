#include "pch.h"

// Code Snippet:
//      How to add a simple death plane to your level. Will automatically
//      kill your character under a certain y-level.
//
// Notes:
// 	    Intended for use in its own mod or as a part of a mod's dll.
//	    Follow the x-hax modding tutorial for help on importing
//      dependencies.

// Level you want to apply this to.
int levelID = LevelIDs_CityEscape;
// Level you want the character to die under.
float yLevel = -50.0f;

// To do this, we're going to use and manipulate GameState.
// GameState is a built-in variable for SA2 mods that keeps
// track of what's happening in game, whether you are in a level,
// menu, loading screen, or if something happens, like a restart or quit.
extern "C" {
  // Method checks every frame.
	__declspec(dllexport) void __cdecl OnFrame() {
    // If we're in game and currently on our death plane's level.
		if (GameState == GameStates_Ingame && CurrentLevel == levelID) {
			// If the main character is under our death plane, kill them.
      if (MainCharObj1[0]->Position.y <= yLevel) {
				GameState = GameStates_NormalRestart;
			}
		}
	}
}
