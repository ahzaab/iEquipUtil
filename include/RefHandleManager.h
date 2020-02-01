#pragma once

#include <limits>
#include <map>
#include <mutex>
#include <optional>
#include <set>

#include "RE/Skyrim.h"
#include "SKSE/API.h"


class RefHandleManager :
	public RE::BSTEventSink<RE::TESUniqueIDChangeEvent>
{
private:
	using SubType = UInt16;

public:
	using UniqueID = SubType;
	using RefHandle = SubType;
	using HandleResult = std::optional<RefHandle>;


	enum { kInvalidRefHandle = static_cast<RefHandle>(-1) };


	struct EntryData
	{
		constexpr EntryData() : invEntryData(0), extraList(0) {}
		constexpr EntryData(RE::InventoryEntryData* a_invEntryData, RE::ExtraDataList* a_extraList) : invEntryData(a_invEntryData), extraList(a_extraList) {}


		RE::InventoryEntryData* invEntryData;
		RE::ExtraDataList* extraList;
	};


	static RefHandleManager* GetSingleton();

	void	Register();
	void	Clear() noexcept;
	bool	Save(SKSE::SerializationInterface* a_intfc, UInt32 a_type, UInt32 a_version);
	bool	Load(SKSE::SerializationInterface* a_intfc, UInt32 a_version);

	HandleResult	ActivateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList*& a_extraList, SInt32 a_count);
	HandleResult	ActivateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList& a_extraList, SInt32 a_count);
	bool			InvalidateAndDispatch(const RE::TESForm* a_item, UniqueID a_uniqueID);
	bool			TryInvalidateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList* a_extraList);

	std::optional<EntryData>	LookupEntry(RE::TESForm* a_item, RefHandle a_handle);
	RefHandle					LookupHandle(UniqueID a_uniqueID);

	bool	IsTrackedType(const RE::TESForm* a_form) const;
	bool	IsInit() const;
	void	SetInit();

private:
	using EventResult = RE::BSEventNotifyControl;
	using Lock = std::recursive_mutex;
	using Locker = std::lock_guard<Lock>;


	enum
	{
		kUniqueID = 0,
		kRefHandle,
		kTotal,

		kInvalidUniqueID = 0,
		kPlayerRefID = 0x14,

		kRefArrSize = std::numeric_limits<UInt16>::max() / 8,
		kLargestHandle = kRefArrSize * 8 - 1
	};


	RefHandleManager();
	RefHandleManager(const RefHandleManager&) = delete;
	RefHandleManager(RefHandleManager&&) = delete;
	virtual ~RefHandleManager() = default;

	RefHandleManager& operator=(const RefHandleManager&) = delete;
	RefHandleManager& operator=(RefHandleManager&&) = delete;

	virtual	EventResult ProcessEvent(const RE::TESUniqueIDChangeEvent* a_event, RE::BSTEventSource<RE::TESUniqueIDChangeEvent>* a_dispatcher) override;

	HandleResult ActivateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList& a_extraList, SInt32 a_count, RefHandle a_handle);

	RefHandle	GetFreeHandle();
	void		MarkHandle(RefHandle a_handle);
	void		UnmarkHandle(RefHandle a_handle);

	UniqueID GetNextUniqueID();


	mutable Lock					_lock;
	std::map<UniqueID, RefHandle>	_idToHandleMap;
	std::map<RefHandle, UniqueID>	_handleToIDMap;
	UInt8							_activeHandles[kRefArrSize];
	bool							_init;
};
