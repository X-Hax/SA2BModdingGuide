#include "pch.h" //or #include "stdafx.h

StartPosition level_startpos = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // Singleplayer & multiplayer (SP, P1, P2) start positions & Y rotation
StartPosition level_endpos = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // Singleplayer & multiplayer (SP, P1, P2) end positions & Y rotation (where sonic does the win stance)
LevelEndPosition level_2pintro = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // P1 & P2 start positions & Y rotation in multiplayer
LevelEndPosition level_endposM23 = { LevelIDs_CityEscape, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // Mission 2 & 3 end positions & Y rotation (where sonic does the win stance)

extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		helperFunctions.RegisterStartPosition(Characters_Sonic, level_startpos);
		helperFunctions.Register2PIntroPosition(Characters_Sonic, level_2pintro);

		if (helperFunctions.Version >= 5)
		{
			helperFunctions.RegisterEndPosition(Characters_Sonic, level_endpos);
			helperFunctions.RegisterMission23EndPosition(Characters_Sonic, level_endposM23);
		}
	}
}
