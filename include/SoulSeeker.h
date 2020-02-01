#pragma once

#include <vector>

#include "RE/Skyrim.h"


namespace SoulSeeker
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	namespace
	{
		using SoulLevel = RE::SOUL_LEVEL;


		enum class FillMethod : UInt32
		{
			kSmallerSoulsFirst = 0,
			kUseLargestSoul = 1
		};


		struct SoulGem
		{
			constexpr SoulGem() :
				SoulGem(SoulLevel::kNone, SoulLevel::kNone, 0, 0)
			{}


			constexpr SoulGem(SoulLevel a_gemSize, SoulLevel a_soulSize, RE::InventoryEntryData* a_entryData, RE::ExtraDataList* a_extraList) :
				gemSize(a_gemSize),
				soulSize(a_soulSize),
				origSoulSize(a_soulSize),
				entryData(a_entryData),
				extraList(a_extraList)
			{}


			RE::TESSoulGem*	GetSoulGem();
			void			RemoveExtraSoul();


			SoulLevel				gemSize;
			SoulLevel				soulSize;
			SoulLevel				origSoulSize;
			RE::InventoryEntryData*	entryData;
			RE::ExtraDataList*		extraList;
		};


		using GemList = std::vector<SoulGem>;


		const SoulGem&	NearestNeighbour(const GemList& a_gems, const SoulGem& a_comp);
		bool			ValidateParams(SoulLevel a_reqCharge, FillMethod a_fillMethod);
		bool			IsReusable(RE::TESSoulGem* a_gem);
		void			ApplyVerticalShift(SoulLevel a_reqCharge, FillMethod a_fillMethod, GemList& a_gems);
		GemList			ParseContainer(RE::TESObjectREFR* a_container, bool a_partialFill);
	}


	SInt32 BringMeASoul(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, UInt32 a_reqCharge, UInt32 a_fillMethod, bool a_partialFill, bool a_wasteOK);

	bool RegisterFuncs(VM* a_vm);
}
