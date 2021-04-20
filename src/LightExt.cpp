#include "pch.h"
#include "LightExt.h"

#include "SKSE/API.h"


namespace LightExt
{
    int32_t GetLightDuration(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectLIGH* a_light)
    {
        if (!a_light) {
            a_vm->TraceStack("a_light is a NONE form!", a_stackID, Severity::kWarning);
            return -1;
        }

        return a_light->data.time;
    }

    void SetLightDuration(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light, int32_t a_duration)
    {
        if (!a_light) {
            a_vm->TraceStack("a_light is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (a_duration < 0) {
            a_vm->TraceStack("a_duration can not be negative!", a_stackID, Severity::kWarning);
            return;
        }

        auto lightID = a_light->formID;
        auto task = SKSE::GetTaskInterface();
        task->AddTask([lightID, a_duration]() {
            auto light = RE::TESForm::LookupByID<RE::TESObjectLIGH>(lightID);
            if (!light) {
                logger::warn("Failed to lookup light by id!"sv);
                return;
            }

            light->data.time = a_duration;
        });
    }

    int32_t GetLightRadius(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESObjectLIGH* a_light)
    {
        if (!a_light) {
            a_vm->TraceStack("a_light is a NONE form!", a_stackID, Severity::kWarning);
            return -1;
        }

        return a_light->data.radius;
    }


    void SetLightRadius(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectLIGH* a_light, int32_t a_radius)
    {
        if (!a_light) {
            a_vm->TraceStack("a_light is a NONE form!", a_stackID, Severity::kWarning);
            return;
        } else if (a_radius < 0) {
            a_vm->TraceStack("a_radius can not be negative!", a_stackID, Severity::kWarning);
            return;
        }

        auto lightID = a_light->formID;
        auto task = SKSE::GetTaskInterface();
        task->AddTask([lightID, a_radius]() {
            auto light = RE::TESForm::LookupByID<RE::TESObjectLIGH>(lightID);
            if (!light) {
                logger::warn("Failed to lookup light by id!"sv);
                return;
            }

            light->data.radius = a_radius;
        });
    }


    bool RegisterFuncs(VM* a_vm)
    {
        a_vm->RegisterFunction("GetLightDuration", "iEquip_LightExt", GetLightDuration, true);
        a_vm->RegisterFunction("GetLightRadius", "iEquip_LightExt", GetLightRadius, true);
        a_vm->RegisterFunction("SetLightRadius", "iEquip_LightExt", SetLightRadius);
        a_vm->RegisterFunction("SetLightDuration", "iEquip_LightExt", SetLightDuration);

        return true;
    }
}
