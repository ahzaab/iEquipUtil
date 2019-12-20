#pragma once

#include <vector>

#include "RE/Skyrim.h"


namespace SoulSeeker
{
	namespace
	{
		enum class FillMethod : UInt32
		{
			kSmallerSoulsFirst = 0,
			kUseLargestSoul = 1
		};


		struct SoulGem
		{
			constexpr SoulGem() :
				SoulGem(RE::SoulLevel::kNone, RE::SoulLevel::kNone, 0, 0)
			{}


			constexpr SoulGem(RE::SoulLevel a_gemSize, RE::SoulLevel a_soulSize, RE::InventoryEntryData* a_entryData, RE::ExtraDataList* a_extraList) :
				gemSize(a_gemSize),
				soulSize(a_soulSize),
				origSoulSize(a_soulSize),
				entryData(a_entryData),
				extraList(a_extraList)
			{}


			RE::TESSoulGem*	GetSoulGem();
			void			RemoveExtraSoul();


			RE::SoulLevel			gemSize;
			RE::SoulLevel			soulSize;
			RE::SoulLevel			origSoulSize;
			RE::InventoryEntryData*	entryData;
			RE::ExtraDataList*		extraList;
		};


		using GemList = std::vector<SoulGem>;


		const SoulGem&	NearestNeighbour(const GemList& a_gems, const SoulGem& a_comp);
		bool			ValidateParams(RE::SoulLevel a_reqCharge, FillMethod a_fillMethod);
		bool			IsReusable(RE::TESSoulGem* a_gem);
		void			ApplyVerticalShift(RE::SoulLevel a_reqCharge, FillMethod a_fillMethod, GemList& a_gems);
		GemList			ParseContainer(RE::TESObjectREFR* a_container, bool a_partialFill);
	}


	SInt32 BringMeASoul(RE::StaticFunctionTag*, UInt32 a_reqCharge, UInt32 a_fillMethod, bool a_partialFill, bool a_wasteOK);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
