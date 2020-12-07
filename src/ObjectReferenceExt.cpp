#include "pch.h"
#include "ObjectReferenceExt.h"

#include <vector>


namespace ObjectReferenceExt
{
    RE::TESForm* GetNthFormOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, uint32_t a_type, uint32_t a_n)
    {
        if (!a_container) {
            a_vm->TraceStack("a_container is a NONE form!", a_stackID, Severity::kWarning);
            return 0;
        } else if (a_type > static_cast<uint32_t>(RE::FormType::Max)) {
            logger::warn("a_type is out of range!"sv);
            return 0;
        }

        auto type = static_cast<RE::FormType>(a_type);
        auto inv = a_container->GetInventory([&](RE::TESBoundObject& a_object) -> bool {
            return a_object.Is(type);
        });

        // i expect this function is used to iterate over an inventory, so this needs to be sorted on something that won't change between game sessions
        std::vector<RE::FormID> items;
        for (auto& item : inv) {
            items.push_back(item.first->formID);
        }

        std::sort(items.begin(), items.end());
        return a_n < items.size() ? RE::TESForm::LookupByID(items[a_n]) : 0;
    }


    int32_t GetNumItemsOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, uint32_t a_type)
    {
        if (!a_container) {
            a_vm->TraceStack("a_container is a NONE form!", a_stackID, Severity::kWarning);
            return -1;
        } else if (a_type > static_cast<uint32_t>(RE::FormType::Max)) {
            a_vm->TraceStack("a_type is out of range!", a_stackID, Severity::kWarning);
            return -1;
        }

        auto type = static_cast<RE::FormType>(a_type);
        auto inv = a_container->GetInventory([&](RE::TESBoundObject& a_object) -> bool {
            return a_object.Is(type);
        });

        int32_t count = 0;
        for (auto& item : inv) {
            if (item.second.first > 0) {
                ++count;
            }
        }

        return count;
    }


    bool RegisterFuncs(VM* a_vm)
    {
        a_vm->RegisterFunction("GetNthFormOfType", "iEquip_ObjectReferenceExt", GetNthFormOfType);
        a_vm->RegisterFunction("GetNumItemsOfType", "iEquip_ObjectReferenceExt", GetNumItemsOfType);

        return true;
    }
}
