#pragma once

namespace InventoryExt
{
    using VM = RE::BSScript::IVirtualMachine;
    using StackID = RE::VMStackID;
    using Severity = RE::BSScript::ErrorLogger::Severity;


    void RegisterForOnRefHandleActiveEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);
    void UnregisterForOnRefHandleActiveEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);

    void RegisterForOnRefHandleInvalidatedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);
    void UnregisterForOnRefHandleInvalidatedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm);

    void                 ClearAllRefHandles(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*);
    void                 EquipItem(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle, RE::Actor* a_actor, uint32_t a_equipSlot, bool a_preventUnequip, bool a_equipSound);
    RE::EnchantmentItem* GetEnchantment(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle);
    RE::BSFixedString    GetLongName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle);
    RE::AlchemyItem*     GetPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle);
    int32_t              GetPoisonCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle);
    uint32_t             GetRefHandleAtInvIndex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_index);
    uint32_t             GetRefHandleFromWornObject(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_equipSlot);
    RE::BSFixedString    GetShortName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle);
    void                 ParseInventory(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*);
    void                 RemovePoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle);
    void                 SetPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle, RE::AlchemyItem* a_newPoison, uint32_t a_newCount);
    void                 SetPoisonCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle, uint32_t a_newCount);

    bool RegisterFuncs(VM* a_vm);
}
