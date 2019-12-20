#include "SpellExt.h"


namespace SpellExt
{
	SInt32 GetBoundSpellWeapType(RE::StaticFunctionTag*, RE::SpellItem* a_spell)
	{
		using Archetype = RE::EffectSetting::Data::Archetype;
		using AV = RE::ActorValue;

		if (!a_spell) {
			_WARNING("a_spell is a NONE form!");
			return -1;
		}

		for (auto& effect : a_spell->effects) {
			if (effect && effect->baseEffect) {
				auto& data = effect->baseEffect->data;
				if (data.magicSkill == AV::kConjuration && data.archetype == Archetype::kBoundWeapon) {
					auto item = data.associatedItem;
					if (item && item->IsWeapon()) {
						auto weap = static_cast<RE::TESObjectWEAP*>(item);
						if (weap->IsBound()) {
							return static_cast<SInt32>(weap->data.animationType);
						}
					}
				}
			}
		}

		return -1;
	}


	bool IsBoundSpell(RE::StaticFunctionTag*, RE::SpellItem* a_spell)
	{
		using Archetype = RE::EffectSetting::Data::Archetype;
		using AV = RE::ActorValue;

		if (!a_spell) {
			_WARNING("a_spell is a NONE form!");
			return false;
		}

		for (auto& effect : a_spell->effects) {
			if (effect && effect->baseEffect) {
				auto& data = effect->baseEffect->data;
				if (data.magicSkill == AV::kConjuration && data.archetype == Archetype::kBoundWeapon) {
					return true;
				}
			}
		}

		return false;
	}


	bool IsHealingSpell(RE::StaticFunctionTag*, RE::SpellItem* a_spell)
	{
		using AV = RE::ActorValue;

		if (!a_spell) {
			_WARNING("a_spell is a NONE form!");
			return false;
		}

		for (auto& effect : a_spell->effects) {
			if (effect && effect->baseEffect) {
				auto& data = effect->baseEffect->data;
				if (data.magicSkill == AV::kRestoration && data.primaryActorValue == AV::kHealth) {
					return true;
				}
			}
		}

		return false;
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetBoundSpellWeapType", "iEquip_SpellExt", GetBoundSpellWeapType);
		a_vm->RegisterFunction("IsBoundSpell", "iEquip_SpellExt", IsBoundSpell);
		a_vm->RegisterFunction("IsHealingSpell", "iEquip_SpellExt", IsHealingSpell);

		return true;
	}
}
