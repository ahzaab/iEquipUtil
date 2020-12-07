#include "PCH.h"
#include "ActorExt.h"

#include <cmath>
#include <cstdint>

#include "Registration.h"


namespace ActorExt
{
    float GetAVDamage(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor, uint32_t a_actorValue)
    {
        using Modifier = RE::ACTOR_VALUE_MODIFIERS;

        if (!a_actor) {
            a_vm->TraceStack("a_actor is a NONE form!", a_stackID, Severity::kWarning);
            return 0.0;
        } else if (a_actorValue >= static_cast<uint32_t>(RE::ActorValue::kTotal)) {
            a_vm->TraceStack("a_actorValue is out of range!", a_stackID, Severity::kWarning);
            return 0.0;
        }

        auto actorValue = static_cast<RE::ActorValue>(a_actorValue);
        switch (actorValue) {
        case RE::ActorValue::kHealth:
            return std::abs(a_actor->healthModifiers.modifiers[Modifier::kDamage]);
        case RE::ActorValue::kMagicka:
            return std::abs(a_actor->magickaModifiers.modifiers[Modifier::kDamage]);
        case RE::ActorValue::kStamina:
            return std::abs(a_actor->staminaModifiers.modifiers[Modifier::kDamage]);
        case RE::ActorValue::kVoicePoints:
            return std::abs(a_actor->voicePointsModifiers.modifiers[Modifier::kDamage]);
        default:
        {
            auto mod = a_actor->avStorage.modifiers[actorValue];
            return mod ? std::abs(mod->modifiers[Modifier::kDamage]) : 0.0f;
        }
        }
    }


    RE::TESRace* GetBaseRace(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor)
    {
        if (!a_actor) {
            a_vm->TraceStack("a_actor is a NONE form!", a_stackID, Severity::kWarning);
            return 0;
        }

        if (a_actor->IsPlayerRef()) {
            auto player = RE::PlayerCharacter::GetSingleton();
            return player->charGenRace;
        } else {
            auto npc = a_actor->GetActorBase();
            return npc ? npc->race : 0;
        }
    }


    RE::TESAmmo* GetEquippedAmmo(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor)
    {
        if (!a_actor) {
            a_vm->TraceStack("a_actor is a NONE form!", a_stackID, Severity::kWarning);
            return 0;
        }

        auto player = RE::PlayerCharacter::GetSingleton();
        auto inv = player->GetInventory([](RE::TESBoundObject& a_object) -> bool {
            return a_object.IsAmmo();
        });

        for (auto& elem : inv) {
            auto& entryData = elem.second.second;
            if (entryData->extraLists) {
                for (auto& xList : *entryData->extraLists) {
                    if (xList && xList->HasType<RE::ExtraWorn>()) {
                        return static_cast<RE::TESAmmo*>(elem.first);
                    }
                }
            }
        }

        return 0;
    }


    RE::TESObjectWEAP* GetEquippedWeapon(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor, uint32_t a_hand)
    {
        using EquippedHand = RE::AIProcess::Hand;

        if (!a_actor) {
            a_vm->TraceStack("a_actor is a NONE form!", a_stackID, Severity::kWarning);
            return 0;
        } else if (!a_actor->currentProcess) {
            a_vm->TraceStack("Actor does not have an AI process!", a_stackID, Severity::kWarning);
            return 0;
        }

        EquippedHand equipHand;
        switch (static_cast<Hand>(a_hand)) {
        case Hand::kRight:
            equipHand = EquippedHand::kRight;
            break;
        case Hand::kLeft:
            equipHand = EquippedHand::kLeft;
            break;
        default:
            logger::warn("Invalid slot ({})!"sv, a_hand);
            return 0;
        }

        auto item = a_actor->currentProcess->equippedObjects[equipHand];
        return item && item->IsWeapon() ? static_cast<RE::TESObjectWEAP*>(item) : 0;
    }


    float GetMagicEffectMagnitude(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, const RE::EffectSetting* a_mgef)
    {
        if (!a_actor) {
            a_vm->TraceStack("a_actor is a NONE form!", a_stackID, Severity::kWarning);
            return 0.0;
        } else if (!a_mgef) {
            a_vm->TraceStack("a_mgef is a NONE form!", a_stackID, Severity::kWarning);
            return 0.0;
        }

        auto activeEffects = a_actor->GetActiveEffectList();
        if (!activeEffects) {
            return 0.0;
        }

        for (auto& activeEffect : *activeEffects) {
            if (activeEffect->GetBaseObject() == a_mgef) {
                return activeEffect->magnitude;
            }
        }
        return 0.0;
    }


    bool RegisterFuncs(VM* a_vm)
    {
        a_vm->RegisterFunction("GetAVDamage", "iEquip_ActorExt", GetAVDamage, true);
        a_vm->RegisterFunction("GetBaseRace", "iEquip_ActorExt", GetBaseRace, true);
        a_vm->RegisterFunction("GetEquippedAmmo", "iEquip_ActorExt", GetEquippedAmmo);
        a_vm->RegisterFunction("GetEquippedWeapon", "iEquip_ActorExt", GetEquippedWeapon, true);
        a_vm->RegisterFunction("GetMagicEffectMagnitude", "iEquip_ActorExt", GetMagicEffectMagnitude);

        return true;
    }
}
