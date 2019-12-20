#pragma once

#include "RE/Skyrim.h"


namespace WeaponExt
{
	bool IsWeaponBound(RE::StaticFunctionTag*, RE::TESObjectWEAP* a_weap);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
