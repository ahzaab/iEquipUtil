#pragma once



namespace FormExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	void	RegisterForBoundWeaponEquippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);
	void	UnregisterForBoundWeaponEquippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);

	void	RegisterForBoundWeaponUnequippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);
	void	UnregisterForBoundWeaponUnequippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);

	bool	HasFire(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	HasIce(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	HasPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	HasShock(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsBandage(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsGrenade(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsJavelin(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsOil(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsSalve(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsSpear(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsSpellRanged(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsSpellWard(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsStaffRanged(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsThrowingAxe(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsThrowingKnife(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);
	bool	IsWax(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);

	bool RegisterFuncs(VM* a_vm);
}
