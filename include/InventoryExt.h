#pragma once

#include "RE/Skyrim.h"


namespace InventoryExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	void	RegisterForOnRefHandleActiveEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);
	void	UnregisterForOnRefHandleActiveEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);

	void	RegisterForOnRefHandleInvalidatedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);
	void	UnregisterForOnRefHandleInvalidatedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);

	void					ClearAllRefHandles(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*);
	void					EquipItem(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, RE::Actor* a_actor, UInt32 a_equipSlot, bool a_preventUnequip, bool a_equipSound);
	RE::EnchantmentItem*	GetEnchantment(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	RE::BSFixedString		GetLongName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	RE::AlchemyItem*		GetPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	SInt32					GetPoisonCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	UInt32					GetRefHandleAtInvIndex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, UInt32 a_index);
	UInt32					GetRefHandleFromWornObject(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, UInt32 a_equipSlot);
	RE::BSFixedString		GetShortName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	void					ParseInventory(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*);
	void					RemovePoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle);
	void					SetPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, RE::AlchemyItem* a_newPoison, UInt32 a_newCount);
	void					SetPoisonCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, UInt32 a_newCount);

	bool RegisterFuncs(VM* a_vm);
}
