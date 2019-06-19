#pragma once

#include "GameBSExtraData.h"  // BSExtraData
#include "GameExtraData.h"
#include "GameObjects.h"  // AlchemyItem
#include "ITypes.h"  // UInt32

#include "RE/Memory.h"  // TES_HEAP_REDEFINE_NEW

#if _WIN64
#include "Relocation.h"
#endif


namespace RE
{
	class ExtraPoison : public BSExtraData
	{
	public:
		ExtraPoison();
		virtual ~ExtraPoison();

		inline static ExtraPoison* Create()
		{
#if _WIN64
			RelocAddr<std::uintptr_t> relVtbl(0x0152C470);	// 1_5_80
			std::uintptr_t vtbl = relVtbl.GetUIntPtr();
#else
			std::uintptr_t vtbl = 0x01079308;
#endif
			return static_cast<ExtraPoison*>(BSExtraData::Create(sizeof(RE::ExtraPoison), vtbl));
		}

		TES_HEAP_REDEFINE_NEW();

#if _WIN64
		// SE: they swapped order of poison and unk
		AlchemyItem*	poison;		// 10
		UInt32			count;		// 18
		UInt32			pad;		// 1C
#else
		UInt32			count;		// 08
		AlchemyItem*	poison;		// 0C
#endif
	};
}
