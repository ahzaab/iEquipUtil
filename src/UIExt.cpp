#include "UIExt.h"


namespace UIExt
{
	RE::TESForm* GetFormAtInventoryIndex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, UInt32 a_index)
	{
		auto ui = RE::UI::GetSingleton();
		auto invMenu = ui->GetMenu<RE::InventoryMenu>();
		if (!invMenu) {
			a_vm->TraceStack("Inventory menu is not open!", a_stackID, Severity::kWarning);
			return 0;
		} else if (!invMenu->itemList) {
			a_vm->TraceStack("Inventory menu has no item list!", a_stackID, Severity::kWarning);
			return 0;
		}

		auto& items = invMenu->itemList->items;
		if (a_index >= items.size()) {
			a_vm->TraceStack("Index is out of range!", a_stackID, Severity::kWarning);
			return 0;
		}

		auto item = items[a_index];
		return (item && item->data.objDesc) ? item->data.objDesc->object : 0;
	}


	bool RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("GetFormAtInventoryIndex", "iEquip_UIExt", GetFormAtInventoryIndex);

		return true;
	}
}
