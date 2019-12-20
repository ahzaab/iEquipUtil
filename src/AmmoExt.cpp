#include "AmmoExt.h"


namespace AmmoExt
{
	bool IsAmmoBound(RE::StaticFunctionTag*, RE::TESAmmo* a_ammo)
	{
		using Object = RE::BGSDefaultObjectManager::DefaultObject;

		if (!a_ammo) {
			_WARNING("a_ammo is a NONE form!");
			return false;
		}

		auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
		auto weapTypeBoundArrow = dobj->GetObject<RE::BGSKeyword>(Object::kWeapTypeBoundArrow);
		return a_ammo->HasKeyword(weapTypeBoundArrow);
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("IsAmmoBound", "iEquip_AmmoExt", IsAmmoBound);

		return true;
	}
}
