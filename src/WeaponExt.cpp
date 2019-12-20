#include "WeaponExt.h"


namespace WeaponExt
{
	bool IsWeaponBound(RE::StaticFunctionTag*, RE::TESObjectWEAP* a_weap)
	{
		if (!a_weap) {
			_WARNING("a_weap is a NONE form!");
			return false;
		}

		return a_weap->IsBound();
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("IsWeaponBound", "iEquip_WeaponExt", IsWeaponBound);

		return true;
	}
}
