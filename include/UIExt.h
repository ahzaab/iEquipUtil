#pragma once

#include "RE/Skyrim.h"


namespace UIExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	RE::TESForm* GetFormAtInventoryIndex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, UInt32 a_index);

	bool RegisterFuncs(VM* a_vm);
}
