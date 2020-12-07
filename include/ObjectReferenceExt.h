#pragma once


namespace ObjectReferenceExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	RE::TESForm*	GetNthFormOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, uint32_t a_type, uint32_t a_n);
	int32_t			GetNumItemsOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, uint32_t a_type);

	bool RegisterFuncs(VM* a_vm);
}
