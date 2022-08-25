#include "pch.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <filesystem>
#include <curl/curl.h>

// MyLevelMod.cpp Version 2.3
// Description:
//    A Script that can import your mod into Sonic Adventure 2,
//    including the feature of being able to read from a user-inputted
//    "level_options.ini" file for simple level customizations.
//
//    If you have any questions, feel free to ask at the x-hax discord.
//    https://discord.gg/gqJCF47
//
//    To compile, its required that you use the included SATools libraries
//    and follow the "Make your own DLL" mod tutorial in the X-hax wiki.
// 
//    Its also required that you install the 'libcurl' package.
//

// Create a custom texture pack with a texture limit of 1000.
NJS_TEXNAME customlevel_texnames[1000]{};
NJS_TEXLIST customlevel_texlist = { arrayptrandlength(customlevel_texnames) };

const float version = 2.3f;
std::string levelID = "13";

// Helper function for checkForUpdate().
int writer(char* data, size_t size, size_t nmemb, std::string* buffer) {
	int result = 0;
	if (buffer != NULL) {
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

// Attempt to detect if theres an update for this mod. (Auto Update doesn't work)
void checkForUpdate(const char* path) {
	PrintDebug("[My Level Mod] Checking for updates...");

	curl_global_init(CURL_GLOBAL_ALL);

	std::string result{ };
	CURL* curl = curl_easy_init();

	if (!curl) {
		PrintDebug("[My Level Mod] (Warning) Could not check for update. [Curl will not instantiate]");
		return;
	}

	// No cache.
	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Cache-control: no-cache");

	// Read version number from github, store result as string.
	curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/X-Hax/SA2BModdingGuide/master/Level%20Editing/My%20Level%20Mod/VERSION.txt");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	CURLcode curlResult = curl_easy_perform(curl);

	if (curlResult != CURLE_OK) {
		PrintDebug("[My Level Mod] (Warning) Could not check for update. [Error reaching internet]");
		return;
	}

	std::string updatePath = std::string(path) + "\\MANUALLY UPDATE TO VERSION " + std::to_string(std::stof(result)) + ".txt";

	// If they need to update, add update file.
	if (version < std::stof(result)) {
		PrintDebug("[My Level Mod] Update detected! Creating update reminder.");

		if (!std::filesystem::exists(updatePath)) {
			std::ofstream updateFile;
			updateFile.open(updatePath, std::ofstream::out);
			updateFile << "An update has been detected for My Level Mod!" << std::endl;
			updateFile << "Please download and update your mod manually. Download link:" << std::endl;
			updateFile << "https://github.com/X-Hax/SA2BModdingGuide/releases" << std::endl;
			updateFile.close();
		}
	}
	// If they don't need to update, but the update file is there, get rid of it.
	else {
		PrintDebug("[My Level Mod] Mod up to date.");

		for (const auto& file : std::filesystem::directory_iterator(path)) {
			std::string filePath = file.path().string();

			if (filePath.find("UPDATE") != std::string::npos) {
				std::filesystem::remove(filePath);
			}
		}

		PrintDebug("[My Level Mod] Cleaned up update reminders.");
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

// Detect incorrect file structure, and fix if necessary.
void fixFileStructure(const char* path) {

	const std::string goodLevelPath = std::string(path) + "\\gd_PC\\level.sa2blvl";
	const std::string goodTexturePath = std::string(path) + "\\gd_PC\\PRS\\texture.pak";

	// Move texture pak / level file to the right position.
	for (const auto& file : std::filesystem::directory_iterator(path)) {
		std::string filePath = file.path().string();

		if (filePath.find(".pak") != std::string::npos) {
			PrintDebug("[My Level Mod] Incorrect texture pack position detected.");

			if (std::rename(filePath.c_str(), goodTexturePath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved texture pack file.");
				PrintDebug("[My Level Mod] Expect a game crash, please run game again.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving texture pack file to right position.");
				PrintDebug("[My Level Mod] (Warning) Texture pack file should be at (\\gd_PC\\PRS\\texture.pak).");
			}
		}
		else if (filePath.find(".sa2blvl") != std::string::npos) {
			PrintDebug("[My Level Mod] Incorrect level file position detected.");

			if (std::rename(filePath.c_str(), goodLevelPath.c_str()) == 0) {
				PrintDebug("[My Level Mod] Successfully moved level file.");
				PrintDebug("[My Level Mod] Expect a game crash, please run game again.");
			}
			else {
				PrintDebug("[My Level Mod] (Warning) Error moving level file to right position.");
				PrintDebug("[My Level Mod] (Warning) Level file should be at (\\gd_PC\\level.sa2blvl).");
			}
		}
	}

	// Change set file to match level id.
	std::string gdPCPath = std::string(path) + "\\gd_PC";
	for (const auto& file : std::filesystem::directory_iterator(gdPCPath)) {
		std::string filePath = file.path().string();
		std::string goodSetPath = std::string(path) + "\\gd_PC\\set00" + levelID;
		std::string debug;

		if (filePath.find("_s.bin") != std::string::npos && filePath.find(levelID) == std::string::npos) {
			debug = "[My Level Mod] (Warning) Improper set file found not matching level ID: " + levelID + ".";
			PrintDebug(debug.c_str());

			goodSetPath += "_s.bin";

			if (std::rename(filePath.c_str(), goodSetPath.c_str()) == 0) {
				debug = "[My Level Mod] Renamed " + filePath + " to " + goodSetPath + ".";

				PrintDebug("[My Level Mod] Successfully fixed set file.");
				PrintDebug(debug.c_str());
			}
			else {
				debug = "[My Level Mod] (Warning) Set file should be at (" + goodSetPath + ").";

				PrintDebug("[My Level Mod] (Warning) Error fixing set file.");
				PrintDebug(debug.c_str());
			}
		}
		else if (filePath.find("_u.bin") != std::string::npos && filePath.find(levelID) == std::string::npos) {
			debug = "[My Level Mod] (Warning) Improper set file found not matching level ID: " + levelID + ".";
			PrintDebug(debug.c_str());

			goodSetPath += "_u.bin";

			if (std::rename(filePath.c_str(), goodSetPath.c_str()) == 0) {
				debug = "[My Level Mod] Renamed " + filePath + " to " + goodSetPath + ".";

				PrintDebug("[My Level Mod] Successfully fixed set file.");
				PrintDebug(debug.c_str());
			}
			else {
				debug = "[My Level Mod] (Warning) Set file should be at (" + goodSetPath + ").";

				PrintDebug("[My Level Mod] (Warning) Error fixing set file.");
				PrintDebug(debug.c_str());
			}
		}
	}
}

// Read from MyLevelMod ini.
// Used to get level information from user input.
void readIniOptions(const char* path, const HelperFunctions& helperFunctions) {
	std::ifstream infile;
	infile.open(std::string(path) + "\\level_options.ini", std::ios::in);
	if (!infile.is_open()) {
		PrintDebug("[My Level Mod] (Warning) Error reading from level options. (is it missing?)");
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
					PrintDebug("[My Level Mod] (Warning) Error reading start position.");
					PrintDebug(e.what());

					PrintDebug("[My Level Mod] (Warning) Defaulting to 0, 0, 0.");
					coords[0] = 0;
					coords[1] = 0;
					coords[2] = 0;
				}

				// 0x4000 == 90 degrees
				std::string debug;
				if (start) {
					PrintDebug("[My Level Mod] Level Start coordinates detected.");
					StartPosition startpos = { LevelIDs_CityEscape, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
					helperFunctions.RegisterStartPosition(Characters_Sonic, startpos);
					debug = "[My Level Mod] Setting Start coordinates to: " + std::to_string(coords[0]) + ", " + std::to_string(coords[1]) + ", " + std::to_string(coords[2]) + ".";
				}
				else {
					PrintDebug("[My Level Mod] Level End coordinates detected.");
					StartPosition endpos = { LevelIDs_CityEscape, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
					helperFunctions.RegisterEndPosition(Characters_Sonic, endpos);
					debug = "[My Level Mod] Setting End coordinates to: " + std::to_string(coords[0]) + ", " + std::to_string(coords[1]) + ", " + std::to_string(coords[2]) + ".";
				}

				PrintDebug(debug.c_str());
			}

			// Read level ID.
			else if (line == "Level") {
				std::getline(infile, line);

				PrintDebug("[My Level Mod] Level ID detected.");

				levelID = line;
				std::string debug = "[My Level Mod] Setting Level ID to " + line + ".";
				PrintDebug(debug.c_str());

				debug = "[My Level Mod] Will port level over objLandTable00" + levelID + ".";
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
	const std::string fullLandTableID = "objLandTable00" + levelID;
	LandTable* Land = (LandTable*)GetProcAddress(v0, fullLandTableID.c_str());

	// Replace the selected table's LandTable with our own.
	try {
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();
	}
	catch (const std::exception& e) {
		// Print debugs if there was an error loading the level.
		PrintDebug("[My Level Mod] (ERROR) Land Table not found.");
		PrintDebug("[My Level Mod] (ERROR) Please make sure you're creating the lvl file properly.");
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
		try {
			checkForUpdate(path);
		}
		catch (const std::exception& e) {
			PrintDebug("[My Level Mod] (ERROR) Exception caught trying to check for update.");
			PrintDebug(e.what());
		}

		readIniOptions(path, helperFunctions);

		try {
			fixFileStructure(path);
		}
		catch (const std::exception& e) {
			PrintDebug("[My Level Mod] (ERROR) Exception caught trying to fix mod file structure.");
			PrintDebug(e.what());
		}

		loadLandTable(path);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

