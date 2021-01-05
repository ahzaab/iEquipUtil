#include "pch.h"
#include "Registration.h"


OnBoundWeaponEquippedRegSet* OnBoundWeaponEquippedRegSet::GetSingleton()
{
	static OnBoundWeaponEquippedRegSet singleton;
	return &singleton;
}


OnBoundWeaponEquippedRegSet::OnBoundWeaponEquippedRegSet() :
	Base("OnBoundWeaponEquipped")
{}


OnBoundWeaponUnequippedRegSet* OnBoundWeaponUnequippedRegSet::GetSingleton()
{
	static OnBoundWeaponUnequippedRegSet singleton;
	return &singleton;
}


OnBoundWeaponUnequippedRegSet::OnBoundWeaponUnequippedRegSet() :
	Base("OnBoundWeaponUnequipped")
{}


OnRefHandleActiveRegSet* OnRefHandleActiveRegSet::GetSingleton()
{
	static OnRefHandleActiveRegSet singleton;
	return &singleton;
}


OnRefHandleActiveRegSet::OnRefHandleActiveRegSet() :
	Base("OnRefHandleActive")
{}


OnRefHandleInvalidatedRegSet* OnRefHandleInvalidatedRegSet::GetSingleton()
{
	static OnRefHandleInvalidatedRegSet singleton;
	return &singleton;
}


OnRefHandleInvalidatedRegSet::OnRefHandleInvalidatedRegSet() :
	Base("OnRefHandleInvalidated")
{}
