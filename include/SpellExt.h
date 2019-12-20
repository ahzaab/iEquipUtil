#pragma once

#include "RE/Skyrim.h"


namespace SpellExt
{
	SInt32	GetBoundSpellWeapType(RE::StaticFunctionTag*, RE::SpellItem* a_spell);
	bool	IsBoundSpell(RE::StaticFunctionTag*, RE::SpellItem* a_spell);
	bool	IsHealingSpell(RE::StaticFunctionTag*, RE::SpellItem* a_spell);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
