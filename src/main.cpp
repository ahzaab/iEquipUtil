#include "skse64_common/skse_version.h"

#include <clocale>
#include <cstdlib>
#include <string>

#include "Events.h"
#include "Hooks.h"
#include "LocaleManager.h"
#include "Papyrus.h"
#include "RefHandleManager.h"
#include "Registration.h"
#include "Settings.h"
#include "version.h"

#include "RE/Skyrim.h"
#include "SKSE/API.h"


namespace
{
	enum : UInt32
	{
		kSerializationVersion = 1,
		kOnBoundWeaponEquipped = 'OBWE',
		kOnBoundWeaponUnequipped = 'OBWU',
		kOnRefHandleActive = 'ORHA',
		kOnRefHandleInvalidated = 'ORHI',
		kRefHandleManager = 'RFHM'
	};


	std::string DecodeTypeCode(UInt32 a_typeCode)
	{
		constexpr std::size_t SIZE = sizeof(UInt32);

		std::string sig;
		sig.resize(SIZE);
		char* iter = reinterpret_cast<char*>(&a_typeCode);
		for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
			sig[j] = iter[i];
		}
		return sig;
	}


	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		if (!OnBoundWeaponEquippedRegSet::GetSingleton()->Save(a_intfc, kOnBoundWeaponEquipped, kSerializationVersion)) {
			_ERROR("Failed to save OnBoundWeaponEquipped regs!\n");
		}

		if (!OnBoundWeaponUnequippedRegSet::GetSingleton()->Save(a_intfc, kOnBoundWeaponUnequipped, kSerializationVersion)) {
			_ERROR("Failed to save OnBoundWeaponUnequipped regs!\n");
		}

		if (!OnRefHandleActiveRegSet::GetSingleton()->Save(a_intfc, kOnRefHandleActive, kSerializationVersion)) {
			_ERROR("Failed to save OnRefHandleActive regs!\n");
		}

		if (!OnRefHandleInvalidatedRegSet::GetSingleton()->Save(a_intfc, kOnRefHandleInvalidated, kSerializationVersion)) {
			_ERROR("Failed to save OnRefHandleInvalidated regs!\n");
		}

		if (!RefHandleManager::GetSingleton()->Save(a_intfc, kRefHandleManager, kSerializationVersion)) {
			_ERROR("Failed to save ref handles!\n");
		}

		_MESSAGE("Finished saving data");
	}


	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		UInt32 type;
		UInt32 version;
		UInt32 length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != kSerializationVersion) {
				_ERROR("Loaded data is out of date! Read (%u), expected (%u) for type code (%s)", version, kSerializationVersion, DecodeTypeCode(type).c_str());
				continue;
			}

			switch (type) {
			case kOnBoundWeaponEquipped:
				{
					auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc)) {
						_ERROR("Failed to load OnBoundWeaponEquipped regs!\n");
					}
				}
				break;
			case kOnBoundWeaponUnequipped:
				{
					auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc)) {
						_ERROR("Failed to load OnBoundWeaponUnequipped regs!\n");
					}
				}
				break;
			case kOnRefHandleActive:
				{
					auto regs = OnRefHandleActiveRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc)) {
						_ERROR("Failed to load OnRefHandleActive regs!\n");
					}
				}
				break;
			case kOnRefHandleInvalidated:
				{
					auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc)) {
						_ERROR("Failed to load OnRefHandleInvalidated regs!\n");
					}
				}
				break;
			case kRefHandleManager:
				{
					auto regs = RefHandleManager::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc, version)) {
						_ERROR("Failed to load ref handles!\n");
					}
				}
				break;
			default:
				_ERROR("Unrecognized record type (%s)!", DecodeTypeCode(type).c_str());
				break;
			}
		}

		_MESSAGE("Finished loading data");
	}


	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			{
				if (Settings::loadSettings()) {
					_MESSAGE("Settings loaded successfully");
				} else {
					_ERROR("Encountered error while loading settings!\n");
				}

				auto events = RE::ScriptEventSourceHolder::GetSingleton();
				events->AddEventSink(Events::EquipEventHandler::GetSingleton());

				auto refManager = RefHandleManager::GetSingleton();
				refManager->Register();

				auto locManager = LocaleManager::GetSingleton();
				locManager->LoadLocalizationStrings();
			}
			break;
		}
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		std::setlocale(LC_ALL, "");

		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\iEquipUtil.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);

		_MESSAGE("iEquipUtil v%s", IEQP_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "iEquipUtil";
		a_info->version = IEQP_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		switch (a_skse->RuntimeVersion()) {
		case RUNTIME_VERSION_1_5_97:
			break;
		default:
			_FATALERROR("Unsupported runtime version %s!\n", a_skse->UnmangledRuntimeVersion().c_str());
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		_MESSAGE("iEquipUtil loaded");

		if (!SKSE::Init(a_skse)) {
			return false;
		}

		auto messaging = SKSE::GetMessagingInterface();
		if (!messaging->RegisterListener("SKSE", MessageHandler)) {
			return false;
		}

		auto serialization = SKSE::GetSerializationInterface();
		serialization->SetUniqueID('IEQP');
		serialization->SetSaveCallback(SaveCallback);
		serialization->SetLoadCallback(LoadCallback);
		_MESSAGE("Registered serialization callbacks");

		Papyrus::Register();
		Hooks::Install();

		return true;
	}
};
