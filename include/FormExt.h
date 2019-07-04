#pragma once

class TESForm;
class VMClassRegistry;
struct StaticFunctionTag;


namespace FormExt
{
	void	RegisterForBoundWeaponEquippedEvent(StaticFunctionTag*, TESForm* a_thisForm);
	void	UnregisterForBoundWeaponEquippedEvent(StaticFunctionTag*, TESForm* a_thisForm);
	void	RegisterForBoundWeaponUnequippedEvent(StaticFunctionTag*, TESForm* a_thisForm);
	void	UnregisterForBoundWeaponUnequippedEvent(StaticFunctionTag*, TESForm* a_thisForm);
	SInt32	GetLightDuration(StaticFunctionTag*, TESForm* a_light);
	SInt32	GetLightRadius(StaticFunctionTag*, TESForm* a_light);
	void	SetLightRadius(StaticFunctionTag*, TESForm* a_light, SInt32 a_radius);
	bool	IsSpear(StaticFunctionTag*, TESForm* a_form);
	bool	IsJavelin(StaticFunctionTag*, TESForm* a_form);
	bool	IsGrenade(StaticFunctionTag*, TESForm* a_form);
	bool	IsThrowingAxe(StaticFunctionTag*, TESForm* a_form);
	bool	IsThrowingKnife(StaticFunctionTag*, TESForm* a_form);
	bool	IsWax(StaticFunctionTag*, TESForm* a_form);
	bool	IsOil(StaticFunctionTag*, TESForm* a_form);
	bool	IsSpellWard(StaticFunctionTag*, TESForm* a_form);
	bool	HasFire(StaticFunctionTag*, TESForm* a_form);
	bool	HasIce(StaticFunctionTag*, TESForm* a_form);
	bool	HasShock(StaticFunctionTag*, TESForm* a_form);
	bool	HasPoison(StaticFunctionTag*, TESForm* a_form);
	bool	IsSalve(StaticFunctionTag*, TESForm* a_form);
	bool	IsBandage(StaticFunctionTag*, TESForm* a_form);
	bool	RegisterFuncs(VMClassRegistry* a_registry);
}
