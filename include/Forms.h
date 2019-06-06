#pragma once

#undef GetForm

#include "GameForms.h"  // LookupFormByID
#include "GameData.h"  // DataHandler


namespace
{
	enum
	{
		kInvalid = 0xFFFFFFFF
	};
}


template <class T = TESForm>
T* GetForm(UInt32 a_rawFormID, const char* a_pluginName)
{
	auto dataHandler = DataHandler::GetSingleton();
	const ModInfo* info = 0;
	UInt32 formID = kInvalid;
	info = dataHandler->LookupModByName(a_pluginName);
	if (info && info->modIndex != 0xFF) {
		formID = info->modIndex << (3 * 8);
#if _WIN64
		formID += info->lightIndex << ((1 * 8) + 4);
#endif
		formID += a_rawFormID;
	}
	return static_cast<T*>(LookupFormByID(formID));
}
