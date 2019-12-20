#pragma once

#include "RE/Skyrim.h"


namespace UIExt
{
	RE::TESForm* GetFormAtInventoryIndex(RE::StaticFunctionTag*, UInt32 a_index);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
