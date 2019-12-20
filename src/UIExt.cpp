#include "UIExt.h"


namespace UIExt
{
	RE::TESForm* GetFormAtInventoryIndex(RE::StaticFunctionTag*, UInt32 a_index)
	{
		auto mm = RE::MenuManager::GetSingleton();
		auto uiStr = RE::InterfaceStrings::GetSingleton();
		auto invMenu = mm->GetMenu<RE::InventoryMenu>(uiStr->inventoryMenu);
		if (!invMenu) {
			_WARNING("Inventory menu is not open!");
			return 0;
		} else if (!invMenu->itemList) {
			_WARNING("Inventory menu has no item list!");
			return 0;
		}

		auto& items = invMenu->itemList->items;
		if (a_index >= items.size()) {
			_WARNING("Index is out of range!");
			return 0;
		}

		auto item = items[a_index];
		return (item && item->data.objDesc) ? item->data.objDesc->object : 0;
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetFormAtInventoryIndex", "iEquip_UIExt", GetFormAtInventoryIndex);

		return true;
	}
}
