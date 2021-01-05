#include "pch.h"
#include "MagicEffectExt.h"


namespace MagicEffectExt
{
	RE::TESForm* GetAssociatedItem(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::EffectSetting* a_effect)
	{
		if (!a_effect) {
			a_vm->TraceStack("a_effect is a NONE form!", a_stackID, Severity::kWarning);
			return 0;
		}

		return a_effect->data.associatedForm;
	}


	bool RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("GetAssociatedItem", "iEquip_MagicEffectExt", GetAssociatedItem, true);

		return true;
	}
}
