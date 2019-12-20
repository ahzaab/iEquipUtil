#pragma once

#include "RE/Skyrim.h"


namespace ObjectReferenceExt
{
	RE::TESForm*	GetNthFormOfType(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, UInt32 a_type, UInt32 a_n);
	SInt32			GetNumItemsOfType(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, UInt32 a_type);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
