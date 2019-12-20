#pragma once

#include "RE/Skyrim.h"


namespace ActorExt
{
	namespace
	{
		enum class Hand : UInt32
		{
			kUnequipped,
			kRight,
			kLeft,
			kDualWield
		};
	}


	float				GetAVDamage(RE::StaticFunctionTag*, RE::Actor* a_actor, UInt32 a_actorValue);
	RE::TESRace*		GetBaseRace(RE::StaticFunctionTag*, RE::Actor* a_actor);
	RE::TESAmmo*		GetEquippedAmmo(RE::StaticFunctionTag*, RE::Actor* a_actor);
	RE::TESObjectWEAP*	GetEquippedWeapon(RE::StaticFunctionTag*, RE::Actor* a_actor, UInt32 a_hand);
	float				GetMagicEffectMagnitude(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::EffectSetting* a_mgef);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
