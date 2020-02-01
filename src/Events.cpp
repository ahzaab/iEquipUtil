#include "Events.h"

#include <limits>

#include "Registration.h"


namespace Events
{
	namespace
	{
		enum
		{
			kSlotID_Default = 0,
			kSlotID_Right = 1,
			kSlotID_Left = 2
		};


		UInt32 GetEquippedSlots(RE::TESObjectWEAP* a_weap)
		{
			auto player = RE::PlayerCharacter::GetSingleton();

			auto rightHand = player->currentProcess->GetEquippedRightHand();
			auto leftHand = player->currentProcess->GetEquippedLeftHand();

			UInt32 slotID = 0;
			if (rightHand && rightHand->formID == a_weap->formID) {
				slotID += kSlotID_Right;
			}
			if (leftHand && leftHand->formID == a_weap->formID) {
				slotID += kSlotID_Left;
			}
			return slotID;
		}


		UInt32 GetUnequippedSlots()
		{
			auto player = RE::PlayerCharacter::GetSingleton();

			UInt32 slotID = !player->currentProcess->GetEquippedRightHand() ? kSlotID_Right : kSlotID_Default;
			slotID += !player->currentProcess->GetEquippedRightHand() ? kSlotID_Left : kSlotID_Default;
			return slotID;
		}
	}


	EquipEventHandler* EquipEventHandler::GetSingleton()
	{
		static EquipEventHandler singleton;
		return &singleton;
	}


	auto EquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_dispatcher)
		-> EventResult
	{
		if (!a_event || !a_event->actor || !a_event->actor->IsPlayerRef()) {
			return EventResult::kContinue;
		}

		auto weap = RE::TESForm::LookupByID<RE::TESObjectWEAP>(a_event->baseObject);
		if (!weap || !weap->IsBound()) {
			return EventResult::kContinue;
		}

		if (a_event->equipped) {
			auto equipSlots = GetEquippedSlots(weap);
			OnBoundWeaponEquippedRegSet::GetSingleton()->QueueEvent(to_underlying(weap->weaponData.animationType), equipSlots);
		} else {
			auto unequipSlots = GetUnequippedSlots();
			OnBoundWeaponUnequippedRegSet::GetSingleton()->QueueEvent(weap, unequipSlots);
		}

		return EventResult::kContinue;
	}
}
