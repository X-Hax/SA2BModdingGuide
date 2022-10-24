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
