#include "pch.h"
#include "IniReader.h"
#include <exception>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <filesystem>

//
// IniReader.cpp v3
// 
// Description: 
//   Ini file reader for My Level Mod and SA2 modding purposes.
//   Can read from a 'level_options.ini' file, and read from
//   spline files and return official LoopHead (spline) objects.
//

IniReader::IniReader(const char* path, const HelperFunctions& helperFunctions) : helperFunctions(helperFunctions) {
	this->optionsPath = _strdup((std::string(path) + "\\level_options.ini").c_str());;
	this->gdPCPath = _strdup((std::string(path) + "\\gd_PC").c_str());
	this->levelID = 13;
}

// Read and configure options from 'level_options.ini'.
void IniReader::loadIniOptions() {
	std::ifstream iniFile;
	iniFile.open(optionsPath, std::ios::in);

	if (iniFile.is_open()) {
		printDebug("Reading from level_options.");

		std::string token;
		while (std::getline(iniFile, token, '=')) {
			// Read level ID.
			if (token == "Level") {
				std::getline(iniFile, token);

				try {
					levelID = std::stoi(token);
					printDebug("Level ID set to " + token + ".");
					printDebug("Will port level over objLandTable00" + token + ".");
				}
				catch (const std::exception& e) {
					printDebug("(Warning) Error reading level ID. Setting to City Escape.");
					printDebug(e.what());
				}
			}

			// Read start and End locations.
			else if (token == "Start" || token == "End") {
				boolean isStart = token == "Start" ? true : false;

				std::getline(iniFile, token);
				std::istringstream ss(token);

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
					printDebug("(Warning) Error reading coordinates. Defaulting to 0, 0, 0.");
					printDebug(e.what());
				}

				if (isStart) {
					printDebug("Level Start coordinates set to: " + token);
					StartPosition startpos = { levelID, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
					helperFunctions.RegisterStartPosition(Characters_Sonic, startpos);
				}
				else {
					printDebug("Level End coordinates set to: " + token);
					StartPosition endpos = { levelID, 0, 0, 0, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] }, { coords[0], coords[1], coords[2] } };
					helperFunctions.RegisterEndPosition(Characters_Sonic, endpos);
				}
			}
		}
		printDebug("Done reading from level_options.");
		iniFile.close();
	}
	else {
		printDebug("(Warning) Error reading from level options. (is it missing?)");
	}
}

// Automatically detect and read from spline ini files.
LoopHead** IniReader::loadSplines() {
	std::vector<LoopHead*> railSplines{};

	// Find ini files in the gd_PC folder, assume they are spline files.
	for (const auto& file : std::filesystem::directory_iterator(gdPCPath)) {
		std::string filePath = file.path().string();

		if (filePath.find(".ini") != std::string::npos) {
			std::ifstream splineFile;
			splineFile.open(filePath, std::ios::in);

			if (splineFile.is_open()) {
				printDebug("Spline file \"" + filePath + "\" found.");

				float totalDistance{};
				std::string code{};
				std::vector<LoopPoint> points{};

				try {
					// Read spline file header (currently only 3 lines).
					std::string token;
					for (int i = 0; i < 2; i++) {
						std::getline(splineFile, token, '=');

						if (token == "TotalDistance") {
							std::getline(splineFile, token);
							totalDistance = std::stof(token);
						}
						else if (token == "Code") {
							std::getline(splineFile, token);
							code = token;
						}
						else {
							printDebug("(Warning) Unexpected error/value in header: " + token + ".");
							printDebug("(Warning) Deleting spline.");
							return nullptr;
						}
					}

					// Skip rest of empty lines in header.
					while (token.find("[") == std::string::npos) {
						std::getline(splineFile, token);
					}

					// If spline is for rails, load spline as rail data.
					if (code == "4980C0") {
						ObjectFunc(RailController, 0x4980C0);

						while (token.find("[") != std::string::npos && !splineFile.eof()) {
							int xRot{}, zRot{};
							float distance{};
							float coords[3]{};

							for (int i = 0; i < 4; i++) {
								std::getline(splineFile, token, '=');

								if (token == "XRotation") {
									std::getline(splineFile, token);
									xRot = std::stoi(token, 0, 16);
								}
								else if (token == "ZRotation") {
									std::getline(splineFile, token);
									zRot = std::stoi(token, 0, 16);
								}
								else if (token == "Distance") {
									std::getline(splineFile, token);
									distance = std::stof(token);
								}
								else if (token == "Position") {
									std::getline(splineFile, token);

									// Remove spaces from line.
									std::string::iterator end_pos = std::remove(token.begin(), token.end(), ' ');
									token.erase(end_pos, token.end());

									std::istringstream ss(token);
									try {
										std::string temp{};
										for (int i = 0; i < 3; i++) {
											std::getline(ss, temp, ',');
											coords[i] = std::stof(temp);
										}
									}
									// Print debug information in case of error.
									catch (const std::exception& e) {
										printDebug("(Warning) Unexpected error/value in spline point: " + token + ".");
										printDebug("(Warning) Deleting spline.");
										printDebug(e.what());
										return nullptr;
									}
								}
								else if (token == "\n") {
									break;
								}
								else if (token != "\n") {
									printDebug("(Warning) Unexpected error/value in spline point: " + token + ".");
									printDebug("(Warning) Deleting spline.");
									return nullptr;
								}
							}

							// Create new point from read data.
							LoopPoint point{ xRot, zRot, distance, { coords[0], coords[1], coords[2] } };
							points.push_back(point);

							// Skip rest of lines to find new point.
							while (token.find("[") == std::string::npos && !splineFile.eof()) {
								std::getline(splineFile, token);
							}
						}
						LoopHead* spline = new LoopHead;
						spline->anonymous_0 = (int16_t)1;
						spline->Count = (int16_t)points.size();
						spline->TotalDistance = totalDistance;
						spline->Points = new LoopPoint[points.size()];
						spline->Object = (ObjectFuncPtr)RailController;
						std::copy(points.begin(), points.end(), spline->Points);
						railSplines.push_back(spline);
					}
					else {
						printDebug("Alternate/Unsupported spline code " + token + "detected.");
						printDebug("Stay tuned for future updates.");
					}
				}
				// Print debugs if there was an error loading the spline.
				catch (const std::exception& e) {
					printDebug("(ERROR) Invalid value found while reading spline file.");
					printDebug(e.what());
				}
			}
			else {
				printDebug("(Warning) Error reading from spline file: " + filePath + ".");
			}
			splineFile.close();
		}
	}

	if (railSplines.size() != 0) {
		printDebug(std::to_string(railSplines.size()) + " rail spline(s) successfully added.");

		const int size = railSplines.size() + 1;
		LoopHead** railSplinesArray = new LoopHead*[size];
		std::copy(railSplines.begin(), railSplines.end(), railSplinesArray);

		railSplinesArray[size - 1] = nullptr;

		return railSplinesArray;
	}
	else {
		printDebug("No splines found.");
	}

	return nullptr;
}

// Debug message helper.
void IniReader::printDebug(std::string message) {
	PrintDebug(("[My Level Mod] " + message).c_str());
}

// Convenience method used to grab level id (right now used for debug printing).
std::string IniReader::getLevelID() {
	return std::to_string(levelID);
}


// Notes:
// For start position and setting rotation, 0x4000 == 90 degrees.
