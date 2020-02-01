#include "WeaponExt.h"


namespace WeaponExt
{
	bool IsWeaponBound(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectWEAP* a_weap)
	{
		if (!a_weap) {
			a_vm->TraceStack("a_weap is a NONE form!", a_stackID, Severity::kWarning);
			return false;
		}

		return a_weap->IsBound();
	}


	bool RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("IsWeaponBound", "iEquip_WeaponExt", IsWeaponBound, true);

		return true;
	}
}
