#pragma once

class Actor;
class TESAmmo;
class TESRace;
class VMClassRegistry;
struct StaticFunctionTag;


namespace ActorExt
{
	TESAmmo*	GetEquippedAmmo(StaticFunctionTag*, Actor* a_actor);
	float		GetAVDamage(StaticFunctionTag*, Actor* a_actor, UInt32 a_actorValue);
	TESRace*	GetBaseRace(StaticFunctionTag*, Actor* a_actor);
	bool		RegisterFuncs(VMClassRegistry* a_registry);
}
