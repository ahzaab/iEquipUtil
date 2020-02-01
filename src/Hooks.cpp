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
			static void InstallHooks()
			{
				REL::Offset<RemoveItem_t**> removeItem(RE::Offset::PlayerCharacter::Vtbl + (0x8 * 0x56));
				_RemoveItem = *removeItem;
				SafeWrite64(removeItem.GetAddress(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_RemoveItem));

				REL::Offset<AddObjectToContainer_t**> addObjectToContainer(RE::Offset::PlayerCharacter::Vtbl + (0x8 * 0x5A));
				_AddObjectToContainer = *addObjectToContainer;
				SafeWrite64(addObjectToContainer.GetAddress(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_AddObjectToContainer));

				REL::Offset<PickUpObject_t**> pickUpItem(RE::Offset::PlayerCharacter::Vtbl + (0x8 * 0xCC));
				_PickUpObject = *pickUpItem;
				SafeWrite64(pickUpItem.GetAddress(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_PickUpObject));

				_MESSAGE("Installed hooks for (%s)", typeid(PlayerCharacterEx).name());
			}

		private:
			using RemoveItem_t = function_type_t<decltype(&RE::PlayerCharacter::RemoveItem)>;	// 56
			inline static RemoveItem_t* _RemoveItem = 0;

			using AddObjectToContainer_t = function_type_t<decltype(&RE::PlayerCharacter::AddObjectToContainer)>;	// 5A
			inline static AddObjectToContainer_t* _AddObjectToContainer = 0;

			using PickUpObject_t = function_type_t<decltype(&RE::PlayerCharacter::PickUpObject)>;	// CC
			inline static PickUpObject_t* _PickUpObject = 0;


			RE::ObjectRefHandle Hook_RemoveItem(RE::TESBoundObject* a_object, SInt32 a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_object && a_extraList && manager->IsInit() && manager->IsTrackedType(a_object)) {
					manager->TryInvalidateAndDispatch(a_object, a_extraList);
				}

				return _RemoveItem(this, a_object, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
			}


			void Hook_AddObjectToContainer(RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, SInt32 a_count, RE::TESObjectREFR* a_fromRefr)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_count <= 0 || !a_object || !manager->IsInit() || !manager->IsTrackedType(a_object) || a_object->HasVMAD()) {
					return _AddObjectToContainer(this, a_object, a_extraList, a_count, a_fromRefr);
				}

				auto countLeft = a_count;

				if (a_extraList) {
					auto count = a_extraList->GetCount();
					countLeft -= count;
					manager->ActivateAndDispatch(a_object, a_extraList, count);
					_AddObjectToContainer(this, a_object, a_extraList, count, a_fromRefr);
				}

				while (countLeft-- > 0) {
					a_extraList = 0;
					manager->ActivateAndDispatch(a_object, a_extraList, 1);
					_AddObjectToContainer(this, a_object, a_extraList, 1, a_fromRefr);
				}
			}


			void Hook_PickUpObject(RE::TESObjectREFR* a_object, UInt32 a_count, bool a_arg3, bool a_playSound)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_count <= 0 || !a_object || !a_object->GetBaseObject() || !manager->IsInit() || !manager->IsTrackedType(a_object->GetBaseObject()) || a_object->HasVMAD()) {
					return _PickUpObject(this, a_object, a_count, a_arg3, a_playSound);
				}

				auto countLeft = a_count;

				auto count = a_object->extraList.GetCount();
				countLeft -= count;
				manager->ActivateAndDispatch(a_object->GetBaseObject(), a_object->extraList, count);
				_PickUpObject(this, a_object, count, a_arg3, a_playSound);

				while (countLeft-- > 0) {
					_AddObjectToContainer(this, a_object->GetBaseObject(), 0, 1, 0);
				}
			}
		};
	}


	void Install()
	{
		PlayerCharacterEx::InstallHooks();
		_MESSAGE("Installed all hooks");
	}
}
