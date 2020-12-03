#include "SA2ModLoader.h"

// Example of a simple object with dynamic collisions that does not move

static constexpr int ColFlags_Solid = 0x1;

void YourObject_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1.Entity;
	
	njSetTexlist(TEXLIST);
	njPushMatrix(0);
	njTranslateV(_nj_current_matrix_ptr_, &data->Position);
	njRotateX(_nj_current_matrix_ptr_, data->Rotation.x);
	njRotateY(_nj_current_matrix_ptr_, data->Rotation.y);
	njRotateZ(_nj_current_matrix_ptr_, data->Rotation.z);
	DrawSA2BModel(MODEL);
	njPopMatrix(1u);
}

void YourObject(ObjectMaster* obj) {
	EntityData1* data = obj->Data1.Entity;
	NJS_OBJECT* dynobj = GetFreeDyncolObjectEntry();

	// Fill in the dyncol information
	dynobj->pos[0] = data->Position.x;
	dynobj->pos[1] = data->Position.y;
	dynobj->pos[2] = data->Position.z;

	dynobj->ang[0] = data->Rotation.x;
	dynobj->ang[1] = data->Rotation.y;
	dynobj->ang[2] = data->Rotation.z;

	dynobj->scl[0] = 1.0f;
	dynobj->scl[1] = 1.0f;
	dynobj->scl[2] = 1.0f;

	dynobj->evalflags = 0xFFFFFFF8;

	// Initialize the dyncol with the solid collision flag
	DynCol_Add(ColFlags_Solid, obj, dynobj);

	obj->EntityData2 = (UnknownData2*)dynobj; // save it in EntityData2, where sa2 expects it

	obj->DeleteSub = ObjectFunc_DynColDelete; // a function that deletes a dyncol set in EntityData2
	obj->MainSub = ObjectFunc_ClipObject; // a function that just runs ClipSetObject
	obj->DisplaySub = YourObject_Display; // custom display
}