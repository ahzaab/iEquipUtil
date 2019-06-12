#include "Events.h"

#include "GameAPI.h"  // g_thePlayer
#include "GameEvents.h"  // EventResult, EventDispatcher
#include "GameObjects.h"  // TESObjectWEAP

#include <limits>  // numeric_limits

#include "Registration.h"  // OnBoundWeaponEquippedRegSet, OnBoundWeaponUnequippedRegSet

#include "RE/TESEquipEvent.h"  // TESEquipEvent


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


		UInt32 GetEquippedSlots(Actor* a_actor, TESObjectWEAP* a_weap)
		{
			if (!a_actor) {
				_ERROR("[WARNING] a_actor is a NONE form!\n");
				return kSlotID_Default;
			} else if (!a_weap) {
				_ERROR("[WARNING] a_weap is a NONE form!\n");
				return kSlotID_Default;
			}

			TESForm* rightHand = a_actor->processManager->equippedObject[ActorProcessManager::kEquippedHand_Right];
			TESForm* leftHand = a_actor->processManager->equippedObject[ActorProcessManager::kEquippedHand_Left];

			UInt32 slotID = 0;
			if (rightHand && rightHand->formID == a_weap->formID) {
				slotID += kSlotID_Right;
			}
			if (leftHand && leftHand->formID == a_weap->formID) {
				slotID += kSlotID_Left;
			}
			return slotID;
		}


		UInt32 GetUnequippedSlots(Actor* a_actor)
		{
			if (!a_actor) {
				_ERROR("[WARNING] a_actor is a NONE form!\n");
				return kSlotID_Default;
			}

			UInt32 slotID = !a_actor->processManager->equippedObject[ActorProcessManager::kEquippedHand_Right] ? kSlotID_Right : kSlotID_Default;
			slotID += !a_actor->processManager->equippedObject[ActorProcessManager::kEquippedHand_Left] ? kSlotID_Left : kSlotID_Default;
			return slotID;
		}
	}


	EventResult EquipEventHandler::ReceiveEvent(RE::TESEquipEvent* a_event, EventDispatcher<RE::TESEquipEvent>* a_dispatcher)
	{
		UInt32 sourceFormID = a_event->akSource ? a_event->akSource->formID : 0;
		UInt32 playerFormID = g_thePlayer ? (*g_thePlayer)->formID : std::numeric_limits<UInt32>::min();
		if (sourceFormID != playerFormID) {
			return kEvent_Continue;
		}

		TESForm* form = LookupFormByID(a_event->formID);
		if (!form || form->formType != kFormType_Weapon) {
			return kEvent_Continue;
		}

		TESObjectWEAP* weap = static_cast<TESObjectWEAP*>(form);
		if ((weap->gameData.flags1 & TESObjectWEAP::GameData::kFlags_BoundWeapon) == 0) {
			return kEvent_Continue;
		}

		if (a_event->isEquipping) {
			UInt32 equipSlots = GetEquippedSlots((*g_thePlayer), weap);
			OnBoundWeaponEquippedRegSet::GetSingleton()->QueueEvent(weap->gameData.type, equipSlots);
		} else {
			UInt32 unequipSlots = GetUnequippedSlots((*g_thePlayer));
			OnBoundWeaponUnequippedRegSet::GetSingleton()->QueueEvent(weap, unequipSlots);
		}

		return kEvent_Continue;
	}


	EquipEventHandler* EquipEventHandler::GetSingleton()
	{
		static EquipEventHandler singleton;
		return &singleton;
	}
}
