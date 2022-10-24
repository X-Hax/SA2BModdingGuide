#include "pch.h" // or #include "stdafx.h"

// Code Snippet:
//      How to import a spline into a level. (Used for rails, loops, etc)
//
// Notes:
// 	    Intended for use in its own mod or as a part of a mod's dll.
//	    Follow the x-hax modding tutorial for help on importing
//      dependencies.

// Step 1: Import a 'spline controller.'
// This will turn your spline into a rail or loop.
ObjectFunc(RailController, 0x4980C0);

// Step 2: Define the points along your spline.
LoopPoint spline_points[] = {
	{ 0, 0, 10.0f, { 0, 0, 0 } },
	{ 0, 0, 10.0f, { 10.0f, 0, 0 } },
	{ 0, 0, 10.0f, { 20.0f, 0, 0 } },
	{ 0, 0, 10.0f, { 30.0f, 0, 0 } },
};
// Overview for a single point:
// {xRotation, zRotation, distance, position of point}

// Step 3: Define a spline.
LoopHead spline = { 1, LengthOfArray<int16_t>(spline_points), 40.0f, spline_points, RailController };
// Overview for above: { unknown, # of points, total distance, list of your points, controller }

// Step 4: Define a list of splines ending with "NULL" or nullptr.
LoopHead* level_splines[] = {
	&spline,
	NULL
};

// Step 5: Load in your splines with the LoadStagePaths function.
// In this example, I load in the splines using a 'Function Hook'
// which automatically runs this code whenever a level loads.
void InitCurrentLevelAndScreenCount_r();
FunctionHook<void> InitCurrentLevelAndScreenCount_h(InitCurrentLevelAndScreenCount, InitCurrentLevelAndScreenCount_r);
void InitCurrentLevelAndScreenCount_r() {
	if (CurrentLevel == LevelIDs_CityEscape) {
		InitCurrentLevelAndScreenCount_h.Original();
		LoadStagePaths((LoopHead**)level_splines);
	}
}

// Note: InitCurrentLevelAndScreenCount_r() is a built in function.
// FunctionHook code supplied by MainMemory on x-hax. Thanks main.
// FunctionHook code subject to change.
// Other solutions may also work, just make sure that LoadStagePaths runs.

// A large portion of spline code can be automatically generated by SATools.
// If you have used the blender addon to create a spline, open an SATools
// project folder, and replace one of the paths of a level in the 'paths'
// folder. Then, in SATools build options, select your level, and choose
// 'Manual.' Then, click stage paths and 'generate c++ code.' That will
// give you the necessary points and a majority of the code.



/*------------------------------------------------------------------------
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
