#pragma once

#include "GameTypes.h"  // BSFixedString

class TESForm;
class VMClassRegistry;
struct StaticFunctionTag;


namespace UIExt
{
	TESForm*	GetFormAtInventoryIndex(StaticFunctionTag*, UInt32 a_index);
	float		GetShoutFillPct(StaticFunctionTag*);
	float		GetShoutCooldownTime(StaticFunctionTag*);
	bool		RegisterFuncs(VMClassRegistry* a_registry);
}
