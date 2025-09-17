#include "pch.h"

NJS_TEXNAME customlevel_texnames[1000]{};
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HMODULE v0 = **datadllhandle;
		LandTable* Land = (LandTable*)GetProcAddress(v0, "objLandTable0013");
    
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();

		// Uncomment these to use city escape's textures
		// NJS_TEXLIST* texlist_landtx13 = (NJS_TEXLIST*)GetProcAddress(v0, "texlist_landtx13");
		// Land->TextureList = texlist_landtx13;
		// Land->TextureName = (char*)"LANDTX13";
		
		// Delete these to use city escape's textures
		Land->TextureList = &customlevel_texlist;
		Land->TextureName = (char*)"TEXTURE";

		WriteData<5>((void*)0x5DCE2D, 0x90);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
