#include "pch.h" //or #include "stdafx.h

StartPosition level_startpos = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // Singleplayer & multiplayer (SP, P1, P2) start positions & Y rotation
StartPosition level_endpos = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // Singleplayer & multiplayer (SP, P1, P2) end positions & Y rotation (where sonic does the win stance)
LevelEndPosition level_2pintro = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // P1 & P2 start positions & Y rotation in multiplayer
LevelEndPosition level_endposM23 = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // Mission 2 & 3 end positions & Y rotation (where sonic does the win stance)

void SetStartEndPoints(const HelperFunctions& helperFunctions, StartPosition* start, LevelEndPosition* start2pIntro, StartPosition* end, LevelEndPosition* missionend) {
	for (int i = 0; i <= Characters_Amy; i++) {
		if (start) {
			helperFunctions.RegisterStartPosition(i, *start);
		}

		if (start2pIntro) {
			helperFunctions.Register2PIntroPosition(i, *start2pIntro);
		}

		if (helperFunctions.Version >= 5) {
			if (end) {
				helperFunctions.RegisterEndPosition(i, *end);
			}

			if (missionend) {
				helperFunctions.RegisterMission23EndPosition(i, *missionend);
			}
		}
	}
}

extern "C" {
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions) {
		SetStartEndPoints(helperFunctions, &level_startpos, &level_2pintro, &level_endpos, &level_endposM23);
	}