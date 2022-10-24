#include "pch.h"
#include "iniReader.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <filesystem>
#include <curl/curl.h>

// MyLevelMod.cpp Version 3.3
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

const float VERSION = 3.3f;
IniReader* iniReader;

// Helper function to print debug messages.
void printDebug(std::string message) {
	PrintDebug(("[My Level Mod] " + message).c_str());
}

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
	printDebug("Checking for updates...");

	curl_global_init(CURL_GLOBAL_ALL);

	std::string result{ };
	CURL* curl = curl_easy_init();

	if (!curl) {
		printDebug("(Warning) Could not check for update. [Curl will not instantiate]");
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
		printDebug("(Warning) Could not check for update. [Error reaching internet]");
		return;
	}

	std::string updatePath = std::string(path) + "\\MANUALLY UPDATE TO VERSION " + std::to_string(std::stof(result)) + ".txt";

	// If they need to update, add update file.
	if (VERSION < std::stof(result)) {
		printDebug("Update detected! Creating update reminder.");

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
		printDebug("Mod up to date.");
		bool cleaned = false;

		for (const auto& file : std::filesystem::directory_iterator(path)) {
			std::string filePath = file.path().string();

			if (filePath.find("UPDATE") != std::string::npos) {
				std::filesystem::remove(filePath);
				cleaned = true;
			}
		}

		if (cleaned) {
			printDebug("Cleaned up update reminders.");
		}
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
			printDebug("Incorrect texture pack position detected.");

			if (std::rename(filePath.c_str(), goodTexturePath.c_str()) == 0) {
				printDebug("Successfully moved texture pack file.");
				printDebug("Expect a game crash, please run game again.");
			}
			else {
				printDebug("(Warning) Error moving texture pack file to right position.");
				printDebug("(Warning) Texture pack file should be at (\\gd_PC\\PRS\\texture.pak).");
			}
		}
		else if (filePath.find(".sa2blvl") != std::string::npos) {
			printDebug("Incorrect level file position detected.");

			if (std::rename(filePath.c_str(), goodLevelPath.c_str()) == 0) {
				printDebug("Successfully moved level file.");
				printDebug("Expect a game crash, please run game again.");
			}
			else {
				printDebug("(Warning) Error moving level file to right position.");
				printDebug("(Warning) Level file should be at (\\gd_PC\\level.sa2blvl).");
			}
		}
	}

	// Change set file to match level id.
	std::string gdPCPath = std::string(path) + "\\gd_PC";
	for (const auto& file : std::filesystem::directory_iterator(gdPCPath)) {
		std::string filePath = file.path().string();
		std::string goodSetPath = std::string(path) + "\\gd_PC\\set00" + iniReader->getLevelID();

		if (filePath.find("_s.bin") != std::string::npos && filePath.find(iniReader->getLevelID()) == std::string::npos) {
			printDebug("(Warning) Improper set file found not matching level ID: " + iniReader->getLevelID() + ".");

			goodSetPath += "_s.bin";

			if (std::rename(filePath.c_str(), goodSetPath.c_str()) == 0) {
				printDebug("Successfully fixed set file.");
				printDebug("Renamed " + filePath + " to " + goodSetPath + ".");
			}
			else {
				printDebug("(Warning) Error fixing set file.");
				printDebug("(Warning) Set file should be at (" + goodSetPath + ").");
			}
		}
		else if (filePath.find("_u.bin") != std::string::npos && filePath.find(iniReader->getLevelID()) == std::string::npos) {
			printDebug("(Warning) Improper set file found not matching level ID: " + iniReader->getLevelID() + ".");

			goodSetPath += "_u.bin";

			if (std::rename(filePath.c_str(), goodSetPath.c_str()) == 0) {
				printDebug("Successfully fixed set file.");
				printDebug("Renamed " + filePath + " to " + goodSetPath + ".");
			}
			else {
				printDebug("(Warning) Error fixing set file.");
				printDebug("(Warning) Set file should be at (" + goodSetPath + ").");
			}
		}
	}
}

// Imports the custom level and texture pack into the game.
void loadLandTable(const char* path) {
	HMODULE v0 = **datadllhandle;

	// Grab LandTable from selected level.
	const std::string fullLandTableID = "objLandTable00" + iniReader->getLevelID();
	LandTable* Land = (LandTable*)GetProcAddress(v0, fullLandTableID.c_str());

	// Replace the selected table's LandTable with our own.
	try {
		*Land = *(new LandTableInfo(std::string(path) + "\\gd_PC\\level.sa2blvl"))->getlandtable();
	}
	catch (const std::exception& e) {
		// Print debugs if there was an error loading the level.
		printDebug("(ERROR) Land Table not found.");
		printDebug("(ERROR) Please make sure you're creating the lvl file properly.");
		printDebug(e.what());
	}

	// Set the level's textures to use our own.
	Land->TextureList = &customlevel_texlist;
	Land->TextureName = (char*)"TEXTURE";

	// (Safety feature) disable chunk system, entire level will be loaded at once.
	WriteData<5>((void*)0x5DCE2D, 0x90);
}

// Function 'Hook,' automatically gets run every time a level is loaded in-game.
void InitCurrentLevelAndScreenCount_r();
FunctionHook<void> InitCurrentLevelAndScreenCount_h(InitCurrentLevelAndScreenCount, InitCurrentLevelAndScreenCount_r);
void InitCurrentLevelAndScreenCount_r() {
	InitCurrentLevelAndScreenCount_h.Original();
	if (CurrentLevel == iniReader->levelID) {
		printDebug("Level load detected. Loading splines.");

		LoopHead** splines = iniReader->loadSplines();
		if (splines) {
			LoadStagePaths(splines);
		}
	}
}

extern "C" {
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions) {
		try {
			checkForUpdate(path);
		}
		catch (const std::exception& e) {
			printDebug("(ERROR) Exception caught trying to check for update.");
			printDebug(e.what());
		}

		iniReader = new IniReader(path, helperFunctions);
		iniReader->loadIniOptions();

		try {
			fixFileStructure(path);
		}
		catch (const std::exception& e) {
			printDebug("(ERROR) Exception caught trying to fix mod file structure.");
			printDebug(e.what());
		}

		loadLandTable(path);
	}
	
    __declspec(dllexport) void __cdecl OnFrame() {
		if (GameState == GameStates_Ingame && CurrentLevel == iniReader->levelID && iniReader->hasSimpleDeathPlane) {
			if (MainCharObj1 != nullptr && MainCharObj1[0] != nullptr && MainCharObj1[0]->Position.y <= iniReader->simpleDeathPlane) {
				GameState = GameStates_NormalRestart;
			}
		}
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
