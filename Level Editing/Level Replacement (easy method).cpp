#include "pch.h" //or #include "stdafx.h

// Easy replacement method, we are just changing the level geometry & textures
// To replace more stuff like sky boxes, you need to find their addresses and hook them.
// Use the complex method if you want to make your level from scratch.

// Custom texlist (remove if you're using a texlist from the game)
NJS_TEXNAME customlevel_texnames[NUMBEROFTEXTURES];
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

LandTableInfo* LoadLandTable(const char* path, NJS_TEXLIST* texlist, const char* texname, const HelperFunctions& helperFunctions) {
	LandTableInfo* info = new LandTableInfo(helperFunctions.GetReplaceablePath(path));
	LandTable* landtable = info->getlandtable();

	landtable->TextureList = texlist;
	landtable->TextureName = (char*)texname;
	
	return info;
}

void LandManagerHook(int a1, LandTable* land) {
	LandTableSA2BModels = false; // the landtable uses the chunk format
	LoadLandManager(land);
}

extern "C" {
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions) {

		// Get the original landtable pointer
		HMODULE hmodule = GetModuleHandle(__TEXT("Data_DLL_orig"));
		LandTable* original_landtable = (LandTable*)GetProcAddress(hmodule, "objLandTable0013"); // "13" is City Escape

		// Replace it with our custom one
		original_landtable = LoadLandTable("resource\\gd_pc\\customlevel.sa2lvl", &customlevel_texlist, "pak_name", helperFunctions)->getlandtable();

		// Use this ONLY if your level file ends in ".sa2lvl"; the address is for City Escape only, ask me if you need another address.
		// WriteCall((void*)0x5DCDF7, LandManagerHook);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}