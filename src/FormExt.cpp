#include "FormExt.h"

#include "GameForms.h"  // TESForm
#include "PapyrusNativeFunctions.h"  // NativeFunction
#include "PapyrusVM.h"  // VMClassRegistry

#include "Registration.h"  // OnBoundWeaponEquippedRegSet, OnBoundWeaponUnequippedRegSet
#include "Settings.h"  // Settings


namespace FormExt
{
	namespace
	{
		UInt32& GetLightRadius(TESObjectLIGH* a_light)
		{
#if _WIN64
			return a_light->unkE0.radius;
#else
			return a_light->unk78.unk04;
#endif
		}
	}


	void RegisterForBoundWeaponEquippedEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnBoundWeaponEquippedRegSet::GetSingleton()->Register<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void UnregisterForBoundWeaponEquippedEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnBoundWeaponEquippedRegSet::GetSingleton()->Unregister<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void RegisterForBoundWeaponUnequippedEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnBoundWeaponUnequippedRegSet::GetSingleton()->Register<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void UnregisterForBoundWeaponUnequippedEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnBoundWeaponUnequippedRegSet::GetSingleton()->Unregister<TESForm>(a_thisForm->formType, a_thisForm);
	}


	SInt32 GetLightDuration(StaticFunctionTag*, TESForm* a_light)
	{
		if (!a_light) {
			_WARNING("[WARNING] a_light is a NONE form!");
			return -1;
		} else if (a_light->formType != kFormType_Light) {
			_WARNING("[WARNING] a_light is a not a light!");
			return -1;
		}

		auto light = static_cast<TESObjectLIGH*>(a_light);
#if _WIN64
		return light->unkE0.time;
#else
		return light->unk78.unk00;
#endif
	}


	SInt32 GetLightRadius(StaticFunctionTag*, TESForm* a_light)
	{
		if (!a_light) {
			_WARNING("[WARNING] a_light is a NONE form!");
			return -1;
		} else if (a_light->formType != kFormType_Light) {
			_WARNING("[WARNING] a_light is a not a light!");
			return -1;
		}

		auto light = static_cast<TESObjectLIGH*>(a_light);
		return GetLightRadius(light);
	}


	void SetLightRadius(StaticFunctionTag*, TESForm* a_light, SInt32 a_radius)
	{
		if (!a_light) {
			_WARNING("[WARNING] a_light is a NONE form!");
			return;
		} else if (a_light->formType != kFormType_Light) {
			_WARNING("[WARNING] a_light is a not a light!");
			return;
		} else if (a_light < 0) {
			_WARNING("[WARNING] a_radius can not be negative!");
			return;
		}

		auto light = static_cast<TESObjectLIGH*>(a_light);
		GetLightRadius(light) = a_radius;
	}


	void ResetLightRadius(StaticFunctionTag*, TESForm* a_light)
	{
		if (!a_light) {
			_WARNING("[WARNING] a_light is a NONE form!");
			return;
		} else if (a_light->formType != kFormType_Light) {
			_WARNING("[WARNING] a_light is a not a light!");
			return;
		}

		auto origLight = static_cast<TESObjectLIGH*>(LookupFormByID(a_light->formID));
		auto light = static_cast<TESObjectLIGH*>(a_light);
		GetLightRadius(light) = GetLightRadius(origLight);
	}


	template <aSetting<TESForm*>& arr>
	bool IsT(TESForm* a_form)
	{
		if (!a_form) {
			_WARNING("[WARNING] a_form is a NONE form!");
			return false;
		}

		return arr.find(a_form->formID) != arr.end();
	}


	bool IsSpear(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::spears>(a_form);
	}


	bool IsJavelin(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::javelins>(a_form);
	}


	bool IsGrenade(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::grenades>(a_form);
	}


	bool IsThrowingAxe(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::throwingAxes>(a_form);
	}


	bool IsThrowingKnife(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::throwingKnives>(a_form);
	}


	bool IsWax(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::waxes>(a_form);
	}


	bool IsOil(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::oils>(a_form);
	}


	bool IsSpellWard(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::spellWards>(a_form);
	}


	bool HasFire(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::fire>(a_form);
	}


	bool HasIce(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::ice>(a_form);
	}


	bool HasShock(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::shock>(a_form);
	}


	bool HasPoison(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::poison>(a_form);
	}


	bool IsSalve(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::salves>(a_form);
	}


	bool IsBandage(StaticFunctionTag*, TESForm* a_form)
	{
		return IsT<Settings::bandages>(a_form);
	}


	bool RegisterFuncs(VMClassRegistry* a_registry)
	{
		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("RegisterForBoundWeaponEquippedEvent", "iEquip_FormExt", RegisterForBoundWeaponEquippedEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("UnregisterForBoundWeaponEquippedEvent", "iEquip_FormExt", UnregisterForBoundWeaponEquippedEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("RegisterForBoundWeaponUnequippedEvent", "iEquip_FormExt", RegisterForBoundWeaponUnequippedEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("UnregisterForBoundWeaponUnequippedEvent", "iEquip_FormExt", UnregisterForBoundWeaponUnequippedEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, SInt32, TESForm*>("GetLightDuration", "iEquip_FormExt", GetLightDuration, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, SInt32, TESForm*>("GetLightRadius", "iEquip_FormExt", GetLightRadius, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, TESForm*, SInt32>("SetLightRadius", "iEquip_FormExt", SetLightRadius, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("ResetLightRadius", "iEquip_FormExt", ResetLightRadius, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsSpear", "iEquip_FormExt", IsSpear, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsJavelin", "iEquip_FormExt", IsJavelin, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsGrenade", "iEquip_FormExt", IsGrenade, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsThrowingAxe", "iEquip_FormExt", IsThrowingAxe, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsThrowingKnife", "iEquip_FormExt", IsThrowingKnife, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsWax", "iEquip_FormExt", IsWax, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsOil", "iEquip_FormExt", IsOil, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsSpellWard", "iEquip_FormExt", IsSpellWard, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("HasFire", "iEquip_FormExt", HasFire, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("HasIce", "iEquip_FormExt", HasIce, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("HasShock", "iEquip_FormExt", HasShock, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("HasPoison", "iEquip_FormExt", HasPoison, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsSalve", "iEquip_FormExt", IsSalve, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsBandage", "iEquip_FormExt", IsBandage, a_registry));

		return true;
	}
}
