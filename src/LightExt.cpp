#include "LightExt.h"

#include "SKSE/API.h"


namespace LightExt
{
	SInt32 GetLightDuration(RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light)
	{
		if (!a_light) {
			_WARNING("a_light is a NONE form!");
			return -1;
		}

		return a_light->data.time;
	}


	SInt32 GetLightRadius(RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light)
	{
		if (!a_light) {
			_WARNING("a_light is a NONE form!");
			return -1;
		}

		return a_light->data.radius;
	}


	void SetLightRadius(RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light, SInt32 a_radius)
	{
		if (!a_light) {
			_WARNING("a_light is a NONE form!");
			return;
		} else if (a_radius < 0) {
			_WARNING("a_radius can not be negative!");
			return;
		}

		auto lightID = a_light->formID;
		auto task = SKSE::GetTaskInterface();
		task->AddTask([lightID, a_radius]()
		{
			auto light = RE::TESForm::LookupByID<RE::TESObjectLIGH>(lightID);
			if (!light) {
				_WARNING("Failed to lookup light by id!");
				return;
			}

			light->data.radius = a_radius;
		});
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetLightDuration", "iEquip_LightExt", GetLightDuration);
		a_vm->RegisterFunction("GetLightRadius", "iEquip_LightExt", GetLightRadius);
		a_vm->RegisterFunction("SetLightRadius", "iEquip_LightExt", SetLightRadius);

		return true;
	}
}
