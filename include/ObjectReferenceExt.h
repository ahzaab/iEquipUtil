#pragma once

#include "RE/Skyrim.h"


namespace ObjectReferenceExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	RE::TESForm*	GetNthFormOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, UInt32 a_type, UInt32 a_n);
	SInt32			GetNumItemsOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, UInt32 a_type);

	bool RegisterFuncs(VM* a_vm);
}
