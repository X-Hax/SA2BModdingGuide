#pragma once
#include "SA2ModLoader.h"
#include <string>
class IniReader {
	public:
		int levelID;
		float simpleDeathPlane;
		bool hasSimpleDeathPlane;
		IniReader(const char* path, const HelperFunctions& helperFunctions);
		void loadIniOptions();
		LoopHead** loadSplines();
		std::string getLevelID();

	private:
		const char* optionsPath;
		const char* gdPCPath;
		const HelperFunctions& helperFunctions;
		void printDebug(std::string message);
		bool isNumber(const std::string& s);
};
