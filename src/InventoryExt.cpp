#include "InventoryExt.h"

#include "GameBSExtraData.h"  // BaseExtraList
#include "GameData.h"  // EquipManager
#include "GameExtraData.h"  // ExtraCount
#include "GameFormComponents.h"  // TESFullName
#include "GameRTTI.h"  // DYNAMIC_CAST
#include "GameTypes.h"  // BSFixedString
#include "PapyrusNativeFunctions.h"  // StaticFunctionTag, NativeFunction
#include "PapyrusVM.h"  // VMClassRegistry

#include <map>  // map
#include <string>  // string
#include <utility>  // pair

#include "InventoryUtil.h"  // ForEachInvEntry, ForEachExtraList
#include "RefHandleManager.h"  // RefHandleManager
#include "Registration.h"  // OnRefHandleActiveRegSet, OnRefHandleInvalidatedRegSet
#include "SKSEInterface.h"

#include "RE/ExtraPoison.h"  // ExtraPoison
#include "RE/ExtraTextDisplayData.h"  // ExtraTextDisplayData
#include "RE/InventoryMenu.h"  // InventoryMenu


namespace InventoryExt
{
	namespace
	{
		using EntryData = RefHandleManager::EntryData;


		enum : UInt32
		{
			kSlotID_Default = 0,
			kSlotID_Right = 1,
			kSlotID_Left = 2
		};


		enum : UInt32
		{
			kXSlotID_LeftHand = 0,
			kXSlotID_RightHand = 1,
			kXSlotID_Shield = 2,
			kXSlotID_Head = 3,
			kXSlotID_Chest = 4,
			kXSlotID_Boots = 5,
			kXSlotID_Gloves = 6
		};


		enum : UInt32
		{
			kFirstPersonFlag_None = 0,
			kFirstPersonFlag_Head = 1 << 0,
			kFirstPersonFlag_Hair = 1 << 1,
			kFirstPersonFlag_Body = 1 << 2,
			kFirstPersonFlag_Hands = 1 << 3,
			kFirstPersonFlag_Forearms = 1 << 4,
			kFirstPersonFlag_Amulet = 1 << 5,
			kFirstPersonFlag_Ring = 1 << 6,
			kFirstPersonFlag_Feet = 1 << 7,
			kFirstPersonFlag_Calves = 1 << 8,
			kFirstPersonFlag_Shield = 1 << 9,
			kFirstPersonFlag_Tail = 1 << 10,
			kFirstPersonFlag_LongHair = 1 << 11,
			kFirstPersonFlag_Circlet = 1 << 12,
			kFirstPersonFlag_Ears = 1 << 13,
			kFirstPersonFlag_DecapitateHead = 1 << 20,
			kFirstPersonFlag_Decapitate = 1 << 21,
			kFirstPersonFlag_FX01 = (UInt32)(1 << 31)
		};


		enum : UInt32
		{
			kFormID_Gold = 0x0000000F
		};


		std::optional<EntryData> LookupEntry(TESForm* a_item, UInt32 a_refHandle)
		{
			auto manager = RefHandleManager::GetSingleton();
			auto result = manager->LookupEntry(a_item, a_refHandle);
			if (!result) {
				_ERROR("[ERROR] Failed to find item!\n");
			}
			return result;
		}


		bool GetSlotByID(UInt32 a_equipSlot, TESForm* a_item, bool a_worn, bool a_wornLeft, BGSEquipSlot*& a_slotOut)
		{
			switch (a_equipSlot) {
			case kSlotID_Default:
				if (a_item->IsWeapon()) {
					_WARNING("[WARNING] a_equipSlot is an invalid slot for a_item!");
					return false;
				} else {
					if (a_worn) {
						_WARNING("[WARNING] a_item is already worn!");
						return false;
					} else {
						a_slotOut = 0;
					}
				}
				break;
			case kSlotID_Right:
				if (!a_item->IsWeapon()) {
					_WARNING("[WARNING] a_equipSlot is an invalid slot for a_item!");
					return false;
				} else {
					if (a_worn) {
						_WARNING("[WARNING] a_item is already worn!");
						return false;
					} else {
						a_slotOut = GetRightHandSlot();
					}
				}
				break;
			case kSlotID_Left:
				if (!a_item->IsWeapon()) {
					_WARNING("[WARNING] a_equipSlot is an invalid slot for a_item!");
					return false;
				} else {
					if (a_wornLeft) {
						_WARNING("[WARNING] a_item is already worn!");
						return false;
					} else {
						a_slotOut = GetLeftHandSlot();
					}
				}
				break;
			default:
				_WARNING("[WARNING] a_equipSlot is not a slot (%i)!", a_equipSlot);
				return false;
			}
			return true;
		}


		bool GetWornObjectFilters(UInt32 a_equipSlot, UInt32& a_xDataType, UInt32& a_formType, UInt32& a_firstPersonFlag)
		{
			switch (a_equipSlot) {
			case kXSlotID_Head:
			case kXSlotID_Chest:
			case kXSlotID_Boots:
			case kXSlotID_Gloves:
			case kXSlotID_RightHand:
			case kXSlotID_Shield:
				a_xDataType = kExtraData_Worn;
				break;
			case kXSlotID_LeftHand:
				a_xDataType = kExtraData_WornLeft;
				break;
			default:
				_WARNING("[WARNING] a_equipSlot is an invalid slot ID!");
				return false;
			}

			switch (a_equipSlot) {
			case kXSlotID_Head:
			case kXSlotID_Chest:
			case kXSlotID_Boots:
			case kXSlotID_Gloves:
			case kXSlotID_Shield:
				a_formType = kFormType_Armor;
				break;
			case kXSlotID_RightHand:
			case kXSlotID_LeftHand:
				a_formType = kFormType_Weapon;
				break;
			default:
				_WARNING("[WARNING] a_equipSlot is an invalid slot ID!");
				return false;
			}

			switch (a_equipSlot) {
			case kXSlotID_Head:
				a_firstPersonFlag = kFirstPersonFlag_Circlet;
				break;
			case kXSlotID_Chest:
				a_firstPersonFlag = kFirstPersonFlag_Body;
				break;
			case kXSlotID_Boots:
				a_firstPersonFlag = kFirstPersonFlag_Feet;
				break;
			case kXSlotID_Gloves:
				a_firstPersonFlag = kFirstPersonFlag_Hands;
				break;
			case kXSlotID_RightHand:
			case kXSlotID_LeftHand:
				a_firstPersonFlag = kFirstPersonFlag_None;
				break;
			case kXSlotID_Shield:
				a_firstPersonFlag = kFirstPersonFlag_Shield;
				break;
			default:
				_WARNING("[WARNING] a_equipSlot is an invalid slot ID!");
				return false;
			}

			return true;
		}
	}


	void RegisterForRefHandleActiveEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnRefHandleActiveRegSet::GetSingleton()->Register<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void UnregisterForRefHandleActiveEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnRefHandleActiveRegSet::GetSingleton()->Unregister<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void RegisterForOnRefHandleInvalidatedEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnRefHandleInvalidatedRegSet::GetSingleton()->Register<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void UnregisterForOnRefHandleInvalidatedEvent(StaticFunctionTag*, TESForm* a_thisForm)
	{
		if (!a_thisForm) {
			_WARNING("[WARNING] a_thisForm is a NONE form!");
			return;
		}

		OnRefHandleInvalidatedRegSet::GetSingleton()->Unregister<TESForm>(a_thisForm->formType, a_thisForm);
	}


	void ParseInventory(StaticFunctionTag*)
	{
		SKSE::AddTask([]()
		{
			using FormID = UInt32;
			using Count = SInt32;

			auto manager = RefHandleManager::GetSingleton();
			auto container = DYNAMIC_CAST((*g_thePlayer)->baseForm, TESForm, TESContainer);
			if (!container) {
				_ERROR("[ERROR] Failed to get player's default inventory!\n");
				manager->SetInit();
				return;
			}

			// extra items
			std::map<FormID, std::pair<Count, InventoryEntryData*>> itemMap;
			ForEachInvEntry([&](InventoryEntryData* a_entryData) -> bool
			{
				if (manager->IsTrackedType(a_entryData->type)) {
					itemMap.insert({ a_entryData->type->formID, { a_entryData->countDelta, a_entryData} });
				}
				return true;
			});

			// default items
			auto xChanges = static_cast<ExtraContainerChanges*>((*g_thePlayer)->extraData.GetByType(kExtraData_ContainerChanges));
			for (UInt32 i = 0; i < container->numEntries; ++i) {
				auto entry = container->entries[i];
				auto it = itemMap.find(entry->form->formID);
				if (it != itemMap.end() && it->first != kFormID_Gold) {
					it->second.first += entry->count;
				} else {
					if (manager->IsTrackedType(entry->form)) {
						auto entryData = InventoryEntryData::Create(entry->form, 0);
						auto& objList = reinterpret_cast<RE::BSSimpleList<InventoryEntryData*> * &>(xChanges->data->objList);
						if (!objList) {
							objList = new RE::BSSimpleList<InventoryEntryData*>();
						}
						objList->push_front(entryData);
						itemMap.insert({ entryData->type->formID, { entry->count, entryData } });
					}
				}
			}

			auto regs = OnRefHandleActiveRegSet::GetSingleton();
			for (auto& item : itemMap) {
				SInt32 rawCount = item.second.first;
				InventoryEntryData* entryData = item.second.second;
				if (entryData->extendDataList) {
					ForEachExtraList(entryData, [&](BaseExtraList* a_extraList) -> bool
					{
						auto xCount = static_cast<ExtraCount*>(a_extraList->GetByType(kExtraData_Count));
						SInt32 count = xCount ? xCount->count : 1;
						rawCount -= count;
						auto result = manager->ActivateHandle(entryData->type, a_extraList);
						if (result) {
							regs->QueueEvent(entryData->type, *result, count);
						}
						return true;
					});
				}
				BaseExtraList* xListOut;
				for (SInt32 i = 0; i < rawCount; ++i) {
					xListOut = 0;
					auto result = manager->ActivateHandle(entryData->type, xListOut);
					if (xListOut) {
						auto& xLists = reinterpret_cast<RE::BSSimpleList<BaseExtraList*> * &>(entryData->extendDataList);
						if (!xLists) {
							xLists = new RE::BSSimpleList<BaseExtraList*>();
						}
						xLists->push_front(xListOut);
					}
					if (result) {
						regs->QueueEvent(entryData->type, *result, 1);
					}
				}
			}

			manager->SetInit();
		});
	}


	UInt32 GetRefHandleAtInvIndex(StaticFunctionTag*, UInt32 a_index)
	{
		MenuManager* mm = MenuManager::GetSingleton();
		UIStringHolder* uiStrHolder = UIStringHolder::GetSingleton();
		RE::InventoryMenu* invMenu = static_cast<RE::InventoryMenu*>(mm->GetMenu(&uiStrHolder->inventoryMenu));
		if (!invMenu) {
			_WARNING("[WARNING] Inventory menu is not open!");
			return RefHandleManager::kInvalidRefHandle;
		}

		auto& items = invMenu->inventoryData->items;
		if (a_index >= items.count) {
			_WARNING("[WARNING] Index is out of range!");
			return RefHandleManager::kInvalidRefHandle;
		}

		auto item = items[a_index];
		if (!item) {
			_ERROR("[ERROR] Failed to find item at index!\n");
			return RefHandleManager::kInvalidRefHandle;
		}

		if (!item->data.objDesc->extendDataList) {
			_ERROR("[ERROR] Item did not have extra data!\n");
			return RefHandleManager::kInvalidRefHandle;
		}

		ExtraUniqueID* xID = 0;
		ForEachExtraList(item->data.objDesc, [&](BaseExtraList* a_extraList) -> bool
		{
			xID = static_cast<ExtraUniqueID*>(a_extraList->GetByType(kExtraData_UniqueID));
			return xID == 0;
		});
		if (!xID) {
			_ERROR("[ERROR] Could not find unique ID for item!\n");
			return RefHandleManager::kInvalidRefHandle;
		}

		auto manager = RefHandleManager::GetSingleton();
		auto handle = manager->LookupHandle(xID->uniqueId);
		if (handle == RefHandleManager::kInvalidRefHandle) {
			_ERROR("[ERROR] Could not find handle for unique ID!\n");
		}
		return handle;
	}


	UInt32 GetRefHandleFromWornObject(StaticFunctionTag*, UInt32 a_equipSlot)
	{
		UInt32 handle = RefHandleManager::kInvalidRefHandle;
		UInt32 xDataType;
		UInt32 formType;
		UInt32 firstPersonFlag;
		if (!GetWornObjectFilters(a_equipSlot, xDataType, formType, firstPersonFlag)) {
			return handle;
		}

		ForEachInvEntry([&](InventoryEntryData* a_entryData) -> bool
		{
			bool found = false;
			if (a_entryData->type->formType == formType && a_entryData->extendDataList) {
				switch (a_equipSlot) {
				case kXSlotID_Head:
				case kXSlotID_Chest:
				case kXSlotID_Boots:
				case kXSlotID_Gloves:
				case kXSlotID_Shield:
					{
						auto armor = static_cast<TESObjectARMO*>(a_entryData->type);
						if ((armor->bipedObject.data.parts & firstPersonFlag) == 0) {
							return true;
						}
					}
					break;
				default:
					break;
				}

				ForEachExtraList(a_entryData, [&](BaseExtraList* a_extraList) -> bool
				{
					if (a_extraList->HasType(xDataType)) {
						auto xID = static_cast<ExtraUniqueID*>(a_extraList->GetByType(kExtraData_UniqueID));
						if (xID) {
							auto manager = RefHandleManager::GetSingleton();
							handle = manager->LookupHandle(xID->uniqueId);
							if (handle != RefHandleManager::kInvalidRefHandle) {
								found = true;
								return false;
							}
						}
					}
					return true;
				});
			}
			return !found;
		});

		if (handle == RefHandleManager::kInvalidRefHandle) {
			_ERROR("[ERROR] Failed to find ref handle!\n");
		}
		return handle;
	}


	BSFixedString GetLongName(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return "";
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return "";
		}

		entryData->extraList->GetDisplayName(a_item);
		auto xText = static_cast<RE::ExtraTextDisplayData*>(entryData->extraList->GetByType(kExtraData_TextDisplayData));
		if (xText) {
			return xText->name;
		} else {
			auto fullName = DYNAMIC_CAST(a_item, TESForm, TESFullName);
			return fullName ? fullName->name.data : "";
		}
	}


	BSFixedString GetShortName(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return "";
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return "";
		}

		entryData->extraList->GetDisplayName(a_item);
		auto xText = static_cast<RE::ExtraTextDisplayData*>(entryData->extraList->GetByType(kExtraData_TextDisplayData));
		if (xText && xText->type == -2) {
			std::string name(xText->name.data, xText->rawNameLen);
			return name.c_str();
		} else {
			auto fullName = DYNAMIC_CAST(a_item, TESForm, TESFullName);
			return fullName ? fullName->name.data : "";
		}
	}


	AlchemyItem* GetPoison(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return 0;
		} else if (a_item->formType != kFormType_Weapon) {
			_WARNING("[WARNING] a_item is not a weapon!");
			return 0;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return 0;
		}

		auto xPoison = static_cast<RE::ExtraPoison*>(entryData->extraList->GetByType(kExtraData_Poison));
		return xPoison ? xPoison->poison : 0;
	}


	void SetPoison(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle, AlchemyItem* a_newPoison, UInt32 a_newCount)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return;
		} else if (a_item->formType != kFormType_Weapon) {
			_WARNING("[WARNING] a_item is not a weapon!");
			return;
		} else if (!a_newPoison) {
			_WARNING("[WARNING] a_newPoison is a NONE form!");
			return;
		} else if (!a_newPoison->IsPoison()) {
			_WARNING("[WARNING] a_newPoison is not a poison!");
			return;
		}

		UInt32 itemID = a_item->formID;
		UInt32 newPoisonID = a_newPoison->formID;
		SKSE::AddTask([itemID, a_refHandle, newPoisonID, a_newCount]()
		{
			auto item = LookupFormByID(itemID);
			if (!item) {
				_WARNING("[WARNING] Failed to lookup item by form id!");
				return;
			}

			auto newPoison = static_cast<AlchemyItem*>(LookupFormByID(newPoisonID));
			if (!item) {
				_WARNING("[WARNING] Failed to lookup poison by form id!");
				return;
			}

			auto entryData = LookupEntry(item, a_refHandle);
			if (!entryData) {
				return;
			}

			auto xPoison = static_cast<RE::ExtraPoison*>(entryData->extraList->GetByType(kExtraData_Poison));
			if (!xPoison) {
				xPoison = RE::ExtraPoison::Create();
				RefHandleManager::AddExtraData(entryData->extraList, xPoison);
			}
			xPoison->poison = newPoison;
			xPoison->count = a_newCount;
		});
	}


	void RemovePoison(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return;
		} else if (a_item->formType != kFormType_Weapon) {
			_WARNING("[WARNING] a_item is not a weapon!");
			return;
		}

		UInt32 itemID = a_item->formID;
		SKSE::AddTask([itemID, a_refHandle]()
		{
			auto item = LookupFormByID(itemID);
			if (!item) {
				_WARNING("[WARNING] Failed to lookup item by form id!");
				return;
			}

			auto entryData = LookupEntry(item, a_refHandle);
			if (!entryData) {
				return;
			}

			auto xPoison = static_cast<RE::ExtraPoison*>(entryData->extraList->GetByType(kExtraData_Poison));
			if (xPoison) {
				entryData->extraList->Remove(kExtraData_Poison, xPoison);
				delete xPoison;
			}
		});
	}


	SInt32 GetPoisonCount(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return 0;
		} else if (a_item->formType != kFormType_Weapon) {
			_WARNING("[WARNING] a_item is not a weapon!");
			return 0;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return 0;
		}

		auto xPoison = static_cast<RE::ExtraPoison*>(entryData->extraList->GetByType(kExtraData_Poison));
		return xPoison ? xPoison->count : 0;
	}


	void SetPoisonCount(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle, UInt32 a_newCount)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return;
		} else if (a_item->formType != kFormType_Weapon) {
			_WARNING("[WARNING] a_item is not a weapon!");
			return;
		}

		UInt32 itemID = a_item->formID;
		SKSE::AddTask([itemID, a_refHandle, a_newCount]
		{
			auto item = LookupFormByID(itemID);
			if (!item) {
				_WARNING("[WARNING] Failed to lookup item by form id!");
				return;
			}

			auto entryData = LookupEntry(item, a_refHandle);
			if (!entryData) {
				return;
			}

			auto xPoison = static_cast<RE::ExtraPoison*>(entryData->extraList->GetByType(kExtraData_Poison));
			if (xPoison) {
				xPoison->count = a_newCount;
			}
		});
	}


	EnchantmentItem* GetEnchantment(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return false;
		}

		auto entryData = LookupEntry(a_item, a_refHandle);
		if (!entryData) {
			return 0;
		}

		TESEnchantableForm* enchForm = DYNAMIC_CAST(a_item, TESForm, TESEnchantableForm);
		if (enchForm && enchForm->enchantment) {
			return enchForm->enchantment;
		} else {
			auto xEnch = static_cast<ExtraEnchantment*>(entryData->extraList->GetByType(kExtraData_Enchantment));
			return xEnch ? xEnch->enchant : 0;
		}
	}


	void EquipItem(StaticFunctionTag*, TESForm* a_item, UInt32 a_refHandle, Actor* a_actor, UInt32 a_equipSlot, bool a_preventUnequip, bool a_equipSound)
	{
		if (!a_item) {
			_WARNING("[WARNING] a_item is a NONE form!");
			return;
		} else if (!a_actor) {
			_WARNING("[WARNING] a_actor is a NONE form!");
			return;
		}

		UInt32 itemID = a_item->formID;
		UInt32 actorHandle = GetOrCreateRefrHandle(a_actor);
		SKSE::AddTask([itemID, a_refHandle, actorHandle, a_equipSlot, a_preventUnequip, a_equipSound]()
		{
			auto item = LookupFormByID(itemID);
			if (!item) {
				_WARNING("[WARNING] Failed to lookup item by form id!");
				return;
			}

			Actor* actor = 0;
#if _WIN64
			NiPointer<TESObjectREFR> refrOut;
			UInt32 refHandle = actorHandle;	// dumb semantics
			LookupREFRByHandle(refHandle, refrOut);
			actor = static_cast<Actor*>(refrOut.get());
#else
			TESObjectREFR* refrOut;
			UInt32 refHandle = actorHandle;
			LookupREFRByHandle(&refHandle, &refrOut);
			actor = static_cast<Actor*>(refrOut);
#endif

			if (!actor) {
				_WARNING("[WARNING] Failed to lookup actor by handle!");
				return;
			}

			auto entryData = LookupEntry(item, a_refHandle);
			if (!entryData) {
				return;
			}

			bool worn = false;
			bool wornLeft = false;
			SInt32 count = 1;
			if (entryData->extraList) {
				auto xCount = static_cast<ExtraCount*>(entryData->extraList->GetByType(kExtraData_Count));
				if (xCount) {
					count = xCount->count;
				}

				auto xWorn = static_cast<ExtraWorn*>(entryData->extraList->GetByType(kExtraData_Worn));
				worn = xWorn != 0;

				auto xWornLeft = static_cast<ExtraWornLeft*>(entryData->extraList->GetByType(kExtraData_WornLeft));
				wornLeft = xWornLeft != 0;
			}

			BGSEquipSlot* slot;
			if (!GetSlotByID(a_equipSlot, item, worn, wornLeft, slot)) {
				return;
			}

			SInt32 countReq = (worn || wornLeft) ? 2 : 1;
			if (count < countReq) {
				_ERROR("[ERROR] Item count is too small to equip!\n");
				return;
			}

			SInt32 equipCount;
			BaseExtraList* extraList;
			if (item->IsAmmo()) {
				equipCount = count;
				extraList = 0;
			} else {
				equipCount = 1;
				extraList = entryData->extraList;
			}

			EquipManager* equipManager = EquipManager::GetSingleton();
			CALL_MEMBER_FN(equipManager, EquipItem)(actor, item, extraList, equipCount, slot, a_equipSound, a_preventUnequip, false, 0);

#if _WIN32
			actor->handleRefObject.DecRef();
#endif
		});
	}


	bool RegisterFuncs(VMClassRegistry* a_registry)
	{
		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("RegisterForRefHandleActiveEvent", "iEquip_InventoryExt", RegisterForRefHandleActiveEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("UnregisterForRefHandleActiveEvent", "iEquip_InventoryExt", UnregisterForRefHandleActiveEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("RegisterForOnRefHandleInvalidatedEvent", "iEquip_InventoryExt", RegisterForOnRefHandleInvalidatedEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, TESForm*>("UnregisterForOnRefHandleInvalidatedEvent", "iEquip_InventoryExt", UnregisterForOnRefHandleInvalidatedEvent, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, void>("ParseInventory", "iEquip_InventoryExt", ParseInventory, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, UInt32, UInt32>("GetRefHandleAtInvIndex", "iEquip_InventoryExt", GetRefHandleAtInvIndex, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, UInt32, UInt32>("GetRefHandleFromWornObject", "iEquip_InventoryExt", GetRefHandleFromWornObject, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, BSFixedString, TESForm*, UInt32>("GetLongName", "iEquip_InventoryExt", GetLongName, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, BSFixedString, TESForm*, UInt32>("GetShortName", "iEquip_InventoryExt", GetShortName, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, AlchemyItem*, TESForm*, UInt32>("GetPoison", "iEquip_InventoryExt", GetPoison, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction4<StaticFunctionTag, void, TESForm*, UInt32, AlchemyItem*, UInt32>("SetPoison", "iEquip_InventoryExt", SetPoison, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, TESForm*, UInt32>("RemovePoison", "iEquip_InventoryExt", RemovePoison, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, TESForm*, UInt32>("RemovePoison", "iEquip_InventoryExt", RemovePoison, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, SInt32, TESForm*, UInt32>("GetPoisonCount", "iEquip_InventoryExt", GetPoisonCount, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction3<StaticFunctionTag, void, TESForm*, UInt32, UInt32>("SetPoisonCount", "iEquip_InventoryExt", SetPoisonCount, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction6<StaticFunctionTag, void, TESForm*, UInt32, Actor*, UInt32, bool, bool>("EquipItem", "iEquip_InventoryExt", EquipItem, a_registry));

		return true;
	}
}
