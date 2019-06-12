#include "Registration.h"

#include "GameForms.h"  // TESForm, LookupFormByID
#include "GameObjects.h"  // TESObjectWEAP
#include "GameRTTI.h"  // DYNAMIC_CAST
#include "GameTypes.h"  // BSFixedString
#include "PapyrusArgs.h"  // PackValue
#include "PapyrusVM.h"  // VMClassRegistry, IFunctionArguments

#include <tuple>  // tuple, tuple_size, get
#include <type_traits>  // enable_if_t, is_integral, false_type

#include "SKSEInterface.h"  // SKSE


namespace
{
	template <class T>
	struct is_numeric : public std::is_integral<T> {};


	template <>
	struct is_numeric<bool> : std::false_type {};


	template <class T, class Enable = void>
	void SetVMValue(VMValue* a_val, T a_arg)
	{
		VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
		PackValue(a_val, &a_arg, registry);
	}


	template <class T, std::enable_if_t<is_numeric<T>::value>>
	void SetVMValue(VMValue* a_val, T a_arg)
	{
		a_val->SetInt(static_cast<SInt32>(a_arg));
	}


	template <>
	void SetVMValue<bool>(VMValue* a_val, bool a_arg)
	{
		a_val->SetBool(a_arg);
	}


	template <>
	void SetVMValue<float>(VMValue* a_val, float a_arg)
	{
		a_val->SetFloat(a_arg);
	}


	template <>
	void SetVMValue<const BSFixedString&>(VMValue* a_val, const BSFixedString& a_arg)
	{
		a_val->SetString(a_arg.data);
	}


	template <class RegParams, class... Args>
	class EventQueueFunctor : public IFunctionArguments
	{
	public:
		EventQueueFunctor(const BSFixedString& a_eventName, Args... a_args) :
			_eventName(a_eventName.data),
			_args(a_args...)
		{}


		virtual ~EventQueueFunctor()
		{
			CALL_MEMBER_FN(&_eventName, Release)();
		}


		virtual bool Copy(Output* a_dst) override
		{
			constexpr UInt32 NUM_ARGS = std::tuple_size<decltype(_args)>::value;
			a_dst->Resize(NUM_ARGS);
			UInt32 i = 0;
			std::apply([&](auto&&... a_args)
			{
				((SetVMValue(a_dst->Get(i++), a_args)), ...);
			}, _args);
			return true;
		}


		void operator()(const EventRegistration<RegParams>& a_reg)
		{
			VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
			registry->QueueEvent(a_reg.handle, &_eventName, this);
		}

	private:
		BSFixedString		_eventName;
		std::tuple<Args...>	_args;
	};
}


OnBoundWeaponEquippedRegSet* OnBoundWeaponEquippedRegSet::GetSingleton()
{
	static OnBoundWeaponEquippedRegSet singleton;
	return &singleton;
}


void OnBoundWeaponEquippedRegSet::QueueEvent(UInt32 a_weaponType, UInt32 a_equipSlot)
{
	SKSE::AddTask([this, a_weaponType, a_equipSlot]()
	{
		ForEach(EventQueueFunctor<RegParams, UInt32, UInt32>(_callback, a_weaponType, a_equipSlot));
	});
}


OnBoundWeaponEquippedRegSet::OnBoundWeaponEquippedRegSet() :
	_callback("OnBoundWeaponEquipped")
{}


OnBoundWeaponUnequippedRegSet* OnBoundWeaponUnequippedRegSet::GetSingleton()
{
	static OnBoundWeaponUnequippedRegSet singleton;
	return &singleton;
}


void OnBoundWeaponUnequippedRegSet::QueueEvent(TESObjectWEAP* a_weap, UInt32 a_equipSlot)
{
	UInt32 weapFormID = a_weap->formID;
	SKSE::AddTask([this, weapFormID, a_equipSlot]()
	{
		TESObjectWEAP* weap = static_cast<TESObjectWEAP*>(LookupFormByID(weapFormID));
		ForEach(EventQueueFunctor<RegParams, TESObjectWEAP*, UInt32>(_callback, weap, a_equipSlot));
	});
}


OnBoundWeaponUnequippedRegSet::OnBoundWeaponUnequippedRegSet() :
	_callback("OnBoundWeaponUnequipped")
{}


OnRefHandleActiveRegSet* OnRefHandleActiveRegSet::GetSingleton()
{
	static OnRefHandleActiveRegSet singleton;
	return &singleton;
}


void OnRefHandleActiveRegSet::QueueEvent(TESForm* a_baseItem, RefHandle a_refHandle, SInt32 a_itemCount)
{
	UInt32 baseItemFormID = a_baseItem->formID;
	SKSE::AddTask([this, baseItemFormID, a_refHandle, a_itemCount]()
	{
		TESForm* baseItem = LookupFormByID(baseItemFormID);
		ForEach(EventQueueFunctor<RegParams, TESForm*, UInt32, SInt32>(_callback, baseItem, a_refHandle, a_itemCount));
		auto fullName = DYNAMIC_CAST(baseItem, TESForm, TESFullName);
	});
}


OnRefHandleActiveRegSet::OnRefHandleActiveRegSet() :
	_callback("OnRefHandleActive")
{}


OnRefHandleInvalidatedRegSet* OnRefHandleInvalidatedRegSet::GetSingleton()
{
	static OnRefHandleInvalidatedRegSet singleton;
	return &singleton;
}


void OnRefHandleInvalidatedRegSet::QueueEvent(TESForm* a_baseItem, RefHandle a_refHandle)
{
	UInt32 baseItemFormID = a_baseItem->formID;
	SKSE::AddTask([this, baseItemFormID, a_refHandle]()
	{
		TESForm* baseItem = LookupFormByID(baseItemFormID);
		ForEach(EventQueueFunctor<RegParams, TESForm*, UInt32>(_callback, baseItem, a_refHandle));
		auto fullName = DYNAMIC_CAST(baseItem, TESForm, TESFullName);
	});
}


OnRefHandleInvalidatedRegSet::OnRefHandleInvalidatedRegSet() :
	_callback("OnRefHandleInvalidated")
{}
