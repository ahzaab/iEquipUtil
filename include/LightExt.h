#pragma once

#include "RE/Skyrim.h"


namespace LightExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	int32_t	GetLightDuration(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectLIGH* a_light);
	int32_t	GetLightRadius(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectLIGH* a_light);
	void	SetLightRadius(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light, int32_t a_radius);

	bool RegisterFuncs(VM* a_vm);
}
