#include "pch.h" //or #include "stdafx.h

//
// Code snippet:
// 	How to change level start and end position.
//
// Notes:
// 	Intended for use in its own mod or as a part of a mod's dll.
//	Follow the x-hax modding tutorial for help on importing
//      dependencies.
//

// Step 1: Create a position
NJS_VECTOR position = { 0, 0, 0 };

// Step 2: Create a 'StartPosition' object
StartPosition start_position = { LevelIDs_CityEscape, 0, 0, 0, position, position, position };
// Here's the overview of the above object:
/* { 
 *  LevelID to use (in this case 13),
 *  x y and z rotation,
 *  singleplayer start,
 *  1p multiplayer start,
 *  2p multiplayer start
 *  }
 */

// Step 3: Run the function 'RegisterStartPosition' in the Init function
extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		helperFunctions.RegisterStartPosition(Characters_Sonic, start_position);
		// Overview of above: RegisterStartPosition(Character you want, StartPosition object)
		
		// Step 4 (Optional): Set end/victory pose position
		helperFunctions.RegisterStartPosition(Characters_Sonic, start_position);
		// Note: You may want to create another StartPosition object for this
	}
}
