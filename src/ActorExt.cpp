#include "ActorExt.h"

#include <cmath>
#include <cstdint>

#include "Registration.h"


namespace ActorExt
{
	float GetAVDamage(RE::StaticFunctionTag*, RE::Actor* a_actor, UInt32 a_actorValue)
	{
		using Modifier = RE::Actor::ActorValueModifiers::Modifier;

		if (!a_actor) {
			_WARNING("a_actor is a NONE form!");
			return 0.0;
		} else if (a_actorValue >= to_underlying(RE::ActorValue::kTotal)) {
			_WARNING("a_actorValue is out of range!");
			return 0.0;
		}

		auto actorValue = static_cast<RE::ActorValue>(a_actorValue);
		switch (actorValue) {
		case RE::ActorValue::kHealth:
			return a_actor->avHealth.modifiers[Modifier::kDamage];
		case RE::ActorValue::kMagicka:
			return a_actor->avMagicka.modifiers[Modifier::kDamage];
		case RE::ActorValue::kStamina:
			return a_actor->avStamina.modifiers[Modifier::kDamage];
		case RE::ActorValue::kVoicePoints:
			return a_actor->avVoicePoints.modifiers[Modifier::kDamage];
		default:
			{
				auto mod = a_actor->avMap.modifiers[actorValue];
				return mod ? mod->modifiers[Modifier::kDamage] : 0.0;
			}
		}
	}


	RE::TESRace* GetBaseRace(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			_WARNING("a_actor is a NONE form!");
			return 0;
		}

		if (a_actor->IsPlayerRef()) {
			auto player = RE::PlayerCharacter::GetSingleton();
			return player->baseRace;
		} else {
			auto npc = a_actor->GetActorBase();
			return npc ? npc->race : 0;
		}
	}


	RE::TESAmmo* GetEquippedAmmo(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			_WARNING("a_actor is a NONE form!");
			return 0;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto inv = player->GetInventory([](RE::TESBoundObject* a_object) -> bool
		{
			return a_object->IsAmmo();
		});

		for (auto& elem : inv) {
			auto entryData = elem.second.second;
			if (entryData->extraLists) {
				for (auto& xList : *entryData->extraLists) {
					if (xList && xList->HasType<RE::ExtraWorn>()) {
						return static_cast<RE::TESAmmo*>(elem.first);
					}
				}
			}
		}

		return 0;
	}


	RE::TESObjectWEAP* GetEquippedWeapon(RE::StaticFunctionTag*, RE::Actor* a_actor, UInt32 a_hand)
	{
		using EquippedHand = RE::AIProcess::Hand;

		if (!a_actor) {
			_WARNING("a_actor is a NONE form!");
			return 0;
		} else if (!a_actor->aiProcess) {
			_WARNING("Actor does not have an AI process!");
			return 0;
		}

		EquippedHand equipHand;
		switch (static_cast<Hand>(a_hand)) {
		case Hand::kRight:
			equipHand = EquippedHand::kRight;
			break;
		case Hand::kLeft:
			equipHand = EquippedHand::kLeft;
			break;
		default:
			_WARNING("Invalid slot (%i)!", a_hand);
			return 0;
		}

		auto item = a_actor->aiProcess->equippedObjects[equipHand];
		return (item && item->IsWeapon()) ? static_cast<RE::TESObjectWEAP*>(item) : 0;
	}


	float GetMagicEffectMagnitude(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::EffectSetting* a_mgef)
	{
		if (!a_actor) {
			_WARNING("a_actor is a NONE form!");
			return 0.0;
		} else if (!a_mgef) {
			_WARNING("a_mgef is a NONE form!");
			return 0.0;
		}

		auto activeEffects = a_actor->GetActiveEffects();
		if (!activeEffects) {
			return 0.0;
		}

		for (auto& activeEffect : *activeEffects) {
			if (activeEffect->GetBaseObject() == a_mgef) {
				return activeEffect->magnitude;
			}
		}
		return 0.0;
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetAVDamage", "iEquip_ActorExt", GetAVDamage);
		a_vm->RegisterFunction("GetBaseRace", "iEquip_ActorExt", GetBaseRace);
		a_vm->RegisterFunction("GetEquippedAmmo", "iEquip_ActorExt", GetEquippedAmmo);
		a_vm->RegisterFunction("GetEquippedWeapon", "iEquip_ActorExt", GetEquippedWeapon);
		a_vm->RegisterFunction("GetMagicEffectMagnitude", "iEquip_ActorExt", GetMagicEffectMagnitude);

		return true;
	}
}
