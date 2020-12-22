#include "pch.h"

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


        uint32_t GetEquippedSlots(RE::TESObjectWEAP* a_weap)
        {
            auto player = RE::PlayerCharacter::GetSingleton();

            auto rightHand = player->currentProcess->GetEquippedRightHand();
            auto leftHand = player->currentProcess->GetEquippedLeftHand();

            uint32_t slotID = 0;
            if (rightHand && rightHand->formID == a_weap->formID) {
                slotID += kSlotID_Right;
            }
            if (leftHand && leftHand->formID == a_weap->formID) {
                slotID += kSlotID_Left;
            }
            return slotID;
        }


        uint32_t GetUnequippedSlots(RE::TESObjectWEAP* a_weap)
        {
            auto player = RE::PlayerCharacter::GetSingleton();

            if (!a_weap) {
                return kSlotID_Default;
            }

            if (a_weap->IsTwoHandedAxe() ||
                a_weap->IsTwoHandedSword() ||
                a_weap->IsBow() ||
                a_weap->IsCrossbow()) {
                return kSlotID_Left + kSlotID_Right;
            }

            uint32_t slotID = !player->currentProcess->GetEquippedRightHand() ? kSlotID_Right : kSlotID_Default;
            slotID += !player->currentProcess->GetEquippedLeftHand() ? kSlotID_Left : kSlotID_Default;
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
            OnBoundWeaponEquippedRegSet::GetSingleton()->QueueEvent(weap->weaponData.animationType.underlying(), equipSlots);
        } else {
            auto unequipSlots = GetUnequippedSlots(weap);
            OnBoundWeaponUnequippedRegSet::GetSingleton()->QueueEvent(weap, unequipSlots);
        }

        return EventResult::kContinue;
    }
}
