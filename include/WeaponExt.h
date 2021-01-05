#pragma once

namespace WeaponExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	bool IsWeaponBound(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectWEAP* a_weap);

	bool RegisterFuncs(VM* a_vm);
}
