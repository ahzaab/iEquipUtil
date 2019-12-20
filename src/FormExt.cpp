#include "FormExt.h"

#include "SKSE/API.h"

#include "Registration.h"
#include "Settings.h"


namespace FormExt
{
	void RegisterForBoundWeaponEquippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
		regs->Register(a_thisForm);
	}


	void UnregisterForBoundWeaponEquippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
		regs->Unregister(a_thisForm);
	}


	void RegisterForBoundWeaponUnequippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
		regs->Register(a_thisForm);
	}


	void UnregisterForBoundWeaponUnequippedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
		regs->Unregister(a_thisForm);
	}


	template <aSetting<RE::TESForm*>& ARRAY>
	bool IsT(RE::TESForm* a_form)
	{
		if (!a_form) {
			_WARNING("a_form is a NONE form!");
			return false;
		}

		return ARRAY.find(a_form->formID) != ARRAY.end();
	}


	bool HasFire(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::fire>(a_form);
	}


	bool HasIce(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::ice>(a_form);
	}


	bool HasPoison(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::poison>(a_form);
	}


	bool HasShock(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::shock>(a_form);
	}


	bool IsBandage(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::bandages>(a_form);
	}


	bool IsGrenade(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::grenades>(a_form);
	}


	bool IsJavelin(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::javelins>(a_form);
	}


	bool IsOil(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::oils>(a_form);
	}


	bool IsSalve(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::salves>(a_form);
	}


	bool IsSpear(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::spears>(a_form);
	}


	bool IsSpellRanged(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::rangedSpells>(a_form);
	}


	bool IsSpellWard(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::spellWards>(a_form);
	}


	bool IsStaffRanged(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::rangedStaves>(a_form);
	}


	bool IsThrowingAxe(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::throwingAxes>(a_form);
	}


	bool IsThrowingKnife(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::throwingKnives>(a_form);
	}


	bool IsWax(RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		return IsT<Settings::waxes>(a_form);
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("RegisterForBoundWeaponEquippedEvent", "iEquip_FormExt", RegisterForBoundWeaponEquippedEvent);
		a_vm->RegisterFunction("UnregisterForBoundWeaponEquippedEvent", "iEquip_FormExt", UnregisterForBoundWeaponEquippedEvent);
		a_vm->RegisterFunction("RegisterForBoundWeaponUnequippedEvent", "iEquip_FormExt", RegisterForBoundWeaponUnequippedEvent);
		a_vm->RegisterFunction("UnregisterForBoundWeaponUnequippedEvent", "iEquip_FormExt", UnregisterForBoundWeaponUnequippedEvent);
		a_vm->RegisterFunction("HasFire", "iEquip_FormExt", HasFire);
		a_vm->RegisterFunction("HasIce", "iEquip_FormExt", HasIce);
		a_vm->RegisterFunction("HasPoison", "iEquip_FormExt", HasPoison);
		a_vm->RegisterFunction("HasShock", "iEquip_FormExt", HasShock);
		a_vm->RegisterFunction("IsBandage", "iEquip_FormExt", IsBandage);
		a_vm->RegisterFunction("IsGrenade", "iEquip_FormExt", IsGrenade);
		a_vm->RegisterFunction("IsJavelin", "iEquip_FormExt", IsJavelin);
		a_vm->RegisterFunction("IsOil", "iEquip_FormExt", IsOil);
		a_vm->RegisterFunction("IsSalve", "iEquip_FormExt", IsSalve);
		a_vm->RegisterFunction("IsSpear", "iEquip_FormExt", IsSpear);
		a_vm->RegisterFunction("IsSpellWard", "iEquip_FormExt", IsSpellWard);
		a_vm->RegisterFunction("IsThrowingAxe", "iEquip_FormExt", IsThrowingAxe);
		a_vm->RegisterFunction("IsThrowingKnife", "iEquip_FormExt", IsThrowingKnife);
		a_vm->RegisterFunction("IsWax", "iEquip_FormExt", IsWax);

		return true;
	}
}
