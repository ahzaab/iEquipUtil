#include "GameEvents.h"  // GetEventDispatcherList
#include "IDebugLog.h"  // gLog, IDebugLog
#include "PluginAPI.h"  // SKSEMessagingInterface, SKSEInterface, PluginInfo
#include "skse_version.h"  // RUNTIME_VERSION

#include <clocale>  // setlocale
#include <cstdlib>  // size_t
#include <string>  // string

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "ActorExt.h"  // RegisterFuncs
#include "AmmoExt.h"  // RegisterFuncs
#include "Events.h"  // EquipEventHandler
#include "FormExt.h"  // RegisterFuncs
#include "Hooks.h"  // InstallHooks
#include "InventoryExt.h"  // InventoryExt
#include "LocaleManager.h"  // LocaleManager
#include "ObjectReferenceExt.h"  // RegisterFuncs
#include "RefHandleManager.h"  // RefHandleManager
#include "Registration.h"  // OnBoundWeaponEquippedRegSet, OnBoundWeaponUnequippedRegSet, OnRefHandleActiveRegSet, OnRefHandleInvalidatedRegSet
#include "Settings.h"  // Settings
#include "SKSEInterface.h"  // SKSE
#include "SoulSeeker.h"  // RegisterFuncs
#include "SpellExt.h"  // RegisterFuncs
#include "StringExt.h"  // RegisterFuncs
#include "UIExt.h"  // RegisterFuncs
#include "version.h"  // IEQUIP_VERSION_VERSTRING, IEQUIP_VERSION_MAJOR
#include "WeaponExt.h"  // RegisterFuncs

#include "RE/EventDispatcherList.h"  // EventDispatcherList


namespace
{
#if _WIN64
	constexpr char LOG_PATH[] = "\\My Games\\Skyrim Special Edition\\SKSE\\iEquipUtil.log";
	constexpr char NAME[] = "iEquipUtil";
#else
	constexpr char LOG_PATH[] = "\\My Games\\Skyrim\\SKSE\\iEquipUtil.log";
	constexpr char NAME[] = "iEquipUtil_LE";
#endif


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


	void SaveCallback(SKSESerializationInterface* a_intfc)
	{
		if (!OnBoundWeaponEquippedRegSet::GetSingleton()->Save(a_intfc, kOnBoundWeaponEquipped, kSerializationVersion)) {
			_ERROR("[ERROR] Failed to save OnBoundWeaponEquipped regs!\n");
		}

		if (!OnBoundWeaponUnequippedRegSet::GetSingleton()->Save(a_intfc, kOnBoundWeaponUnequipped, kSerializationVersion)) {
			_ERROR("[ERROR] Failed to save OnBoundWeaponUnequipped regs!\n");
		}

		if (!OnRefHandleActiveRegSet::GetSingleton()->Save(a_intfc, kOnRefHandleActive, kSerializationVersion)) {
			_ERROR("[ERROR] Failed to save OnRefHandleActive regs!\n");
		}

		if (!OnRefHandleInvalidatedRegSet::GetSingleton()->Save(a_intfc, kOnRefHandleInvalidated, kSerializationVersion)) {
			_ERROR("[ERROR] Failed to save OnRefHandleInvalidated regs!\n");
		}

		if (!RefHandleManager::GetSingleton()->Save(a_intfc, kRefHandleManager, kSerializationVersion)) {
			_ERROR("[ERROR] Failed to save ref handles!\n");
		}

		_MESSAGE("[MESSAGE] Finished saving data");
	}


	void LoadCallback(SKSESerializationInterface* a_intfc)
	{
		UInt32 type;
		UInt32 version;
		UInt32 length;
		while (a_intfc->GetNextRecordInfo(&type, &version, &length)) {
			if (version != kSerializationVersion) {
				_ERROR("[ERROR] Loaded data is out of date! Read (%u), expected (%u) for type code (%s)", version, kSerializationVersion, DecodeTypeCode(type).c_str());
				continue;
			}

			switch (type) {
			case kOnBoundWeaponEquipped:
				{
					auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc, version)) {
						_ERROR("[ERROR] Failed to load OnBoundWeaponEquipped regs!\n");
					}
				}
				break;
			case kOnBoundWeaponUnequipped:
				{
					auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc, version)) {
						_ERROR("[ERROR] Failed to load OnBoundWeaponUnequipped regs!\n");
					}
				}
				break;
			case kOnRefHandleActive:
				{
					auto regs = OnRefHandleActiveRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc, version)) {
						_ERROR("[ERROR] Failed to load OnRefHandleActive regs!\n");
					}
				}
				break;
			case kOnRefHandleInvalidated:
				{
					auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc, version)) {
						_ERROR("[ERROR] Failed to load OnRefHandleInvalidated regs!\n");
					}
				}
				break;
			case kRefHandleManager:
				{
					auto regs = RefHandleManager::GetSingleton();
					regs->Clear();
					if (!regs->Load(a_intfc, version)) {
						_ERROR("[ERROR] Failed to load ref handles!\n");
					}
				}
				break;
			default:
				_ERROR("[ERROR] Unrecognized record type (%s)!", DecodeTypeCode(type).c_str());
				break;
			}
		}

		_MESSAGE("[MESSAGE] Finished loading data");
	}


	void MessageHandler(SKSEMessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSEMessagingInterface::kMessage_DataLoaded:
			{
				if (Settings::loadSettings()) {
					_MESSAGE("[MESSAGE] Settings loaded successfully");
				} else {
					_ERROR("[ERROR] Encountered error while loading settings!\n");
				}
#if _WIN64
				auto eventDispatcherList = reinterpret_cast<RE::EventDispatcherList*>(GetEventDispatcherList());
				eventDispatcherList->equipEventSource.AddEventSink(Events::EquipEventHandler::GetSingleton());
				eventDispatcherList->uniqueIDChangeEventSource.AddEventSink(RefHandleManager::GetSingleton());
#else
				RE::g_equipEventDispatcher->AddEventSink(Events::EquipEventHandler::GetSingleton());
				RE::g_uniqueIDChangedEventDispatcher->AddEventSink(RefHandleManager::GetSingleton());
#endif
				LocaleManager* locManager = LocaleManager::GetSingleton();
				locManager->LoadLocalizationStrings();
			}
			break;
		}
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		std::setlocale(LC_ALL, "");

		gLog.OpenRelative(CSIDL_MYDOCUMENTS, LOG_PATH);
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("%s v%s", NAME, IEQUIP_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = NAME;
		a_info->version = IEQUIP_VERSION_MAJOR;

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		switch (a_skse->runtimeVersion) {
#if _WIN64
		case RUNTIME_VERSION_1_5_73:
		case RUNTIME_VERSION_1_5_80:
#else
		case RUNTIME_VERSION_1_9_32_0:
#endif
			break;
		default:
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		_MESSAGE("[MESSAGE] %s loaded", NAME);

		if (!SKSE::Init(a_skse)) {
			return false;
		}

		auto pluginHandle = SKSE::GetPluginHandle();

		auto serialization = SKSE::GetSerializationInterface();
		serialization->SetUniqueID(pluginHandle, 'IEQP');
		serialization->SetSaveCallback(pluginHandle, SaveCallback);
		serialization->SetLoadCallback(pluginHandle, LoadCallback);
		_MESSAGE("[MESSAGE] Registered serialization callbacks");

		auto messaging = SKSE::GetMessagingInterface();
		messaging->RegisterListener(pluginHandle, "SKSE", MessageHandler);
		_MESSAGE("[MESSAGE] Registered messaging listener");

		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(ActorExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_ActorExt");
		papyrus->Register(AmmoExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_AmmoExt");
		papyrus->Register(FormExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_FormExt");
		papyrus->Register(InventoryExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_InventoryExt");
		papyrus->Register(ObjectReferenceExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_ObjectReferenceExt");
		papyrus->Register(SoulSeeker::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_SoulSeeker");
		papyrus->Register(SpellExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_SpellExt");
		papyrus->Register(StringExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_StringExt");
		papyrus->Register(UIExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_UIExt");
		papyrus->Register(WeaponExt::RegisterFuncs);
		_MESSAGE("[MESSAGE] Registered iEquip_WeaponExt");

		InstallHooks();
		_MESSAGE("[MESSAGE] Installed hooks");

		return true;
	}
};
