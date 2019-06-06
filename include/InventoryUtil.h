#pragma once

#include "GameAPI.h"  // g_thePlayer
#include "GameBSExtraData.h"  // BaseExtraList
#include "GameExtraData.h"  // InventoryEntryData, ExtraContainerChanges

#include "function_ref.h"  // function_view

#include "RE/BSTList.h"  // BSSimpleList

namespace
{
	inline void ForEachInvEntryImpl(TESObjectREFR* a_objRef, llvm::function_ref<bool(InventoryEntryData*)> a_fn)
	{
		auto xChanges = static_cast<ExtraContainerChanges*>(a_objRef->extraData.GetByType(kExtraData_ContainerChanges));
		if (!xChanges || !xChanges->data || !xChanges->data->objList) {
			return;
		}

		auto invList = reinterpret_cast<RE::BSSimpleList<InventoryEntryData*>&>(*xChanges->data->objList);
		for (auto& item : invList) {
			if (!a_fn(item)) {
				break;
			}
		}
	}
}


inline void ForEachInvEntry(llvm::function_ref<bool(InventoryEntryData*)> a_fn)
{
	ForEachInvEntryImpl(*g_thePlayer, a_fn);
}


inline void ForEachInvEntry(TESObjectREFR* a_objRef, llvm::function_ref<bool(InventoryEntryData*)> a_fn)
{
	ForEachInvEntryImpl(a_objRef, a_fn);
}


inline void ForEachExtraList(InventoryEntryData* a_entryData, llvm::function_ref<bool(BaseExtraList*)> a_fn)
{
	if (a_entryData->extendDataList) {
		auto xLists = reinterpret_cast<RE::BSSimpleList<BaseExtraList*>&>(*a_entryData->extendDataList);
		for (auto& xList : xLists) {
			if (!a_fn(xList)) {
				break;
			}
		}
	}
}
