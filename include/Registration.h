#pragma once

#include "RE/Skyrim.h"
#include "SKSE/RegistrationSet.h"

#include "RefHandleManager.h"


// 1. Weapon type
// 2. Equip slot
class OnBoundWeaponEquippedRegSet : public SKSE::RegistrationSet<UInt32, UInt32>
{
public:
	using Base = SKSE::RegistrationSet<UInt32, UInt32>;


	static OnBoundWeaponEquippedRegSet* GetSingleton();

private:
	OnBoundWeaponEquippedRegSet();
	OnBoundWeaponEquippedRegSet(const OnBoundWeaponEquippedRegSet&) = delete;
	OnBoundWeaponEquippedRegSet(OnBoundWeaponEquippedRegSet&&) = delete;
	~OnBoundWeaponEquippedRegSet() = default;

	OnBoundWeaponEquippedRegSet& operator=(const OnBoundWeaponEquippedRegSet&) = delete;
	OnBoundWeaponEquippedRegSet& operator=(OnBoundWeaponEquippedRegSet&&) = delete;
};


// 1. Weapon
// 2. Equip slot
class OnBoundWeaponUnequippedRegSet : public SKSE::RegistrationSet<RE::TESObjectWEAP*, UInt32>
{
public:
	using Base = SKSE::RegistrationSet<RE::TESObjectWEAP*, UInt32>;


	static OnBoundWeaponUnequippedRegSet* GetSingleton();

private:
	OnBoundWeaponUnequippedRegSet();
	OnBoundWeaponUnequippedRegSet(const OnBoundWeaponUnequippedRegSet&) = delete;
	OnBoundWeaponUnequippedRegSet(OnBoundWeaponUnequippedRegSet&&) = delete;
	~OnBoundWeaponUnequippedRegSet() = default;

	OnBoundWeaponUnequippedRegSet& operator=(const OnBoundWeaponUnequippedRegSet&) = delete;
	OnBoundWeaponUnequippedRegSet& operator=(OnBoundWeaponUnequippedRegSet&&) = delete;
};


// 1. Base item
// 2. Ref handle
// 2. Item count
class OnRefHandleActiveRegSet : public SKSE::RegistrationSet<RE::TESForm*, UInt32, SInt32>
{
public:
	using Base = SKSE::RegistrationSet<RE::TESForm*, UInt32, SInt32>;


	static OnRefHandleActiveRegSet* GetSingleton();

private:
	OnRefHandleActiveRegSet();
	OnRefHandleActiveRegSet(const OnRefHandleActiveRegSet&) = delete;
	OnRefHandleActiveRegSet(OnRefHandleActiveRegSet&&) = delete;
	~OnRefHandleActiveRegSet() = default;

	OnRefHandleActiveRegSet& operator=(const OnRefHandleActiveRegSet&) = delete;
	OnRefHandleActiveRegSet& operator=(OnRefHandleActiveRegSet&&) = delete;
};


// 1. Base item
// 2. Ref handle
class OnRefHandleInvalidatedRegSet : public SKSE::RegistrationSet<RE::TESForm*, UInt32>
{
public:
	using Base = SKSE::RegistrationSet<RE::TESForm*, UInt32>;
	using RefHandle = RefHandleManager::RefHandle;


	static OnRefHandleInvalidatedRegSet* GetSingleton();

private:
	OnRefHandleInvalidatedRegSet();
	OnRefHandleInvalidatedRegSet(const OnRefHandleInvalidatedRegSet&) = delete;
	OnRefHandleInvalidatedRegSet(OnRefHandleInvalidatedRegSet&&) = delete;
	~OnRefHandleInvalidatedRegSet() = default;

	OnRefHandleInvalidatedRegSet& operator=(const OnRefHandleInvalidatedRegSet&) = delete;
	OnRefHandleInvalidatedRegSet& operator=(OnRefHandleInvalidatedRegSet&&) = delete;
};
