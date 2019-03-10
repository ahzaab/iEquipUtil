#pragma once

struct StaticFunctionTag;
class TESAmmo;
class VMClassRegistry;


bool IsAmmoBound(StaticFunctionTag* a_base, TESAmmo* a_ammo);


namespace AmmoExt
{
	bool RegisterFuncs(VMClassRegistry* a_registry);
}
