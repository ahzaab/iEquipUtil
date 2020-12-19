#include "pch.h"
#include "InventoryExt.h"

#include <map>
#include <string>
#include <utility>

#include "RefHandleManager.h"
#include "Registration.h"

namespace InventoryExt
{
    namespace
    {
        using EntryData = RefHandleManager::EntryData;

        enum class EquipSlot : uint32_t
        {
            kDefault = 0,
            kRight = 1,
            kLeft = 2
        };


        enum class XEquipSlot : uint32_t
        {
            kLeftHand = 0,
            kRightHand = 1,
            kShield = 2,
            kHead = 3,
            kChest = 4,
            kBoots = 5,
            kGloves = 6
        };


        std::optional<EntryData> LookupEntry(VM* a_vm, StackID a_stackID, RE::TESForm* a_item, uint32_t a_refHandle)
        {
            auto manager = RefHandleManager::GetSingleton();
            auto result = manager->LookupEntry(a_item, static_cast<RefHandleManager::RefHandle>(a_refHandle));
            if (!result) {
                a_vm->TraceStack("Failed to find item!", a_stackID, Severity::kError);
            }
            return result;
        }


        std::optional<RE::BGSEquipSlot*> GetSlotByID(VM* a_vm, StackID a_stackID, EquipSlot a_equipSlot, const RE::TESForm* a_item, bool a_worn, bool a_wornLeft)
        {
            using Object = RE::DEFAULT_OBJECT;

            auto              dobj = RE::BGSDefaultObjectManager::GetSingleton();
            RE::BGSEquipSlot* slotOut = 0;

            switch (a_equipSlot) {
            case EquipSlot::kDefault:
                if (a_item->IsWeapon()) {
                    a_vm->TraceStack("a_equipSlot is an invalid slot for a_item!", a_stackID, Severity::kWarning);
                    return std::nullopt;
                }

                if (a_worn) {
                    a_vm->TraceStack("a_item is already worn!", a_stackID, Severity::kWarning);
                    return std::nullopt;
                }

                slotOut = 0;
                break;
            case EquipSlot::kRight:
                if (!a_item->IsWeapon()) {
                    a_vm->TraceStack("a_equipSlot is an invalid slot for a_item!", a_stackID, Severity::kWarning);
                    return std::nullopt;
                }

                if (a_worn) {
                    a_vm->TraceStack("a_item is already worn!", a_stackID, Severity::kWarning);
                    return std::nullopt;
                }

                slotOut = dobj->GetObject<RE::BGSEquipSlot>(Object::kRightHandEquip);
                break;
            case EquipSlot::kLeft:
                if (!a_item->IsWeapon()) {
                    a_vm->TraceStack("a_equipSlot is an invalid slot for a_item!", a_stackID, Severity::kWarning);
                    return std::nullopt;
                }

                if (a_wornLeft) {
                    a_vm->TraceStack("a_item is already worn!", a_stackID, Severity::kWarning);
                    return std::nullopt;
                }

                slotOut = dobj->GetObject<RE::BGSEquipSlot>(Object::kLeftHandEquip);
                break;
            default:
                a_vm->VTraceStack("a_equipSlot is not a slot (%i)!", a_stackID, Severity::kWarning, a_equipSlot);
                return std::nullopt;
            }

            return std::make_optional(slotOut);
        }


        bool GetWornObjectFilters(VM* a_vm, StackID a_stackID, XEquipSlot a_equipSlot, RE::ExtraDataType& a_xDataType, RE::FormType& a_formType, RE::BGSBipedObjectForm::FirstPersonFlag& a_firstPersonFlag)
        {
            using FPFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

            switch (a_equipSlot) {
            case XEquipSlot::kHead:
            case XEquipSlot::kChest:
            case XEquipSlot::kBoots:
            case XEquipSlot::kGloves:
            case XEquipSlot::kRightHand:
            case XEquipSlot::kShield:
                a_xDataType = RE::ExtraDataType::kWorn;
                break;
            case XEquipSlot::kLeftHand:
                a_xDataType = RE::ExtraDataType::kWornLeft;
                break;
            default:
                a_vm->TraceStack("a_equipSlot is an invalid slot ID!", a_stackID, Severity::kWarning);
                return false;
            }

            switch (a_equipSlot) {
            case XEquipSlot::kHead:
            case XEquipSlot::kChest:
            case XEquipSlot::kBoots:
            case XEquipSlot::kGloves:
            case XEquipSlot::kShield:
                a_formType = RE::FormType::Armor;
                break;
            case XEquipSlot::kRightHand:
            case XEquipSlot::kLeftHand:
                a_formType = RE::FormType::Weapon;
                break;
            default:
                a_vm->TraceStack("a_equipSlot is an invalid slot ID!", a_stackID, Severity::kWarning);
                return false;
            }

            switch (a_equipSlot) {
            case XEquipSlot::kHead:
                a_firstPersonFlag = FPFlag::kCirclet;
                break;
            case XEquipSlot::kChest:
                a_firstPersonFlag = FPFlag::kBody;
                break;
            case XEquipSlot::kBoots:
                a_firstPersonFlag = FPFlag::kFeet;
                break;
            case XEquipSlot::kGloves:
                a_firstPersonFlag = FPFlag::kHands;
                break;
            case XEquipSlot::kRightHand:
            case XEquipSlot::kLeftHand:
                a_firstPersonFlag = FPFlag::kNone;
                break;
            case XEquipSlot::kShield:
                a_firstPersonFlag = FPFlag::kShield;
                break;
            default:
                a_vm->TraceStack("a_equipSlot is an invalid slot ID!", a_stackID, Severity::kWarning);
                return false;
            }

            return true;
        }
    }


    void RegisterForOnRefHandleActiveEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
    {
        if (!a_thisForm) {
            a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
            return;
        }

        auto regs = OnRefHandleActiveRegSet::GetSingleton();
        regs->Register(a_thisForm);
    }


    void UnregisterForOnRefHandleActiveEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
    {
        if (!a_thisForm) {
            a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
            return;
        }

        auto regs = OnRefHandleActiveRegSet::GetSingleton();
        regs->Unregister(a_thisForm);
    }


    void RegisterForOnRefHandleInvalidatedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
    {
        if (!a_thisForm) {
            a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
            return;
        }

        auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
        regs->Register(a_thisForm);
    }


    void UnregisterForOnRefHandleInvalidatedEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_thisForm)
    {
        if (!a_thisForm) {
            a_vm->TraceStack("a_thisForm is a NONE form!", a_stackID, Severity::kWarning);
            return;
        }

        auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
        regs->Unregister(a_thisForm);
    }


    void ClearAllRefHandles([[maybe_unused]] VM* a_vm, [[maybe_unused]] StackID a_stackID, RE::StaticFunctionTag*)
    {
        auto regs = RefHandleManager::GetSingleton();
        regs->Clear();
    }


    void EquipItem(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle, RE::Actor* a_actor, uint32_t a_equipSlot, bool a_preventUnequip, bool a_equipSound)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (!a_actor) {
            a_vm->TraceStack("a_actor is a NONE form!", a_stackID, Severity::kWarning);
            return;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return;
        }

        bool    worn = false;
        bool    wornLeft = false;
        int32_t count = 1;
        if (entryData->extraList) {
            auto xCount = entryData->extraList->GetByType<RE::ExtraCount>();
            if (xCount) {
                count = xCount->count;
            }

            worn = entryData->extraList->HasType(RE::ExtraDataType::kWorn);
            wornLeft = entryData->extraList->HasType(RE::ExtraDataType::kWornLeft);
        }

        auto equipSlot = static_cast<EquipSlot>(a_equipSlot);
        auto slot = GetSlotByID(a_vm, a_stackID, equipSlot, a_item, worn, wornLeft);
        if (!slot) {
            return;
        }

        int32_t countReq = (worn || wornLeft) ? 2 : 1;
        if (count < countReq) {
            a_vm->TraceStack("Item count is too small to equip!", a_stackID, Severity::kError);
            return;
        }

        int32_t            equipCount;
        RE::ExtraDataList* extraList;
        if (a_item->IsAmmo()) {
            equipCount = count;
            extraList = 0;
        } else {
            equipCount = 1;
            extraList = entryData->extraList;
        }

        auto equipManager = RE::ActorEquipManager::GetSingleton();
        auto a_object = static_cast<RE::TESBoundAnimObject*>(a_item);
        equipManager->EquipObject(a_actor, a_object, extraList, equipCount, *slot, a_equipSound, a_preventUnequip, false, 0);
    }


    RE::EnchantmentItem* GetEnchantment(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return nullptr;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return nullptr;
        }

        auto xEnch = entryData->extraList->GetByType<RE::ExtraEnchantment>();
        if (xEnch) {
            return xEnch->enchantment;
        }

        auto enchForm = a_item->As<RE::TESEnchantableForm>();
        return enchForm ? enchForm->formEnchanting : 0;
    }


    RE::BSFixedString GetLongName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return "";
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return "";
        }

        //entryData->invEntryData->GetDisplayName(); // unused?
        auto xText = entryData->extraList->GetByType<RE::ExtraTextDisplayData>();
        if (xText) {
            return xText->displayName;
        } else {
            auto fullName = a_item->As<RE::TESFullName>();
            return fullName ? fullName->fullName : "";
        }
    }


    RE::AlchemyItem* GetPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return 0;
        } else if (!a_item->IsWeapon()) {
            a_vm->TraceStack("a_item is not a weapon!", a_stackID, Severity::kWarning);
            return 0;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return 0;
        }

        auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
        return xPoison ? xPoison->poison : 0;
    }


    int32_t GetPoisonCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return 0;
        } else if (!a_item->IsWeapon()) {
            a_vm->TraceStack("a_item is not a weapon!", a_stackID, Severity::kWarning);
            return 0;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return 0;
        }

        auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
        return xPoison ? xPoison->count : 0;
    }


    uint32_t GetRefHandleAtInvIndex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_index)
    {
        auto ui = RE::UI::GetSingleton();
        auto invMenu = ui->GetMenu<RE::InventoryMenu>();
        if (!invMenu) {
            a_vm->TraceStack("Inventory menu is not open!", a_stackID, Severity::kWarning);
            return RefHandleManager::kInvalidRefHandle;
        } else if (!invMenu->itemList) {
            a_vm->TraceStack("Inventory menu is has no item list!", a_stackID, Severity::kWarning);
            return RefHandleManager::kInvalidRefHandle;
        }

        auto& items = invMenu->itemList->items;
        if (a_index >= items.size()) {
            a_vm->TraceStack("Index is out of range!", a_stackID, Severity::kWarning);
            return RefHandleManager::kInvalidRefHandle;
        }

        auto item = items[a_index];
        if (!item) {
            a_vm->TraceStack("Failed to find item at index!", a_stackID, Severity::kError);
            return RefHandleManager::kInvalidRefHandle;
        } else if (!item->data.objDesc) {
            a_vm->TraceStack("Item did not have a entry data!", a_stackID, Severity::kError);
            return RefHandleManager::kInvalidRefHandle;
        } else if (!item->data.objDesc->extraLists) {
            a_vm->TraceStack("Item did not have extra data!", a_stackID, Severity::kError);
            return RefHandleManager::kInvalidRefHandle;
        }

        for (auto& xList : *item->data.objDesc->extraLists) {
            auto xID = xList->GetByType<RE::ExtraUniqueID>();
            if (xID) {
                auto manager = RefHandleManager::GetSingleton();
                auto handle = manager->LookupHandle(xID->uniqueID);
                if (handle == RefHandleManager::kInvalidRefHandle) {
                    a_vm->TraceStack("Could not find handle for unique ID!", a_stackID, Severity::kError);
                }
                return handle;
            }
        }

        a_vm->TraceStack("Could not find unique ID for item!", a_stackID, Severity::kError);
        return RefHandleManager::kInvalidRefHandle;
    }


    uint32_t GetRefHandleFromWornObject(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_equipSlot)
    {
        using FPFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

        uint32_t          handle = RefHandleManager::kInvalidRefHandle;
        RE::ExtraDataType xDataType;
        RE::FormType      formType;
        FPFlag            fpFlag;
        auto              xEquipSlot = static_cast<XEquipSlot>(a_equipSlot);
        if (!GetWornObjectFilters(a_vm, a_stackID, xEquipSlot, xDataType, formType, fpFlag)) {
            return handle;
        }

        auto player = RE::PlayerCharacter::GetSingleton();
        auto inv = player->GetInventory([&](RE::TESBoundObject& a_object) -> bool {
            if (a_object.Is(formType)) {
                switch (xEquipSlot) {
                case XEquipSlot::kHead:
                case XEquipSlot::kChest:
                case XEquipSlot::kBoots:
                case XEquipSlot::kGloves:
                case XEquipSlot::kShield:
                {
                    auto armor = a_object.As<RE::TESObjectARMO>();
                    return armor->HasPartOf(fpFlag);
                }
                default:
                    return true;
                }
            } else {
                return false;
            }
        });

        for (auto& item : inv) {
            auto& entryData = item.second.second;
            if (entryData->extraLists) {
                for (auto& xList : *entryData->extraLists) {
                    if (xList->HasType(xDataType)) {
                        auto xID = xList->GetByType<RE::ExtraUniqueID>();
                        if (xID) {
                            auto manager = RefHandleManager::GetSingleton();
                            return manager->LookupHandle(xID->uniqueID);
                        }
                    }
                }
            }
        }

        a_vm->TraceStack("Failed to find ref handle!", a_stackID, Severity::kError);
        return RefHandleManager::kInvalidRefHandle;
    }


    RE::BSFixedString GetShortName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return "";
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return "";
        }

        //entryData->invEntryData->GetDisplayName(); // unused?
        auto xText = entryData->extraList->GetByType<RE::ExtraTextDisplayData>();
        if (xText && xText->IsPlayerSet()) {
            std::string name(xText->displayName.c_str(), xText->customNameLength);
            return name.c_str();
        } else {
            auto fullName = a_item->As<RE::TESFullName>();
            return fullName ? fullName->fullName : "";
        }
    }

    void InitializeInventory([[maybe_unused]] VM* a_vm, [[maybe_unused]] StackID a_stackID, RE::StaticFunctionTag*)
    {
        auto task = SKSE::GetTaskInterface();
        task->AddTask([]() -> void {
            auto manager = RefHandleManager::GetSingleton();
            auto player = RE::PlayerCharacter::GetSingleton();
            auto invChanges = player->GetInventoryChanges();
            auto inv = player->GetInventory([&](RE::TESBoundObject& a_object) -> bool {
                return manager->IsTrackedType(&a_object);
            });


            for (auto& elem : inv) {
                auto  item = elem.first;
                auto  rawCount = elem.second.first;
                auto& entryData = elem.second.second;

                while (rawCount-- > 0) {
                    if (std::find_if(invChanges->entryList->begin(), invChanges->entryList->end(), [item](const RE::InventoryEntryData* arg) {
                            return arg->GetObject() == item;
                        }) == invChanges->entryList->end()) {
                        auto entry = new RE::InventoryEntryData(item->As<RE::TESBoundObject>(), 0);

                        invChanges->AddEntryData(entry);
                        if (entry->extraLists->empty()) {
                            auto xList = new RE::ExtraDataList();
                            //xList->SetInventoryChanges(invChanges);
                            entry->extraLists->push_front(xList);
                        }
                    }
                }
            }
        });
    }


    void ParseInventory([[maybe_unused]] VM* a_vm, [[maybe_unused]] StackID a_stackID, RE::StaticFunctionTag*)
    {
        InventoryExt::InitializeInventory(a_vm, a_stackID, nullptr);
        auto task = SKSE::GetTaskInterface();
        task->AddTask([]() -> void {
            auto manager = RefHandleManager::GetSingleton();
            auto player = RE::PlayerCharacter::GetSingleton();
            auto inv = player->GetInventory([&](RE::TESBoundObject& a_object) -> bool {
                return manager->IsTrackedType(&a_object);
            });


            for (auto& elem : inv) {
                auto  item = elem.first;
                auto  rawCount = elem.second.first;
                auto& entryData = elem.second.second;

                if (entryData->extraLists) {
                    for (auto& xList : *entryData->extraLists) {
                        if (xList) {
                            auto count = xList->GetCount();
                            rawCount -= count;
                            manager->ActivateAndDispatch(item, xList, count);
                        }
                    }
                }

                RE::ExtraDataList* xList;
                while (rawCount-- > 0) {
                    xList = 0;
                    manager->ActivateAndDispatch(item, xList, 1);
                    entryData->AddExtraList(xList);
                }
            }

            manager->SetInit();
        });
    }


    void RemovePoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (!a_item->IsWeapon()) {
            a_vm->TraceStack("a_item is not a weapon!", a_stackID, Severity::kWarning);
            return;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return;
        }

        auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
        if (xPoison) {
            entryData->extraList->Remove(xPoison);
            delete xPoison;
        }
    }


    void SetPoison(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle, RE::AlchemyItem* a_newPoison, uint32_t a_newCount)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (!a_item->IsWeapon()) {
            a_vm->TraceStack("a_item is not a weapon!", a_stackID, Severity::kWarning);
            return;
        } else if (!a_newPoison) {
            a_vm->TraceStack("a_newPoison is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (!a_newPoison->IsPoison()) {
            a_vm->TraceStack("a_newPoison is not a poison!", a_stackID, Severity::kWarning);
            return;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return;
        }

        auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
        if (!xPoison) {
            xPoison = new RE::ExtraPoison(a_newPoison, a_newCount);
            entryData->extraList->Add(xPoison);
        }
        xPoison->poison = a_newPoison;
        xPoison->count = a_newCount;
    }


    void SetPoisonCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_item, uint32_t a_refHandle, uint32_t a_newCount)
    {
        if (!a_item) {
            a_vm->TraceStack("a_item is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (!a_item->IsWeapon()) {
            a_vm->TraceStack("a_item is not a weapon!", a_stackID, Severity::kWarning);
            return;
        }

        auto entryData = LookupEntry(a_vm, a_stackID, a_item, a_refHandle);
        if (!entryData) {
            return;
        }

        auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
        if (xPoison) {
            xPoison->count = a_newCount;
        }
    }


    bool RegisterFuncs(VM* a_vm)
    {
        a_vm->RegisterFunction("RegisterForOnRefHandleActiveEvent", "iEquip_InventoryExt", RegisterForOnRefHandleActiveEvent, true);
        a_vm->RegisterFunction("UnregisterForOnRefHandleActiveEvent", "iEquip_InventoryExt", UnregisterForOnRefHandleActiveEvent, true);
        a_vm->RegisterFunction("RegisterForOnRefHandleInvalidatedEvent", "iEquip_InventoryExt", RegisterForOnRefHandleInvalidatedEvent, true);
        a_vm->RegisterFunction("UnregisterForOnRefHandleInvalidatedEvent", "iEquip_InventoryExt", UnregisterForOnRefHandleInvalidatedEvent, true);
        a_vm->RegisterFunction("ClearAllRefHandles", "iEquip_InventoryExt", ClearAllRefHandles);
        a_vm->RegisterFunction("EquipItem", "iEquip_InventoryExt", EquipItem);
        a_vm->RegisterFunction("GetEnchantment", "iEquip_InventoryExt", GetEnchantment);
        a_vm->RegisterFunction("GetLongName", "iEquip_InventoryExt", GetLongName);
        a_vm->RegisterFunction("GetPoison", "iEquip_InventoryExt", GetPoison);
        a_vm->RegisterFunction("GetPoisonCount", "iEquip_InventoryExt", GetPoisonCount);
        a_vm->RegisterFunction("GetRefHandleAtInvIndex", "iEquip_InventoryExt", GetRefHandleAtInvIndex);
        a_vm->RegisterFunction("GetRefHandleFromWornObject", "iEquip_InventoryExt", GetRefHandleFromWornObject);
        a_vm->RegisterFunction("GetShortName", "iEquip_InventoryExt", GetShortName);
        a_vm->RegisterFunction("ParseInventory", "iEquip_InventoryExt", ParseInventory);
        a_vm->RegisterFunction("RemovePoison", "iEquip_InventoryExt", RemovePoison);
        a_vm->RegisterFunction("SetPoison", "iEquip_InventoryExt", SetPoison);
        a_vm->RegisterFunction("SetPoisonCount", "iEquip_InventoryExt", SetPoisonCount);

        return true;
    }
}
