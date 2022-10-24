#include "pch.h" //or #include "stdafx.h" for older versions of visual studio.

// SA2B Easy Level Import Script
// Description:
//    An advanced script that completely replaces a level, detatching it from any
//    level-tied features. Use this method to be in full control of what happens
//    in your level.
//
//    No music, skybox, events, etc will load in unless you program it in here.
//
//    Visit the x-hax discord for any questions.
//    https://discord.gg/gqJCF47
//

HelperFunctions HelperFunctionsGlobal;

LandTableInfo* CurrentLandTableInfo = nullptr;

// Create a custom texture pack (REPLACE NUMBEROFTEXTURES WITH REAL NUMBER)
NJS_TEXNAME customlevel_texnames[NUMBEROFTEXTURES];
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

void LoadLevelMusic(char* name) {
	char character;
	int c = 0;

	do {
		character = name[c];
		CurrentSongName[c++] = character;
	} while (character);

	PlayMusic(name);
	ResetMusic();
}

void LoadLandTable(const char* path, LandTableFormat format, NJS_TEXLIST* texlist, const char* texname) {
	CurrentLandTableInfo = new LandTableInfo(HelperFunctionsGlobal.GetReplaceablePath(path));
	LandTable* landtable = CurrentLandTableInfo->getlandtable();

	landtable->TextureList = texlist;
	landtable->TextureName = (char*)texname;
	LoadLandManager(landtable);

	LandTableSA2BModels = format == LandTableFormat_SA2B ? true : false;
}

void UnloadLandTable() {
	if (CurrentLandTableInfo) {
		delete CurrentLandTableInfo;
		CurrentLandTableInfo = nullptr;
		CurrentLandTable = nullptr;
	}
}

void CustomLevelInit() {
	SetDrawingPlanes(-3.0f, -65535.0f); // Draw distance (minimum, maximum), must be negative

	// SA2 loads a list of texpacks and distribute them into dozens of smaller texlists
	// Here we load City Escape texpacks (containg textures for common objects, common effects & city escape objects)
	LoadTexPacks((TexPackInfo*)0x109E810, (NJS_TEXLIST***)0x109E748);

	// This loads your level file
	LoadLandTable("resource\\gd_pc\\customlevel.sa2lvl", LandTableFormat_SA2, &customlevel_texlist, "CustomPAKName"); 

	LoadSetObject(&CityEscape_ObjectList, LoadStageSETFile(NULL, 2048)); // 2048 is the buffer, make it higher if there is a huge lot of objects in your level
	LoadStageLight("stg13_light.bin");
	LoadLevelMusic((char*)"songname.adx");
	LoadFogData_Fogtask("stg13_fog.bin", (FogData*)0x1A280C8);
	LoadStageSounds("se_ac_gf.mlt", (void*)0x8A0F60);

	//LoadStagePaths(PathList);
	//LoadDeathZones(Deathzones);

	// SA2 allows you to change what happens when rings are lost or itembox are triggered, we use the default ones here
	DropRingsFunc_ptr = DropRings;
	DisplayItemBoxItemFunc_ptr = DisplayItemBoxItem;

	// Chaos Drives to use for this level, we use the default ones here
	*(void**)0x1DE4680 = (void*)0x6B6C20;
	*(void**)0x1DE4684 = (void*)0x6BBAE0;
	*(void**)0x1DE4688 = (void*)0x6BC450;
	*(void**)0x1DE468C = (void*)0x6BC4A0;
}

void CustomLevelDelete() {
	FreeTexPacks((NJS_TEXLIST***)0x109E748, (TexPackInfo*)0x109E810);

	UnloadLandTable(); // This unloads your level when the level is exited, saving memory.

	DropRingsFunc_ptr = nullptr;
	DisplayItemBoxItemFunc_ptr = nullptr;

	*(void**)0x1DE4680 = nullptr;
	*(void**)0x1DE4684 = nullptr;
	*(void**)0x1DE4688 = nullptr;
	*(void**)0x1DE468C = nullptr;

	DeleteObject_(LandManagerPtr);
	DeleteObject_(SetObject_ptr);
}

void CustomLevelManager(ObjectMaster* obj) {
	EntityData1* data = obj->Data1->Entity;

	// Runs every frame in your level, contain a few variables you can use in "data".
}

extern "C" {
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions) {
		HelperFunctionsGlobal = helperFunctions;

		// Replace the City Escape level
		CityEscapeHeader.Init = CustomLevelInit; // Runs when the level is loading
		CityEscapeHeader.anonymous_2 = CustomLevelDelete; // Runs when the level is exited
		CityEscapeHeader.subprgmanager = CustomLevelManager; // Object that is loaded when your level loads, and destroyed when stage exit is called
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
