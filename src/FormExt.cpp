#include "FormExt.h"

#include "SKSE/API.h"

#include "Registration.h"
#include "Settings.h"


namespace FormExt
{
	void RegisterForBoundWeaponEquippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
			return;
		}

		auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
		regs->Register(a_thisForm);
	}


	void UnregisterForBoundWeaponEquippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
			return;
		}

		auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
		regs->Unregister(a_thisForm);
	}


	void RegisterForBoundWeaponUnequippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
			return;
		}

		auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
		regs->Register(a_thisForm);
	}


	void UnregisterForBoundWeaponUnequippedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
			return;
		}

		auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
		regs->Unregister(a_thisForm);
	}


	template <aSetting<RE::TESForm*>& ARRAY>
	bool IsT(VM* a_vm, StackID a_stackID, const RE::TESForm* a_form)
	{
		if (!a_form) {
			a_vm->TraceStack("a_form is a NONE form!", a_stackID, Severity::kWarning);
			return false;
		}

		return ARRAY->find(a_form->formID) != ARRAY->end();
	}


	bool HasFire(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::fire>(a_vm, a_stackID, a_form);
	}


	bool HasIce(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::ice>(a_vm, a_stackID, a_form);
	}


	bool HasPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::poison>(a_vm, a_stackID, a_form);
	}


	bool HasShock(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::shock>(a_vm, a_stackID, a_form);
	}


	bool IsBandage(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::bandages>(a_vm, a_stackID, a_form);
	}


	bool IsGrenade(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::grenades>(a_vm, a_stackID, a_form);
	}


	bool IsJavelin(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::javelins>(a_vm, a_stackID, a_form);
	}


	bool IsOil(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::oils>(a_vm, a_stackID, a_form);
	}


	bool IsSalve(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::salves>(a_vm, a_stackID, a_form);
	}


	bool IsSpear(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::spears>(a_vm, a_stackID, a_form);
	}


	bool IsSpellRanged(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::rangedSpells>(a_vm, a_stackID, a_form);
	}


	bool IsSpellWard(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::spellWards>(a_vm, a_stackID, a_form);
	}


	bool IsStaffRanged(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::rangedStaves>(a_vm, a_stackID, a_form);
	}


	bool IsThrowingAxe(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::throwingAxes>(a_vm, a_stackID, a_form);
	}


	bool IsThrowingKnife(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::throwingKnives>(a_vm, a_stackID, a_form);
	}


	bool IsWax(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form)
	{
		return IsT<Settings::waxes>(a_vm, a_stackID, a_form);
	}


	bool RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("RegisterForBoundWeaponEquippedEvent", "iEquip_FormExt", RegisterForBoundWeaponEquippedEvent, true);
		a_vm->RegisterFunction("UnregisterForBoundWeaponEquippedEvent", "iEquip_FormExt", UnregisterForBoundWeaponEquippedEvent, true);
		a_vm->RegisterFunction("RegisterForBoundWeaponUnequippedEvent", "iEquip_FormExt", RegisterForBoundWeaponUnequippedEvent, true);
		a_vm->RegisterFunction("UnregisterForBoundWeaponUnequippedEvent", "iEquip_FormExt", UnregisterForBoundWeaponUnequippedEvent, true);
		a_vm->RegisterFunction("HasFire", "iEquip_FormExt", HasFire, true);
		a_vm->RegisterFunction("HasIce", "iEquip_FormExt", HasIce, true);
		a_vm->RegisterFunction("HasPoison", "iEquip_FormExt", HasPoison, true);
		a_vm->RegisterFunction("HasShock", "iEquip_FormExt", HasShock, true);
		a_vm->RegisterFunction("IsBandage", "iEquip_FormExt", IsBandage, true);
		a_vm->RegisterFunction("IsGrenade", "iEquip_FormExt", IsGrenade, true);
		a_vm->RegisterFunction("IsJavelin", "iEquip_FormExt", IsJavelin, true);
		a_vm->RegisterFunction("IsOil", "iEquip_FormExt", IsOil, true);
		a_vm->RegisterFunction("IsSalve", "iEquip_FormExt", IsSalve, true);
		a_vm->RegisterFunction("IsSpear", "iEquip_FormExt", IsSpear, true);
		a_vm->RegisterFunction("IsSpellWard", "iEquip_FormExt", IsSpellWard, true);
		a_vm->RegisterFunction("IsThrowingAxe", "iEquip_FormExt", IsThrowingAxe, true);
		a_vm->RegisterFunction("IsThrowingKnife", "iEquip_FormExt", IsThrowingKnife, true);
		a_vm->RegisterFunction("IsWax", "iEquip_FormExt", IsWax, true);

		return true;
	}
}
