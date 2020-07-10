#include "Papyrus.h"

#include "SKSE/API.h"

#include "ActorExt.h"
#include "AmmoExt.h"
#include "FormExt.h"
#include "InventoryExt.h"
#include "LightExt.h"
#include "MagicEffectExt.h"
#include "ObjectReferenceExt.h"
#include "SoulSeeker.h"
#include "SpellExt.h"
#include "StringExt.h"
#include "UIExt.h"
#include "WeaponExt.h"


namespace Papyrus
{
	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();

		papyrus->Register(ActorExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_ActorExt");

		papyrus->Register(AmmoExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_AmmoExt");

		papyrus->Register(FormExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_FormExt");

		papyrus->Register(InventoryExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_InventoryExt");

		papyrus->Register(LightExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_LightExt");

		papyrus->Register(MagicEffectExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_MagicEffectExt");
		
		papyrus->Register(ObjectReferenceExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_ObjectReferenceExt");

		papyrus->Register(SoulSeeker::RegisterFuncs);
		_MESSAGE("Registered iEquip_SoulSeeker");

		papyrus->Register(SpellExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_SpellExt");

		papyrus->Register(StringExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_StringExt");

		papyrus->Register(UIExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_UIExt");

		papyrus->Register(WeaponExt::RegisterFuncs);
		_MESSAGE("Registered iEquip_WeaponExt");
	}
}
