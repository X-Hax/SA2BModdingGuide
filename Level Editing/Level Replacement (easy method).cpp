#include "pch.h" //or #include "stdafx.h" for older versions of visual studio

// SA2B Easy Level Import Script
// Description:
//    A simple script that allows a user to import their level
//    into Sonic Adventure 2. When creating your mod, make sure
//    to include the level file in your mod folder following
//    this structure:
//
//    MyMod/gd_PC/level.sa2blvl
//
//    Visit the x-hax discord for any questions.
//    https://discord.gg/gqJCF47
//

// Code that creates a custom texturepack, with a limit of 1000 textures.
// If you would like to use City Escape's textures, delete these two lines
// and follow the debug instructions below.
NJS_TEXNAME customlevel_texnames[1000]{};
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HMODULE v0 = **datadllhandle;

		// Grab LandTable from City Escape.
		LandTable* Land = (LandTable*)GetProcAddress(v0, "objLandTable0013");

		// Replace City Escape's LandTable with our own.
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();

		// DEBUG: Uncomment these lines to use City Escape's Original texture pack.
		// NJS_TEXLIST* texlist_landtx13 = (NJS_TEXLIST*)GetProcAddress(v0, "texlist_landtx13");
		// Land->TextureList = &texlist_landtx13;
		// Land->TextureName = (char*)"LANDTX13";
		
		// Set the level's textures to use our own. (Delete these to use city escape's textures)
		Land->TextureList = &customlevel_texlist;
		Land->TextureName = (char*)"TEXTURE";

		// (Safety feature) disables level chunk system. (whole level will be rendered)
		WriteData<5>((void*)0x5DCE2D, 0x90);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}



/*
Copyright 2022 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
