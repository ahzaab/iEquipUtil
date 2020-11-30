#include"Hooks.h"

#include "skse64_common/SafeWrite.h"

#include <cassert>
#include <cstdint>
#include <typeinfo>

#include "RE/Skyrim.h"
#include "REL/Relocation.h"

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
				REL::Offset<RemoveItem_t*> removeItem(RE::Offset::PlayerCharacter::Vtbl, 0x8 * 0x56);
				_RemoveItem = *removeItem;
				SafeWrite64(removeItem.address(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_RemoveItem));

				REL::Offset<AddObjectToContainer_t*> addObjectToContainer(RE::Offset::PlayerCharacter::Vtbl, 0x8 * 0x5A);
				_AddObjectToContainer = *addObjectToContainer;
				SafeWrite64(addObjectToContainer.address(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_AddObjectToContainer));

				REL::Offset<PickUpObject_t*> pickUpItem(RE::Offset::PlayerCharacter::Vtbl, 0x8 * 0xCC);
				_PickUpObject = *pickUpItem;
				SafeWrite64(pickUpItem.address(), unrestricted_cast<std::uintptr_t>(&PlayerCharacterEx::Hook_PickUpObject));

				_MESSAGE("Installed hooks for (%s)", typeid(PlayerCharacterEx).name());
			}

		private:
			using RemoveItem_t = decltype(&RE::PlayerCharacter::RemoveItem);	// 56
			inline static REL::Function<RemoveItem_t> _RemoveItem;

			using AddObjectToContainer_t = decltype(&RE::PlayerCharacter::AddObjectToContainer);	// 5A
			inline static REL::Function<AddObjectToContainer_t> _AddObjectToContainer;

			using PickUpObject_t = decltype(&RE::PlayerCharacter::PickUpObject);	// CC
			inline static REL::Function<PickUpObject_t> _PickUpObject;


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
				if (a_count <= 0 || !a_object || !manager->IsInit() || !manager->IsTrackedType(a_object) /* || a_object->HasVMAD() */ ) {
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
					//manager->ActivateAndDispatch(a_object, a_extraList, 1);
					_AddObjectToContainer(this, a_object, a_extraList, 1, a_fromRefr);
				}

				auto invChanges = this->GetInventoryChanges();

				if (invChanges && invChanges->entryList)
				{
					for (auto& entry : *invChanges->entryList)
					{
						auto item = entry->object;

						if (item == a_object)
						{
							auto rawCount = entry->countDelta;

							if (entry->extraLists)
							{
								for (auto& xList : *entry->extraLists)
								{
									if (xList)
									{
										auto count = xList->GetCount();
										rawCount -= count;
										manager->ActivateAndDispatch(item, xList, count);
									}
								}
							}

							RE::ExtraDataList* xList;
							while (rawCount-- > 0)
							{
								xList = 0;
								manager->ActivateAndDispatch(item, xList, 1);
								entry->AddExtraList(xList);
							}
						}
					}
				}
			}


			void Hook_PickUpObject(RE::TESObjectREFR* a_object, UInt32 a_count, bool a_arg3, bool a_playSound)
			{
				auto manager = RefHandleManager::GetSingleton();
				if (a_count <= 0 || !a_object || !a_object->GetBaseObject() || !manager->IsInit() || !manager->IsTrackedType(a_object->GetBaseObject()) /* || a_object->HasVMAD() */ ) {
					return _PickUpObject(this, a_object, a_count, a_arg3, a_playSound);
				}

				auto countLeft = a_count;

				auto count = a_object->extraList.GetCount();
				countLeft -= count;
				manager->ActivateAndDispatch(a_object->GetBaseObject(), a_object->extraList, count);
				_PickUpObject(this, a_object, count, a_arg3, a_playSound);

				while (countLeft-- > 0) {
					_AddObjectToContainer(this, a_object->GetBaseObject(), nullptr, 1, nullptr);
				}

				auto invChanges = this->GetInventoryChanges();

				if (invChanges && invChanges->entryList) {
					for (auto& entry : *invChanges->entryList) {
						
						auto item = entry->object;

						if (item == a_object->GetBaseObject())
						{
							auto rawCount = entry->countDelta;

							if (entry->extraLists)
							{
								for (auto& xList : *entry->extraLists) {
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
								entry->AddExtraList(xList);
							}
						}
					}
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
