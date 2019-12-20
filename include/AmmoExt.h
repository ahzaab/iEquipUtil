#pragma once

#include "RE/Skyrim.h"


namespace AmmoExt
{
	bool IsAmmoBound(RE::StaticFunctionTag*, RE::TESAmmo* a_ammo);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
