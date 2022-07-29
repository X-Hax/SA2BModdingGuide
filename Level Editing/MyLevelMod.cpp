#include "pch.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>

// SA2B Level Import Script
// Description: A script that can import any user-given level file or texturepack automatically
// If you would like to build this for yourself, make sure to include "LandTableInfo.cpp" and other
// SA2 mod libraries included with SATools

// Debug messages go straight to SA2 Mod Loader debug txt file, and show when the mods are loaded

// Create custom texlist
NJS_TEXNAME customlevel_texnames[1000]{}; // 126 by default
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

extern "C"
{
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions)
	{
		// Move files to correct positions
		const std::string oldTexturePath = std::string(path) + "\\texture.pak";
		const std::string newTexturePath = std::string(path) + "\\gd_PC\\PRS\\texture.pak";

		const std::string oldLevelPath = std::string(path) + "\\level.sa2blvl";
		const std::string newLevelPath = std::string(path) + "\\gd_PC\\level.sa2blvl";

		try {
			if (std::rename(oldTexturePath.c_str(), newTexturePath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved texture pack file.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving texture pack file. Maybe its in the right place already?");
			}

			if (std::rename(oldLevelPath.c_str(), newLevelPath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved level file.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving level file. Maybe its in the right place already?");
			}
		}
		catch (const std::exception& e) // reference to the base of a polymorphic object
		{
			PrintDebug(e.what()); // information from length_error printed
		}

		// Get original City Escape Level
		HMODULE v0 = **datadllhandle;

		// Grab LandTable from City Escape
		LandTable* Land = (LandTable*)GetProcAddress(v0, "objLandTable0013");

		// Replace City Escape's LandTable with our own
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();

		// DEBUG: City Escape Original texture pack
		// NJS_TEXLIST* texlist_landtx13 = (NJS_TEXLIST*)GetProcAddress(v0, "texlist_landtx13");

		if (Land) {
			// Set the level's textures to use our own
			Land->TextureList = &customlevel_texlist; // or texlist_landtx13
			Land->TextureName = (char*)"TEXTURE"; // or "LANDTX13"

			// IDK what this does lol something with blockbits
			WriteData<5>((void*)0x5DCE2D, 0x90);

			// Read Start / End positions from file
			std::ifstream infile;
			infile.open(std::string(path) + "\\level_options.ini", std::ios::in);
			if (!infile.is_open()) {
				PrintDebug("[My Level Mod] Error reading from level options (is it missing?)");
			}
			else {
				PrintDebug("[My Level Mod] Reading from level_options...");

				std::string line, key, value;
				while (std::getline(infile, line, '='))
				{
					std::string debug = "[My Level Mod] " + line;
					PrintDebug(debug.c_str());

					if (line == "Start" || line == "End") {
						boolean start = line == "Start" ? true : false;

						std::getline(infile, line);
						std::istringstream ss(line);

						std::string debug = "[My Level Mod] " + line;
						PrintDebug(debug.c_str());

						float coords[3]{};
						std::string temp;
						for (int i = 0; i < 3; i++) {
							std::getline(ss, temp, ',');
							coords[i] = std::stof(temp);
						}

						debug = "[My Level Mod] coordinates caught! " + std::to_string(coords[0]) + ", " + std::to_string(coords[1]) + ", " + std::to_string(coords[2]);
						PrintDebug(debug.c_str());

						// 0x4000 == 90 degrees
						if (start) {
							PrintDebug("[My Level Mod] Setting Start Location!");
							StartPosition startpos = { LevelIDs_CityEscape, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
							helperFunctions.ClearStartPositionList(Characters_Sonic);
							helperFunctions.RegisterStartPosition(Characters_Sonic, startpos);
						}
						else {
							PrintDebug("[My Level Mod] Setting End Location!");
							StartPosition endpos = { LevelIDs_CityEscape, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
							helperFunctions.ClearEndPositionList(Characters_Sonic);
							helperFunctions.RegisterEndPosition(Characters_Sonic, endpos);
						}
					}
				}

				PrintDebug("[My Level Mod] Done reading from level_options");

				infile.close();
			}
		}
		else {
			// Print debugs if there was an error loading the level
			PrintDebug("[My Level Mod] Land Table not found.");
			PrintDebug("[My Level Mod] Please make sure you're creating the lvl file properly.");
		}
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

