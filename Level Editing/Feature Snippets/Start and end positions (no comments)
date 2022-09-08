#include "pch.h" //or #include "stdafx.h

NJS_VECTOR position = { 0, 0, 0 };
StartPosition start_position = { LevelIDs_CityEscape, 0, 0, 0, position, position, position };

extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		helperFunctions.RegisterStartPosition(Characters_Sonic, start_position);
		helperFunctions.RegisterStartPosition(Characters_Sonic, start_position);
	}
}
