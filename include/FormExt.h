#pragma once

#include "RE/Skyrim.h"


namespace FormExt
{
	void	RegisterForBoundWeaponEquippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);
	void	UnregisterForBoundWeaponEquippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);

	void	RegisterForBoundWeaponUnequippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);
	void	UnregisterForBoundWeaponUnequippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);

	bool	HasFire(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	HasIce(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	HasPoison(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	HasShock(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsBandage(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsGrenade(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsJavelin(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsOil(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsSalve(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsSpear(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsSpellRanged(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsSpellWard(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsStaffRanged(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsThrowingAxe(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsThrowingKnife(RE::StaticFunctionTag*, RE::TESForm* a_form);
	bool	IsWax(RE::StaticFunctionTag*, RE::TESForm* a_form);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
