#include"Hooks.h"

#include "skse64/GameReferences.h"
#include "skse64_common/SafeWrite.h"

#include <cassert>
#include <cstdint>
#include <typeinfo>

#include "RE/Skyrim.h"

#include "RefHandleManager.h"
#include "Registration.h"


namespace Hooks
{
	namespace
	{
		class PlayerCharacterEx : public RE::PlayerCharacter
		{
		public:
			using RemoveItem_t = function_type_t<decltype(&RE::PlayerCharacter::RemoveItem)>;	// 56
			inline static RemoveItem_t* _RemoveItem = 0;

			using AddItem_t = function_type_t<decltype(&RE::PlayerCharacter::AddItem)>;	// 5A
			inline static AddItem_t* _AddItem = 0;

			using PickUpItem_t = function_type_t<decltype(&RE::PlayerCharacter::PickUpItem)>;	// CC
			inline static PickUpItem_t* _PickUpItem = 0;


			RE::RefHandle& Hook_RemoveItem(RE::RefHandle& a_dropHandle, RE::TESBoundObject* a_item, SInt32 a_count, RemoveType a_mode, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, void* a_arg7, void* a_arg8)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_item && a_extraList && manager->IsInit() && manager->IsTrackedType(a_item)) {
					manager->TryInvalidateAndDispatch(a_item, a_extraList);
				}

				return _RemoveItem(this, a_dropHandle, a_item, a_count, a_mode, a_extraList, a_moveToRef, a_arg7, a_arg8);
			}


			void Hook_AddItem(RE::TESBoundObject* a_item, RE::ExtraDataList* a_extraList, SInt32 a_count, RE::TESObjectREFR* a_fromRefr)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_count <= 0 || !a_item || !manager->IsInit() || !manager->IsTrackedType(a_item) || a_item->HasVMAD()) {
					return _AddItem(this, a_item, a_extraList, a_count, a_fromRefr);
				}

				auto countLeft = a_count;

				if (a_extraList) {
					auto count = a_extraList->GetCount();
					countLeft -= count;
					manager->ActivateAndDispatch(a_item, a_extraList, count);
					_AddItem(this, a_item, a_extraList, count, a_fromRefr);
				}

				while (countLeft-- > 0) {
					a_extraList = 0;
					manager->ActivateAndDispatch(a_item, a_extraList, 1);
					_AddItem(this, a_item, a_extraList, 1, a_fromRefr);
				}
			}


			void Hook_PickUpItem(RE::TESObjectREFR* a_item, UInt32 a_count, bool a_arg3, bool a_playSound)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_count <= 0 || !a_item || !a_item->baseForm || !manager->IsInit() || !manager->IsTrackedType(a_item->baseForm) || a_item->HasVMAD()) {
					return _PickUpItem(this, a_item, a_count, a_arg3, a_playSound);
				}

				auto countLeft = a_count;

				auto count = a_item->extraData.GetCount();
				countLeft -= count;
				manager->ActivateAndDispatch(a_item->baseForm, a_item->extraData, count);
				_PickUpItem(this, a_item, count, a_arg3, a_playSound);

				while (countLeft-- > 0) {
					AddItem(a_item->baseForm, 0, 1, 0);
				}
			}


			static void InstallHooks()
			{
				REL::Offset<RemoveItem_t**> removeItem(RE::Offset::PlayerCharacter::Vtbl + (0x8 * 0x56));
				_RemoveItem = *removeItem;
				SafeWrite64(removeItem.GetAddress(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_RemoveItem));

				REL::Offset<AddItem_t**> addItem(RE::Offset::PlayerCharacter::Vtbl + (0x8 * 0x5A));
				_AddItem = *addItem;
				SafeWrite64(addItem.GetAddress(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_AddItem));

				REL::Offset<PickUpItem_t**> pickUpItem(RE::Offset::PlayerCharacter::Vtbl + (0x8 * 0xCC));
				_PickUpItem = *pickUpItem;
				SafeWrite64(pickUpItem.GetAddress(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_PickUpItem));

				_MESSAGE("Installed hooks for (%s)", typeid(PlayerCharacterEx).name());
			}
		};
	}


	void Install()
	{
		PlayerCharacterEx::InstallHooks();
		_MESSAGE("Installed all hooks");
	}
}
