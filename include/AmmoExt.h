#pragma once




namespace AmmoExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	bool IsAmmoBound(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESAmmo* a_ammo);

	bool RegisterFuncs(VM* a_vm);
}
