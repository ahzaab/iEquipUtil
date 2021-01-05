#include "pch.h"
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
        logger::info("Registered iEquip_ActorExt");

        papyrus->Register(AmmoExt::RegisterFuncs);
        logger::info("Registered iEquip_AmmoExt");

        papyrus->Register(FormExt::RegisterFuncs);
        logger::info("Registered iEquip_FormExt");

        papyrus->Register(InventoryExt::RegisterFuncs);
        logger::info("Registered iEquip_InventoryExt");

        papyrus->Register(LightExt::RegisterFuncs);
        logger::info("Registered iEquip_LightExt");

        papyrus->Register(MagicEffectExt::RegisterFuncs);
        logger::info("Registered iEquip_MagicEffectExt");

        papyrus->Register(ObjectReferenceExt::RegisterFuncs);
        logger::info("Registered iEquip_ObjectReferenceExt");

        papyrus->Register(SoulSeeker::RegisterFuncs);
        logger::info("Registered iEquip_SoulSeeker");

        papyrus->Register(SpellExt::RegisterFuncs);
        logger::info("Registered iEquip_SpellExt");

        papyrus->Register(StringExt::RegisterFuncs);
        logger::info("Registered iEquip_StringExt");

        papyrus->Register(UIExt::RegisterFuncs);
        logger::info("Registered iEquip_UIExt");

        papyrus->Register(WeaponExt::RegisterFuncs);
        logger::info("Registered iEquip_WeaponExt");
    }
}
