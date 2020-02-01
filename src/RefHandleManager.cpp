#include "RefHandleManager.h"

#include <algorithm>
#include <cstring> 
#include <type_traits>
#include <vector>

#include "Registration.h"


RefHandleManager* RefHandleManager::GetSingleton()
{
	static RefHandleManager singleton;
	return &singleton;
}


void RefHandleManager::Register()
{
	auto events = RE::ScriptEventSourceHolder::GetSingleton();
	events->AddEventSink(this);
}


void RefHandleManager::Clear() noexcept
{
	Locker locker(_lock);

	_idToHandleMap.clear();
	_handleToIDMap.clear();
	std::memset(_activeHandles, 0, sizeof(_activeHandles));
	_init = false;
}


bool RefHandleManager::Save(SKSE::SerializationInterface* a_intfc, UInt32 a_type, UInt32 a_version)
{
	Locker locker(_lock);

	a_intfc->OpenRecord(a_type, a_version);

	a_intfc->WriteRecordData(_init);

	std::size_t numEntries = _idToHandleMap.size();
	a_intfc->WriteRecordData(numEntries);

	SubType buf[kTotal];
	for (auto& it : _idToHandleMap) {
		buf[kUniqueID] = it.first;
		buf[kRefHandle] = it.second;
		a_intfc->WriteRecordData(&buf, sizeof(buf));
	}

	return true;
}


bool RefHandleManager::Load(SKSE::SerializationInterface* a_intfc, UInt32 a_version)
{
	Locker locker(_lock);

	a_intfc->ReadRecordData(_init);

	std::size_t numEntries;
	if (!a_intfc->ReadRecordData(numEntries)) {
		_ERROR("Failed to load handle count!\n");
		return false;
	}

	SubType buf[kTotal];
	for (std::size_t i = 0; i < numEntries; ++i) {
		if (a_intfc->ReadRecordData(buf)) {
			_idToHandleMap.insert(std::make_pair(buf[kUniqueID], buf[kRefHandle]));
			_handleToIDMap.insert(std::make_pair(buf[kRefHandle], buf[kUniqueID]));
			MarkHandle(buf[kRefHandle]);
		} else {
			_ERROR("Failed to load ref handles!\n");
			return false;
		}
	}

	return true;
}


auto RefHandleManager::ActivateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList*& a_extraList, SInt32 a_count)
-> HandleResult
{
	assert(a_item);
	assert(IsTrackedType(a_item));
	Locker locker(_lock);

	auto handle = GetFreeHandle();
	if (handle == kInvalidRefHandle) {
		return std::nullopt;
	}

	if (!a_extraList) {
		a_extraList = new RE::ExtraDataList();
	}

	return ActivateAndDispatch(a_item, *a_extraList, a_count, handle);
}


auto RefHandleManager::ActivateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList& a_extraList, SInt32 a_count)
-> HandleResult
{
	assert(a_item);
	assert(IsTrackedType(a_item));
	Locker locker(_lock);

	auto handle = GetFreeHandle();
	if (handle == kInvalidRefHandle) {
		return std::nullopt;
	}

	return ActivateAndDispatch(a_item, a_extraList, a_count, handle);
}


bool RefHandleManager::InvalidateAndDispatch(const RE::TESForm* a_item, UniqueID a_uniqueID)
{
	assert(a_item);
	Locker locker(_lock);

	auto it = _idToHandleMap.find(a_uniqueID);
	if (it == _idToHandleMap.end()) {
		return false;
	}

	auto handle = it->second;
	_handleToIDMap.erase(handle);
	UnmarkHandle(handle);
	_idToHandleMap.erase(it);

	auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
	regs->QueueEvent(a_item, handle);
	return true;
}


bool RefHandleManager::TryInvalidateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList* a_extraList)
{
	assert(a_item);
	assert(a_extraList);

	auto xID = a_extraList->GetByType<RE::ExtraUniqueID>();
	if (!xID) {
		return false;
	}

	return InvalidateAndDispatch(a_item, xID->uniqueID);
}


auto RefHandleManager::LookupEntry(RE::TESForm* a_item, RefHandle a_handle)
	-> std::optional<EntryData>
{
	auto object = a_item->As<RE::TESBoundObject>();
	if (!object) {
		_ERROR("Form is not a bound object!\n");
		return std::nullopt;
	}

	if (a_handle > kLargestHandle || a_handle == kInvalidRefHandle) {
		_ERROR("Ref handle is invalid!\n");
		return std::nullopt;
	}

	Locker locker(_lock);

	auto idIt = _handleToIDMap.find(a_handle);
	if (idIt == _handleToIDMap.end()) {
		_ERROR("Handle not found in map!\n");
		return std::nullopt;
	}
	auto idToFind = idIt->second;

	auto player = RE::PlayerCharacter::GetSingleton();
	auto inv = player->GetInventory([&](RE::TESBoundObject* a_object) -> bool
	{
		return a_object == object;
	});

	auto invIt = inv.find(object);
	if (invIt == inv.end()) {
		return std::nullopt;
	}

	std::optional<EntryData> result;
	auto& entryData = invIt->second.second;
	if (entryData->extraLists) {
		for (auto& xList : *entryData->extraLists) {
			auto xID = xList->GetByType<RE::ExtraUniqueID>();
			if (xID && xID->uniqueID == idToFind) {
				result.emplace(entryData.get(), xList);
				break;
			}
		}
	}
	return result;
}


auto RefHandleManager::LookupHandle(UniqueID a_uniqueID)
	-> RefHandle
{
	Locker locker(_lock);

	auto it = _idToHandleMap.find(a_uniqueID);
	return it != _idToHandleMap.end() ? it->second : kInvalidRefHandle;
}


bool RefHandleManager::IsTrackedType(const RE::TESForm* a_form) const
{
	assert(a_form);

	switch (a_form->formType) {
	case RE::FormType::Armor:
	case RE::FormType::Weapon:
		return true;
	default:
		return false;
	}
}


bool RefHandleManager::IsInit() const
{
	Locker locker(_lock);
	return _init;
}


void RefHandleManager::SetInit()
{
	Locker locker(_lock);
	_init = true;
}


RefHandleManager::RefHandleManager() :
	_lock(),
	_idToHandleMap(),
	_handleToIDMap(),
	_activeHandles{ 0 },
	_init(false)
{}


auto RefHandleManager::ProcessEvent(const RE::TESUniqueIDChangeEvent* a_event, RE::BSTEventSource<RE::TESUniqueIDChangeEvent>* a_dispatcher)
	-> EventResult
{
	if (!IsInit() || !a_event) {
		return EventResult::kContinue;
	}

	if ((a_event->oldBaseID != kPlayerRefID && a_event->newBaseID != kPlayerRefID) || a_event->oldUniqueID == a_event->newUniqueID) {
		return EventResult::kContinue;
	}
	auto item = RE::TESForm::LookupByID(a_event->objectID);

	Locker locker(_lock);

	auto it = _idToHandleMap.find(a_event->oldUniqueID);
	if (it != _idToHandleMap.end()) {	// update id change
		auto handle = it->second;
		_handleToIDMap.erase(handle);
		_idToHandleMap.erase(it);

		if (a_event->newUniqueID == kInvalidUniqueID) {
			auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
			if (item) {
				regs->QueueEvent(item, handle);
			}
		} else {
			_handleToIDMap.insert(std::make_pair(handle, a_event->newUniqueID));
			_idToHandleMap.insert(std::make_pair(a_event->newUniqueID, handle));
		}
	} else if (item && IsTrackedType(item)) {	// consider tracking form if we didn't before
		auto handle = GetFreeHandle();
		if (handle != kInvalidRefHandle) {
			_handleToIDMap.insert(std::make_pair(handle, a_event->newUniqueID));
			_idToHandleMap.insert(std::make_pair(a_event->newUniqueID, handle));
			auto regs = OnRefHandleActiveRegSet::GetSingleton();
			regs->QueueEvent(item, handle, 1);
		}
	}

	return EventResult::kContinue;
}


auto RefHandleManager::ActivateAndDispatch(const RE::TESForm* a_item, RE::ExtraDataList& a_extraList, SInt32 a_count, RefHandle a_handle)
-> HandleResult
{
	auto xID = a_extraList.GetByType<RE::ExtraUniqueID>();
	if (!xID) {
		xID = new RE::ExtraUniqueID();
		xID->baseID = kPlayerRefID;
		xID->uniqueID = GetNextUniqueID();
		a_extraList.Add(xID);
	}

	_idToHandleMap.insert({ xID->uniqueID, a_handle });
	_handleToIDMap.insert({ a_handle, xID->uniqueID });

	auto regs = OnRefHandleActiveRegSet::GetSingleton();
	regs->QueueEvent(a_item, a_handle, a_count);

	return HandleResult(a_handle);
}


auto RefHandleManager::GetFreeHandle()
	-> RefHandle
{
	for (std::size_t i = 0; i < kRefArrSize; ++i) {
		if (_activeHandles[i] != 0xFF) {
			for (UInt8 j = 0; j < 8; ++j) {
				if (((_activeHandles[i] >> j) & 1) == 0) {
					_activeHandles[i] |= 1 << j;
					return (i * 8) + j;
				}
			}
		}
	}
	_FATALERROR("Ran out of ref handles!\n");
	return kInvalidRefHandle;
}


void RefHandleManager::MarkHandle(RefHandle a_handle)
{
	constexpr std::size_t SIZE = sizeof(std::remove_extent_t<decltype(_activeHandles)>) * 8;
	std::size_t idx = a_handle / SIZE;
	UInt8 bit = 1 << (a_handle % SIZE);
	_activeHandles[idx] |= bit;
}


void RefHandleManager::UnmarkHandle(RefHandle a_handle)
{
	constexpr std::size_t SIZE = sizeof(std::remove_extent_t<decltype(_activeHandles)>) * 8;
	std::size_t idx = a_handle / SIZE;
	UInt8 bit = 1 << (a_handle % SIZE);
	_activeHandles[idx] &= ~bit;
}


auto RefHandleManager::GetNextUniqueID()
	-> UniqueID
{
	auto player = RE::PlayerCharacter::GetSingleton();
	auto invChanges = player->GetInventoryChanges();
	return invChanges ? invChanges->GetNextUniqueID() : 0;
}
