#include "pch.h" // or #include "stdafx.h"

ObjectFunc(RailController, 0x4980C0);

LoopPoint spline_points[] = {
	{ 0, 0, 10.0f, { 0, 0, 0 } },
	{ 0, 0, 10.0f, { 10.0f, 0, 0 } },
	{ 0, 0, 10.0f, { 20.0f, 0, 0 } },
	{ 0, 0, 10.0f, { 30.0f, 0, 0 } },
};

LoopHead spline = { 0, LengthOfArray<int16_t>(spline_points), 40.0f, spline_points, RailController };

LoopHead* level_splines[] = {
	&spline,
	NULL
};

void InitCurrentLevelAndScreenCount_r();
FunctionHook<void> InitCurrentLevelAndScreenCount_h(InitCurrentLevelAndScreenCount, InitCurrentLevelAndScreenCount_r);
void InitCurrentLevelAndScreenCount_r() {
	if (CurrentLevel == LevelIDs_CityEscape) {
		InitCurrentLevelAndScreenCount_h.Original();
		LoadStagePaths((LoopHead**)level_splines);
	}
}



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
