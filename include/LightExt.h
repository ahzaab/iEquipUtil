#pragma once

#include "RE/Skyrim.h"


namespace LightExt
{
	SInt32	GetLightDuration(RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light);
	SInt32	GetLightRadius(RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light);
	void	SetLightRadius(RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light, SInt32 a_radius);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
