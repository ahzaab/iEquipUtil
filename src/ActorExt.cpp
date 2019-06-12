#include "ActorExt.h"

#include "GameBSExtraData.h"  // BaseExtraList
#include "GameData.h"  // EquipManager
#include "GameExtraData.h"  // ExtraContainerChanges, InventoryEntryData
#include "GameForms.h"  // TESForm, BGSEquipSlot
#include "GameObjects.h"  // AlchemyItem, TESAmmo
#include "GameReferences.h"  // Actor
#include "PapyrusNativeFunctions.h"  // StaticFunctionTag, NativeFunction
#include "PapyrusVM.h"  // VMClassRegistry

#include <cmath>  // abs
#include <cstdint>  // uintptr_t

#include "InventoryUtil.h"  // ForEachInvEntry
#include "Registration.h"  // Registration

#include "RE/BSTList.h"  // BSSimpleList
#include "RE/ExtraPoison.h"  // ExtraPoison
#include "RE/Memory.h"  // free


namespace ActorExt
{
	namespace
	{
		enum class ActorValue8 : UInt8
		{
			kNone = static_cast<std::underlying_type_t<ActorValue8>>(-1),
			kAggresion = 0,
			kConfidence = 1,
			kEnergy = 2,
			kMorality = 3,
			kMood = 4,
			kAssistance = 5,
			kOneHanded = 6,
			kTwoHanded = 7,
			kArchery = 8,
			kBlock = 9,
			kSmithing = 10,
			kHeavyArmor = 11,
			kLightArmor = 12,
			kPickpocket = 13,
			kLockpicking = 14,
			kSneak = 15,
			kAlchemy = 16,
			kSpeech = 17,
			kAlteration = 18,
			kConjuration = 19,
			kDestruction = 20,
			kIllusion = 21,
			kRestoration = 22,
			kEnchanting = 23,
			kHealth = 24,
			kMagicka = 25,
			kStamina = 26,
			kHealRate = 27,
			kMagickaRate = 28,
			StaminaRate = 29,
			kSpeedMult = 30,
			kInventoryWeight = 31,
			kCarryWeight = 32,
			kCriticalChance = 33,
			kMeleeDamage = 34,
			kUnarmedDamage = 35,
			kMass = 36,
			kVoicePoints = 37,
			kVoiceRate = 38,
			kDamageResist = 39,
			kPoisonResist = 40,
			kResistFire = 41,
			kResistShock = 42,
			kResistFrost = 43,
			kResistMagic = 44,
			kResistDisease = 45,
			kUnknown46 = 46,
			kUnknown47 = 47,
			kUnknown48 = 48,
			kUnknown49 = 49,
			kUnknown50 = 50,
			kUnknown51 = 51,
			kUnknown52 = 52,
			kParalysis = 53,
			kInvisibility = 54,
			kNightEye = 55,
			kDetectLifeRange = 56,
			kWaterBreathing = 57,
			kWaterWalking = 58,
			kUnknown59 = 59,
			kFame = 60,
			kInfamy = 61,
			kJumpingBonus = 62,
			kWardPower = 63,
			kRightItemCharge = 64,
			kArmorPerks = 65,
			kShieldPerks = 66,
			kWardDeflection = 67,
			kVariable01 = 68,
			kVariable02 = 69,
			kVariable03 = 70,
			kVariable04 = 71,
			kVariable05 = 72,
			kVariable06 = 73,
			kVariable07 = 74,
			kVariable08 = 75,
			kVariable09 = 76,
			kVariable10 = 77,
			kBowSpeedBonus = 78,
			kFavorActive = 79,
			kFavorsPerDay = 80,
			kFavorsPerDayTimer = 81,
			kLeftItemCharge = 82,
			kAbsorbChance = 83,
			kBlindness = 84,
			kWeaponSpeedMult = 85,
			kShoutRecoveryMult = 86,
			kBowStaggerBonus = 87,
			kTelekinesis = 88,
			kFavorPointsBonus = 89,
			kLastBribedIntimidated = 90,
			kLastFlattered = 91,
			kMovementNoiseMult = 92,
			kBypassVendorStolenCheck = 93,
			kBypassVendorKeywordCheck = 94,
			kWaitingForPlayer = 95,
			kOneHandedModifier = 96,
			kTwoHandedModifier = 97,
			kMarksmanModifier = 98,
			kBlockModifier = 99,
			kSmithingModifier = 100,
			kHeavyArmorModifier = 101,
			kLightArmorModifier = 102,
			kPickpocketModifier = 103,
			kLockpickingModifier = 104,
			kSneakingModifier = 105,
			kAlchemyModifier = 106,
			kSpeechcraftModifier = 107,
			kAlterationModifier = 108,
			kConjurationModifier = 109,
			kDestructionModifier = 110,
			kIllusionModifier = 111,
			kRestorationModifier = 112,
			kEnchantingModifier = 113,
			kOneHandedSkillAdvance = 114,
			kTwoHandedSkillAdvance = 115,
			kMarksmanSkillAdvance = 116,
			kBlockSkillAdvance = 117,
			kSmithingSkillAdvance = 118,
			kHeavyArmorSkillAdvance = 119,
			kLightArmorSkillAdvance = 120,
			kPickpocketSkillAdvance = 121,
			kLockpickingSkillAdvance = 122,
			kSneakingSkillAdvance = 123,
			kAlchemySkillAdvance = 124,
			kSpeechcraftSkillAdvance = 125,
			kAlterationSkillAdvance = 126,
			kConjurationSkillAdvance = 127,
			kDestructionSkillAdvance = 128,
			kIllusionSkillAdvance = 129,
			kRestorationSkillAdvance = 130,
			kEnchantingSkillAdvance = 131,
			kLeftWeaponSpeedMultiply = 132,
			kDragonSouls = 133,
			kCombatHealthRegenMultiply = 134,
			kOneHandedPowerModifier = 135,
			kTwoHandedPowerModifier = 136,
			kMarksmanPowerModifier = 137,
			kBlockPowerModifier = 138,
			kSmithingPowerModifier = 139,
			kHeavyArmorPowerModifier = 140,
			kLightArmorPowerModifier = 141,
			kPickpocketPowerModifier = 142,
			kLockpickingPowerModifier = 143,
			kSneakingPowerModifier = 144,
			kAlchemyPowerModifier = 145,
			kSpeechcraftPowerModifier = 146,
			kAlterationPowerModifier = 147,
			kConjurationPowerModifier = 148,
			kDestructionPowerModifier = 149,
			kIllusionPowerModifier = 150,
			kRestorationPowerModifier = 151,
			kEnchantingPowerModifier = 152,
			kDragonRend = 153,
			kAttackDamageMult = 154,
			kHealRateMult = 155,
			kMagickaRateMult = 156,
			StaminaRateMult = 157,
			kWerewolfPerks = 158,
			kVampirePerks = 159,
			kGrabActorOffset = 160,
			kGrabbed = 161,
			kUnknown162 = 162,
			kReflectDamage = 163
		};


		enum : UInt32
		{
			kHandSlot_Left = 0,
			kHandSlot_Right = 1
		};


		struct ActorValueModifiers
		{
			struct Modifiers
			{
				enum
				{
					kPermanent = 0,
					kTemporary,
					kDamage,
					kTotal
				};
			};


			// members
			float modifiers[Modifiers::kTotal];	// 0
		};
		//STATIC_ASSERT(sizeof(ActorValueModifiers) == 0xC);


		struct ActorValueMap
		{
			template<typename T>
			struct LocalMap
			{
				T* operator[](ActorValue8 a_actorValue)
				{
					if (actorValues && entries) {
						UInt32 idx = 0;
						while (actorValues[idx] != (ActorValue8)0) {
							if (actorValues[idx] == a_actorValue) {
								break;
							}
							++idx;
						}
						if (actorValues[idx] != (ActorValue8)0) {
							return &entries[idx];
						}
					}
					return 0;
				}


				// members
				ActorValue8*	actorValues;	// 00
				T*				entries;		// 08
			};
			//STATIC_ASSERT(sizeof(LocalMap<float>) == 0x10);


			// members
			LocalMap<float>					baseValues;	// 00
			LocalMap<ActorValueModifiers>	modifiers;	// 10
		};
		//STATIC_ASSERT(sizeof(ActorValueMap) == 0x20);
	}


	TESAmmo* GetEquippedAmmo(StaticFunctionTag*, Actor* a_actor)
	{
		if (!a_actor) {
			_WARNING("[WARNING] a_actor is a NONE form!");
			return 0;
		}

		TESAmmo* ammo = 0;
		ForEachInvEntry(a_actor, [&](InventoryEntryData* a_entryData) -> bool
		{
			if (a_entryData->type->formType == kFormType_Ammo) {
				ForEachExtraList(a_entryData, [&](BaseExtraList* a_extraList) -> bool
				{
					if (a_extraList->HasType(kExtraData_Worn)) {
						ammo = static_cast<TESAmmo*>(a_entryData->type);
					}
					return ammo == 0;
				});
			}
			return ammo == 0;
		});
		return ammo;
	}


	float GetAVDamage(StaticFunctionTag*, Actor* a_actor, UInt32 a_actorValue)
	{
#if _WIN64
		constexpr std::uintptr_t avMapOffset = 0x200;
		constexpr std::uintptr_t avHealthOffset = 0x228;
		constexpr std::uintptr_t avMagickaOffset = 0x234;
		constexpr std::uintptr_t avStaminaOffset = 0x240;
		constexpr std::uintptr_t avVoicePointsOffset = 0x24C;
#else
		constexpr std::uintptr_t avMapOffset = 0x13C;
		constexpr std::uintptr_t avHealthOffset = 0x150;
		constexpr std::uintptr_t avMagickaOffset = 0x15C;
		constexpr std::uintptr_t avStaminaOffset = 0x168;
		constexpr std::uintptr_t avVoicePointsOffset = 0x174;
#endif

		if (!a_actor) {
			_WARNING("[WARNING] a_actor is a NONE form!");
			return 0.0;
		} else if (a_actorValue > 163) {
			_WARNING("[WARNING] a_actorValue is out of range!");
			return 0.0;
		}

		std::uintptr_t actorBase = reinterpret_cast<std::uintptr_t>(a_actor);
		ActorValueModifiers* avMods = 0;
		ActorValue8 actorValue = static_cast<ActorValue8>(a_actorValue);
		switch (actorValue) {
		case ActorValue8::kHealth:
			avMods = reinterpret_cast<ActorValueModifiers*>(actorBase + avHealthOffset);
			break;
		case ActorValue8::kMagicka:
			avMods = reinterpret_cast<ActorValueModifiers*>(actorBase + avMagickaOffset);
			break;
		case ActorValue8::kStamina:
			avMods = reinterpret_cast<ActorValueModifiers*>(actorBase + avStaminaOffset);
			break;
		case ActorValue8::kVoicePoints:
			avMods = reinterpret_cast<ActorValueModifiers*>(actorBase + avVoicePointsOffset);
			break;
		default:
			{
				ActorValueMap* avMap = reinterpret_cast<ActorValueMap*>(actorBase + avMapOffset);
				avMods = avMap->modifiers[actorValue];
			}
			break;
		}

		return avMods ? std::abs(avMods->modifiers[ActorValueModifiers::Modifiers::kDamage]) : 0.0;
	}


	TESRace* GetBaseRace(StaticFunctionTag*, Actor* a_actor)
	{
		if (!a_actor) {
			_WARNING("[WARNING] a_actor is a NONE form!");
			return 0;
		}

		if (a_actor->formID == (*g_thePlayer)->formID) {
			return (*g_thePlayer)->PlayerCharacter::race;
		} else {
			return static_cast<TESNPC*>(a_actor->baseForm)->race.race;
		}
	}


	float GetMagicEffectMagnitude(StaticFunctionTag*, Actor* a_actor, EffectSetting* a_mgef)
	{
		if (!a_actor) {
			_WARNING("[WARNING] a_actor is a NONE form!");
			return 0.0;
		} else if (!a_mgef) {
			_WARNING("[WARNING] a_mgef is a NONE form!");
			return 0.0;
		}

		RE::BSSimpleList<ActiveEffect*>* activeEffects = reinterpret_cast<RE::BSSimpleList<ActiveEffect*>*>(a_actor->magicTarget.GetActiveEffects());
		if (!activeEffects) {
			return 0.0;
		}

		for (auto& activeEffect : *activeEffects) {
			if (activeEffect->effect && activeEffect->effect->mgef == a_mgef) {
				return activeEffect->effect->magnitude;
			}
		}
		return 0.0;
	}


	bool RegisterFuncs(VMClassRegistry* a_registry)
	{
		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, TESAmmo*, Actor*>("GetEquippedAmmo", "iEquip_ActorExt", GetEquippedAmmo, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, float, Actor*, UInt32>("GetAVDamage", "iEquip_ActorExt", GetAVDamage, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, TESRace*, Actor*>("GetBaseRace", "iEquip_ActorExt", GetBaseRace, a_registry));

		a_registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, float, Actor*, EffectSetting*>("GetMagicEffectMagnitude", "iEquip_ActorExt", GetMagicEffectMagnitude, a_registry));

		return true;
	}
}
