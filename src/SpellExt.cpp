#include "SpellExt.h"


namespace SpellExt
{
	SInt32 GetBoundSpellWeapType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::SpellItem* a_spell)
	{
		using Archetype = RE::EffectArchetypes::ArchetypeID;
		using AV = RE::ActorValue;

		if (!a_spell) {
			a_vm->TraceStack("a_spell is a NONE form!", a_stackID, Severity::kError);
			return -1;
		}

		for (auto& effect : a_spell->effects) {
			if (effect && effect->baseEffect) {
				auto& data = effect->baseEffect->data;
				if (data.associatedSkill == AV::kConjuration && data.archetype == Archetype::kBoundWeapon) {
					auto item = data.associatedForm;
					if (item && item->IsWeapon()) {
						auto weap = static_cast<RE::TESObjectWEAP*>(item);
						if (weap->IsBound()) {
							return static_cast<SInt32>(weap->weaponData.animationType);
						}
					}
				}
			}
		}

		return -1;
	}


	bool IsBoundSpell(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::SpellItem* a_spell)
	{
		using Archetype = RE::EffectArchetypes::ArchetypeID;
		using AV = RE::ActorValue;

		if (!a_spell) {
			a_vm->TraceStack("a_spell is a NONE form!", a_stackID, Severity::kError);
			return false;
		}

		for (auto& effect : a_spell->effects) {
			if (effect && effect->baseEffect) {
				auto& data = effect->baseEffect->data;
				if (data.associatedSkill == AV::kConjuration && data.archetype == Archetype::kBoundWeapon) {
					return true;
				}
			}
		}

		return false;
	}


	bool IsHealingSpell(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::SpellItem* a_spell)
	{
		using AV = RE::ActorValue;

		if (!a_spell) {
			a_vm->TraceStack("a_spell is a NONE form!", a_stackID, Severity::kError);
			return false;
		}

		for (auto& effect : a_spell->effects) {
			if (effect && effect->baseEffect) {
				auto& data = effect->baseEffect->data;
				if (data.associatedSkill == AV::kRestoration && data.primaryAV == AV::kHealth) {
					return true;
				}
			}
		}

		return false;
	}


	bool RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("GetBoundSpellWeapType", "iEquip_SpellExt", GetBoundSpellWeapType, true);
		a_vm->RegisterFunction("IsBoundSpell", "iEquip_SpellExt", IsBoundSpell, true);
		a_vm->RegisterFunction("IsHealingSpell", "iEquip_SpellExt", IsHealingSpell, true);

		return true;
	}
}
