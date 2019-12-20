#include "InventoryExt.h"

#include <map>
#include <string>
#include <utility>

#include "RefHandleManager.h"
#include "Registration.h"

#include "SKSE/API.h"


namespace InventoryExt
{
	namespace
	{
		using EntryData = RefHandleManager::EntryData;


		enum class EquipSlot : UInt32
		{
			kDefault = 0,
			kRight = 1,
			kLeft = 2
		};


		enum class XEquipSlot : UInt32
		{
			kLeftHand = 0,
			kRightHand = 1,
			kShield = 2,
			kHead = 3,
			kChest = 4,
			kBoots = 5,
			kGloves = 6
		};


		std::optional<EntryData> LookupEntry(RE::TESForm* a_item, UInt32 a_refHandle)
		{
			auto manager = RefHandleManager::GetSingleton();
			auto result = manager->LookupEntry(a_item, a_refHandle);
			if (!result) {
				_ERROR("[ERROR] Failed to find item!\n");
			}
			return result;
		}


		std::optional<RE::BGSEquipSlot*> GetSlotByID(EquipSlot a_equipSlot, RE::TESForm* a_item, bool a_worn, bool a_wornLeft)
		{
			using Object = RE::BGSDefaultObjectManager::DefaultObject;

			auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
			RE::BGSEquipSlot* slotOut = 0;

			switch (a_equipSlot) {
			case EquipSlot::kDefault:
				if (a_item->IsWeapon()) {
					_WARNING("a_equipSlot is an invalid slot for a_item!");
					return std::nullopt;
				}

				if (a_worn) {
					_WARNING("a_item is already worn!");
					return std::nullopt;
				}

				slotOut = 0;
				break;
			case EquipSlot::kRight:
				if (!a_item->IsWeapon()) {
					_WARNING("a_equipSlot is an invalid slot for a_item!");
					return std::nullopt;
				}

				if (a_worn) {
					_WARNING("a_item is already worn!");
					return std::nullopt;
				}

				slotOut = dobj->GetObject<RE::BGSEquipSlot>(Object::kRightHand);
				break;
			case EquipSlot::kLeft:
				if (!a_item->IsWeapon()) {
					_WARNING("a_equipSlot is an invalid slot for a_item!");
					return std::nullopt;
				}

				if (a_wornLeft) {
					_WARNING("a_item is already worn!");
					return std::nullopt;
				}

				slotOut = dobj->GetObject<RE::BGSEquipSlot>(Object::kLeftHand);
				break;
			default:
				_WARNING("a_equipSlot is not a slot (%i)!", a_equipSlot);
				return std::nullopt;
			}

			return std::make_optional(slotOut);
		}


		bool GetWornObjectFilters(XEquipSlot a_equipSlot, RE::ExtraDataType& a_xDataType, RE::FormType& a_formType, RE::BGSBipedObjectForm::FirstPersonFlag& a_firstPersonFlag)
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
				_WARNING("a_equipSlot is an invalid slot ID!");
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
				_WARNING("a_equipSlot is an invalid slot ID!");
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
				_WARNING("a_equipSlot is an invalid slot ID!");
				return false;
			}

			return true;
		}
	}


	void RegisterForOnRefHandleActiveEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnRefHandleActiveRegSet::GetSingleton();
		regs->Register(a_thisForm);
	}


	void UnregisterForOnRefHandleActiveEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnRefHandleActiveRegSet::GetSingleton();
		regs->Unregister(a_thisForm);
	}


	void RegisterForOnRefHandleInvalidatedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
		regs->Register(a_thisForm);
	}


	void UnregisterForOnRefHandleInvalidatedEvent(RE::StaticFunctionTag*, RE::TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("a_thisForm is a NONE form!");
			return;
		}

		auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
		regs->Unregister(a_thisForm);
	}


	void EquipItem(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, RE::Actor* a_actor, UInt32 a_equipSlot, bool a_preventUnequip, bool a_equipSound)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return;
		} else if (!a_actor) {
			_WARNING("a_actor is a NONE form!");
			return;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return;
		}

		bool worn = false;
		bool wornLeft = false;
		SInt32 count = 1;
		if (entryData->extraList) {
			auto xCount = entryData->extraList->GetByType<RE::ExtraCount>();
			if (xCount) {
				count = xCount->count;
			}

			worn = entryData->extraList->HasType(RE::ExtraDataType::kWorn);
			wornLeft = entryData->extraList->HasType(RE::ExtraDataType::kWornLeft);
		}

		auto equipSlot = static_cast<EquipSlot>(a_equipSlot);
		auto slot = GetSlotByID(equipSlot, a_item, worn, wornLeft);
		if (!slot) {
			return;
		}

		SInt32 countReq = (worn || wornLeft) ? 2 : 1;
		if (count < countReq) {
			_ERROR("Item count is too small to equip!\n");
			return;
		}

		SInt32 equipCount;
		RE::ExtraDataList* extraList;
		if (a_item->IsAmmo()) {
			equipCount = count;
			extraList = 0;
		} else {
			equipCount = 1;
			extraList = entryData->extraList;
		}

		auto equipManager = RE::ActorEquipManager::GetSingleton();
		equipManager->EquipItem(a_actor, a_item, extraList, equipCount, *slot, a_equipSound, a_preventUnequip, false, 0);
	}


	RE::EnchantmentItem* GetEnchantment(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return false;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return 0;
		}

		auto xEnch = entryData->extraList->GetByType<RE::ExtraEnchantment>();
		if (xEnch) {
			return xEnch->objectEffect;
		}

		auto enchForm = a_item->As<RE::TESEnchantableForm*>();
		return enchForm ? enchForm->objectEffect : 0;
	}


	RE::BSFixedString GetLongName(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return "";
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return "";
		}

		entryData->invEntryData->GenerateName();
		auto xText = entryData->extraList->GetByType<RE::ExtraTextDisplayData>();
		if (xText) {
			return xText->name;
		} else {
			auto fullName = a_item->As<RE::TESFullName*>();
			return fullName ? fullName->fullName : "";
		}
	}


	RE::AlchemyItem* GetPoison(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return 0;
		} else if (!a_item->IsWeapon()) {
			_WARNING("a_item is not a weapon!");
			return 0;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return 0;
		}

		auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
		return xPoison ? xPoison->poison : 0;
	}


	SInt32 GetPoisonCount(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return 0;
		} else if (!a_item->IsWeapon()) {
			_WARNING("a_item is not a weapon!");
			return 0;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return 0;
		}

		auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
		return xPoison ? xPoison->count : 0;
	}


	UInt32 GetRefHandleAtInvIndex(RE::StaticFunctionTag*, UInt32 a_index)
	{
		auto mm = RE::MenuManager::GetSingleton();
		auto uiStr = RE::InterfaceStrings::GetSingleton();
		auto invMenu = mm->GetMenu<RE::InventoryMenu>(uiStr->inventoryMenu);
		if (!invMenu) {
			_WARNING("Inventory menu is not open!");
			return RefHandleManager::kInvalidRefHandle;
		} else if (!invMenu->itemList) {
			_WARNING("Inventory menu is has no item list!");
			return RefHandleManager::kInvalidRefHandle;
		}

		auto& items = invMenu->itemList->items;
		if (a_index >= items.size()) {
			_WARNING("Index is out of range!");
			return RefHandleManager::kInvalidRefHandle;
		}

		auto item = items[a_index];
		if (!item) {
			_ERROR("Failed to find item at index!\n");
			return RefHandleManager::kInvalidRefHandle;
		} else if (!item->data.objDesc) {
			_ERROR("Item did not have a entry data!\n");
			return RefHandleManager::kInvalidRefHandle;
		} else if (!item->data.objDesc->extraLists) {
			_ERROR("Item did not have extra data!\n");
			return RefHandleManager::kInvalidRefHandle;
		}

		RE::ExtraUniqueID* xID = 0;
		for (auto& xList : *item->data.objDesc->extraLists) {
			auto xID = xList->GetByType<RE::ExtraUniqueID>();
			if (xID) {
				auto manager = RefHandleManager::GetSingleton();
				auto handle = manager->LookupHandle(xID->uniqueID);
				if (handle == RefHandleManager::kInvalidRefHandle) {
					_ERROR("Could not find handle for unique ID!\n");
				}
				return handle;
			}
		}

		_ERROR("Could not find unique ID for item!\n");
		return RefHandleManager::kInvalidRefHandle;
	}


	UInt32 GetRefHandleFromWornObject(RE::StaticFunctionTag*, UInt32 a_equipSlot)
	{
		using FPFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

		UInt32 handle = RefHandleManager::kInvalidRefHandle;
		RE::ExtraDataType xDataType;
		RE::FormType formType;
		FPFlag fpFlag;
		auto xEquipSlot = static_cast<XEquipSlot>(a_equipSlot);
		if (!GetWornObjectFilters(xEquipSlot, xDataType, formType, fpFlag)) {
			return handle;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto inv = player->GetInventory([&](RE::TESBoundObject* a_object) -> bool
		{
			if (a_object->Is(formType)) {
				switch (xEquipSlot) {
				case XEquipSlot::kHead:
				case XEquipSlot::kChest:
				case XEquipSlot::kBoots:
				case XEquipSlot::kGloves:
				case XEquipSlot::kShield:
					{
						auto armor = static_cast<RE::TESObjectARMO*>(a_object);
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
			auto entryData = item.second.second;
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

		_ERROR("Failed to find ref handle!\n");
		return RefHandleManager::kInvalidRefHandle;
	}


	RE::BSFixedString GetShortName(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return "";
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return "";
		}

		entryData->invEntryData->GenerateName();
		auto xText = entryData->extraList->GetByType<RE::ExtraTextDisplayData>();
		if (xText && xText->IsPlayerSet()) {
			std::string name(xText->name.c_str(), xText->rawNameLen);
			return name.c_str();
		} else {
			auto fullName = a_item->As<RE::TESFullName*>();
			return fullName ? fullName->fullName : "";
		}
	}


	void ParseInventory(RE::StaticFunctionTag*)
	{
		auto task = SKSE::GetTaskInterface();
		task->AddTask([]() -> void
		{
			auto manager = RefHandleManager::GetSingleton();
			auto player = RE::PlayerCharacter::GetSingleton();
			auto inv = player->GetInventory([&](RE::TESBoundObject* a_object) -> bool
			{
				return manager->IsTrackedType(a_object);
			});

			for (auto& elem : inv) {
				auto item = elem.first;
				auto rawCount = elem.second.first;
				auto entryData = elem.second.second;

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


	void RemovePoison(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return;
		} else if (!a_item->IsWeapon()) {
			_WARNING("a_item is not a weapon!");
			return;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return;
		}

		auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
		if (xPoison) {
			entryData->extraList->Remove(xPoison);
			delete xPoison;
		}
	}


	void SetPoison(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, RE::AlchemyItem* a_newPoison, UInt32 a_newCount)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return;
		} else if (!a_item->IsWeapon()) {
			_WARNING("a_item is not a weapon!");
			return;
		} else if (!a_newPoison) {
			_WARNING("a_newPoison is a NONE form!");
			return;
		} else if (!a_newPoison->IsPoison()) {
			_WARNING("a_newPoison is not a poison!");
			return;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return;
		}

		auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
		if (!xPoison) {
			xPoison = new RE::ExtraPoison();
			entryData->extraList->Add(xPoison);
		}
		xPoison->poison = a_newPoison;
		xPoison->count = a_newCount;
	}


	void SetPoisonCount(RE::StaticFunctionTag*, RE::TESForm* a_item, UInt32 a_refHandle, UInt32 a_newCount)
	{
		if (!a_item) {
			_WARNING("a_item is a NONE form!");
			return;
		} else if (!a_item->IsWeapon()) {
			_WARNING("a_item is not a weapon!");
			return;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return;
		}

		auto xPoison = entryData->extraList->GetByType<RE::ExtraPoison>();
		if (xPoison) {
			xPoison->count = a_newCount;
		}
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("RegisterForOnRefHandleActiveEvent", "iEquip_InventoryExt", RegisterForOnRefHandleActiveEvent);
		a_vm->RegisterFunction("UnregisterForOnRefHandleActiveEvent", "iEquip_InventoryExt", UnregisterForOnRefHandleActiveEvent);
		a_vm->RegisterFunction("RegisterForOnRefHandleInvalidatedEvent", "iEquip_InventoryExt", RegisterForOnRefHandleInvalidatedEvent);
		a_vm->RegisterFunction("UnregisterForOnRefHandleInvalidatedEvent", "iEquip_InventoryExt", UnregisterForOnRefHandleInvalidatedEvent);
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
