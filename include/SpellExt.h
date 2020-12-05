#pragma once

#include "RE/Skyrim.h"


namespace SpellExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	int32_t	GetBoundSpellWeapType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::SpellItem* a_spell);
	bool	IsBoundSpell(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::SpellItem* a_spell);
	bool	IsHealingSpell(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::SpellItem* a_spell);

	bool RegisterFuncs(VM* a_vm);
}
