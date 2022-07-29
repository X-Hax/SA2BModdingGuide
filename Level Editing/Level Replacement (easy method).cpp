#include "pch.h" //or #include "stdafx.h

// SA2B Easy Level Import Script
// Description: A script that can import any user-given level file or texturepack automatically
//              Use the complex method if you want to add more complex features to your level.

// Create custom texlist
// Remove this if you would like to use city escape's texture pack
// And uncomment the Debug line & values on lines 38, 39 below
NJS_TEXNAME customlevel_texnames[1000]{};
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		// Get original City Escape Level
		HMODULE v0 = **datadllhandle;

		// Grab LandTable from City Escape
		LandTable* Land = (LandTable*)GetProcAddress(v0, "objLandTable0013");

		// Replace City Escape's LandTable with our own
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();

		// DEBUG: Use City Escape's Original texture pack
		// NJS_TEXLIST* texlist_landtx13 = (NJS_TEXLIST*)GetProcAddress(v0, "texlist_landtx13");

		// Set the level's textures to use our own (replace these values with the comments for city escape)
		Land->TextureList = &customlevel_texlist; // or texlist_landtx13
		Land->TextureName = (char*)"TEXTURE"; // or "LANDTX13"

		// IDK what this does lol something with blockbits
		WriteData<5>((void*)0x5DCE2D, 0x90);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
