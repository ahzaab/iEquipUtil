#include "ObjectReferenceExt.h"

#include <vector>


namespace ObjectReferenceExt
{
	RE::TESForm* GetNthFormOfType(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, UInt32 a_type, UInt32 a_n)
	{
		if (!a_container) {
			_WARNING("a_container is a NONE form!");
			return 0;
		} else if (a_type > to_underlying(RE::FormType::Max)) {
			_WARNING("a_type is out of range!");
			return 0;
		}

		auto type = static_cast<RE::FormType>(a_type);
		auto inv = a_container->GetInventory([&](RE::TESBoundObject* a_object) -> bool
		{
			return a_object->Is(type);
		});

		// i expect this function is used to iterate over an inventory, so this needs to be sorted on something that won't change between game sessions
		std::vector<RE::FormID> items;
		for (auto& item : inv) {
			items.push_back(item.first->formID);
		}

		std::sort(items.begin(), items.end());
		return a_n < items.size() ? RE::TESForm::LookupByID(items[a_n]) : 0;
	}


	SInt32 GetNumItemsOfType(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, UInt32 a_type)
	{
		if (!a_container) {
			_WARNING("a_container is a NONE form!");
			return -1;
		} else if (a_type > to_underlying(RE::FormType::Max)) {
			_WARNING("a_type is out of range!");
			return -1;
		}

		auto type = static_cast<RE::FormType>(a_type);
		auto inv = a_container->GetInventory([&](RE::TESBoundObject* a_object) -> bool
		{
			return a_object->Is(type);
		});

		SInt32 count = 0;
		for (auto& item : inv) {
			if (item.second.first > 0) {
				++count;
			}
		}

		return count;
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetNthFormOfType", "iEquip_ObjectReferenceExt", GetNthFormOfType);
		a_vm->RegisterFunction("GetNumItemsOfType", "iEquip_ObjectReferenceExt", GetNumItemsOfType);

		return true;
	}
}
