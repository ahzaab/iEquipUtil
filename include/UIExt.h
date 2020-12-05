#pragma once

#include "RE/Skyrim.h"


namespace UIExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	RE::TESForm* GetFormAtInventoryIndex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_index);

	bool RegisterFuncs(VM* a_vm);
}
