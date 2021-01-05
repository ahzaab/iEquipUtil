#pragma once




namespace ActorExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	namespace
	{
		enum class Hand : uint32_t
		{
			kUnequipped,
			kRight,
			kLeft,
			kDualWield
		};
	}


	float				GetAVDamage(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor, uint32_t a_actorValue);
	RE::TESRace*		GetBaseRace(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor);
	RE::TESAmmo*		GetEquippedAmmo(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor);
	RE::TESObjectWEAP*	GetEquippedWeapon(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor, uint32_t a_hand);
	float				GetMagicEffectMagnitude(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, const RE::EffectSetting* a_mgef);

	bool RegisterFuncs(VM* a_vm);
}
