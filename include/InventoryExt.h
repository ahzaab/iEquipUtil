#pragma once

#include "RE/Skyrim.h"


namespace InventoryExt
{
	void	RegisterForOnRefHandleActiveEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);
	void	UnregisterForOnRefHandleActiveEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);

	void	RegisterForOnRefHandleInvalidatedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);
	void	UnregisterForOnRefHandleInvalidatedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm);

	void					EquipItem(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, RE::Actor* a_actor, UInt32 a_equipSlot, bool a_preventUnequip, bool a_equipSound);
	RE::EnchantmentItem*	GetEnchantment(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	RE::BSFixedString		GetLongName(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	RE::AlchemyItem*		GetPoison(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	SInt32					GetPoisonCount(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	UInt32					GetRefHandleAtInvIndex(RE::StaticFunctionTag*, UInt32 a_index);
	UInt32					GetRefHandleFromWornObject(RE::StaticFunctionTag*, UInt32 a_equipSlot);
	RE::BSFixedString		GetShortName(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	void					ParseInventory(RE::StaticFunctionTag*);
	void					RemovePoison(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	void					SetPoison(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, RE::AlchemyItem* a_newPoison, UInt32 a_newCount);
	void					SetPoisonCount(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, UInt32 a_newCount);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
