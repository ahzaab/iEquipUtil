#pragma once


namespace MagicEffectExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	RE::TESForm* GetAssociatedItem(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::EffectSetting* a_effect);

	bool RegisterFuncs(VM* a_vm);
}
