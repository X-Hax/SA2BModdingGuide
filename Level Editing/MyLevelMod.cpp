#include "pch.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <filesystem> 

// MyLevelMod.cpp
// Description:
//    A Script that can import your mod into Sonic Adventure 2,
//    including the feature of being able to read from a user-inputted
//    "level_options.ini" file for simple level customizations.
//
//    If you have any questions, feel free to ask at the x-hax discord.
//    https://discord.gg/gqJCF47
//

// Create a custom texture pack with a texture limit of 1000.
NJS_TEXNAME customlevel_texnames[1000]{};
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

std::string levelID = "objLandTable0013";

// Detect incorrect file structure, and fix if necessary.
void fixFileStructure(const char* path) {
	const std::string badTexturePath1 = std::string(path) + "\\texture.pak";
	const std::string badTexturePath2 = std::string(path) + "\\gd_PC\\texture.pak";
	const std::string newTexturePath = std::string(path) + "\\gd_PC\\PRS\\texture.pak";

	const std::string badLevelPath = std::string(path) + "\\level.sa2blvl";
	const std::string newLevelPath = std::string(path) + "\\gd_PC\\level.sa2blvl";

	try {
		if (std::filesystem::exists(badTexturePath1)) {
			PrintDebug("[My Level Mod] Incorrect texture pack position detected.");

			if (std::rename(badTexturePath1.c_str(), newTexturePath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved texture pack file.");
				PrintDebug("[My Level Mod] Expect a game crash, please run game again.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving texture pack file to right position.");
				PrintDebug("[My Level Mod] (Warning) Texture pack file should be at (\\gd_PC\\PRS\\texture.pak).");
			}
		}

		if (std::filesystem::exists(badTexturePath2)) {
			PrintDebug("[My Level Mod] Incorrect texture pack position detected.");

			if (std::rename(badTexturePath2.c_str(), newTexturePath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved texture pack file.");
				PrintDebug("[My Level Mod] Expect a game crash, please run game again.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving texture pack file to right position.");
				PrintDebug("[My Level Mod] (Warning) Texture pack file should be at (\\gd_PC\\PRS\\texture.pak).");
			}
		}

		if (std::filesystem::exists(badLevelPath)) {
			PrintDebug("[My Level Mod] Incorrect level file position detected.");

			if (std::rename(badLevelPath.c_str(), newTexturePath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved level file.");
				PrintDebug("[My Level Mod] Expect a game crash, please run game again.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving level file to right position.");
				PrintDebug("[My Level Mod] (Warning) Level file should be at (\\gd_PC\\level.sa2blvl).");
			}
		}
	}
	// Print debug information in case of error.
	catch (const std::exception& e)
	{
		PrintDebug(e.what());
	}
}

// Read from MyLevelMod ini.
// Used to get level information from user input.
void readIniOptions(const char* path, const HelperFunctions& helperFunctions) {
	std::ifstream infile;
	infile.open(std::string(path) + "\\level_options.ini", std::ios::in);
	if (!infile.is_open()) {
		PrintDebug("[My Level Mod] Error reading from level options (is it missing?)");
	}
	else {
		PrintDebug("[My Level Mod] Reading from level_options...");

		std::string line, key, value;
		while (std::getline(infile, line, '=')) {
			// Read start and End location.
			if (line == "Start" || line == "End") {
				boolean start = line == "Start" ? true : false;

				std::getline(infile, line);
				std::istringstream ss(line);

				// Get x, y, and z coordinates.
				float coords[3]{};
				try {
					std::string temp;
					for (int i = 0; i < 3; i++) {
						std::getline(ss, temp, ',');
						coords[i] = std::stof(temp);
					}
				}
				// Print debug information in case of error.
				catch (const std::exception& e) {
					PrintDebug("[My Level Mod] Error reading start position.");
					PrintDebug(e.what());

					PrintDebug("[My Level Mod] Defaulting to 0, 0, 0.");
					coords[0] = 0;
					coords[1] = 0;
					coords[2] = 0;
				}

				// 0x4000 == 90 degrees
				if (start) {
					PrintDebug("[My Level Mod] Start coordinates detected.");
					StartPosition startpos = { LevelIDs_CityEscape, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
					helperFunctions.ClearStartPositionList(Characters_Sonic);
					helperFunctions.RegisterStartPosition(Characters_Sonic, startpos);
				}
				else {
					PrintDebug("[My Level Mod] End coordinates detected.");
					StartPosition endpos = { LevelIDs_CityEscape, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
					helperFunctions.ClearEndPositionList(Characters_Sonic);
					helperFunctions.RegisterEndPosition(Characters_Sonic, endpos);
				}

				std:: string debug = "Setting new coordinates to: " + std::to_string(coords[0]) + ", " + std::to_string(coords[1]) + ", " + std::to_string(coords[2]) + ".";
				PrintDebug(debug.c_str());
			}

			// Read level ID.
			else if (line == "Level") {
				std::getline(infile, line);

				PrintDebug("[My Level Mod] Level ID detected.");

				levelID = "objLandTable00" + line;
				std::string debug = "[My Level Mod] Setting Level ID to " + line + ".";
				PrintDebug(debug.c_str());
			}
		}

		PrintDebug("[My Level Mod] Done reading from level_options.");

		infile.close();
	}
}

// Imports the custom level and texture pack into the game.
void loadLandTable(const char* path) {
	HMODULE v0 = **datadllhandle;

	// Grab LandTable from selected level.
	LandTable* Land = (LandTable*)GetProcAddress(v0, levelID.c_str());

	// Replace the selected table's LandTable with our own.
	try {
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();
	}
	catch (const std::exception& e) {
		// Print debugs if there was an error loading the level.
		PrintDebug("[My Level Mod] Land Table not found.");
		PrintDebug("[My Level Mod] Please make sure you're creating the lvl file properly.");
		PrintDebug(e.what());
	}

	// Set the level's textures to use our own.
	Land->TextureList = &customlevel_texlist;
	Land->TextureName = (char*)"TEXTURE";

	// (Safety feature) disable chunk system, entire level will be loaded at once.
	WriteData<5>((void*)0x5DCE2D, 0x90);
}

extern "C" {
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions) {
		fixFileStructure(path);
		readIniOptions(path, helperFunctions);
		loadLandTable(path);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

