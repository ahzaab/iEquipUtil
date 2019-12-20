#include "SoulSeeker.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>


namespace SoulSeeker
{
	namespace
	{
		RE::TESSoulGem* SoulGem::GetSoulGem()
		{
			assert(entryData && entryData->object && entryData->object->IsSoulGem());
			return static_cast<RE::TESSoulGem*>(entryData->object);
		}


		void SoulGem::RemoveExtraSoul()
		{
			if (entryData->extraLists) {
				for (auto& xList : *entryData->extraLists) {
					auto xSoul = xList->GetByType<RE::ExtraSoul>();
					if (xSoul) {
						xList->Remove(xSoul);
						delete xSoul;
						break;
					}
				}
			}
		}


		const SoulGem& NearestNeighbour(const GemList& a_gems, const SoulGem& a_comp)
		{
			static auto diffX = [](const SoulGem& a_lhs, const SoulGem& a_rhs) -> float
			{
				return static_cast<float>(a_lhs.gemSize - a_rhs.gemSize);
			};

			static auto diffY = [](const SoulGem& a_lhs, const SoulGem& a_rhs) -> float
			{
				return static_cast<float>(a_lhs.soulSize - a_rhs.soulSize);
			};

			static auto euclidean_distance = [](const SoulGem& a_lhs, const SoulGem& a_rhs) -> float
			{
				return std::sqrtf(std::powf(diffX(a_lhs, a_rhs), 2) + std::powf(diffY(a_lhs, a_rhs), 2));
			};

			std::size_t idx = 0;
			float min = std::numeric_limits<float>::max();
			float tmp;
			for (std::size_t i = 0; i < a_gems.size(); ++i) {
				tmp = euclidean_distance(a_gems[i], a_comp);
				if (tmp < min) {
					min = tmp;
					idx = i;
				}
			}
			return a_gems[idx];
		}


		bool ValidateParams(RE::SoulLevel a_reqCharge, FillMethod a_fillMethod)
		{
			if (a_reqCharge < RE::SoulLevel::kPetty || a_reqCharge > RE::SoulLevel::kGrand) {
				_WARNING("Invalid soul size! (%i)", to_underlying(a_reqCharge));
				return false;
			}

			switch (a_fillMethod) {
			case FillMethod::kSmallerSoulsFirst:
			case FillMethod::kUseLargestSoul:
				break;
			default:
				_WARNING("Invalid fill method! (%i)", to_underlying(a_fillMethod));
				return false;
			}

			return true;
		}


		bool IsReusable(RE::TESSoulGem* a_gem)
		{
			using Object = RE::BGSDefaultObjectManager::DefaultObject;
			auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
			auto reusableSoulGem = dobj->GetObject<RE::BGSKeyword>(Object::kReusableSoulGem);
			return a_gem->HasKeyword(reusableSoulGem);
		}


		void ApplyVerticalShift(RE::SoulLevel a_reqCharge, FillMethod a_fillMethod, GemList& a_gems)
		{
			switch (a_fillMethod) {
			case FillMethod::kSmallerSoulsFirst:
				for (auto& gem : a_gems) {
					gem.soulSize += a_reqCharge;
				}
				break;
			case FillMethod::kUseLargestSoul:
				for (auto& gem : a_gems) {
					if (gem.soulSize < a_reqCharge) {
						gem.soulSize = a_reqCharge - gem.soulSize + RE::SoulLevel::kGrand;
					}
				}
				break;
			}
		}


		GemList ParseContainer(RE::TESObjectREFR* a_container, bool a_partialFill)
		{
			auto inv = a_container->GetInventory([](RE::TESBoundObject* a_object) -> bool
			{
				return a_object->IsSoulGem();
			});

			GemList acceptedGems;
			GemList rejectedGems;
			auto considerGem = [&](SoulGem a_soulGem) -> void
			{
				if (a_soulGem.soulSize > RE::SoulLevel::kNone) {
					if (!a_partialFill) {
						if (a_soulGem.soulSize == a_soulGem.gemSize) {
							acceptedGems.emplace_back(std::move(a_soulGem));
						} else {
							rejectedGems.emplace_back(std::move(a_soulGem));
						}
					} else {
						acceptedGems.emplace_back(std::move(a_soulGem));
					}
				}
			};

			for (auto& item : inv) {
				auto entryData = item.second.second;
				if (entryData->extraLists) {
					auto soulGem = static_cast<RE::TESSoulGem*>(entryData->object);
					auto gemSize = soulGem->GetMaximumCapacity();

					SInt32 rawCount = item.second.first;
					for (auto& xList : *entryData->extraLists) {
						auto xSoul = xList->GetByType<RE::ExtraSoul>();
						if (xSoul) {
							auto soulSize = xSoul->containedSoul;
							considerGem({ gemSize, soulSize, entryData, xList });
						}
						rawCount -= xList->GetCount();
					}

					if (rawCount > 0) {
						auto soulSize = soulGem->containedSoul;
						considerGem({ gemSize, soulSize, entryData, 0 });
					}
				}
			}

			return acceptedGems.empty() ? rejectedGems : acceptedGems;
		}
	}


	SInt32 BringMeASoul(RE::StaticFunctionTag*, UInt32 a_reqCharge, UInt32 a_fillMethod, bool a_partialFill, bool a_wasteOK)
	{
		using RemoveType = RE::TESObjectREFR::RemoveType;

		auto reqCharge = static_cast<RE::SoulLevel>(a_reqCharge);
		auto fillMethod = static_cast<FillMethod>(a_fillMethod);
		if (!ValidateParams(reqCharge, fillMethod)) {
			_WARNING("Parameters failed to validate!");
			return -1;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto gems = ParseContainer(player, a_partialFill);
		if (gems.empty()) {
			_ERROR("No soul gems found!\n");
			return -1;
		}

		ApplyVerticalShift(reqCharge, fillMethod, gems);

		SoulGem idealGem(reqCharge, reqCharge, 0, 0);
		SoulGem foundGem;
		if (!a_wasteOK) {
			GemList noWaste;
			for (auto& gem : gems) {
				if (gem.soulSize <= reqCharge) {
					noWaste.push_back(gem);
				}
			}
			if (!noWaste.empty()) {
				foundGem = NearestNeighbour(noWaste, idealGem);
			}
		}

		if (!foundGem.entryData) {
			foundGem = NearestNeighbour(gems, idealGem);
		}

		foundGem.RemoveExtraSoul();

		auto gem = foundGem.GetSoulGem();
		if (!IsReusable(gem)) {
			UInt32 droppedHandle;
			player->RemoveItem(droppedHandle, gem, 1, RemoveType::kRemove, foundGem.extraList, 0);
		}
		return to_underlying(foundGem.origSoulSize);
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("BringMeASoul", "iEquip_SoulSeeker", BringMeASoul);

		return true;
	}
}
