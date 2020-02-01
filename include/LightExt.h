#pragma once

#include "RE/Skyrim.h"


namespace LightExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	SInt32	GetLightDuration(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectLIGH* a_light);
	SInt32	GetLightRadius(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectLIGH* a_light);
	void	SetLightRadius(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light, SInt32 a_radius);

	bool RegisterFuncs(VM* a_vm);
}
