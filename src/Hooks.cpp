#include"Hooks.h"

#include "GameBSExtraData.h"  // BaseExtraList
#include "GameExtraData.h"  // InventoryEntryData
#include "GameForms.h"  // TESForm
#include "GameObjects.h"  // TESObjectREFR
#include "GameReferences.h"  // Actor, PlayerCharacter
#include "SafeWrite.h"  // SafeWrite

#include <cstdint>  // uintptr_t

#include "InventoryUtil.h"  // ForEachInvEntry, ForEachExtraList
#include "RefHandleManager.h"  // RefHandleManager
#include "Registration.h"  // OnRefHandleInvalidatedRegSet, OnRefHandleActiveRegSet

#include "RE/BSTList.h"  // BSSimpleList


#if _WIN64
#include "Relocation.h"  // RelocPtr, RelocAddr
#endif


namespace
{
	class PlayerCharacterEx : public PlayerCharacter
	{
	public:
		using RemoveItem_t = UInt32 * (TESObjectREFR::*)(UInt32& a_dropHandle, TESForm* a_item, SInt32 a_count, UInt32 a_removeType, BaseExtraList* a_extraList, TESObjectREFR* a_moveToRef, void* a_arg7, void* a_arg8);
		inline static RemoveItem_t _RemoveItem;

		using AddItem_t = void(TESObjectREFR::*)(TESForm* a_item, BaseExtraList* a_extraList, SInt32 a_count, TESObjectREFR* a_fromRefr);
		inline static AddItem_t _AddItem;

		using PickUpItem_t = void(Actor::*)(TESObjectREFR* a_item, SInt32 a_count, bool a_arg3, bool a_playSound);
		inline static PickUpItem_t _PickUpItem;


		UInt32* Hook_RemoveItem(UInt32& a_dropHandle, TESForm* a_item, SInt32 a_count, UInt32 a_removeType, BaseExtraList* a_extraList, TESObjectREFR* a_moveToRef, void* a_arg7, void* a_arg8)
		{
			auto manager = RefHandleManager::GetSingleton();
			if (!manager->IsInit() || !manager->IsTrackedType(a_item)) {
				return (this->*_RemoveItem)(a_dropHandle, a_item, a_count, a_removeType, a_extraList, a_moveToRef, a_arg7, a_arg8);
			}

#if _WIN64
			UInt32* ret = g_invalidRefHandle;
#else
			UInt32* ret = const_cast<UInt32*>(g_invalidRefHandle);
#endif
			auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
			if (a_extraList || a_count == 1) {
				auto result = manager->InvalidateHandle(a_item, a_extraList);
				ret = (this->*_RemoveItem)(a_dropHandle, a_item, a_count, a_removeType, a_extraList, a_moveToRef, a_arg7, a_arg8);
				if (result) {
					regs->QueueEvent(a_item, *result);
				}
			} else {
				std::size_t i = 0;
				ForEachInvEntry([&](InventoryEntryData* a_entryData) -> bool
				{
					if (a_entryData->type == a_item) {
						while (a_entryData->extendDataList && i < a_count) {
							auto xLists = reinterpret_cast<RE::BSSimpleList<BaseExtraList*>&>(*a_entryData->extendDataList);
							if (xLists.empty()) {
								break;
							} else {
								auto result = manager->InvalidateHandle(a_item, xLists.front());
								ret = (this->*_RemoveItem)(a_dropHandle, a_item, 1, a_removeType, xLists.front(), a_moveToRef, a_arg7, a_arg8);
								if (result) {
									regs->QueueEvent(a_item, *result);
								}
								++i;
							}
						}
						return false;
					} else {
						return true;
					}
				});
				if (i < a_count) {
					ret = (this->*_RemoveItem)(a_dropHandle, a_item, a_count - i, a_removeType, 0, a_moveToRef, a_arg7, a_arg8);
				}
			}

			return ret;
		}


		void Hook_AddItem(TESForm* a_item, BaseExtraList* a_extraList, SInt32 a_count, TESObjectREFR* a_fromRefr)
		{
			auto manager = RefHandleManager::GetSingleton();
			if (!manager->IsInit() || !manager->IsTrackedType(a_item)) {
				(this->*_AddItem)(a_item, a_extraList, a_count, a_fromRefr);
				return;
			}

			auto regs = OnRefHandleActiveRegSet::GetSingleton();
			if (a_extraList) {
				auto result = manager->ActivateHandle(a_item, a_extraList);
				(this->*_AddItem)(a_item, a_extraList, a_count, a_fromRefr);
				if (result) {
					regs->QueueEvent(a_item, *result, a_count);
				}
			} else {
				for (std::size_t i = 0; i < a_count; ++i) {
					BaseExtraList* xListOut = 0;
					auto result = manager->ActivateHandle(a_item, xListOut);
					(this->*_AddItem)(a_item, xListOut, 1, a_fromRefr);
					if (result) {
						regs->QueueEvent(a_item, *result, 1);
					}
				}
			}
		}


		void Hook_PickUpItem(TESObjectREFR* a_item, UInt32 a_count, bool a_arg3, bool a_playSound)
		{
			auto manager = RefHandleManager::GetSingleton();
			if (!manager->IsInit() || !manager->IsTrackedType(a_item)) {
				(this->*_PickUpItem)(a_item, a_count, a_arg3, a_playSound);
				return;
			}

			auto result = manager->ActivateHandle(a_item->baseForm, a_item->extraData);
			TESForm* baseform = a_item->baseForm;

			(this->*_PickUpItem)(a_item, a_count, a_arg3, a_playSound);

			if (result) {
				OnRefHandleActiveRegSet::GetSingleton()->QueueEvent(baseform, *result, a_count);
			}
		}


		static void InstallHooks()
		{
#if _WIN64
			constexpr std::uintptr_t PLAYER_CHARACTER_VTBL = 0x016635F0;	// 1_5_80
#else
			constexpr std::uintptr_t PLAYER_CHARACTER_VTBL = 0x010D201C;
#endif
			constexpr std::uintptr_t PSIZE = sizeof(void*);

			{
#if _WIN64
				RelocPtr<RemoveItem_t> vFunc(PLAYER_CHARACTER_VTBL + (PSIZE * 0x56));
				_RemoveItem = *vFunc;
				SafeWrite64(vFunc.GetUIntPtr(), GetFnAddr(&Hook_RemoveItem));
#else
				std::uintptr_t addr = PLAYER_CHARACTER_VTBL + (PSIZE * 0x56);
				_RemoveItem = *reinterpret_cast<RemoveItem_t*>(addr);
				SafeWrite32(addr, GetFnAddr(&Hook_RemoveItem));
#endif
				_MESSAGE("[MESSAGE] Installed remove item hook");
			}

			{
#if _WIN64
				RelocPtr<AddItem_t> vFunc(PLAYER_CHARACTER_VTBL + (PSIZE * 0x5A));
				_AddItem = *vFunc;
				SafeWrite64(vFunc.GetUIntPtr(), GetFnAddr(&Hook_AddItem));
#else
				std::uintptr_t addr = PLAYER_CHARACTER_VTBL + (PSIZE * 0x5A);
				_AddItem = *reinterpret_cast<AddItem_t*>(addr);
				SafeWrite32(addr, GetFnAddr(&Hook_AddItem));
#endif
				_MESSAGE("[MESSAGE] Installed add item hook");
			}

			{
#if _WIN64
				RelocPtr<PickUpItem_t> vFunc(PLAYER_CHARACTER_VTBL + (PSIZE * 0xCC));
				_PickUpItem = *vFunc;
				SafeWrite64(vFunc.GetUIntPtr(), GetFnAddr(&Hook_PickUpItem));
#else
				std::uintptr_t addr = PLAYER_CHARACTER_VTBL + (PSIZE * 0xCB);
				_PickUpItem = *reinterpret_cast<PickUpItem_t*>(addr);
				SafeWrite32(addr, GetFnAddr(&Hook_PickUpItem));
#endif
				_MESSAGE("[MESSAGE] Installed pick up item hook");
			}
		}
	};
}


void InstallHooks()
{
	PlayerCharacterEx::InstallHooks();
}
